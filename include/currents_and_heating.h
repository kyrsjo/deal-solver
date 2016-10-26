/*
 * currents_and_heating.h
 *
 *  Created on: Jul 28, 2016
 *      Author: kristjan
 */

#ifndef INCLUDE_CURRENTS_AND_HEATING_H_
#define INCLUDE_CURRENTS_AND_HEATING_H_


#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/dofs/dof_renumbering.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_system.h>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/base/logstream.h>
#include <deal.II/base/timer.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/data_out.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/solver_gmres.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/sparse_direct.h>	// UMFpack

#include <fstream>
#include <iostream>

#include "mesh_preparer.h" // for BoundaryId-s.. probably should think of a better place for them
#include "physical_quantities.h"
#include "laplace.h"

// forward declaration for Laplace to exist when declaring CurrentsAndHeating
namespace laplace {template<int dim> class Laplace;}

namespace currents_heating {
	using namespace dealii;
	using namespace laplace;

	template<int dim>
	class CurrentsAndHeating {
	public:
		CurrentsAndHeating(PhysicalQuantities pq_, Laplace<dim> *laplace_);

		CurrentsAndHeating(PhysicalQuantities pq_, Laplace<dim> *laplace_,
				Triangulation<dim>* previous_triangulation_,
				DoFHandler<dim>* previous_dof_handler_,
				Vector<double>* previous_solution_);

		void run();

		Triangulation<dim>* getp_triangulation();
		Vector<double>* getp_solution();
		DoFHandler<dim>* getp_dof_handler();

	private:
		void setup_system();
		void assemble_system_newton(bool first_iteration);
		void solve();
		void output_results(const unsigned int iteration) const;

		void setup_mapping();
		void set_initial_condition();
		void set_initial_condition_slow();

		static constexpr unsigned int currents_degree = 1;
		static constexpr unsigned int heating_degree  = 1;

		static constexpr double ambient_temperature = 300.0;

		Triangulation<dim> triangulation;
		FESystem<dim> fe;
		DoFHandler<dim> dof_handler;

		SparsityPattern sparsity_pattern;
		SparseMatrix<double> system_matrix;

		Vector<double> present_solution;
		Vector<double> newton_update;
		Vector<double> system_rhs;

		PhysicalQuantities pq;
		Laplace<dim> *laplace;

		/** Bijective mapping of interface faces
		 * (copper_cell_index, copper_cell_face) <-> (vacuum_cell_index, vacuum_cell_face)
		 */
		std::map< std::pair<unsigned, unsigned>, std::pair<unsigned, unsigned> > interface_map;

		/** Previous step mesh and solution for setting the initial condition */
		Triangulation<dim>* previous_triangulation;
		DoFHandler<dim>* previous_dof_handler;
		Vector<double>* previous_solution;
		bool interp_initial_conditions;
	};

} // end currents_heating namespace


#endif /* INCLUDE_CURRENTS_AND_HEATING_H_ */

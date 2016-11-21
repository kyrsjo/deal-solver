/*
 * laplace.h
 *
 *  Created on: Jul 26, 2016
 *      Author: kristjan
 */

#ifndef INCLUDE_LAPLACE_H_
#define INCLUDE_LAPLACE_H_

#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_reordering.h>
#include <deal.II/grid/grid_tools.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/base/logstream.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/data_out.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>

#include <fstream>
#include <iostream>

#include "currents_and_heating.h" // for friend class declaration
#include "mesh_preparer.h" // for BoundaryId-s.. probably should think of a better place for them

// forward declaration for friend class
namespace currents_heating {template<int dim> class CurrentsAndHeating;}

namespace laplace {
	using namespace dealii;

	template<int dim>
	class Laplace {
	public:
		Laplace();
		void run();

		Triangulation<dim>* getp_triangulation();
		DoFHandler<dim>* getp_dof_handler();


		void set_applied_field(const double applied_field_);

		void import_mesh_from_file(const std::string file_name, const std::string out_name = "");
		bool import_mesh_directly(std::vector<Point<dim> > vertices, std::vector<CellData<dim> > cells);

		double probe_field(const Point<dim> &p) const;

		std::vector<double> get_potential(const std::vector<int> &cell_indexes,
										  const std::vector<int> &vert_indexes);

		std::vector<Tensor<1, dim>> get_field(const std::vector<int> &cell_indexes,
									  	  	  const std::vector<int> &vert_indexes);

		void setup_system();
		void assemble_system();
		void solve();
		void output_results(const std::string filename = "field_solution.vtk") const;

	private:
		static constexpr unsigned int shape_degree = 1;
		static constexpr unsigned int quadrature_degree = shape_degree + 1;

		static constexpr double applied_field_default = 2.0;
		double applied_field;

		Triangulation<dim> triangulation;
		FE_Q<dim> fe;
		DoFHandler<dim> dof_handler;

		SparsityPattern sparsity_pattern;
		SparseMatrix<double> system_matrix;

		Vector<double> solution;
		Vector<double> system_rhs;


		friend class currents_heating::CurrentsAndHeating<dim>;
	};

} // namespace laplace

#endif /* INCLUDE_LAPLACE_H_ */

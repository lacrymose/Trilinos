#ifndef AMESOS2_SOLVER_CPP
#define AMESOS2_SOLVER_CPP

#include "Amesos2_Solver_decl.hpp"

#ifdef HAVE_AMESOS2_EXPLICIT_INSTANTIATION
#  include "Amesos2_Solver_def.hpp"
#  include "Teuchos_ExplicitInstantiationHelpers.hpp"
namespace Amesos {
/* Need to figure out the explicit instantiation system for Amesos2, since we
 * are instantiating not only on the Scalar types, but Matrices and Vectors
 */

// TEUCHOS_CLASS_TEMPLATE_INSTANT_SCALAR_TYPES( )

}
#endif  // HAVE_AMESOS2_EXPLICIT_INSTANTIATION

#endif  // AMESOS2_SOLVER_CPP

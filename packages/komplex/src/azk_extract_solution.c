/*******************************************************************************
 * Copyright 1995, Sandia Corporation.  The United States Government retains a *
 * nonexclusive license in this software as prescribed in AL 88-1 and AL 91-7. *
 * Export of this program may require a license from the United States         *
 * Government.                                                                 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "az_aztec.h"
#include "azk_komplex.h"

/*! \file
\brief Extraction routine for getting the solution of a Komplex system.

KOMPLEX is an add-on module to AZTEC that allows users to solve
complex-valued
linear systems.

KOMPLEX solves a complex-valued linear system Ax = b by solving
an equivalent real-valued system of twice the dimension.
Specifically,
writing in terms of real and imaginary parts, we have

 \f[ (A_r + i*A_i)*(x_r + i*x_i) = (b_r + i*b_i) \f]

  or by separating into real and imaginary equations we have

\f[
  \left( \begin{array}{rr}
                                    A_r & -A_i\\
                                    A_i &  A_r
                             \end{array}
   \right)
   \left( \begin{array}{r}
                                    x_r\\
                                    x_i
                             \end{array}
   \right)
   =
   \left( \begin{array}{r}
                                    b_r\\
                                    b_i
                             \end{array}
   \right)
\f]
  which is a real-valued system of twice the size.  If we find xr and
xi, we
  can form the solution to the original system as x = xr +i*xi.


KOMPLEX accept user linear systems in three forms with either global
or local index values.

1) The first form is true complex.  The user passes in an MSR or VBR
format matrix where the values are stored like Fortran complex
numbers.
Thus, the values array is of type double that is twice as long as the
number of complex values.  Each complex entry is stored with real part
followed by imaginary part (as in Fortran).

2) The second form stores real and imaginary parts separately, but the
pattern for each is identical.  Thus only the values of the imaginary
part are passed to the creation routines.

3) The third form accepts two real-valued matrices with no assumption
about the structure of the matrices.  Each matrix is multiplied by a
user-supplied complex constant.  This is the most general form.

Each of the above forms supports a global or local index set.  By this
we mean that the index values (stored in bindx) refer to the global
problem indices, or the local indices (for example after calling
AZ_transform).

*/

/*! \fn void AZK_extract_solution_k2c(int *options, 
                    double *params, int *proc_config,
			     AZ_MATRIX *Amat_komplex, AZ_PRECOND *Prec, double *vk, 
			     double *vc)

\brief Extract a Complex vector from a Komplex vector.

   Transforms a komplex vector to a complex vector.

\param options (In)
       Determines specific solution method and other parameters.
\param params (In)
       Drop tolerance and convergence tolerance info.
\param proc_config (In)
       Machine configuration.  proc_config[AZ_node] is the node
       number.  proc_config[AZ_N_procs] is the number of processors.
\param Amat_komplex (In)
       Komplex version of matrix stored as an AZ_MATRIX structure.
\param Prec (In)
       Preconditioner for Amat stored as an AZ_PRECOND structure.
\param vk (In)
       Komplex version of vector.

\param vc (Out)
       Contains a complex vector with the
       real/imag parts interleaved as in Fortran complex format.  Note
       that the user must allocate sufficient storage for results.

*/

void AZK_extract_solution_k2c(int *options, double *params, int *proc_config,
			     AZ_MATRIX *Amat_komplex, AZ_PRECOND *Prec, double *vk, 
			     double *vc)
{
  AZ_KOMPLEX *linsys_pass_data;
  int *rpntr, *data_org, *update_index;

  linsys_pass_data = (AZ_KOMPLEX *) Amat_komplex->aux_ptr;

  update_index = linsys_pass_data->update_index;
  data_org = Amat_komplex->data_org;
  rpntr = Amat_komplex->rpntr;

  AZ_invorder_vec(vk, data_org, update_index, rpntr, vc);

  return;
}
/*! \fn void AZK_extract_solution_k2g(int *options, 
                    double *params, int *proc_config,
                    AZ_MATRIX *Amat_komplex, AZ_PRECOND *Prec, double
*vk,
                    double *vr, double *vi )
   
\brief Extract real/imaginary parts of a complex vector from a Komplex
vector.

   Transforms a komplex vector to real and imaginary parts.

\param options (In)
       Determines specific solution method and other parameters.
\param params (In)
       Drop tolerance and convergence tolerance info. 
\param proc_config (In)
       Machine configuration.  proc_config[AZ_node] is the node
       number.  proc_config[AZ_N_procs] is the number of processors.
\param Amat_komplex (In)
       Komplex version of matrix stored as an AZ_MATRIX structure.
\param Prec (In)
       Preconditioner for Amat stored as an AZ_PRECOND structure.
\param vk (In)
       Komplex version of vector.

\param vc (Out) 
       Contains a complex vector with the
       real/imag parts interleaved as in Fortran complex format.  Note
       that the user must allocate sufficient storage for results.

*/  
    
void AZK_extract_solution_k2g(int *options, double *params, int *proc_config,
                    AZ_MATRIX *Amat_komplex, AZ_PRECOND *Prec, double *vk,
                    double *vr, double *vi )
{
  /* k2g returns the same vectors as k2ri (for now) so just call k2ri */
  AZK_extract_solution_k2ri(options, params, proc_config, Amat_komplex,
                           Prec, vk, vr, vi);
  return;
}
/*! \fn void AZK_extract_solution_k2ri(int *options, 
                    double *params, int *proc_config,
			     AZ_MATRIX *Amat_komplex, AZ_PRECOND *Prec, double *vk, 
			     double *vr, double *vi )

\brief Extract real/imaginary parts of a complex vector from a Komplex vector.

   Transforms a komplex vector to real and imaginary parts.

\param options (In)
       Determines specific solution method and other parameters.
\param params (In)
       Drop tolerance and convergence tolerance info.
\param proc_config (In)
       Machine configuration.  proc_config[AZ_node] is the node
       number.  proc_config[AZ_N_procs] is the number of processors.
\param Amat_komplex (In)
       Komplex version of matrix stored as an AZ_MATRIX structure.
\param Prec (In)
       Preconditioner for Amat stored as an AZ_PRECOND structure.
\param vk (In)
       Komplex version of vector.

\param vc (Out)
       Contains a complex vector with the
       real/imag parts interleaved as in Fortran complex format.  Note 
       that the user must allocate sufficient storage for results.

*/

void AZK_extract_solution_k2ri(int *options, double *params, int *proc_config,
			     AZ_MATRIX *Amat_komplex, AZ_PRECOND *Prec, double *vk, 
			     double *vr, double *vi )
{
  AZ_KOMPLEX *linsys_pass_data;
  int *rpntr, *data_org, *update_index;
  int i, N_equations, N_real;
  double *tmp;

  linsys_pass_data = (AZ_KOMPLEX *) Amat_komplex->aux_ptr;
  data_org = Amat_komplex->data_org;
  N_equations = data_org[AZ_N_internal] + data_org[AZ_N_border];
  N_real = N_equations/2;

  if (linsys_pass_data->From_Global_Indices)
    {
      update_index = linsys_pass_data->update_index;
      rpntr = Amat_komplex->rpntr;
            
      tmp = (double *) AZ_allocate(N_equations*sizeof(double));
      if (tmp == NULL) 
      AZ_perror("AZK_extract_solution_k2ri: Out of memory.");
      
      AZ_invorder_vec(vk, data_org, update_index, rpntr, tmp);
      
      /* Recover solution */
      
      for (i=0; i <N_real; i++)
	{
	  vr[i] = tmp[2*i  ];
	  vi[i] = tmp[2*i+1];
	}
      AZ_free ((void *) tmp);
    }
  else
      /* Recover solution */
      
      for (i=0; i <N_real; i++)
	{
	  vr[i] = vk[2*i  ];
	  vi[i] = vk[2*i+1];
	}
    
  return;
}

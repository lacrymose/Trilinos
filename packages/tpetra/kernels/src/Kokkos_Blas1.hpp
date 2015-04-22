/*
//@HEADER
// ************************************************************************
//
//          Kokkos: Node API and Parallel Node Kernels
//              Copyright (2008) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef KOKKOS_BLAS1_HPP_
#define KOKKOS_BLAS1_HPP_

#include <Kokkos_Blas1_impl.hpp>
#include <type_traits> // requires C++11

namespace KokkosBlas {

/// \brief Return the dot product of the two vectors x and y.
///
/// \tparam XVector Type of the first vector x; a 1-D Kokkos::View.
/// \tparam YVector Type of the second vector y; a 1-D Kokkos::View.
///
/// \param x [in] Input 1-D View.
/// \param y [in] Input 1-D View.
///
/// \return The dot product result; a single value.
template<class XVector,class YVector>
typename Kokkos::Details::InnerProductSpaceTraits<typename XVector::non_const_value_type>::dot_type
dot (const XVector& x, const YVector& y)
{
  static_assert ((int) XVector::rank == (int) YVector::rank,
                 "KokkosBlas::dot: Vector ranks do not match.");
  static_assert (XVector::rank == 1, "KokkosBlas::dot: "
                 "Both Vector inputs must have rank 1.");

  // Check compatibility of dimensions at run time.
  if (x.dimension_0 () != y.dimension_0 ()) {
    std::ostringstream os;
    os << "KokkosBlas::dot: Dimensions do not match: "
       << ", x: " << x.dimension_0 () << " x 1"
       << ", y: " << y.dimension_0 () << " x 1";
    Kokkos::Impl::throw_runtime_exception (os.str ());
  }

  typedef Kokkos::View<typename XVector::const_value_type*,
    typename XVector::array_layout,
    typename XVector::device_type,
    Kokkos::MemoryTraits<Kokkos::Unmanaged>,
    typename XVector::specialize> XVector_Internal;
  typedef Kokkos::View<typename YVector::const_value_type*,
    typename YVector::array_layout,
    typename YVector::device_type,
    Kokkos::MemoryTraits<Kokkos::Unmanaged>,
    typename YVector::specialize> YVector_Internal;
  XVector_Internal x_i = x;
  YVector_Internal y_i = y;

  return Impl::Dot<typename XVector_Internal::value_type*,
                   typename XVector_Internal::array_layout,
                   typename XVector_Internal::device_type,
                   typename XVector_Internal::memory_traits,
                   typename XVector_Internal::specialize,
                   typename YVector_Internal::value_type*,
                   typename YVector_Internal::array_layout,
                   typename YVector_Internal::device_type,
                   typename YVector_Internal::memory_traits,
                   typename YVector_Internal::specialize
                   >::dot (x_i, y_i);
}

} // namespace KokkosBlas

#endif // KOKKOS_BLAS1_HPP_

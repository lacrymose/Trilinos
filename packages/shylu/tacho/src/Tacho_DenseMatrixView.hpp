#ifndef __TACHO_DENSE_MATRIX_VIEW_HPP__
#define __TACHO_DENSE_MATRIX_VIEW_HPP__

/// \file Tacho_DenseMatrixView.hpp
/// \brief dense matrix view object.
/// \author Kyungjoo Kim (kyukim@sandia.gov)

#include "Tacho_Util.hpp"

namespace Tacho { 

  /// \class DenseMatrixView
  /// \breif Dense matrix view object set a 2D view that defines a computing region.
  template<typename DenseMatBaseType>
  class DenseMatrixView {
  public:
    typedef DenseMatBaseType mat_base_type;
    
    typedef typename mat_base_type::value_type   value_type;
    typedef typename mat_base_type::ordinal_type ordinal_type;
    typedef typename mat_base_type::size_type    size_type;
    typedef typename mat_base_type::space_type   space_type;
    
    template<typename>
    friend class DenseMatrixView;

  private:
    mat_base_type _base;     // shallow copy of the base matrix

    ordinal_type  _offm;     // offset in rows
    ordinal_type  _offn;     // offset in cols

    ordinal_type  _m;        // # of rows
    ordinal_type  _n;        // # of cols

  public:


    /// Interface functions
    /// ------------------------------------------------------------------
    /// Properties:
    /// - Compile with Device (o),
    /// - Callable in KokkosFunctors (o)

    KOKKOS_INLINE_FUNCTION
    void setView(const DenseMatBaseType &base,
                 const ordinal_type offm, const ordinal_type m,
                 const ordinal_type offn, const ordinal_type n) {
      _base = base;
      
      _offm = offm; _m = m;
      _offn = offn; _n = n;
    }

    KOKKOS_INLINE_FUNCTION
    const DenseMatBaseType& BaseObject() const { return _base; }

    KOKKOS_INLINE_FUNCTION
    ordinal_type  OffsetRows() const { return _offm; }

    KOKKOS_INLINE_FUNCTION
    ordinal_type  OffsetCols() const { return _offn; }

    KOKKOS_INLINE_FUNCTION    
    ordinal_type  NumRows() const { return _m; }

    KOKKOS_INLINE_FUNCTION
    ordinal_type  NumCols() const { return _n; }

    KOKKOS_INLINE_FUNCTION
    value_type& Value(const ordinal_type i,
                      const ordinal_type j) { return _base.Value(_offm+i, _offn+j); }
    
    KOKKOS_INLINE_FUNCTION
    value_type Value(const ordinal_type i,
                     const ordinal_type j) const { return _base.Value(_offm+i, _offn+j); }
    
    KOKKOS_INLINE_FUNCTION
    value_type* ValuePtr() const { return &_base.Value(_offm, _offn); }

    /// ------------------------------------------------------------------

    /// Constructors
    /// ------------------------------------------------------------------
    /// Properties:
    /// - Compile with Device (o),
    /// - Callable in KokkosFunctors
    ///   - Default and copy constructors are allowed in KokkosFunctors.
    ///   - Creating internal workspace is not allowed in KokkosFunctors.

    /// \brief Default constructor.
    KOKKOS_INLINE_FUNCTION
    DenseMatrixView()
      : _base(),
        _offm(0),
        _offn(0),
        _m(0),
        _n(0)
    { } 

    /// \brief Copy constructor (shallow copy)
    KOKKOS_INLINE_FUNCTION
    DenseMatrixView(const DenseMatrixView &b)
      : _base(b._base),
        _offm(b._offm),
        _offn(b._offn),
        _m(b._m),
        _n(b._n)
    { } 

    /// \brief Wrapping the base object 
    DenseMatrixView(const DenseMatBaseType &b)
      : _base(b),
        _offm(0),
        _offn(0),
        _m(b.NumRows()),
        _n(b.NumCols())
    { } 

    /// \brief Wrapping the base object with view 
    DenseMatrixView(const DenseMatBaseType &b,
                    const ordinal_type offm, const ordinal_type m,
                    const ordinal_type offn, const ordinal_type n) 
      : _base(b),
        _offm(offm),
        _offn(offn),
        _m(m),
        _n(n) 
    { } 

    /// Print out
    /// ------------------------------------------------------------------
    /// Properties:
    /// - Compile with Device (x),
    /// - Callable in KokkosFunctors (x)

    /// \brief print out to stream
    std::ostream& showMe(std::ostream &os) const {
      const int w = 4;
      if (_base.isValueArrayNull())
        os << "-- Base object is null --";
      else
        os << _base.Label() << "::View, "
           << " Offs ( " << std::setw(w) << _offm << ", " << std::setw(w) << _offn << " ); "
           << " Dims ( " << std::setw(w) << _m    << ", " << std::setw(w) << _n    << " ); ";

      return os;
    }
    
    std::ostream& showMeDetail(std::ostream &os) const {
      showMe(os) << std::endl;
      
      std::streamsize prec = os.precision();
      os.precision(8);
      os << std::scientific;
      
      const int w = 10;
      if (_base.isValueArrayNull()) {
        for (ordinal_type i=0;i<NumRows();++i) {
          for (ordinal_type j=0;j<NumCols();++j) {
            const value_type val = this->Value(i,j);
            os << std::setw(w) << val << "  ";
          }
          os << std::endl;
        }
      }

      os.unsetf(std::ios::scientific);
      os.precision(prec);
      
      return os;
    }

    /// \brief stream operator over-riding.
    template<typename T>
    friend std::ostream& std::operator<<(std::ostream &os, const DenseMatrixView<T> &self) {
      return self.showMe(os);
    }

  };
}

//----------------------------------------------------------------------------

#endif

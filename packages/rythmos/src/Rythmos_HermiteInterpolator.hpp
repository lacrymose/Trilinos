//@HEADER
// ***********************************************************************
//
//                           Rythmos Package
//                 Copyright (2006) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Todd S. Coffey (tscoffe@sandia.gov)
//
// ***********************************************************************
//@HEADER

#ifndef Rythmos_HERMITE_INTERPOLATOR_H
#define Rythmos_HERMITE_INTERPOLATOR_H

#include "Rythmos_InterpolatorBase.hpp"
#include "Thyra_VectorStdOps.hpp"

namespace Rythmos {
/** 
  This class implements piecewise Hermite interpolation on each interval where the data is:
  (t0,x(t0)), (t1,x(t1)), (t0,x'(t0)), (t1,x'(t1))
  The Hermite Interpolation polynomial is:
  H_3(t) = x[z0] + x[z0,z1](t-t0) + x[z0,z1,z2](t-t0)^2 + x[z0,z1,z2,z3](t-t0)^2(t-t1)
  where z0 = z1 = t0 and z2 = z3 = t1 and
  x[z0,z1] = x'(t0) and x[z2,z3] = x'(t1)
  This reduces to:
  H_3(t) = x(t0) + x'(t0)(t-t0) + ((x(t1)-x(t0))/(t1-t0) - x'(t0))(t-t0)^2/(t1-t0)
           +(x'(t1) - 2(x(t1)-x(t0))/(t1-t0) + x'(t0))(t-t0)^2(t-t1)/(t1-t0)^2
  With derivative:
  H_3'(t) =        x'(t0) + 2*((x(t1)-x(t0))/(t1-t0) - x'(t0))(t-t0)/(t1-t0)
           +(x'(t1) - 2(x(t1)-x(t0))/(t1-t0) + x'(t0))[2*(t-t0)(t-t1) + (t-t0)^2]/(t1-t0)^2
  With the error expression:
  x(t) - H_3(t) = (x^{(3)}(\xi(t))/(4!))(t-t0)^2(t-t1)^2
  The Hermite Interpolant will match 3rd degree polynomials exactly with both function values and derivatives.
  **/
template<class Scalar>
class HermiteInterpolator : virtual public InterpolatorBase<Scalar>
{
  public:

    /// Destructor
    ~HermiteInterpolator() {};

    /// Constructor
    HermiteInterpolator();

    void setNodes(
      const RCP<const typename DataStore<Scalar>::DataStoreVector_t> & nodes
      );

    /// Interpolation:
    /** \brief Hermite interpolation function.
     *
     * <b>Preconditions:</b><ul>
     * <li>Preconditions of InterpolatorBase<Scalar> apply 
     * <li><tt>(*nodes_)[i].xdot != Teuchos::null</tt> for all <tt>i=0..(*nodes_).size()-1</tt>
     * </ul>
     */
    void interpolate(
      const Array<Scalar> &t_values,
      typename DataStore<Scalar>::DataStoreVector_t *data_out
      ) const;

    /// Order of interpolation:
    int order() const; 

    /// Inherited from Teuchos::Describable
    /** \brief . */
    std::string description() const;

    /** \brief . */
    void describe(
      Teuchos::FancyOStream &out,
      const Teuchos::EVerbosityLevel verbLevel
      ) const;

    /// Redefined from ParameterListAcceptor
    /** \brief . */
    void setParameterList(RCP<ParameterList> const& paramList);

    /** \brief . */
    RCP<ParameterList> getNonconstParameterList();

    /** \brief . */
    RCP<ParameterList> unsetParameterList();

    void assertInterpolatePreconditions(
        const typename DataStore<Scalar>::DataStoreVector_t &data_in
        ,const Array<Scalar> &t_values
        ,typename DataStore<Scalar>::DataStoreVector_t *data_out
        ) const;

  private:

    RCP<const typename DataStore<Scalar>::DataStoreVector_t> nodes_;

    RCP<ParameterList> parameterList;

};

template<class Scalar>
HermiteInterpolator<Scalar>::HermiteInterpolator()
{
}

template<class Scalar>
void HermiteInterpolator<Scalar>::setNodes(
  const RCP<const typename DataStore<Scalar>::DataStoreVector_t> & nodes
  )
{
  nodes_ = nodes;
}

template<class Scalar>
void HermiteInterpolator<Scalar>::interpolate(
    const Array<Scalar> &t_values
    ,typename DataStore<Scalar>::DataStoreVector_t *data_out ) const
{

  //TEST_FOR_EXCEPT_MSG(true, "Error, ths function is not tested!" );

  typedef Teuchos::ScalarTraits<Scalar> ST;
#ifdef TEUCHOS_DEBUG
  assertInterpolatePreconditions((*nodes_),t_values,data_out);
#endif // TEUCHOS_DEBUG
  RCP<Teuchos::FancyOStream> out = this->getOStream();
  Teuchos::OSTab ostab(out,1,"HI::interpolator");
  if ( Teuchos::as<int>(this->getVerbLevel()) >= Teuchos::as<int>(Teuchos::VERB_HIGH) ) {
    *out << "(*nodes_):" << std::endl;
    for (unsigned int i=0 ; i<(*nodes_).size() ; ++i) {
      *out << "(*nodes_)[" << i << "] = " << std::endl;
      (*nodes_)[i].describe(*out,Teuchos::VERB_EXTREME);
    }
    *out << "t_values = " << std::endl;
    for (unsigned int i=0 ; i<t_values.size() ; ++i) {
      *out << "t_values[" << i << "] = " << t_values[i] << std::endl;
    }
    for (unsigned int i=0; i<data_out->size() ; ++i) {
      *out << "data_out[" << i << "] = " << std::endl;
      (*data_out)[i].describe(*out,Teuchos::VERB_EXTREME);
    }
  }
  data_out->clear();
  if (t_values.size() == 0) {
    return;
  }
  
  if ((*nodes_).size() == 1) {
    // trivial case of one node
    // preconditions assert that t_values[0] == (*nodes_)[0].time so we can just pass it out
    DataStore<Scalar> DS((*nodes_)[0]);
    data_out->push_back(DS);
  } else {
    // (*nodes_).size() >= 2
    int n = 0;
    for (int i=0 ; i<Teuchos::as<int>((*nodes_).size())-1 ; ++i) {
      const Scalar& t0 = (*nodes_)[i].time;
      const Scalar& t1 = (*nodes_)[i+1].time;
      while ((t0 <= t_values[n]) && (t_values[n] <= t1)) {
        const Scalar& t = t_values[n];
        // First we check for exact node matches:
        if (t == t0) {
          DataStore<Scalar> DS((*nodes_)[i]);
          data_out->push_back(DS);
        } else if (t == t1) {
          DataStore<Scalar> DS((*nodes_)[i+1]);
          data_out->push_back(DS);
        } else {
          RCP<const Thyra::VectorBase<Scalar> > x0    = (*nodes_)[i  ].x;
          RCP<const Thyra::VectorBase<Scalar> > x1    = (*nodes_)[i+1].x;
          RCP<const Thyra::VectorBase<Scalar> > xdot0 = (*nodes_)[i  ].xdot;
          RCP<const Thyra::VectorBase<Scalar> > xdot1 = (*nodes_)[i+1].xdot;
          
          // 10/10/06 tscoffe:  this could be expensive:
          RCP<Thyra::VectorBase<Scalar> > tmp_vec = x0->clone_v(); 
          RCP<Thyra::VectorBase<Scalar> > xdot_temp = x1->clone_v(); 
          Scalar dt = t1-t0;
          Scalar dt2 = dt*dt;
          Scalar t_t0 = t - t0;
          Scalar t_t1 = t - t1;
          Scalar tmp_t;

          // Compute numerical divided difference:
          Thyra::Vt_S(&*xdot_temp,Scalar(ST::one()/dt));
          Thyra::Vp_StV(&*xdot_temp,Scalar(-ST::one()/dt),*x0);

          // interpolate this point
          DataStore<Scalar> DS;
          DS.time = t;

          //  H_3(t) = x(t0) + xdot(t0)(t-t0) + ((x(t1)-x(t0))/(t1-t0) - xdot(t0))(t-t0)^2/(t1-t0)
          //           +(xdot(t1) - 2(x(t1)-x(t0))/(t1-t0) + xdot(t0))(t-t0)^2(t-t1)/(t1-t0)^2
          RCP<Thyra::VectorBase<Scalar> > x_vec = x0->clone_v(); 
          Thyra::Vp_StV(&*x_vec,t_t0,*xdot0);
          tmp_t = t_t0*t_t0/dt;
          Thyra::V_StVpStV(&*tmp_vec,tmp_t,*xdot_temp,Scalar(-ST::one()*tmp_t),*xdot0);
          Thyra::Vp_V(&*x_vec,*tmp_vec);
          tmp_t = t_t0*t_t0*t_t1/dt2;
          Thyra::V_StVpStV(&*tmp_vec,tmp_t,*xdot1,Scalar(-2*tmp_t),*xdot_temp);
          Thyra::Vp_StV(&*tmp_vec,tmp_t,*xdot0);
          Thyra::Vp_V(&*x_vec,*tmp_vec);
          DS.x = x_vec;

          //  H_3'(t) =        xdot(t0) + 2*((x(t1)-x(t0))/(t1-t0) - xdot(t0))(t-t0)/(t1-t0)
          //           +(xdot(t1) - 2(x(t1)-x(t0))/(t1-t0) + xdot(t0))[2*(t-t0)(t-t1) + (t-t0)^2]/(t1-t0)^2
          RCP<Thyra::VectorBase<Scalar> > xdot_vec = xdot0->clone_v(); 
          tmp_t = t_t0/dt;
          Thyra::Vp_StV(&*xdot_vec,Scalar(2*tmp_t),*xdot_temp);
          Thyra::Vp_StV(&*xdot_vec,Scalar(-2*tmp_t),*xdot0);
          tmp_t = Scalar((2*t_t0*t_t1+t_t0*t_t0)/dt2);
          Thyra::V_StVpStV(&*tmp_vec,tmp_t,*xdot1,Scalar(-2*tmp_t),*xdot_temp);
          Thyra::Vp_StV(&*tmp_vec,tmp_t,*xdot0);
          Thyra::Vp_V(&*xdot_vec,*tmp_vec);
          DS.xdot = xdot_vec;
          
          // Accuracy:
          // f(x) - H_3(x) = (f^{(3)}(\xi(x))/(4!))(x-x0)^2(x-x1)^2
          DS.accuracy = (t_t0)*(t_t0)*(t_t1)*(t_t1);

          // Push DataStore object onto vector:
          data_out->push_back(DS);
        }
        n++;
        if (n == Teuchos::as<int>(t_values.size())) {
          return;
        }
      }
    }
  } // (*nodes_).size() == 1
}

// non-member constructor
template<class Scalar>
RCP<HermiteInterpolator<Scalar> > hermiteInterpolator()
{
  RCP<HermiteInterpolator<Scalar> > hi = rcp(new HermiteInterpolator<Scalar>() );
  return hi;
}

template<class Scalar>
int HermiteInterpolator<Scalar>::order() const
{
  return(3);
}

template<class Scalar>
std::string HermiteInterpolator<Scalar>::description() const
{
  std::string name = "Rythmos::HermiteInterpolator";
  return(name);
}

template<class Scalar>
void HermiteInterpolator<Scalar>::describe(
      Teuchos::FancyOStream       &out
      ,const Teuchos::EVerbosityLevel      verbLevel
      ) const
{
  if ( (Teuchos::as<int>(verbLevel) == Teuchos::as<int>(Teuchos::VERB_DEFAULT) ) ||
       (Teuchos::as<int>(verbLevel) >= Teuchos::as<int>(Teuchos::VERB_LOW)     )
     )
  {
    out << description() << "::describe" << std::endl;
  }
  else if (Teuchos::as<int>(verbLevel) >= Teuchos::as<int>(Teuchos::VERB_LOW))
  {}
  else if (Teuchos::as<int>(verbLevel) >= Teuchos::as<int>(Teuchos::VERB_MEDIUM))
  {}
  else if (Teuchos::as<int>(verbLevel) >= Teuchos::as<int>(Teuchos::VERB_HIGH))
  {}
}

template <class Scalar>
void HermiteInterpolator<Scalar>::setParameterList(RCP<ParameterList> const& paramList)
{
  parameterList = paramList;
  int outputLevel = parameterList->get( "outputLevel", int(-1) );
  outputLevel = std::min(std::max(outputLevel,-1),4);
  this->setVerbLevel(static_cast<Teuchos::EVerbosityLevel>(outputLevel));
}

template <class Scalar>
RCP<ParameterList> HermiteInterpolator<Scalar>::getNonconstParameterList()
{
  return(parameterList);
}

template <class Scalar>
RCP<ParameterList> HermiteInterpolator<Scalar>::unsetParameterList()
{
  RCP<ParameterList> temp_param_list = parameterList;
  parameterList = Teuchos::null;
  return(temp_param_list);
}

template<class Scalar>
void HermiteInterpolator<Scalar>::assertInterpolatePreconditions(
        const typename DataStore<Scalar>::DataStoreVector_t &data_in
        ,const Array<Scalar> &t_values
        ,typename DataStore<Scalar>::DataStoreVector_t *data_out
        ) const
{
  assertBaseInterpolatePreconditions(data_in,t_values,data_out);
  for (int i=0; i<Teuchos::as<int>(data_in.size()) ; ++i) {
    TEST_FOR_EXCEPTION(
        data_in[i].xdot == Teuchos::null, std::logic_error,
        "Error, data_in[" << i << "].xdot == Teuchos::null.\n"
        );
  }
}

} // namespace Rythmos

#endif // Rythmos_HERMITE_INTERPOLATOR_H

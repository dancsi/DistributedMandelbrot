#ifndef color_gray_trait_component_float
#define color_gray_trait_component_float

#include "../../category.hpp"

#include "../../../_internal/utility/component/array.hpp"

#include "../../../generic/trait/component.hpp"



namespace color
 {
  namespace trait
   {

    template< >
     struct component< ::color::category::gray_float >
      : public ::color::_internal::utility::component::array< float, unsigned >
      {
      };

    template< >
     struct component< ::color::category::gray_double >
      : public ::color::_internal::utility::component::array< double, unsigned >
      {
      };

    template< >
     struct component< ::color::category::gray_ldouble >
      : public ::color::_internal::utility::component::array< long double, unsigned >
      {
      };

   }
 }

#endif

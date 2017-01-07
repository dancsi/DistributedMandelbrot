#ifndef color_trait_index
#define color_trait_index

// ::color::trait::index< category >

#include "../../_internal/utility/type/index.hpp"

namespace color
 {
  namespace trait
   {

    template< typename category_name >
     struct index
      : public ::color::_internal::utility::type::index< unsigned >
      {
       public:

         typedef ::color::_internal::utility::type::index< unsigned > utility_type;

         typedef typename utility_type::instance_type              instance_type;

         typedef typename utility_type::const_type const_type;

         typedef typename utility_type::return_image_type       return_image_type;
         typedef typename utility_type::return_original_type    return_original_type;

         typedef typename utility_type::input_const_type      input_const_type;
         typedef typename utility_type::input_type            input_type;
         typedef typename utility_type::output_type           output_type;
      };

   }
 }

#endif

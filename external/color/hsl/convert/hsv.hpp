#ifndef color_hsl_convert_hsv
#define color_hsl_convert_hsv

#include "../../_internal/convert.hpp"
#include "../../hsv/hsv.hpp"

#include "../../_internal/normalize.hpp"
#include "../../_internal/diverse.hpp"

namespace color
 {
  namespace _internal
   {

    template< typename hsl_tag_name, typename hsv_tag_name >
     struct convert
      <
        ::color::category::hsl< hsl_tag_name >
       ,::color::category::hsv< hsv_tag_name >
      >
      {
       public:
         typedef ::color::category::hsl< hsl_tag_name > category_left_type;
         typedef ::color::category::hsv< hsv_tag_name > category_right_type;
         typedef double scalar_type;

         typedef ::color::trait::container<category_left_type>     container_left_trait_type;
         typedef ::color::trait::container<category_right_type>    container_right_trait_type;

         typedef typename container_left_trait_type::input_type         container_left_input_type;
         typedef typename container_right_trait_type::input_const_type  container_right_const_input_type;

         typedef ::color::_internal::diverse< category_left_type >    diverse_type;
         typedef ::color::_internal::normalize< category_right_type > normalize_type;

         static void process
          (
            container_left_input_type         left
           ,container_right_const_input_type  right
          )
          {
           scalar_type h  = normalize_type::template process<0>( container_right_trait_type::template get<0>( right ) );
           scalar_type ss = normalize_type::template process<1>( container_right_trait_type::template get<1>( right ) );
           scalar_type v  = normalize_type::template process<2>( container_right_trait_type::template get<2>( right ) );

           scalar_type l = v * ( 2-ss ) / 2;
           scalar_type s = 0 ;
           // TODO if( ( l < -1e6 ) || ( 0.9999 < l ) )
            {
             //h = 0;
             s = v*ss/( 1- fabs( 2 * l - 1 ) );
            }

           container_left_trait_type::template set<0>( left, diverse_type::template process<0>( h ) );
           container_left_trait_type::template set<1>( left, diverse_type::template process<1>( s ) );
           container_left_trait_type::template set<2>( left, diverse_type::template process<2>( l ) );
          }
      };

   }
 }

#endif

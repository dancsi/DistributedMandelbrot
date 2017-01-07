#ifndef color_gray_convert_gray
#define color_gray_convert_gray

#include "../place/place.hpp"
#include "../category.hpp"
#include "../trait/trait.hpp"

#include "../../_internal/convert.hpp"

#include "../../_internal/reformat.hpp"









namespace color
 {
  namespace _internal
   {

    template< typename tag_left_name, typename tag_right_name >
     struct convert
      <
        ::color::category::gray< tag_left_name >
       ,::color::category::gray< tag_right_name>
      >
      {
       public:
         typedef ::color::category::gray< tag_left_name > category_left_type;
         typedef ::color::category::gray< tag_right_name> category_right_type;
         typedef double scalar_type;

         typedef ::color::trait::container<category_left_type>     container_left_trait_type;
         typedef ::color::trait::container<category_right_type>    container_right_trait_type;

         typedef ::color::_internal::reformat< category_left_type, category_right_type, scalar_type >    reformat_type;

         typedef typename container_left_trait_type::input_type         container_left_input_type;
         typedef typename container_right_trait_type::input_const_type  container_right_const_input_type;

         static void process
          (
            container_left_input_type         left
           ,container_right_const_input_type  right
          )
          {
           enum
            {
              gl = ::color::place::_internal::gray<category_left_type>::position_enum
             ,gr = ::color::place::_internal::gray<category_right_type>::position_enum
            };

           container_left_trait_type::template set<gl>( left, reformat_type::template process<gl,gr>( container_right_trait_type::template get<gr>( right ) ) );
          }
       };

   }
 }

#endif

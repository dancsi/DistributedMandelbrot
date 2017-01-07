#ifndef color__internal_utility_component_pack_8
#define color__internal_utility_component_pack_8

// ::color::_internal::utility::component::pack8< index_type >

#include "./unsigned.hpp"


namespace color
 {
  namespace _internal
   {
    namespace utility
     {
      namespace component
       {

        template< typename index_name >
         struct pack8
          : public ::color::_internal::utility::component::Unsigned< std::uint8_t, index_name >
          {
          };

        template< typename index_name > using split2222_t = ::color::_internal::utility::component::pack8<index_name>;

        template< typename index_name > using split233_t = ::color::_internal::utility::component::pack8<index_name>;
        template< typename index_name > using split323_t = ::color::_internal::utility::component::pack8<index_name>;
        template< typename index_name > using split332_t = ::color::_internal::utility::component::pack8<index_name>;

        template< typename index_name > using split224_t = ::color::_internal::utility::component::pack8<index_name>;
        template< typename index_name > using split242_t = ::color::_internal::utility::component::pack8<index_name>;
        template< typename index_name > using split422_t = ::color::_internal::utility::component::pack8<index_name>;

       }
     }
   }
 }

#endif
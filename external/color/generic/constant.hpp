#ifndef color_contant_generic
#define color_contant_generic

// ::color::constant::generic< typename ::color::akin::generic< category_name >::akin_type > >::()

// ::color::constant::hue::formula_enum, // ::color::constant::hue::hexagon_entity,

// ::color::constant::hue::_internal::algorithm< ::color::constant::hue::formula_enum >

#include "./category.hpp"
#include "./trait/scalar.hpp"


namespace color
 {
  namespace constant
   {

    template< typename category_name >
     struct generic
      {
       public:
         typedef category_name  category_type;

         typedef typename ::color::trait::scalar< category_name >::instance_type scalar_type;

         static /* constexpr*/ scalar_type const zero()  { return scalar_type( 0   ); }
         static /* constexpr*/ scalar_type const half()  { return scalar_type( 0.5 ); }
         static /* constexpr*/ scalar_type const one()   { return scalar_type( 1   ); }
         static /* constexpr*/ scalar_type const two()   { return scalar_type( 2   ); }
         static /* constexpr*/ scalar_type const sqrt_3()   { return sqrt( scalar_type(3) ); }
         static /* constexpr*/ scalar_type const pi( )      { return scalar_type( 3.141592653589793238462643383279502884197169399375105820974944592307816406286  ); }
         static /* constexpr*/ scalar_type const pi_half( ) { return scalar_type( 3.141592653589793238462643383279502884197169399375105820974944592307816406286/2  ); }
         static /* constexpr*/ scalar_type const two_pi()   { return scalar_type( 2 * 3.141592653589793238462643383279502884197169399375105820974944592307816406286 ); }
      };

    namespace hue
     {

      enum formula_enum
       {
         error_entity
        ,hexagon_entity     // =  Preucil
        ,polar_atan2_entity // EHB(atan2) = Evans, Hanson, and Brewer
        ,polar_acos_entity  // EHB(acos)  = Evans, Hanson, and Brewer
        ,polar_entity = polar_atan2_entity  // Default polar formula
       };

      namespace _internal
       {

        template < enum ::color::constant::hue::formula_enum hue_number > struct algorithm{};

       }

     }

   }
 }

#endif

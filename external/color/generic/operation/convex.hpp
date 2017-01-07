#ifndef  color_generic_operation_convex_123
#define color_generic_operation_convex_123

#include "../../generic/model.hpp"
#include "../../generic/trait/scalar.hpp"

 namespace color
  {
   namespace operation
    {
     namespace _internal
      {

       template< typename category_name >
        struct convex
         {
          public:
            typedef category_name  category_type;

            typedef typename ::color::trait::scalar<category_type>::instance_type    scalar_type;

            typedef scalar_type  const&  scalar_const_input_type;

            typedef ::color::trait::index<category_type>         index_trait_type;
            typedef ::color::trait::container< category_type >   container_trait_type;
            typedef ::color::model<category_type>     model_type;

            typedef model_type &  model_input_type;
            typedef model_type const&  model_const_input_type;

            typedef typename index_trait_type::instance_type  index_type;

            static model_type & process( model_input_type result, scalar_const_input_type scalar, model_const_input_type right )
             {
              for( index_type index = 0; index < container_trait_type::size(); index ++ )
               {
                result.set( index, scalar * result.get( index ) +( scalar_type( 1 ) - scalar ) *right.get( index )  );
               }
              return result;
             }

            static model_type & process(  model_input_type  result, model_const_input_type left, scalar_const_input_type scalar, model_const_input_type right )
             {
              for( index_type index = 0; index < container_trait_type::size(); index ++ )
               {
                result.set( index, scalar * left.get( index ) +(scalar_type( 1 ) - scalar ) *right.get( index )  );
               }
              return result;
             }

         };
      }


     template< typename category_name, typename scalar_name >
      void convex
       (
         ::color::model<category_name>      & result
        ,scalar_name                            const& scalar
        ,color::model<category_name> const& right
       )
       {
        ::color::operation::_internal::convex<category_name>::process( result, scalar, right );
       }

     template< typename category_name, typename scalar_name >
      void convex
       (
         ::color::model<category_name>      & result
        ,color::model<category_name> const& left
        ,scalar_name                            const& scalar
        ,color::model<category_name> const& right
       )
       {
        ::color::operation::_internal::convex<category_name>::process( result, left, scalar, right );
       }

    }
  }

#endif

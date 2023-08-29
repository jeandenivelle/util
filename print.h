
#ifndef PRINT_
#define PRINT_  1

#include <iostream>

namespace std
{
   template< typename T >
   requires requires( const T t, std::ostream& out ) { { t. print( out ) }; }
   inline std::ostream& operator << ( std::ostream& out, const T& t )
   {
      t. print( out );
      return out;
   }
}

#endif


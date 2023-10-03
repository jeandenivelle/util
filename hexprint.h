
// Written by Hans de Nivelle, October 2023.

#ifndef UTIL_HEXPRINT_
#define UTIL_HEXPRINT_

#include <iostream>
#include <concepts>

namespace util
{

   void hexprint( std::ostream& out, std::integral auto i )
   {
      short unsigned shift = 8 * sizeof(i);
      while( shift )  
      {
         shift -= 4;

         auto dig = ( i >> shift ) & 0XF;

         if( dig >= 10 )
            out << (char)( 'A' + ( dig - 10 ));
         else
            out << (char)( '0' + dig );  
      } 
   }

}

#endif


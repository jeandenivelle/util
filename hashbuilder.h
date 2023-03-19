
// Written by Hans de Nivelle, December 2020, part of the Easy project.

#ifndef UTIL_HASHBUILDER_
#define UTIL_HASHBUILDER_   1

#include <iostream>

namespace util
{

   struct hashbuilder
   {
      size_t val;

      hashbuilder( ) 
         : val(0)
      { }

      hashbuilder( size_t s )
         : val(s)
      { }

      void clear( ) 
         { val = 0; }
   };


   inline hashbuilder& operator << ( hashbuilder& b, int i )
   { 
      b.val ^= b.val << 13;
      b.val ^= b.val >> 7;
      b.val ^= b.val << 17; 
      b.val ^= i;
      return b; 
   }


   inline hashbuilder& operator << ( hashbuilder& b, size_t s )
   { 
      b.val ^= b.val << 13;
      b.val ^= b.val >> 7;
      b.val ^= b.val << 17;  
      b.val ^= s;
      return b;  
   }

   inline hashbuilder& operator << ( hashbuilder& b, const char* p )
   {
      while(*p)
         b << * ( p ++ );
      return b; 
   }   

   inline std::ostream& operator << ( std::ostream& out, const hashbuilder& b )
      { return out << "hashbuilder( " << b. val << " )"; } 

}

#endif


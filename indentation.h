
// Written by Hans de Nivelle, September 2016.

#ifndef INDENTATION_
#define INDENTATION_  1

#include <iostream>

class indentation
{
   unsigned int s;

public: 
   indentation( ) : s(0) { } 
   explicit indentation( unsigned int s ) : s(s) { }

   indentation operator + ( unsigned int t ) const
      { return indentation{s+t}; }

   indentation operator - ( unsigned int t ) const
      { return indentation{s-t}; }

   indentation& operator ++ ( ) 
      { ++ s; return *this; }
   indentation& operator -- ( ) 
      { -- s; return *this; }

   indentation& operator += ( unsigned int i ) 
      { s += i; return *this; }
   indentation& operator -= ( unsigned int i )
      { s -= i; return *this; }
 
   indentation operator ++ ( int )
      { auto original = *this; ++ s; return original; }
   indentation operator -- ( int )
      { auto original = *this; -- s; return original; } 

   unsigned int size( ) const { return s; } 
      // Expected size when printed.

   void clear( ) { s = 0; } 
};


inline std::ostream& operator << ( std::ostream& out, indentation ind ) 
{
   for( unsigned int i = 0; i != ind. size( ); ++ i )
      out << ' '; 
   return out;
}
 
#endif


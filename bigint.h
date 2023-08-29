
// Written by Hans de Nivelle, July 2011.

#ifndef BIGINT_
#define BIGINT_  1

#include <iostream> 
#include <vector> 
#include <stdexcept>
#include <string>
#include <string_view>

// #include "util/dyn_vector.h"

class bigint
{
public: 
   using word = uint16_t;

public: 
   using container = std::vector< word > ; 
   using iterator = container :: iterator;
   using const_iterator = container :: const_iterator;

   container repr; 
      // Least significant word comes first.

   int8_t sign;
      // 1 denotes positive, -1 denotes negative. 

public: 
   // Compare two blocks of words of equal length. The iterators
   // p1, p2 must be set at the end of the blocks. 
   // (One position behind it.) 

   static short int compare( const_iterator p1, const_iterator p2,
                             size_t len );

   static short int compare( const container& v1, const container& v2 );
      // Is only correct when there are no non-significant
      // zeroes. 

   static short int compare( const bigint& i1, const bigint& i2 ); 


   // The following method is part of the multiplication algorithm.
   // It adds ( q1 .. q2 ) * factor to ( p1 .. ).
   // The result has to fit.

   static void add( iterator p1, 
                    const_iterator q1, const_iterator q2,
                    word factor );

   // The following method is part of the division algorithm.
   // We try to subtract ( q1 .. q2 ) * factor from ( p1 .. p2 );restword.
   // p2 is computed from the length q2 - q1. After the subtraction,
   // restword must be zero. 
   // It is possible that the subtraction does not fit, because
   // the estimated factor can be one too high. (More should not
   // happen, but some bad estimation algorithms produce 2 too high.) 
   // In that case, the factor is decreased by one until the subtraction
   // fits. We return the highest factor that fits. 
   // If the original estimation was good, this is the original
   // factor or 1 less.

   static word subtract( iterator p1, word restword, 
                         const_iterator q1, const_iterator q2, word factor );

   static void add( const_iterator p1, const_iterator p2, 
                    const_iterator q1, const_iterator q2,
                    iterator r1, iterator r2 );
      // r1 .. r2 must be large enough to hold the result.
      // Otherwise, very bad things will happen. 
      // It is guaranteed that all words in r1 .. r2 are written. 

   static void subtract( const_iterator p1, const_iterator p2,
                         const_iterator q1, const_iterator q2,
                         iterator r1, iterator r2 );
      // r1 .. r2 must be large enough to hold the result, and
      // the result must be positive. It is guaranteed that all words
      // in the range r1 .. r2 are written.

   static void addorsub( const bigint& b1, const bigint& b2, int s2,
                         bigint& r ); 
      // It is allowed that b1 is the same as r, or b2 is the same
      // as r. Dependent on the sign of b2 and s2, we add or subtract,
      // and put the result in res. 

   static void multiply( const container& v1,
                         const container& v2,
                         container& res );
      // This function will not work correctly when res is identical
      // to v1 or v2. res should be zero in advance, and it
      // should be big enough to contain the result, because
      // multiply does not check this. 

   void multiply_add( word m, word c );
      // Multiply ourselves by m and add c.
      

   // Divide (x1,x2,x3) / (y1,y2). This is used as estimation for the
   // real big division. x1 and y1 are the most significant word.
   // The result (x1,x2,x3)/(y1,y2) can be 0X10000, due to the fact that 
   // the estimated result can be too high. 

   // We tried different implementations, but it turned out that 
   // the simplest one, using floating point, is the most
   // efficient.

   static uint32_t divide_fp( word x1, word x2, word x3, word y1, word y2 )
   {
      return static_cast< uint32_t > (
             ( 65536.0 * 65536.0 * x1 + 65536.0 * x2 + x3 ) /
             ( 65536.0 * y1 + y2 ));
   }


   static uint32_t divide( word x1, word x2, word x3, word y1, word y2 );

   static container divide( const container& d, container& rest );
      // The most significant bit in d must not be zero.
      // Subtract d from rest as many times as possible.
      // How many times will be returned. 

   // Ensure that our container has length at least s:

   void ensure( size_t s )
   {
      repr. reserve(s);
      while( repr. size( ) < s )
         repr. push_back(0);
   }

   // Remove leading non-significant zeroes:

   void reduce( )
   {
      while( repr. size( ) && repr. back( ) == 0 )
         repr. pop_back( );
   }

   bigint( const container& v, uint8_t s )
      : repr(v),
        sign(s)
   { }

   bigint( container&& v, uint8_t s )
      : repr( std::move(v)),
        sign(s)
   { }

public: 
   bigint( ) 
      : sign(1)
   { }

   bigint( int16_t );
   bigint( int32_t );
   bigint( int64_t );

   bigint( uint16_t );
   bigint( uint32_t );
   bigint( uint64_t );

   bigint( std::string_view v, word base = 10 );

   bigint( const std::string& s, word base = 10 ) 
      : bigint( std::string_view(s), base )
   { }

   bigint( const char* c, word base = 10 )
      : bigint( std::string_view(c), base )
   { }

   bigint( const bigint& ) = default;
   bigint( bigint&& ) noexcept = default;
   bigint& operator = ( const bigint& ) = default;
   bigint& operator = ( bigint&& ) noexcept = default;
   ~bigint( ) = default;

   static bool testmachine( ); 
      // We test that a word is 16 bits and unsigned.
      // We test that an unsigned int is 32 bits and really unsigned. 
      // We test that uint64_t has 64 bits. 
      // We check that right shifts have no sign extension.

   // Checksum can be used for checking correctness of operations:

   uint32_t checksum( uint32_t prime ) const;

   static uint32_t
   checksum( const std::string_view v, uint32_t prime, word base = 10 );

   static uint32_t
   checksum( const std::string& s, uint32_t prime, word base = 10 )
   {
      return checksum( std::string_view(s), prime, base );
   }

   static uint32_t
   checksum( const char* c, uint32_t prime, word base = 10 )
   {   
      return checksum( std::string_view(c), prime, base );
   }

   void operator += ( const bigint& b ) { addorsub( *this, b, 1, *this ); } 
   void operator -= ( const bigint& b ) { addorsub( *this, b, -1, *this ); }

   bigint operator + ( const bigint& b ) const
   { 
     bigint res;
     addorsub( *this, b, 1, res );
     return res; 
   }

   bigint operator - ( const bigint& b ) const  
   { 
      bigint res;
      addorsub( *this, b, -1, res );
      return res; 
   }

   void operator *= ( const bigint& b )
   {
      bigint res;
      res. ensure( repr. size( ) + b. repr. size( ));
      multiply( repr, b. repr, res. repr );
      res. reduce( );
      res. sign = sign * b. sign; 
      *this = std::move( res ); 
   }

   bigint operator / ( const bigint& b ) const 
   {
      if( b. repr. size( ) == 0 )
         throw std::runtime_error( "cannot divide by zero" ); 

      bigint i = *this;
      bigint quot = bigint( divide( b. repr, i. repr ), sign * b. sign ); 
      quot. reduce( );
      return quot;
   } 

   void operator /= ( const bigint& b )
   {
      if( b. repr. size( ) == 0 )
         throw std::runtime_error( "cannot divide by zero" ); 

      bigint i = *this; 
      *this = bigint( divide( b. repr, i. repr ), sign * b. sign );
      reduce( ); 
   }

   bigint operator % ( const bigint& b ) const 
   {
      if( b. repr. size( ) == 0 )
         throw std::runtime_error( "cannot divide by zero" );

      bigint i = *this; 
      divide( b. repr, i. repr );
      i. reduce( );
      return bigint( i. repr, sign ); 
   }

   bigint operator - ( ) const
   {
      return bigint( repr, - sign );
   }

   int8_t getsign( ) const
   {
      return sign;
   }

   bool iszero( ) const { return repr. size( ) == 0; } 
      // We assume that we are reduced. 

   uint32_t getunsigned( ) const;

   int32_t getsigned( ) const;

   double approximation( ) const;
   
   size_t hash( ) const;
  
   void print( std::ostream& out ) const; 

   friend bigint operator * ( const bigint& , const bigint& );    
};


inline bigint operator * ( const bigint& b1, const bigint& b2 ) 
{
   bigint res;
   res. ensure( b1. repr. size( ) + b2. repr. size( ));
   bigint::multiply( b1. repr, b2. repr, res. repr );
   res. reduce( );
   res. sign = b1. sign * b2. sign;
   return res;
}


inline bool operator == ( const bigint& i1, const bigint& i2 )
{
   return bigint::compare(i1,i2) == 0;
}

inline bool operator != ( const bigint& i1, const bigint& i2 )
{
   return bigint::compare(i1,i2) != 0;
}

inline bool operator < ( const bigint& i1, const bigint& i2 )
{
   return bigint::compare(i1,i2) == -1;
}

inline bool operator > ( const bigint& i1, const bigint& i2 )
{
   return bigint::compare(i1,i2) == 1;
}

inline bool operator <= ( const bigint& i1, const bigint& i2 )
{
   return bigint::compare(i1,i2) != 1;
}

inline bool operator >= ( const bigint& i1, const bigint& i2 )
{
   return bigint::compare(i1,i2) != -1;
}

std::ostream& operator << ( std::ostream& out, bigint b );
   // We really need a copy of b, because it will be converted to 
   // decimal, which involves calculation with b.

#endif




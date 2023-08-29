
// Written by Hans de Nivelle, June 2011.

#include "bigint.h"

namespace
{

   uint16_t convxdigit( char ch ) 
   {
      if( isdigit(ch) ) 
         return ch - '0';
      
      if( ch >= 'A' && ch <= 'F' )
         return ch - 'A' + 10;

      if( ch >= 'a' && ch <= 'f' )
         return ch - 'a' + 10;
   
      return 0; 
   }
}

bigint::bigint( int16_t x )
{
   uint16_t u;
   if( x < 0 )
   {
      sign = -1;
      u = -x;      // It fits because y is unsigned.
   }
   else
   {
      sign = 1;
      u = x;
   }

   while(u)
   {
      repr. push_back( u & 0XFFFF );
      u >>= 16;
   }
}


bigint::bigint( int32_t x )
{
   uint32_t u;
   if( x < 0 )
   {
      sign = -1;
      u = -x;      // It fits because y is unsigned.
   }
   else
   {
      sign = 1;
      u = x;
   }

   while(u)
   {
      repr. push_back( u & 0XFFFF );
      u >>= 16;
   }
}


bigint::bigint( int64_t x )
{
   uint64_t u;
   if( x < 0 )
   {
      sign = -1;
      u = -x;      // It fits because y is unsigned. 
   }
   else
   {
      sign = 1;
      u = x;
   }

   while(u)
   {
      repr. push_back( u & 0XFFFF );
      u >>= 16;
   }
} 


bigint::bigint( uint16_t u )
   : sign(1) 
{
   while(u)
   {
      repr. push_back( u & 0XFFFF );
      u >>= 16;
   }
}


bigint::bigint( uint32_t u )
   : sign(1) 
{
   while(u)
   {
      repr. push_back( u & 0XFFFF );
      u >>= 16;
   }
}


bigint::bigint( uint64_t u )
   : sign(1)
{
   while(u)
   {
      repr. push_back( u & 0XFFFF );
      u >>= 16;
   }
}


bigint::bigint( std::string_view s, word base ) 
   : sign(1)
{
   auto it = s. begin( ); 
   while( it != s. end( ) && isspace( *it ))
      ++ it;

   if( it == s. end( ))
      throw std::invalid_argument( "cannot construct bigint, no digit found" ); 

   if ( *it == '-' )
   {
      sign = -1;
      ++ it; 
   }
   else
   {
      if( *it == '+' )
         ++ it;
   }

   while( it != s. end( ) && isspace( *it ))
      ++ it;

   if( it == s. end( ))
      throw std::invalid_argument( "cannot construct bigint, no digit found" );

   // We treat 16 separately, because it does not need conversion. 

   if( base == 16 )
   {
      auto itrev = s. end( ); 

      uint16_t shift = 0;  
      uint16_t w = 0;  // Our current word. 

      while( itrev != it )
      {
         char ch = *( -- itrev );

         if( !isspace( ch ))
         {
            if( !isxdigit( ch )) 
               throw std::invalid_argument( 
                              "cannot construct bigint, found non-hexdigit" );

            uint16_t d = convxdigit( ch );

            w |= ( d << shift ); 

            shift += 4; 
            if( shift == 16 ) 
            {
               repr.push_back(w);
               w = 0;
               shift = 0;
            }
         }
      }

      if(w)  
         repr.push_back(w);

      while( repr. size( ) && repr. back( ) == 0 )
         repr. pop_back( ); 
   }
   else 
   {
      if( base < 2 || base > 16 )
         throw std::runtime_error( "only bases between 2 and 16 can be handled" );
      
      while( it != s. end( ))
      {
         char c = *it ++ ;

         if( !isspace(c))
         {
            if( !isxdigit(c))
               throw std::invalid_argument( 
                              "cannot construct bigint, found non-digit" );

            multiply_add( base, convxdigit(c) ); 
         }
      }
   }

}



void bigint::add( iterator p1,
                  const_iterator q1, const_iterator q2,
                  word factor )
{
   uint32_t carry = 0;

   iterator p = p1;
   const_iterator q = q1;

   while( q != q2 )
   {
      carry += *p + *q * factor;
      *p = carry & 0XFFFF;

      carry >>= 16;
      
      ++ p; ++ q;
   }

   while( carry )
   {
      carry += *p;
      *p = carry & 0XFFFF;

      carry >>= 16;

      ++ p; 
   }
}


bigint::word bigint::subtract( iterator p1,
                               word restword, 
                               const_iterator q1, const_iterator q2,
                               word factor )
{
   iterator p = p1;
   const_iterator q = q1;

   uint32_t carry = 0;
      // Carry is zero or negative. In case there is something in carry,
      // the most significant part must be FFFF.

   while( q != q2 )
   {
      carry += *p;
      carry -= factor * *q;

      *p = carry & 0XFFFF;

      carry >>= 16;

      // We need sign extension in carry, because it represents a 
      // negative number. 

      if( carry )
         carry |= 0XFFFF0000;

      ++ p; ++ q;
   }

   carry += restword;  
   restword = carry & 0XFFFF; 

   carry >>= 16; 

   // If we have something left in carry, we decrease factor,
   // and add *q1 ... *q2 to *p1 ... *p2, until we are positive again. 

   if( carry )
   {
      // We are now negative, so we need to go another time through
      // zero to become positive again.

      carry = 0;
      while( carry == 0 )
      {
         p = p1;
         q = q1; 
         while( q != q2 )
         {
            carry += *p + *q;
            *p = carry & 0XFFFF;

            carry >>= 16;

            ++ p;
            ++ q;
         }

         carry += restword; 
         restword = carry & 0XFFFF;

         carry >>= 16;

         -- factor; 
      }
   }

   if( restword )
      throw std::runtime_error( "restword should have been zero" );

   return factor;
}


void bigint::add( const_iterator p1, const_iterator p2,
                  const_iterator q1, const_iterator q2,
                  iterator r1, iterator r2 )
{
   uint32_t carry = 0;

   const_iterator p = p1;
   const_iterator q = q1;
   iterator r = r1;

   while( p != p2 && q != q2 )
   {
      carry += *p + *q;
      *r = carry & 0XFFFF;

      carry >>= 16;

      ++ p; ++ q; ++ r; 
   }

   // The following is correct because either p != p2 or
   // q != q2. This guarantees that at most one of the while loops
   // will be entered. 

   while( p != p2 )
   {
      carry += *p;
      *r = carry;

      carry >>= 16;

      ++ p; ++ r; 
   }

   while( q != q2 )
   {
      carry += *q;
      *r = carry;

      carry >>= 16;

      ++ q; ++ r;
   }

   while( r != r2 )
   {   
      *r = carry; 
      carry >>= 16;
      ++ r;
   }

   if( carry )
      throw std::runtime_error( "there is carry left" ); 
}


void bigint::subtract( const_iterator p1, const_iterator p2,
                       const_iterator q1, const_iterator q2,
                       iterator r1, iterator r2 )
{
   const_iterator p = p1;
   const_iterator q = q1;
   iterator r = r1;

   uint32_t carry = 0;
   while( p != p2 && q != q2 )
   {
      carry += *p;
      carry -= *q;

      *r = carry & 0XFFFF;

      carry >>= 16;

      // We need sign extension in carry, because it represents a
      // negative number.

      if( carry )
         carry |= 0XFFFF0000;

      ++ p; ++ q; ++ r;
   }

   // The following code is correct because either
   // p == p2 or q == q2. This implies that only one of
   // the while loops will be entered.

   while( p != p2 )
   {
      carry += *p;
      *r = carry; 

      carry >>= 16;
      
      if( carry )
         carry |= 0XFFFF0000;

      ++ p; ++ r;
   }

   while( q != q2 )
   {
      carry -= *q;
      *r = carry;

      carry >>= 16;


      if( carry )
         carry |= 0XFFFF0000;

      ++ q; ++ r;
   }

   while( r != r2 )
   {
      *r = carry;
      carry >>= 16;

      if( carry )
         carry |= 0XFFFF0000;
  
      ++ r;
   }

   if( carry )
      throw std::runtime_error( "there is carry left" );
}


void bigint::addorsub( const bigint& b1, 
                       const bigint& b2, int32_t s2, bigint& r )
{
   if( b1. sign == s2 * b2. sign )
   {
      r. ensure( b1. repr. size( ) + 1 );
      r. ensure( b2. repr. size( ) + 1 ); 

      r. sign = b1. sign; 

      add( b1. repr. begin( ), b1. repr. end( ),
           b2. repr. begin( ), b2. repr. end( ),
           r. repr. begin( ), r. repr. end( )); 
   }
   else
   {
      if( compare( b1. repr, b2. repr ) == -1 )
      {
         // We have b1 + -b2 and b1 < b2. 

         r. sign = s2 * b2. sign; 

         r. ensure( b2. repr. size( )); 
         subtract( b2. repr. begin( ), b2. repr. end( ),
                   b1. repr. begin( ), b1. repr. end( ),
                   r. repr. begin( ), r. repr. end( ));
      }
      else
      {
         r. sign = b1. sign; 

         r. ensure( b1. repr. size( ));
         subtract( b1. repr. begin( ), b1. repr. end( ),
                   b2. repr. begin( ), b2. repr. end( ),
                   r. repr. begin( ), r. repr. end( )); 
      }
   }  
   r. reduce( );
}


void bigint::multiply( const container& v1,
                       const container& v2,
                       container& res )
{
   auto q = res. begin( );
 
   for( auto p = v1. begin( ); p != v1. end( ); ++ p )
   {
      add( q, v2. begin( ), v2. end( ), *p );
      ++ q;
   }
}


void bigint::multiply_add( word m, word c )
{
   for( word& w : repr )
   {
      uint32_t digit = m * w + c;
      w = ( digit & 0XFFFF );
      c = ( digit >> 16 ) & 0XFFFF; 
   }
   if(c)
      repr. push_back(c); 
}

// These are helper functions, belonging to the different types 
// of division estimators.

namespace
{

   inline void shift( bigint::word& x1, bigint::word& x2, bigint::word& x3, 
                      bigint::word m, uint32_t k )
   {
      x3 >>= k;
      x3 |= ( x2 & m ) << ( 16 - k );

      x2 >>= k;
      x2 |= ( x1 & m ) << ( 16 - k );

      x1 >>= k;
   }


   inline void shift( bigint::word& y1, bigint::word& y2, bigint::word m, 
                      uint32_t k )
   {
      y2 >>= k;
      y2 |= ( y1 & m ) << ( 16 - k );

      y1 >>= k;
   }


   inline bool lesseq( bigint::word v1, bigint::word v2, bigint::word v3, 
                       bigint::word w1, bigint::word w2, bigint::word w3 )
   {
      if( v1 < w1 ) return true;
      if( v1 > w1 ) return false;

      if( v2 < w2 ) return true;
      if( v2 > w2 ) return false;

      if( v3 <= w3 ) return true;
      return false;
   }


   inline void shiftleft( bigint::word& w1, bigint::word& w2, bigint::word& w3 )
   {
      w1 <<= 1;
      if( w2 & 0X8000 )
         w1 |= 1;

      w2 <<= 1;
      if( w3 & 0X8000 )
         w2 |= 1;

      w3 <<= 1;
   }


   inline void shiftright( bigint::word& w1, bigint::word& w2, bigint::word& w3 )
   {
      w3 >>= 1;
      if( w2 & 1 )
         w3 |= 0X8000;

      w2 >>= 1;
      if( w1 & 1 )
         w2 |= 0X8000;

      w1 >>= 1;
   }


   inline void subtract( bigint::word& w1, bigint::word& w2, bigint::word& w3,
                         bigint::word v1, bigint::word v2, bigint::word v3 )
   {
      uint32_t carry = w3;
      carry -= v3;
      w3 = carry;
 
      carry >>= 16;
      if( carry & 0XFFFF )
         carry |= 0XFFFF0000;
 
      carry += w2;
      carry -= v2;
      w2 = carry;

      carry >>= 16;
      if( carry & 0XFFFF )
         carry |= 0XFFFF0000;

      carry += w1;
      carry -= v1;
      w1 = carry;
   }
}


#if 0 
 
uint_32_t bigint::divide( word x1, word x2, word x3, word y2, word y3 )
{
   word y1 = 0;

   uint32_t result = 0;
   uint32_t bit = 1;

   while( lesseq( y1, y2, y3, x1, x2, x3 ) && ( y1 & 0X8000 ) == 0 )
   {
      bit <<= 1;
      shiftleft( y1, y2, y3 );
   }

   while( bit != 0 )
   {
      // printf( "bit = %x\n", bit ); 
      // printf( "X = %x %x %x\n", x1, x2, x3 );
      // printf( "Y = %x %x %x\n", y1, y2, y3 );

      if( lesseq( y1, y2, y3, x1, x2, x3 ))
      {
         subtract( x1, x2, x3, y1, y2, y3 );
         result |= bit;
      }

      shiftright( y1, y2, y3 );
      bit >>= 1;
   }

   
   return result;
}

#endif



#if 1

// This function can be off by 2. 

uint32_t bigint::divide( word x1, word x2, word x3, word y1, word y2 )
{
#if 0 
   std::cout << "divide : ";
   std::cout << "[ " << x1 << ", " << x2 << ", " << x3 << " ] / ";
   std::cout << "[ " << y1 << ", " << y2 << " ]\n";
#endif 
   if( x1 == 0 )
      return (( static_cast< uint32_t > ( x2 ) << 16 ) |
                static_cast< uint32_t > ( x3 )) /
             (( static_cast< uint32_t > ( y1 ) << 16 ) |
                static_cast< uint32_t > ( y2 ));

  
   if( x1 & 0X8000 )
   {
      x3 = x2;
      x2 = x1;
      x1 = 0;

      y2 = y1;
      y1 = 0;
   }
     
   if( x1 & 0X7F80 )
   {
      shift( x1, x2, x3, 0X00FF, 8 );
      shift( y1, y2, 0X00FF, 8 );
   }

   if( x1 & 0X0078 )
   {
      shift( x1, x2, x3, 0X000F, 4 );
      shift( y1, y2, 0X000F, 4 );
   }

   if( x1 & 0X0006 )
   {
      shift( x1, x2, x3, 0X0003, 2 );
      shift( y1, y2, 0X0003, 2 );
   }

   if( x1 )
   {
      shift( x1, x2, x3, 0X0001, 1 );
      shift( y1, y2, 0X0001, 1 );
   }

#if 0 
   std::cout << "reduced to "; 
   std::cout << "[ " << x1 << ", " << x2 << ", " << x3 << " ] / ";
   std::cout << "[ " << y1 << ", " << y2 << " ]\n";
#endif 

   uint32_t est = (( static_cast< uint32_t > ( x2 ) << 16 ) | 
                     static_cast< uint32_t > ( x3 )) / 
                  (( static_cast< uint32_t > ( y1 ) << 16 ) | 
                     static_cast< uint32_t > ( y2 ));

   return est; 
}

#endif 
   
   
bigint::container 
bigint::divide( const container& d, container& rest )
{
   
   if( d. size( ) == 0 || d. back( ) == 0 )
      throw std::runtime_error( "cannot divide by zero" );

   if( d. size( ) <= rest. size( ))
   {
      container quotient( rest. size( ) - d. size( ) + 1, 0 );

      // fd will the front digit of d. We use fd to estimate how
      // many times the division will fit. 
      // fd must be viewed as two words, with the first word before
      // the comma, and the second word behind the comma. 

      word d1 = d. back( ); 
      word d2 = 0;
      if( d. size( ) > 1 )
         d2 = d. end( ) [ -2 ];

      iterator q1 = rest. end( ) - d. size( ); 
         // Possible with vector< > :: iterators.
      iterator q2 = rest. end( ); 

      iterator dig = quotient. end( );
      while( dig != quotient. begin( ))      
      {
         -- dig; 

         // We need to make an estimation of how many times we can
         // subtract d from rest.
       
         word x1 = 0;
         if( q2 != rest. end( ))
            x1 = *q2; 
         word x2 = q2[ -1 ];
         word x3 = 0;
         if( d. size( ) > 1 )
            x3 = q2 [ -2 ];
        
         uint32_t factor = divide_fp( x1, x2, x3, d1, d2 ); 

         // The estimated factor can be one too high, but we know that
         // the true factor necessarily fits in a single digit:

         if( factor >= 65536 ) 
            factor = 65535;
 
         *dig = subtract( q1, x1, d. begin( ), d. end( ), factor );
        
         if( q2 != rest. end( ))
            *q2 = 0; 
 
         if( *dig != factor && *dig + 1u != factor )
            throw std::runtime_error( "something went wrong" ); 

         -- q1;
         -- q2; 
      }
      return quotient;
   }
   else
      return container( ); 
}


short int bigint::compare( const_iterator p1, const_iterator p2, size_t len )
{
   while( len )
   {
      -- p1;
      -- p2;
      -- len;

      if( *p1 != *p2 )
      {
         if( *p1 < *p2 ) 
            return -1;
         else
            return 1;
      }
   }
   return 0;
}


short int bigint::compare( const container& v1, const container& v2 )
{
   // This works only for normalized numbers of course:

   if( v1. size( ) != v2. size( ))
   {
      if( v1. size( ) < v2. size( ))
         return -1;
      else
         return 1;
   }

   size_t k = v1. size( );

   return compare( v1. end( ), v2. end( ), k );
}


short int bigint::compare( const bigint& i1, const bigint& i2 )
{
   // We do this, because 0 can have both positive and negative sign.

   short int s1 = i1. sign;
   if( i1. repr. size( ) == 0 ) 
      s1 = 0;

   short int s2 = i2. sign;
   if( i2. repr. size( ) == 0 )
      s2 = 0;

   if( s1 < s2 ) return -1;
   if( s1 > s2 ) return 1;

   if( s1 == 0 ) return 0;
   if( s1 < 0 )
      return compare( i2. repr, i1. repr );
   else
      return compare( i1. repr, i2. repr );
}

   
bool bigint::testmachine( )
{
   // We check that words have 16 bits:

   word w = 1;
   for( unsigned int i = 0; i < 16; ++ i )
   {
      if( w == 0 ) return 0;
      if( w < 0 ) return 0;
      w <<= 1;
   }
   if( w != 0 ) return 0; 

   // We don't want sign extension in right shifts:

   w = 0X8000;
   w >>= 1;
   if( w != 0X4000 )
      return false;

   uint32_t x = 1;
   for( unsigned int i = 0; i < 32; ++ i )
   {
      if( x == 0 ) return 0;
      if( x < 0 ) return 0; 
      x <<= 1;
   }
   if( x != 0 ) return 0;

   static_assert( sizeof( word ) == 2, "word must be two bytes" );

   static_assert( sizeof( uint16_t ) == 2, "uint16_t not 16 bytes" ); 
   static_assert( sizeof( uint32_t ) == 4, "uint32_t not 32 bytes" ); 
   static_assert( sizeof( uint64_t ) == 8, "uint64_t not 64 bytes" );

   return true; 
}


uint32_t bigint::checksum( uint32_t prime ) const
{
   uint64_t sum = 0;
   uint64_t weight = 1;

   // Remember that repr starts with least significant:

   for( auto w : repr )
   {
      sum = sum + weight * w;
      sum = sum % prime;

      weight <<= 16; 
      weight = weight % prime;
   }

   if( sign < 0 )
   {
      sum = prime - sum;
      if( sum == prime )
         sum = 0;
   } 
   return sum;
}


uint32_t 
bigint::checksum( const std::string_view v, uint32_t prime, word base )
{

   int8_t sign = 1;

   auto it = v. begin( );

   while( it != v. end( ) && isspace( *it )) 
      ++ it;

   if( it == v. end( )) return 0;

   if( *it == '-' ) 
   {
      sign = -1;
      ++ it;
   }
   else
   {
      if( *it == '+' )
         ++ it;
   }

   uint64_t sum = 0;
   if( base <= 16 ) 
   {
      while( it != v. end( )) 
      {
         char c = *it ++; 
         if( !isspace(c)) 
         {
            sum = sum * base + convxdigit(c);
            sum = sum % prime;
         }
      }
   } 
   else
      throw std::invalid_argument( "base is too high" );

   if( sign < 0 )
   {
      sum = prime - sum;
      if( sum == prime )
         sum = 0;
   }
   
   return sum;
}

size_t bigint::hash( ) const
{
   size_t hv = ( sign >= 0 ) ? 0 : 1;
 
   for( auto w : repr )
   {
      hv ^= hv << 13;
      hv ^= hv >> 7;
      hv ^= hv << 17;
      hv ^= w;
   }
   return hv; 
}


uint32_t bigint::getunsigned( ) const
{
   if( repr. size( ) > 2 )
      throw std::runtime_error( "cannot make unsigned, number is too big" );

   if( sign < 0 )
      throw std::runtime_error( "cannot make unsigned, number is negative" );

   uint32_t res = 0;
   uint16_t shift = 0;
   for( auto w : repr )
   {
      res |= ( w << shift ); 
      shift += 16;
   }
   return res; 
}

int32_t bigint::getsigned( ) const
{
   if( repr. size( ) > 2 )
      throw std::runtime_error( "cannot make unsigned, number is too big" );

   uint32_t u = 0;
   uint16_t shift = 0;
   for( auto w : repr )
   {
      u |= ( w << shift );
      shift += 16;
   }

   if( sign > 0 )
   {
      if( u >= 0X80000000 )
         throw std::runtime_error( "cannot make signed, number is too big" );
      return u;
   }
   else
   { 
      if( u >= 0X80000001 ) 
         throw std::runtime_error( "cannot make signed, number is too negative" );
      return -u; 
   }
}

double bigint::approximation( ) const
{
   double res = 0.0;
   double weight = 1.0;
   for( auto w : repr )
   {
      res += weight * w;
      weight *= 65536.0;
   }

   if( sign == 1 )
      return res;
   else
      return -res;
}


std::ostream& operator << ( std::ostream& stream, bigint b )
{
   if( b. iszero( )) 
      b. sign = 1;

   if( b. sign < 0 )
   {
      b = -b; 
      stream << '-';
   }

   std::string dec;
   while( !b. iszero( ))
   {
      uint32_t dig = b. checksum(10); 
      dec. push_back( dig + '0' );
      b /= 10;
   }
 
   if( dec. size( ))
   {
      size_t i = dec. size( );
      while( i != 0 )
      {
         -- i;
         stream << dec[i];
      }
   }
   else
      stream << '0';

   return stream;
 
   for( auto w : b. repr )
   {
      stream << w << "  ";
   }
   return stream;
}



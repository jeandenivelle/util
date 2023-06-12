
// Written by Hans de Nivelle, January 2022. 
// This type is used for normalizing string constants.
// Changed it into a template on 2022.08.19.

#ifndef UTIL_NORMALIZED_
#define UTIL_NORMALIZED_ 1 

#include <unordered_set>
#include <unordered_map>

namespace util
{

   template< 
      typename K, 
      typename H = std::hash<K>, 
      typename E = std::equal_to<K> 
   > 
   class normalized
   {
      using indextype = std::unordered_set<K,H,E> ;

      indextype :: const_iterator p;

   private:
      static indextype& getindex( ) 
      {
         static indextype index;
         return index;
      }
   
   public:
      normalized( ) = delete; 
     
      normalized( const K& k )
      {
         auto& ind = getindex( ); 
         auto val = ind. insert(k);
         p = val. first;
      }
   
      normalized( K&& k )
      {
         auto& ind = getindex( );
         auto val = ind. insert( std::move(k) );
         p = val. first;
      }
 
      const K& value( ) const
         { return *p; }

      bool operator == ( normalized str ) const 
         { return p == str.p; } 
      bool operator != ( normalized str ) const 
         { return p != str.p; } 

      struct hash
      { 
         std::hash< const K* > hh; 
            // This is defined on pointers, it doesn't even look
            // at the Keys.

         hash( ) = default; 

         size_t operator( ) ( const normalized& n ) const 
         {
            return hh( & *n.p ); 
         }
      };

      struct equal_to
      {
         equal_to( ) = default;

         bool operator( ) ( const normalized& n1, const normalized& n2 ) const
         {
            return n1.p == n2.p;
         }
      };

      // less is a total order, but no further assumptions should be 
      // made. The order can be different between different runs.
      // It should be used with care.

      struct less
      {
         less( ) = default;
  
         bool operator( ) ( const normalized& n1, const normalized& n2 ) const
         {
            return & *n1.p < & *n2.p;
         }
      };

      // So that we can enumerate all Keys that we know about. 

      static indextype::const_iterator all_begin( ) 
         { return getindex( ). begin( ); } 
      static indextype::const_iterator all_end( ) 
         { return getindex( ). end( ); } 

      template< typename V > 
      using unordered_map = std::unordered_map< normalized<K,H,E>,
                            V, hash, equal_to > ;
   };


   template< typename K, typename H, typename E > 
   inline 
   std::ostream& operator << ( std::ostream& out, normalized<K,H,E> n )
   {
      out << n. value( );
      return out;
   }

}

#endif


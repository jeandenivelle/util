
#ifndef UTIL_UNIQUE_PTR_
#define UTIL_UNIQUE_PTR_   1

#include <utility>

namespace util
{

   template< typename D >
   class unique_ptr
   { 
      D* p;  

   public:
      unique_ptr( ) 
         : p( nullptr ) 
      { } 

      unique_ptr( D* p ) noexcept 
         : p(p)
      { }

      unique_ptr( unique_ptr&& u ) noexcept 
         : p( std::exchange( u.p, nullptr ))
      { }

      unique_ptr( const unique_ptr& ) = delete;

      unique_ptr& operator = ( unique_ptr&& u ) noexcept
      {
         if( p != u.p )
         {
            std::swap( p, u.p );
            delete u.p;
            u.p = nullptr;
         }
         return *this;
      }

      unique_ptr& operator = ( const unique_ptr& ) = delete;

      // We become owner of p:

      unique_ptr& operator = ( D* p ) 
      {
         delete ( this -> p );
         ( this -> p ) = p; 
         return *this;    
      }

      ~unique_ptr( )
      {
         delete p;
      }

      D& operator * ( )
      {
         return *p;
      }

      const D& operator * ( ) const
      {
         return *p;
      }

      D* operator -> ( )
      {
         return p;
      }

      const D* operator -> ( ) const
      {
         return p;
      }

      operator bool( ) const
      {
         return p != nullptr;
      }

      bool operator == ( const unique_ptr& ) const = delete;
      bool operator != ( const unique_ptr& ) const = delete;
      bool operator <  ( const unique_ptr& ) const = delete;
      bool operator >  ( const unique_ptr& ) const = delete;
      bool operator <= ( const unique_ptr& ) const = delete;
      bool operator >= ( const unique_ptr& ) const = delete;

      template< typename C >
      const C* try_cast( ) const { return dynamic_cast< const C* > (p); }
         // Be very careful with this pointer. Use it for checking only.
         // Never use it to construct a new shared_ptr!

      template< typename C >
      C* try_cast( ) { return dynamic_cast< C* > (p); }

      void swap( unique_ptr& u ) noexcept
      {
         std::swap( p, u. p );
      }
 
   };

}

#endif


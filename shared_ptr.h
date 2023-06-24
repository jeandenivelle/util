
// Written by Hans de Nivelle, 17 march 2021.
// Revised 31 march 2021.
// A better shared_ptr. I dislike std::shared_ptr.

// The current (5th?) implementation allocates its counter only
// when it shares. I assume that most pointers 
// don't share.
// Unfortunately, the copy constructor can throw
// and becomes a bit more complicated. Also,
// the counter field must be mutable. 

#ifndef UTIL_SHARED_PTR_
#define UTIL_SHARED_PTR_  1

#include <cstddef>
#include <utility>

namespace util
{

   template< typename D > 
   class shared_ptr
   {
      D* p;
      mutable size_t* counter;

   public: 
      shared_ptr( ) noexcept
         : p( nullptr ),
           counter( nullptr )
      { }

      shared_ptr( D* p ) noexcept
         : p(p),
           counter( nullptr ) 
      { }

      shared_ptr( shared_ptr&& sh ) noexcept
         : p( std::exchange( sh.p, nullptr )),
           counter( std::exchange( sh. counter, nullptr ))
      { }


      // I assume that if the new throws, the destructor
      // will not be called.

      shared_ptr( const shared_ptr& sh )
         : p( sh.p ),
           counter( sh. counter ? sh. counter : new size_t(2)) 
      {
         if( sh. counter )
            ++ *counter; 
         else
            sh. counter = counter;
      }

      shared_ptr& operator = ( shared_ptr&& sh ) noexcept
      {
         std::swap( p, sh. p );
         std::swap( counter, sh. counter );
         return *this; 
      }

      shared_ptr& operator = ( const shared_ptr& sh ) 
      {
         *this = shared_ptr( sh );
         return *this; 
      }

      ~shared_ptr( ) noexcept
      {
         if( counter )
         {
            if( ! -- * counter )
            {
               delete counter; 
               delete p; 
            }
         }
         else
            delete p; 
      }

      // Completely unprotected:

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

      bool operator == ( const shared_ptr& ) const = delete;
      bool operator != ( const shared_ptr& ) const = delete;
      bool operator <  ( const shared_ptr& ) const = delete;
      bool operator >  ( const shared_ptr& ) const = delete;
      bool operator <= ( const shared_ptr& ) const = delete;
      bool operator >= ( const shared_ptr& ) const = delete;

      template< typename C > 
      const C* try_cast( ) const { return dynamic_cast< const C* > (p); } 
         // Be very careful with this pointer. Use it for checking only.
         // Never use it to construct a new shared_ptr! 

      template< typename C >
      C* try_cast( ) { return dynamic_cast< C* > (p); } 

      void swap( shared_ptr& sh ) noexcept
      {
         std::swap( p, sh. p );
         std::swap( counter, sh. counter );
      }

   };

}

#endif



#pragma once

#if defined(_MSC_VER)
    #pragma warning( disable : 4244 )
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4267 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4290 )
    #pragma warning( disable : 4786 )
    #pragma warning( disable : 4305 )
    #pragma warning( disable : 4996 )
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__) || defined( __MWERKS__)
    #  if defined( HOGBOX_LIBRARY_STATIC )
    #    define HOGBOXVIS_EXPORT
    #  elif defined( HOGBOXVIS_LIBRARY )
    #    define HOGBOXVIS_EXPORT   __declspec(dllexport)
    #  else
    #    define HOGBOXVIS_EXPORT   __declspec(dllimport)
    #  endif
#else
    #  define HOGBOXVIS_EXPORT
#endif

/**

\namespace hogboxDB

*/


#ifndef UCORE_DLL_H
#define UCORE_DLL_H


#if defined(_WIN32) || defined(__CYGWIN__)
#  if defined(UCORE)
#    define UCOREAPI __declspec(dllexport)
#  else
#    define UCOREAPI __declspec(dllimport)
#  endif
#elif defined(__GNUC__)
#  if defined(UCORE)
#    define UCOREAPI __attribute__((visibility("default")))
#  else
#    define UCOREAPI
#   endif
#else
#  define  UCOREAPI
#endif // UCORE_DLL_H


#endif

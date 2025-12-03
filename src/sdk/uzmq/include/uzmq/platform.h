#ifndef UZMQ__PLATFORM_H
#define UZMQ__PLATFORM_H

#ifdef _WIN32
#   ifdef __LIB_EXPORT__
#      define UZMQAPI __declspec(dllexport)
#   else
#      define UZMQAPI __declspec(dllimport)
#   endif
#elif __unix__
#    define UZMQAPI __attribute__((visibility("default")))
#endif


#endif // PLATFORM_H

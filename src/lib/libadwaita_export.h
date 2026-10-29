#ifndef LIBADWAITA_EXPORT_H
#define LIBADWAITA_EXPORT_H

#if defined(_WIN32)
#  if defined (libadwaita6priv_EXPORTS) || defined (libadwaitapriv_EXPORTS)
#    define LIBADWAITA_EXPORT __declspec(dllexport)
#  else
#    define LIBADWAITA_EXPORT __declspec(dllimport)
#  endif
#elif defined(__GNUC__)
#    define LIBADWAITA_EXPORT __attribute__((visibility("default")))
#else
#    define LIBADWAITA_EXPORT
#endif

#if defined(_WIN32)
#  if defined (libadwaita6_EXPORTS) || defined (libadwaita_EXPORTS)
#    define LIBADWAITA_MAIN_EXPORT __declspec(dllexport)
#  else
#    define LIBADWAITA_MAIN_EXPORT __declspec(dllimport)
#  endif
#elif defined(__GNUC__)
#    define LIBADWAITA_MAIN_EXPORT __attribute__((visibility("default")))
#else
#    define LIBADWAITA_MAIN_EXPORT
#endif

#ifndef LIBADWAITA_NO_EXPORT
#   define LIBADWAITA_NO_EXPORT __attribute__((visibility("hidden")))
#endif

#ifndef LIBADWAITA_DEPRECATED
#  define LIBADWAITA_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef LIBADWAITA_DEPRECATED_EXPORT
#  define LIBADWAITA_DEPRECATED_EXPORT LIBADWAITA_EXPORT LIBADWAITA_DEPRECATED
#endif

#ifndef LIBADWAITA_DEPRECATED_NO_EXPORT
#  define LIBADWAITA_DEPRECATED_NO_EXPORT LIBADWAITA_NO_EXPORT LIBADWAITA_DEPRECATED
#endif

#endif

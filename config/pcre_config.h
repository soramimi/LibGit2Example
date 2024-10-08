/* config.h for CMake builds */

#define HAVE_DIRENT_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define HAVE_WINDOWS_H 1
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1

#define HAVE_TYPE_TRAITS_H 1
#define HAVE_BITS_TYPE_TRAITS_H 1

#define HAVE_BCOPY 1
#define HAVE_MEMMOVE 1
#define HAVE_STRERROR 1
#define HAVE_STRTOLL 1
#define HAVE_STRTOQ 1
#define HAVE__STRTOI64 1

#define PCRE_STATIC 1

#define SUPPORT_PCRE8 1
#define SUPPORT_PCRE16 1
#define SUPPORT_PCRE32 1
// #define SUPPORT_JIT 1
// #define SUPPORT_PCREGREP_JIT 1
#define SUPPORT_UTF 1
#define SUPPORT_UCP 1
// #define EBCDIC 1
// #define EBCDIC_NL25 1
#define BSR_ANYCRLF 1
#define NO_RECURSE 1

#define HAVE_LONG_LONG 1
#define HAVE_UNSIGNED_LONG_LONG 1

#define SUPPORT_LIBBZ2 1
#define SUPPORT_LIBZ 1
#define SUPPORT_LIBEDIT 1
#define SUPPORT_LIBREADLINE 1

// #define SUPPORT_VALGRIND 1
#define SUPPORT_GCOV 1

#define NEWLINE			'\n'
#define POSIX_MALLOC_THRESHOLD	10
#define LINK_SIZE		2
#define PARENS_NEST_LIMIT       10
#define MATCH_LIMIT		100
#define MATCH_LIMIT_RECURSION	100
// #define PCREGREP_BUFSIZE        @PCREGREP_BUFSIZE@

#define MAX_NAME_SIZE	32
#define MAX_NAME_COUNT	10000

/* end config.h for CMake builds */

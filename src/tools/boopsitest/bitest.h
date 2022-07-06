#ifndef HAVE_BITEST_H
#define HAVE_BITEST_H

#define ENABLE_DPRINTF

#ifdef ENABLE_DPRINTF

#define DPRINTF(...) dprintf(__VA_ARGS__)

void dprintf(const char *format, ...);

#else

#define DPRINTF(...)

#endif

void _debug_putc(char c);

void dprintf(const char *format, ...);

#endif

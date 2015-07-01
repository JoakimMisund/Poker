#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define DPRINT(args...) {fprintf( stderr, "%10s:%-3d - ", __FILE__, __LINE__);fprintf( stderr, args);}
#else
#define DPRINT(args...)
#endif
#endif

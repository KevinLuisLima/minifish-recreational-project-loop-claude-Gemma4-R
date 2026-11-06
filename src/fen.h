#ifndef FEN_H
#define FEN_H

#include "types.h"

/* Parse a FEN string into a Board structure.
   Returns true on success, false if the FEN is invalid. */
bool fen_parse(Board *board, const char *fen);

#endif /* FEN_H */

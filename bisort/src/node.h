/* For copyright information, see olden_v1.0/COPYRIGHT */

/* =============== NODE STRUCTURE =================== */

struct node { 
  int value;
  struct node *left;
  struct node *right;
};

typedef struct node HANDLE;

typedef struct future_cell_int{
  HANDLE *value;
} future_cell_int;

#ifdef CHERID_STUDY
#include <stdlib.h>
#else
extern void *malloc(unsigned);
#endif

#define NIL ((HANDLE *) 0)

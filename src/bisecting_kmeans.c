#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <limits.h>

#include "tommy_helper.h"
#include "err_codes.h"

#include "../vendor/tommyarray.h"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr,
            "Usage: %s <dist.txt>\n", argv[0]);

    exit(1);
  }

  char name[16];
  char* fname = argv[1];
  FILE* f;

  PANIC_UNLESS_FILE_CAN_BE_READ(stderr, fname);

  f = fopen(fname, "r");
  PANIC_IF(stderr,
           f == NULL,
           FILE_ERR,
           FILE_ERR_MSG,
           fname,
           "reading");

  tommy_array* vectors = NULL;
  tommy_array* names = NULL;
  TOMMY_ARRAY_INIT(vectors);
  TOMMY_ARRAY_INIT(names);

  int num_cols = 0;
  int i = 0;
  int j = 0;
  int num_vectors = 0;
  double* vec = NULL;

  /* Get number of columns */
  fscanf(f, "%d", &num_cols);

  int num_data_cols = num_cols - 1;

  /* Read input data */
  while (fscanf(f, "%s", name) == 1) {
    tommy_array_insert(names, strdup(name));
    ++num_vectors;

    /* Make the array to hold the dists */
    vec = malloc(sizeof(double) * num_data_cols);

    for (i = 0; i < num_cols - 1; ++i) {
      fscanf(f, "%lf", &vec[i]);
    }

    /* Add it to the vectors array */
    tommy_array_insert(vectors, vec);
  }

  fprintf(stderr, "Num vectors: %d\n", num_vectors);
  fprintf(stderr, "Num data cols: %d\n", num_data_cols);

  /* Print out the vectors */
  for (i = 0; i < num_vectors; ++i) {
    vec = tommy_array_get(vectors, i);

    fprintf(stdout, "Vector %d -- %-6.6s: ", i, (char*)tommy_array_get(names, i));

    for (j = 0; j < num_data_cols; ++j) {
      fprintf(stdout, " %.3lf", vec[j]);
    }
    fprintf(stdout, "\n");
  }


  return 0;
}

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <limits.h>

#include "tommy_helper.h"
#include "err_codes.h"

#include "../vendor/kseq.h"
#include "../vendor/tommyarray.h"

#define CHAR_A 65

KSEQ_INIT(gzFile, gzread)

double
distance(char* seq1, char* seq2, int len)
{
  int matches = 0;
  int total = 0;
  int eq = 0;
  int c1_not_gap = 0;
  int c2_not_gap = 0;

  char c1, c2;

  /* with -O3 this is vectorizable with gcc-7! */
  for (int i = 0; i < len; ++i) {
    c1 = seq1[i];
    c2 = seq2[i];

    eq = c1 == c2;
    c1_not_gap = c1 >= CHAR_A;
    c2_not_gap = c2 >= CHAR_A;

    matches += eq && c1_not_gap;
    total += c1_not_gap && c2_not_gap;
  }

  return 1 - matches / (double)total;
}

int
main(int argc, char *argv[])
{

  if (argc != 2) {
    fprintf(stderr,
            "Usage: %s <seqs.fa>\n", argv[0]);

    exit(1);
  }

  int seq_len = -1;
  long l = 0;
  double dist = 0.0;
  gzFile fp = gzopen(argv[1], "r");
  kseq_t* seq = kseq_init(fp);
  tommy_array* seqs = NULL;
  tommy_array* seq_names = NULL;
  TOMMY_ARRAY_INIT(seqs);
  TOMMY_ARRAY_INIT(seq_names);


  unsigned long num_seqs = 0;
  while ((l = kseq_read(seq)) >= 0) {
    if (++num_seqs % 10000 == 0) {
      fprintf(stderr,
              "LOG -- Reading seq %lu\r",
              num_seqs);
    }

    if (seq_len == -1) {
      seq_len = seq->seq.l;
    }
    else {
      assert(seq_len == seq->seq.l);
    }

    tommy_array_insert(seqs, strdup(seq->seq.s));
    tommy_array_insert(seq_names, strdup(seq->name.s));
  }

  assert(num_seqs == tommy_array_size(seqs));

  for (int i = 0; i < num_seqs - 1; ++i) {
    for (int j = i + 1; j < num_seqs; ++j) {
      dist = distance(tommy_array_get(seqs, i),
                      tommy_array_get(seqs, j),
                      seq_len);

      fprintf(stdout,
              "%s\t%s\t%lf\n",
              (char*)tommy_array_get(seq_names, i),
              (char*)tommy_array_get(seq_names, j),
              dist);
    }
  }

  for (int i = 0; i < num_seqs; ++i) {
    free(tommy_array_get(seqs, i));
    free(tommy_array_get(seq_names, i));
  }

  tommy_array_done(seqs);
  tommy_array_done(seq_names);
  free(seqs);
  free(seq_names);

  kseq_destroy(seq);
  gzclose(fp);

  return 0;
}

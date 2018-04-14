/*
  1. Pick seed sequences
  2. Save 6gram profile of seed sequences
  3. For all other sequences
  - For each seed sequence
  - Count # of 6gram matches
*/

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <limits.h>

#include "tommy_helper.h"
#include "err_codes.h"

#include "../vendor/kseq.h"
#include "../vendor/tommyarray.h"
#include "../vendor/tommyhashlin.h"

#define CHAR_A 65

KSEQ_INIT(gzFile, gzread)

int
get_num_seed_seqs(int total_seqs)
{
  return (int)ceil(pow(log(total_seqs), 2));
}

typedef struct kmer_t {
  char* kmer;
  tommy_node node;
} kmer_t;

int
kmer_compare(const void* arg, const void* kmer)
{
  return strcmp((const char*)arg,
                ((const struct kmer_t*)kmer)->kmer);
}

kmer_t*
kmer_init(){
  kmer_t* kmer = malloc(sizeof(kmer_t));

  return kmer;
}

void kmer_destroy(kmer_t* kmer)
{
  free(kmer->kmer);
  free(kmer);
}



#define KMER_LEN 6


int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr,
            "Usage: %s <seqs.fa>\n", argv[0]);

    exit(1);
  }

  int seq_idx = 0;
  int kmer_idx = 0;
  long l = 0;

  char tmp_kmer[KMER_LEN+1];
  tmp_kmer[KMER_LEN] = '\0';

  kmer_t* ret_val = NULL;
  kmer_t* kmer = NULL;

  gzFile seed_fp = NULL;
  gzFile seqs_fp = NULL;
  kseq_t* seq = NULL;
  tommy_hashlin* kmers = NULL;

  tommy_uint32_t hash_val;


  tommy_array* kmer_tables = NULL;
  TOMMY_ARRAY_INIT(kmer_tables);

  /* Get number of seqs */
  seed_fp = gzopen(argv[1], "r");
  seqs_fp = gzopen(argv[2], "r");
  seq = kseq_init(seed_fp);

  unsigned long num_seqs = 0;
  while ((l = kseq_read(seq)) >= 0) {
    if (++num_seqs % 10000 == 0) {
      fprintf(stderr,
              "LOG -- Reading seq %lu\r",
              num_seqs);
    }

    TOMMY_HASHLIN_INIT(kmers);

    for (seq_idx = 0; seq_idx < seq->seq.l - KMER_LEN + 1; ++seq_idx) {
      for (kmer_idx = 0; kmer_idx < KMER_LEN; ++kmer_idx) {
        tmp_kmer[kmer_idx] = seq->seq.s[seq_idx + kmer_idx];
      }

      hash_val = tommy_strhash_u32(0, tmp_kmer);
      ret_val = tommy_hashlin_search(kmers,
                                     kmer_compare,
                                     &tmp_kmer,
                                     hash_val);

      if (!ret_val) {
        fprintf(stderr, "kmer: %s\n", tmp_kmer);
        /* New kmer, add it! */
        kmer = kmer_init();
        kmer->kmer = strdup(&tmp_kmer);

        tommy_hashlin_insert(kmers,
                             &kmer->node,
                             kmer,
                             hash_val);
      }
    }

    tommy_array_insert(kmer_tables, kmers);
  }
  kseq_destroy(seq);

  seq = kseq_init(seqs_fp);
  num_seqs = 0;
  while ((l = kseq_read(seq)) >= 0) {
    for (seq_idx = 0; seq_idx < seq->seq.l - KMER_LEN + 1; ++seq_idx) {
      for (kmer_idx = 0; kmer_idx < KMER_LEN; ++kmer_idx) {
        tmp_kmer[kmer_idx] = seq->seq.s[seq_idx + kmer_idx];
      }
      hash_val = tommy_strhash_u32(0, tmp_kmer);
      ret_val = tommy_hashlin_search(kmers,
                                     kmer_compare,
                                     &tmp_kmer,
                                     hash_val);

  }
  kseq_destroy(seq);

  for (i = 0; i < tommy_array_size(kmer_tables); ++i) {
    kmers = tommy_array_get(kmer_tables, i);
    tommy_hashlin_foreach(kmers, (tommy_foreach_func*)kmer_destroy);
    tommy_hashlin_done(kmers);
    free(kmers);
  }

  tommy_array_done(kmer_tables);
  free(kmer_tables);

  gzclose(seed_fp);
  gzclose(seqs_fp);

  return 0;
}

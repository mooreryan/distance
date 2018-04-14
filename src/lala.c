#include <string.h>

double
distance(char* seq1, char* seq2, int len)
{
  int matches = 0;
  int mm[len];
  for (int i = 0; i < len; ++i) {
    mm[i] = seq1[i] == seq2[i];
  }

  for (int i = 0; i < len; ++i) {
    matches += mm[i];
  }

  return matches / (double)len;
}

int main(int argc, char *argv[])
{

  char* s1 = strdup("apple");
  char* s2 = strdup("APPLE");

  double val = distance(s1, s2, 5);

  return 0;
}

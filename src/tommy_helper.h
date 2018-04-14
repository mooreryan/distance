#define TOMMY_HASHLIN_INIT(name)                \
  do {                                          \
    name = malloc(sizeof *name);                \
    PANIC_MEM(stderr, name);                    \
    tommy_hashlin_init(name);                   \
  } while (0)

#define TOMMY_ARRAY_INIT(name)                  \
  do {                                          \
    name = malloc(sizeof *name);                \
    PANIC_MEM(stderr, name);                    \
    tommy_array_init(name);                     \
  } while (0)

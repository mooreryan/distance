CC = /usr/local/bin/gcc-7
MKDIR_P = mkdir -p

CFLAGS = -Wall -g
LDFLAGS = -lz

VENDOR = vendor
SRC = src
BIN = bin
TEST_D = test_files

OBJS := $(VENDOR)/tommyarray.o \
        $(VENDOR)/tommyhashlin.o \
        $(VENDOR)/tommyhash.o \
        $(VENDOR)/tommylist.o


.PHONY: all
.PHONY: clean
.PHONY: test

all: bin_dir jc embed

bin_dir:
	$(MKDIR_P) $(BIN)

bisecting_kmeans: $(OBJS)
	$(CC) $(CFLAGS) -fopt-info-optimized -O0 -o $(BIN)/$@ $^ $(SRC)/$@.c $(LDFLAGS)

test_bisecting_kmeans: bisecting_kmeans
	$(BIN)/bisecting_kmeans $(TEST_D)/dist.txt

jc: $(OBJS)
	$(CC) $(CFLAGS) -fopt-info-optimized -O3 -o $(BIN)/$@ $^ $(SRC)/$@.c $(LDFLAGS)

embed: $(OBJS)
	$(CC) $(CFLAGS) -fopt-info-optimized -O3 -o $(BIN)/$@ $^ $(SRC)/$@.c $(LDFLAGS)

embed_g: $(OBJS)
	$(CC) $(CFLAGS) -fopt-info-optimized -O0 -o $(BIN)/embed_g $^ $(SRC)/embed.c $(LDFLAGS)

test_embed_g: embed_g
	$(BIN)/embed_g $(TEST_D)/kmer_test.fa

clean:
	-rm -r $(BIN) $(OBJS) $(VENDOR)/*.o

test: jc
	$(BIN)/jc $(TEST_D)/test.fa > $(TEST_D)/lala && diff $(TEST_D)/lala $(TEST_D)/test_expected.txt && rm $(TEST_D)/lala

jc_no_v: $(OBJS)
	/usr/local/bin/gcc-7 $(CFLAGS) -fopt-info-optimized -O3 -fno-tree-vectorize -o $(BIN)/$@ $^ $(SRC)/jc.c $(LDFLAGS)

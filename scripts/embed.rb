#!/usr/bin/env ruby
Signal.trap("PIPE", "EXIT")
require "abort_if"

include AbortIf
include AbortIf::Assert

require "parse_fasta"

require "set"

KMER_LEN = 2

seed_seqs_f = ARGV[0]
all_seqs_f = ARGV[1]

def kmer_set str
  Set.new str.chars.each_cons(KMER_LEN).map(&:join)
end

AbortIf.logger.info { "Hashing seeds" }

seed_seqs = {}
ParseFasta::SeqFile.open(seed_seqs_f).each_record do |rec|
  kmers = kmer_set rec.seq

  seed_seqs[rec.id] = kmers
end

AbortIf.logger.info { "Calculating distance to seeds" }

seq_idx = 0
ParseFasta::SeqFile.open(all_seqs_f).each_record do |rec|
  STDERR.printf("Reading -- %d\r", seq_idx) if (seq_idx % 1000).zero?

  kmers = kmer_set rec.seq

  dists = seed_seqs.map do |id, seed_kmers|
    min_size = kmers.count > seed_kmers.count ? seed_kmers.count : kmers.count

    (1 - (kmers.intersection(seed_kmers).count / min_size.to_f)).round(3)
  end

  puts [rec.id, dists].join "\t"
  seq_idx += 1
end

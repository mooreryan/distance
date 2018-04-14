#!/usr/bin/env ruby
Signal.trap("PIPE", "EXIT")
require "abort_if"

include AbortIf
include AbortIf::Assert

require "parse_fasta"

require "set"

AbortIf.logger.info { "Counting" }
num_seqs = `grep -c "^>" #{ARGV.first}`.chomp.to_i

def num_to_take num_seqs
  val = (Math.log2(num_seqs) ** 2).ceil

  if val >= 1
    val
  else
    1
  end
end

take_these = Set.new (0..num_seqs-1).to_a.shuffle[0, num_to_take(num_seqs)]

AbortIf.logger.info { "Picking seed seqs" }

seq_idx = 0
ParseFasta::SeqFile.open(ARGV.first).each_record do |rec|
time parallel ruby embed.rb SEED.fa {} > EMBED.{} ::: *.fold_{0..31}
  puts rec if take_these.include? seq_idx

  seq_idx += 1
end

require 'google_hash'
require 'benchmark'

for name in [Hash, GoogleHashSparse, GoogleHashDense] do
 GC.start
 subject = name.new
 GC.start
 puts name,  Benchmark.realtime { 1000.times { 1000.times {|n| subject[n] = 4}}}
 GC.start
end
puts 'here1'

1000000.times { GoogleHashSparse.new ; GC.start}



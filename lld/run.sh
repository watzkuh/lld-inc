set -e
if [ "$1" = "debug" ]; then
   ninja lld -C ../out/debug -j 2000 lld
else
   ninja lld -C ../out/release -j 2000 lld
fi
if [ "$1" = "test" ]; then
  /home/kai/master/llvm-project/out/release/bin/llvm-lit /home/kai/master/llvm-project/lld/test/COFF -s -vv
fi
cd /home/kai/master/test/forward/
if [ "$1" = "debug" ]; then
  ninja debug
fi

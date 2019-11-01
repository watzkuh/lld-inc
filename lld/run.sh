set -e
cd /home/kai/master/llvm-project/out/release
ninja lld
if [ "$1" = "test" ]; then
  /home/kai/master/llvm-project/out/release/bin/llvm-lit /home/kai/master/llvm-project/lld/test/COFF -s -vv
fi
cd /home/kai/master/test
if [ "$1" = "debug" ]; then
   ninja debug
fi
ninja
wine a.exe
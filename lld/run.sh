set -e
cd /home/kai/master/llvm-project/out
if [ "$1" = "test" ]; then
   ninja check-lld
fi
ninja lld
cd /home/kai/master/test
if [ "$1" = "debug" ]; then
   ninja debug
fi
ninja
wine a.exe
cd /home/kai/master/llvm-project/out || exit
if [ "$1" = "test" ]; then
   ninja check-lld
fi
ninja lld || exit
cd /home/kai/master/test || exit
if [ "$1" = "debug" ]; then
   ninja debug
fi
ninja
wine a.exe
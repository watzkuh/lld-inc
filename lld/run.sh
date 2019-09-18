cd /home/kai/master/llvm-project/out || exit
if [ "$1" = "test" ]; then
   ninja check-lld
fi
ninja lld || exit
cd /home/kai/master/test || exit
ninja clean
ninja
wine a.exe
if [ "$1" == "" ] ; then
  file=main.bin
else
  file=$1
fi
./st-flash write $file 8000000
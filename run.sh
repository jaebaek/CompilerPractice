#!/usr/bin/env sh

g++ tiling.cpp -o no_tile
g++ tiling.cpp -DTILE_SIZE=8 -o tile8
g++ tiling.cpp -DTILE_SIZE=16 -o tile16
g++ tiling.cpp -DTILE_SIZE=32 -o tile32
g++ tiling.cpp -DTILE_SIZE=64 -o tile64

start=`date +%s%N`
./no_tile
end=`date +%s%N`
echo Execution time of "no_tile" was `expr $end - $start` nanoseconds.

start=`date +%s%N`
./tile8
end=`date +%s%N`
echo Execution time of "tile8" was `expr $end - $start` nanoseconds.

start=`date +%s%N`
./tile16
end=`date +%s%N`
echo Execution time of "tile16" was `expr $end - $start` nanoseconds.

start=`date +%s%N`
./tile32
end=`date +%s%N`
echo Execution time of "tile32" was `expr $end - $start` nanoseconds.

start=`date +%s%N`
./tile64
end=`date +%s%N`
echo Execution time of "tile64" was `expr $end - $start` nanoseconds.

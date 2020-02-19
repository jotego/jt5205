#!/bin/bash

# it uses the files from Final Fight
make || exit 1

if which iverilog 2>/dev/null; then 
    SIM1="iverilog  -o sim -DDUMP"
    SIM2=" && sim -lxt"
else
    SIM1="ncverilog +access+r +define+DUMP +define+NCVERILOG"
    SIM2=
fi

NAME=chunk/chunk_$1.bin

if [ ! -e $NAME ]; then 
    echo "ERROR: cannot find file $NAME"
    echo "Use the hex number of the chunk in the chunk folder as the argument"
    exit 1
fi

ln -sf $NAME test_data.bin
$SIM1 ../../hdl/jt5205{_adpcm,_interpol2x,_timing,}.v test.v $SIM2
raw2wav -s 8000 < sound.raw
mv out.wav chunk/$(basename $NAME .bin)_sim.wav

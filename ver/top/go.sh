#!/bin/bash
# This test uses the files from Final Fight.
# A folder chunk will be created by the makefile with all samples in those files
# dumped in binary, wav and hex formats
# the hex format kas the extension .oki and contains the list
# of values as MAME calculates them. It can be used for comparison

if [[ ! -e ff_18.11c || ! -e ff_19.12c ]]; then
    echo This test uses the files ff_18.11c and ff_19.12c from Final Fight
    echo copy them to this folder
    exit 1
fi

if [ ! -d chunk ]; then rm -f chunk_ok; fi

make || exit 1

if which iverilog >/dev/null; then
    SIM1="iverilog -o sim -DDUMP -DSIMULATION -s test"
else
    SIM1="ncverilog +access+r +define+DUMP +define+NCVERILOG +define+SIMULATION"
fi

mkdir -p chunk
NAME=chunk/chunk_$1.bin

if [ ! -e $NAME ]; then
    echo "ERROR: cannot find file $NAME"
    echo "Use the hex number of the chunk in the chunk folder as the argument"
    exit 1
fi

ln -sf $NAME test_data.bin
$SIM1 ../../hdl/jt5205{_adpcm,_interpol2x,_timing,}.v test.v || exit $?
if [ -e sim ]; then
    sim -lxt
fi
raw2wav -s 8000 < sound.raw
mv out.wav chunk/$(basename $NAME .bin)_sim.wav

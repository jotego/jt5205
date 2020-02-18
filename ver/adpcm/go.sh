#!/bin/bash

if which ncverilog; then
    ncverilog test.v ../../hdl/jt5205{_adpcm,_timing,_interpol2x,}.v \
        +access+r +define+SIMULATION \
        +define+NCVERILOG
else
    iverilog test.v ../../hdl/jt5205{_adpcm,_timing,_interpol2x,}.v \
    -DSIMULATION -DSINESIM -o sim \
    || exit 1
    sim -lxt
fi

raw2wav -s 8000 < sound.raw
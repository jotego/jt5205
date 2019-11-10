#!/bin/bash



if which ncverilog; then
    ncverilog test.v ../../hdl/jt5205_adpcm.v +access+r +define+SIMULATION \
        +define+NCVERILOG
else
    iverilog test.v ../../hdl/jt5205{_adpcm,_timing,}.v \
    -DSIMULATION -DSINESIM -o sim \
    || exit 1
    sim -lxt
fi
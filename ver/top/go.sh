#!/bin/bash

for i in chunk*.bin; do
    ln -sf $i test_data.bin
    iverilog ../../hdl/jt5205{_adpcm,_interpol2x,_timing,}.v test.v -o sim -DDUMP && sim -lxt
    exit 0
done

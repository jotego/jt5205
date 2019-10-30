#!/bin/bash

ncverilog test.v ../../hdl/jt5205_adpcm.v +access+r +define+SIMULATION \
    +define+NCVERILOG
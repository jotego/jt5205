#!/bin/bash

verilator ../../hdl/jt5205*.v --top-module jt5205 -cc test.cc --exe --trace -DJT5205_DEBUG || exit $=
make -j -C obj_dir -f Vjt5205.mk Vjt5205 || exit $?
sim

chunk_ok: cps2test raw2wav
	mkdir -p chunk
	rm chunk/*
	cps2test ff_18.11c ff_19.12c
	touch chunk_ok

cps2test: okiadpcm.cc cps2test.cc WaveWritter.cc

raw2wav: raw2wav.cc WaveWritter.cc
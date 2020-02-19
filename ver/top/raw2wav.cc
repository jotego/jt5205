#include "WaveWritter.hpp"
#include <iostream>
#include <cstring>
#include <sstream>

using namespace std;

int main(int argc, char *argv[]) {
    int rate = 55780;
    for( int k=0; k<argc; k++ ) {
        if( strcmp(argv[k],"-s")==0) {
            if( ++k==argc ) {
                cerr << "ERROR: expecting argument after -s ";
                return 1;
            }
            stringstream s(argv[k]);
            s >> rate;
        }
    }

    WaveWritter ww("out.wav",rate, false);
    while( !cin.eof() ) {
        int16_t data[2];
        cin.read( (char*)&data, 4 );
        if( cin.eof() ) break;
        // data[1] = data[0]; // convert mono to stereo
        ww.write(data);
    }
}
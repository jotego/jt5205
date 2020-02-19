// Takes CPS ADPCM files and convert them to test binary files
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "okiadpcm.h"
#include "WaveWritter.hpp"

using namespace std;

void read_files( int argc, char *argv[], char *b);
void dump_chunks( oki_adpcm_state& oki, char *b );
int  parse( char *b );
void translate( char *b, int start, int end, oki_adpcm_state& oki, int j );

const int k256 = 256*1024*1024;

int main( int argc, char *argv[]) {
    char *buf = new char[k256];
    try {
        oki_adpcm_state oki;
        read_files( argc, argv, buf );
        dump_chunks(oki, buf);
        delete[] buf;
    }
    catch( char const*str ) {
        cout << "ERROR: " << str << '\n';
        delete[] buf;
        return 1;
    }
}

void read_files( int argc, char *argv[], char *b) {
    if( argc < 2 )  throw "Provide ADPCM ROMs from CPS games in the command line";
    ifstream fin(argv[1],ios_base::binary);
    if( fin.bad() ) throw "Cannot find provided file";
    fin.read( b, k256 );
    int rdcnt = fin.gcount();
    if( rdcnt < k256 && argc>2 ) {
        // read the second file
        fin.close();
        fin.open( argv[2], ios_base::binary );
        if( fin.bad() ) throw "Cannot find the 2nd provided file";
        b += rdcnt;
        fin.read( b, k256-rdcnt );        
    }
}

void dump_chunks( oki_adpcm_state& oki, char *b ) {
    char *b0 = b;
    for( int k=8,j=0; k<0x400; k+=8, b+=8 ) {
        int start = parse( b     );
        int end   = parse( b + 3 );
        if( start > k256 || end > k256 || end <= start ) continue;
        // translate the chunk
        cout << "Phrase " << (k>>3) << " " << hex << start << " - " << hex << end << '\n';
        stringstream fname;
        fname << "chunk/chunk_" << hex << j << ".bin";
        ofstream fout( fname.str(), ios_base::binary );
        fout.write( &b0[start], end-start );
        translate( b0, start, end, oki, j );
        j++;
    }
}

int parse( char *b ) {
    int d[3];
    d[2] = b[0]; d[2] &= 0xff;
    d[1] = b[1]; d[1] &= 0xff;
    d[0] = b[2]; d[0] &= 0xff;
    return (d[2]<<16) | (d[1]<<8) | d[0];
}

void translate( char *b, int start, int end, oki_adpcm_state& oki, int j ) {
    stringstream fname;
    fname << "chunk/chunk_" << hex << j << ".wav";

    oki.reset();
    WaveWritter w( fname.str(), 8000, false );
    fname.str("");
    fname << "chunk/chunk_" << hex << j << ".oki";
    ofstream fout(fname.str());
    for( int k=start; k<end; k++ ) {
        int nibble = (b[k]>>4)&0xf;
        int diff;
        int16_t data[2];
        data[0] = oki.clock( nibble, diff );
        data[1] = data[0];
        w.write( data );
        fout << "0x" << hex << nibble << "\t * " << dec << data[0] << "\t (" << diff << ")\n";

        nibble = b[k]&0xf;
        data[0] = oki.clock( nibble, diff );
        data[1] = data[0];
        w.write( data );        
        fout << "0x" << hex << nibble << "\t * " << dec << data[0] << "\t (" << diff << ")\n";
    }
}
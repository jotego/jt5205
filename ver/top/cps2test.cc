// Takes CPS ADPCM files and convert them to test binary files
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "okiadpcm.h"

using namespace std;

void read_files( int argc, char *argv[], char *b);
void dump_chunks( oki_adpcm_state& oki, char *b );
int  parse( char *b );

const int k256 = 256*1024*1024;

int main( int argc, char *argv[]) {
    char *buf = new char[k256];
    try {
        oki_adpcm_state oki;
        read_files( argc, argv, buf );
        dump_chunks(oki, buf);
        delete[] buf;
    }
    catch( char *str ) {
        cout << "ERROR: " << str << '\n';
        delete[] buf;
        return 1;
    }
}

void read_files( int argc, char *argv[], char *b) {
    if( argc < 1 )  throw "Provide ADPCM ROMs from CPS games in the command line";
    ifstream fin(argv[1],ios_base::binary);
    if( fin.bad() ) throw "Cannot find provided file";
    fin.read( b, k256 );
    int rdcnt = fin.gcount();
    if( rdcnt < k256 && argc>1 ) {
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
        cout << "Phrase " << (k>>3) << " " << hex << start << " - " << hex << end << '\n';
        if( start > k256 || end > k256 || end <= start ) continue;
        // translate the chunk
        stringstream fname;
        fname << "chunk_" << j << ".bin";
        ofstream fout( fname.str(), ios_base::binary );
        fout.write( &b0[start], end-start );
        j++;
        translate( b0, start, end, oki, j );
    }
}

int  parse( char *b ) {
    int d[3];
    d[2] = b[0]; d[2] &= 0xff;
    d[1] = b[1]; d[1] &= 0xff;
    d[0] = b[2]; d[0] &= 0xff;
    return (d[2]<<16) | (d[1]<<8) | d[0];
}

void translate( char *b, int start, int end, oki_adpcm_state& oki, int j ) {
    int 
    oki.reset();
}
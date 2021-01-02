#include <cstdlib>

#include "Vjt5205.h"
#include "verilated_vcd_c.h"

using namespace std;

class RTL {
    Vjt5205 top;
    VerilatedVcdC vcd;
    vluint64_t sim_time, half_period;
public:
    bool dump;
    RTL();
    void reset();
    void din( int data ) { top.din = data; }
    int raw();
    int sample() { return top.debug_cen_lo; }
    void clk(int cnt);
    void next_sample( int cnt=1);
};

RTL::RTL() {
    Verilated::traceEverOn(true);
    top.trace(&vcd, 99);
    vcd.open("test.vcd");
    sim_time=0;
    half_period = 5;
    reset();
    dump = true;
}

void RTL::reset() {
    top.rst=1;
    top.clk=0;
    top.cen=1;
    top.sel=2;
    top.din=0;
    next_sample(12);
    top.rst=0;
}

void RTL::clk( int cnt ) {
    while( cnt-- ) {
        sim_time += half_period;
        top.clk=0;
        top.eval();
        if( dump ) vcd.dump(sim_time);

        sim_time += half_period;
        top.clk=1;
        top.eval();
        if( dump )  vcd.dump(sim_time);
    }
}

void RTL::next_sample( int cnt ) {
    while( cnt-- ) {
        do {
            clk(1);
        } while( !sample() );
    };
}

int RTL::raw() {
    int x = top.debug_raw;
    if( x>>11 ) x |= ~0x7FF;
    return x;
}

class Emu {
    int m_diff_lookup[16*49];
    const static int8_t s_index_shift[8];
    int s_diff_lookup[49*16];
public:
    int m_step;
    int m_signal;
    Emu();
    void reset() { m_step = m_signal = 0; };
    int clock(uint8_t nibble);
};

class Dly {
    int data[8];
    int wr, dly;
public:
    Dly(int _dly);
    void reset();
    void push( int d );
    int read();
};

int main( int argc, char *argv[] ) {
    RTL rtl;
    Emu emu;
    Dly dly(2), steps(2);
    bool verbose=false;
    rtl.dump = false;

    if( verbose ) printf("\nDin       RTL     EMU\n");
    rtl.reset();
    emu.reset();
    dly.reset(); steps.reset();
    int din;
    for( int j=0; j<10'000'000; j++ ) {
        bool bad = false;
        din = rand()%16;
        rtl.din( din );
        rtl.next_sample();
        emu.clock(din);
        dly.push( emu.m_signal );
        steps.push( emu.m_step );
        if( dly.read() != rtl.raw() ) bad = true;
        if( verbose ) {
            printf("%4d --> %5d (--)  %5d (%2d)", din, rtl.raw(), dly.read(), steps.read() );
            if( bad ) {
                putchar('*');
                printf("FAIL\n");
                return 1;
            }
            putchar('\n');
        }
    }
    printf("PASS\n");
    return 0;
}

Dly::Dly(int _dly) {
    dly = _dly;
    reset();
}

void Dly::reset() {
    for( int k=0; k<8; k++ ) data[k] = 0;
    wr = 0;
}

void Dly::push( int d ) {
    data[wr++] = d;
    wr &= 0x7;
}

int Dly::read() {
    int n = wr-dly;
    n&=7;
    return data[n];
}

const int8_t Emu::s_index_shift[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

Emu::Emu()
{
    m_signal = 0;
    m_step = 0;
    /* nibble to bit map */
    // nibble to bit map
    static const int8_t nbl2bit[16][4] =
    {
        { 1, 0, 0, 0}, { 1, 0, 0, 1}, { 1, 0, 1, 0}, { 1, 0, 1, 1},
        { 1, 1, 0, 0}, { 1, 1, 0, 1}, { 1, 1, 1, 0}, { 1, 1, 1, 1},
        {-1, 0, 0, 0}, {-1, 0, 0, 1}, {-1, 0, 1, 0}, {-1, 0, 1, 1},
        {-1, 1, 0, 0}, {-1, 1, 0, 1}, {-1, 1, 1, 0}, {-1, 1, 1, 1}
    };

    // loop over all possible steps
    for (int step = 0; step <= 48; step++)
    {
        // compute the step value
        int stepval = floor(16.0 * pow(11.0 / 10.0, (double)step));

        // loop over all nibbles and compute the difference
        for (int nib = 0; nib < 16; nib++)
        {
            s_diff_lookup[step*16 + nib] = nbl2bit[nib][0] *
                (stepval   * nbl2bit[nib][1] +
                    stepval/2 * nbl2bit[nib][2] +
                    stepval/4 * nbl2bit[nib][3] +
                    stepval/8);
        }
    }
}


//-------------------------------------------------
//  clock - decode single nibble and update
//  ADPCM output
//-------------------------------------------------

int Emu::clock(uint8_t nibble) {
    // update the signal
    m_signal += s_diff_lookup[m_step * 16 + (nibble & 15)];

    // clamp to the maximum
    if (m_signal > 2047)
        m_signal = 2047;
    else if (m_signal < -2048)
        m_signal = -2048;

    // adjust the step size and clamp
    m_step += s_index_shift[nibble & 7];
    if (m_step > 48)
        m_step = 48;
    else if (m_step < 0)
        m_step = 0;

    // return the signal
    return m_signal;
}

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

void csv(int *m_diff_lookup) {
    for( int nib=0; nib<16;nib++ ) {
        cout << nib << ',';
        for( int step=0; step<=48; step++ ) {
            cout << m_diff_lookup[(step<<4)+nib] << ',';        
        }
        cout << '\n';
    }
}

void verilog(int *m_diff_lookup) {
    for( int step=0; step<=48; step++ ) {
        for( int nib=0; nib<8;nib++ ) {
            int d = m_diff_lookup[(step<<4)+nib];
            cout << "delta[" << setfill(' ') << setw(3) << ((step<<3)+nib) 
                << "] = 12'd" 
                << setfill('0') << setw(4) << d << "; ";
        }
        cout << '\n';
    }
}

void step_lut() {
    for(int j=0;j<8;j++) {
        for( int k=0;k<7;k++) {
        int step = j*7+k;
        if(step>48) break;
        /* compute the step value */
        int stepval = floor (16.0 * pow (11.0 / 10.0, (double)step));
        cout << "delta[" << setfill(' ') << setw(2) << step << "] = 11'd" 
            << setfill('0') << setw(4) << stepval << "; ";
        }
        cout << '\n';
    }
}

int main() {
    int m_diff_lookup[16*49];
    /* nibble to bit map */
    static const int nbl2bit[16][4] =
    {
        { 1, 0, 0, 0}, { 1, 0, 0, 1}, { 1, 0, 1, 0}, { 1, 0, 1, 1},
        { 1, 1, 0, 0}, { 1, 1, 0, 1}, { 1, 1, 1, 0}, { 1, 1, 1, 1},
        {-1, 0, 0, 0}, {-1, 0, 0, 1}, {-1, 0, 1, 0}, {-1, 0, 1, 1},
        {-1, 1, 0, 0}, {-1, 1, 0, 1}, {-1, 1, 1, 0}, {-1, 1, 1, 1}
    };

    int step, nib;

    /* loop over all possible steps */
    for (step = 0; step <= 48; step++)
    {
        /* compute the step value */
        int stepval = floor (16.0 * pow (11.0 / 10.0, (double)step));

        /* loop over all nibbles and compute the difference */
        for (nib = 0; nib < 16; nib++)
        {
            m_diff_lookup[step*16 + nib] = nbl2bit[nib][0] *
                (stepval   * nbl2bit[nib][1] +
                    stepval/2 * nbl2bit[nib][2] +
                    stepval/4 * nbl2bit[nib][3] +
                    stepval/8);
        }
    }
    // verilog(m_diff_lookup);
    step_lut();
    return 0;
}
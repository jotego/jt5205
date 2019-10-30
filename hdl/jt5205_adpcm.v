/*  This file is part of JT5205.
    JT5205 program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    JT5205 program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with JT5205.  If not, see <http://www.gnu.org/licenses/>.

    Author: Jose Tejada Gomez. Twitter: @topapate
    Version: 1.0
    Date: 30-10-2019 */

module jt5205_adpcm(
    input                      rst,
    input                      clk,
    input                      cen /* direct_enable */,        
    input             [ 3:0]   din,
    output reg signed [11:0]   sound
);

reg [ 5:0] step, step_inc;
reg [ 6:0] step_lut[0:48];
reg [ 6:0] step_size;

reg [11:0] step_sum;
reg        up;
reg [ 2:0] factor;
reg [ 3:0] din_copy;
reg [ 5:0] next_step;

always @(posedge clk, posedge rst) begin
    if( rst ) begin
        factor    <= 3'd0;
        up        <= 1'b0;
        next_step <= 6'd0;
        step_size <= 7'd0;
        step_sum  <= 12'd0;
    end else begin
        up <= cen;
        if( up ) begin
            factor    <= din_copy[2:0];
            step_size <= step_lut[step];            
            step_sum  <= { 8'd0, step_lut[step][6:3]};
            next_step <= din_copy[2] ? (step+step_inc) : (step-6'd1);
        end else begin
            if(factor[0]) begin
                step_sum  <= step_sum + step_size;
            end
            step_size <= step_size>>1;
            factor    <= factor>>1;
            if( next_step>6'd48)
                next_step <= din_copy[2] ? 6'd48 : 6'd0;
        end
    end        
end

always @(posedge clk, posedge rst) begin
    if( rst ) begin
        step     <= 6'd0;
        sound    <= 12'd0;
        din_copy <= 4'd0;
    end else if(cen) begin
        case( din[1:0] )
            2'd0: step_inc <= 6'd2;
            2'd1: step_inc <= 6'd4;
            2'd2: step_inc <= 6'd6;
            2'd3: step_inc <= 6'd8;
        endcase
        din_copy  <= din;
        step      <= next_step;
        sound     <= din_copy[3] ? sound - step_sum : sound + step_sum;
    end
end

initial begin
    step_lut[ 0] = 7'd1;
    step_lut[ 1] = 7'd1;
    step_lut[ 2] = 7'd1;
    step_lut[ 3] = 7'd1;
    step_lut[ 4] = 7'd1;
    step_lut[ 5] = 7'd1;
    step_lut[ 6] = 7'd1;
    step_lut[ 7] = 7'd1;
    step_lut[ 8] = 7'd2;
    step_lut[ 9] = 7'd2;
    step_lut[10] = 7'd2;
    step_lut[11] = 7'd2;
    step_lut[12] = 7'd3;
    step_lut[13] = 7'd3;
    step_lut[14] = 7'd3;
    step_lut[15] = 7'd4;
    step_lut[16] = 7'd4;
    step_lut[17] = 7'd5;
    step_lut[18] = 7'd5;
    step_lut[19] = 7'd6;
    step_lut[20] = 7'd6;
    step_lut[21] = 7'd7;
    step_lut[22] = 7'd8;
    step_lut[23] = 7'd8;
    step_lut[24] = 7'd9;
    step_lut[25] = 7'd10;
    step_lut[26] = 7'd11;
    step_lut[27] = 7'd13;
    step_lut[28] = 7'd14;
    step_lut[29] = 7'd15;
    step_lut[30] = 7'd17;
    step_lut[31] = 7'd19;
    step_lut[32] = 7'd21;
    step_lut[33] = 7'd23;
    step_lut[34] = 7'd25;
    step_lut[35] = 7'd28;
    step_lut[36] = 7'd30;
    step_lut[37] = 7'd34;
    step_lut[38] = 7'd37;
    step_lut[39] = 7'd41;
    step_lut[40] = 7'd45;
    step_lut[41] = 7'd49;
    step_lut[42] = 7'd54;
    step_lut[43] = 7'd60;
    step_lut[44] = 7'd66;
    step_lut[45] = 7'd72;
    step_lut[46] = 7'd80;
    step_lut[47] = 7'd88;
    step_lut[48] = 7'd97;
end

endmodule
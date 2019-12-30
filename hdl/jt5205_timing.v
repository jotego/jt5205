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

module jt5205_timing(
    input             rst,
    input             clk,
    (* direct_enable *) input cen,
    input      [ 1:0] sel,        // s pin
    output            cen_lo,
    output            cenb_lo,
    output            cen_mid
);

reg [6:0] cnt;
reg       pre, preb;
reg [6:0] lim;

always @(posedge clk) begin
    case(sel)
        2'd0: lim <= 7'd95;
        2'd1: lim <= 7'd63;
        2'd2: lim <= 7'd47;
        2'd3: lim <= 7'd1;
    endcase
end

always @(posedge clk, posedge rst) begin
    if(rst) begin
        cnt <= 7'd0;
        pre <= 1'b0;
        preb<= 1'b0;
    end else if(cen) begin
        cnt    <= cnt + 7'd1;
        pre    <= 1'b0;
        preb   <= 1'b0;
        if(cnt==lim) begin
            cnt <= 7'd0;
            pre <= 1'b1;
        end
        if(cnt==(lim>>1)) preb <=1'b1;
    end
end

reg pre2, pre2b;
always @(negedge clk) begin
    pre2  <= pre;
    pre2b <= preb;
end

assign cen_lo  = pre2 &cen;
assign cenb_lo = pre2b&cen;
assign cen_mid = (pre2|pre2b)&cen;

endmodule
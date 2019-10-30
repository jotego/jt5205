module test;

reg rst;
reg clk, cen=1'b0;
reg [3:0] din;
wire signed [11:0] sound;

jt5205_adpcm uut(
    .rst        ( rst       ),
    .clk        ( clk       ),
    .cen        ( cen       ),
    .din        ( din       ),
    .sound      ( sound     )
);

reg [3:0] data[0:1024*32-1];

initial begin
    clk=1'b0;
    $readmemh( "data.hex", data);
    forever #10 clk=~clk;
end

initial begin
    rst = 1'b0;
    #15 rst=1'b1;
    #75 rst=1'b0;
end

integer cnt=0;

always @(posedge clk) begin
    din <= data[cnt];
    cnt <= cnt+1;
    if( cnt==100 ) $finish;
end

integer cen_cnt=0;

always @(negedge clk) begin
    cen <= 1'b0;
    if(cen_cnt==0) cen<=1'b1;
    cen_cnt <= cen_cnt==0 ? 4 : (cen_cnt-1);
end

`ifndef NCVERILOG
    initial begin
        $dumpfile("test.lxt");
        $dumpvars(2,test);
        $dumpon;
    end
`else
    initial begin
        $shm_open("test.shm");
        $shm_probe(test,"AS");
    end
`endif

endmodule
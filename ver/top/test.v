module test;

reg                    rst, clk, cen;
reg           [ 1:0]   sel;
reg           [ 3:0]   din;
wire   signed [11:0]   sound;
wire                   irq;

integer file, fcnt, rdcnt;

reg [7:0] test_data[0:(2**20)-1];

initial begin
    file=$fopen("test_data.bin","rb");
    fcnt=$fread(test_data, file);
    $fclose(file);
end

initial begin
    rst   = 1'b1;
    #190 rst = 1'b0;
end

initial begin
    clk   = 1'b0;
    cen   = 1'b0;
    sel   = 2'b10;
    rdcnt = 0;
    din   = 0;
    forever #10 clk = ~clk;
end

reg        last_irq;
wire [7:0] next = test_data[rdcnt>>1];

always @(posedge clk) begin
    cen <= ~cen;
    last_irq <= irq;
    din   <= !rdcnt[0] ? next[7:4] : next[3:0];
    if( irq && !last_irq) begin
        rdcnt <= rdcnt+1;
    end
    if( (rdcnt>>1) > fcnt ) $finish;
end


jt5205 uut(
    .rst    ( rst       ),
    .clk    ( clk       ),
    .cen    ( cen       ),
    .sel    ( sel       ),
    .din    ( din       ),
    .sound  ( sound     ),
    .irq    ( irq       )
);

`ifdef DUMP
    initial $display ("INFO: will dump waveforms.");
    `ifndef NCVERILOG
        initial begin
            $dumpfile("test.lxt");
            $dumpvars(0,test);
            $dumpon;
        end
    `else
        initial begin
            $shm_open("test.shm");
            $shm_probe(test,"AS");
        end
    `endif
`endif


integer fsnd;
initial begin
    fsnd=$fopen("sound.raw","wb");
end
wire signed [15:0] snd_log = { sound, 4'b0 };
always @(posedge irq) begin
    $fwrite(fsnd,"%u", {snd_log, snd_log});
end

endmodule
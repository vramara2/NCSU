//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// DUT




module ECE564MyDesign
	    (

            //---------------------------------------------------------------------------
            // Control
            //
            input  wire               xxx__dut__run            ,             
            output reg                dut__xxx__busy           , // high when computing
            input  wire               clk                      ,
            input  wire               reset_b                  ,


            //--------------------------------------------------------------------------- 
            //---------------------------------------------------------------------------
            // SRAM interface
            //
            output reg  [11:0]        dut__sram__write_address  ,
            output reg  [15:0]        dut__sram__write_data     ,
            output reg                dut__sram__write_enable   ,
            output reg  [11:0]        dut__sram__read_address   ,
            input  wire [15:0]        sram__dut__read_data      ,
            
            //---------------------------------------------------------------------------
            // f memory interface
            //
            output reg  [11:0]        dut__fmem__read_address   ,
            input  wire [15:0]        fmem__dut__read_data      ,  // read data

            //---------------------------------------------------------------------------
            // g memory interface
            //
            output reg  [11:0]        dut__gmem__read_address   ,
            input  wire [15:0]        gmem__dut__read_data      ,  // read data

            //---------------------------------------------------------------------------
            // i memory interface
            //
            output reg  [11:0]        dut__imem__read_address   ,
            input  wire [15:0]        imem__dut__read_data      ,  // read data
            
            //---------------------------------------------------------------------------
            // o memory interface
            //
            output reg  [11:0]        dut__omem__read_address   ,
            input  wire [15:0]        omem__dut__read_data      ,  // read data

            //---------------------------------------------------------------------------
            // Tanh look up table tanhmem 
            //
            output reg  [11:0]        dut__tanhmem__read_address     ,
            input  wire [15:0]        tanhmem__dut__read_data        ,   // read data
       
  
            //Enable declarations///
            output reg tanhmem__dut__read_enable,
            output reg fmem__dut__read_enable,
            output reg omem__dut__read_enable,
            output reg imem__dut__read_enable,
            output reg gmem__dut__read_enable,
            output reg sram__dut__read_enable
            );

  //---------------------------------------------------------------------------
  //
  //<<<<----  YOUR CODE HERE    ---->>>>

  //net declarations//
  wire matrix_row_done;
  wire computation_done;
  wire [18:0] g_out_value;
  wire carry_out;
 
  tanh_interpolation t1(.reset_b(reset_b), .clk(clk), .matrix_row_done(matrix_row_done), .dut_busy(dut__xxx__busy), .carry(carry_out), .g_out(g_out_value), .tanh_read_address(dut__tanhmem__read_address), .tanh_read_data(tanhmem__dut__read_data), .tanh_read_en(tanhmem__dut__read_enable), .sram_wr_address(dut__sram__write_address), .sram_wr_value(dut__sram__write_data), .sram_wr_en(dut__sram__write_enable), .computation_done(computation_done));

  multiplier m1(.reset_b(reset_b), .clk(clk), .w_read_address(dut__gmem__read_address), .x_read_address(dut__sram__read_address), .carry(carry_out), .w_read_data(gmem__dut__read_data), .x_read_data(sram__dut__read_data), .w_read_en(gmem__dut__read_enable), .x_read_en(sram__dut__read_enable), .dut_busy(dut__xxx__busy), .g_out(g_out_value), .matrix_row_done(matrix_row_done), .computation_done(computation_done));

  control_path c1(.reset_b(reset_b), .clk(clk), .dut_xxx_run(xxx__dut__run), .dut_computation_done(computation_done), .dut_xxx_busy(dut__xxx__busy)); 

endmodule


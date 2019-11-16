module tanh_interpolation(reset_b, clk, matrix_row_done, dut_busy, carry, g_out, tanh_read_data, tanh_read_en, tanh_read_address, sram_wr_address, sram_wr_value, sram_wr_en, computation_done);

input reset_b, clk, matrix_row_done, dut_busy;
input [18:0] g_out;
input carry;
output computation_done;
output [15:0] sram_wr_value;
input [15:0] tanh_read_data;
output [11:0] tanh_read_address, sram_wr_address;
output sram_wr_en, tanh_read_en;
reg [8:0] count = 9'b0;
reg tanh_read_en;
reg computation_done, computation_done_;
reg sram_wr_en;
reg [18:0] indexed_gout_value;
reg [11:0] tanh_read_address, sram_wr_address = 12'h1FE;
reg [11:0] tanh_read_address_1;
reg [15:0] sram_wr_value;
reg [2:0] current_state, next_state;
reg signed [15:0] y0, y1;
reg signed [40:0] y;
reg signed [19:0] x, x0, x1;
reg [34:0] y_inter_1, y_inter_2;
reg [40:0] y_inter_1_shift, y_inter_2_shift, signed_y_value;
parameter [2:0]
  s0 = 3'b000,
  s1 = 3'b010,
  s2 = 3'b001,
  s3 = 3'b011,
  s4 = 3'b111;



always @(posedge clk)
begin
  if(!reset_b)
  begin
    current_state <= s0;
    computation_done <= 0;
  end
  else
  begin
    current_state <= next_state;
    computation_done <= computation_done_;
  end
end

always @(*)
begin
if(!reset_b)
begin
  computation_done_ = 1'b0;
  count = 9'h0;
  next_state = s0;
  tanh_read_address = 12'b0;
  tanh_read_address = 12'b0;
  sram_wr_en = 1'b0;
  tanh_read_en = 1'b0;
  sram_wr_value = 16'b0;
end
casex(current_state)

  s0:
  begin
    tanh_read_address = 12'b0;
    tanh_read_en = 1'b0;
    sram_wr_value = 16'b0;
    sram_wr_en = 1'b0;
    if(computation_done_ == 1'b1)
    begin
      computation_done_ = 1'b0;
      sram_wr_address = 12'h1FE;
    end
    if(matrix_row_done == 1'b1 && count != 9'h100)
    begin
      next_state = s1;
    end
    else
    begin
      next_state = s0;
    end
    if(count == 9'h100)
    begin
      count = 9'h0;
    end
  end

  s1:
  begin
      if(g_out > 19'd130560) //4 integer bits and 15 decimal bits
      begin
        if(carry == 1'b0)
        begin
          sram_wr_value = 16'b0111111111101010;
          next_state = s4;
        end
        else
        begin
          sram_wr_value = 16'b1000000000010110;
          next_state = s4;
        end
      end
      else
      begin
          x = {g_out};  ///check the value assignments...
          indexed_gout_value = (g_out >> 9);
          tanh_read_address = indexed_gout_value[11:0] << 1;  //since address ranges are in 2, 4, 6, .. 
          tanh_read_address_1 = indexed_gout_value[11:0] << 1;
          tanh_read_en = 1'b1;
          x0 = {indexed_gout_value << 9};
          next_state = s2;
      end
  end

  s2:
  begin
    y0 = tanh_read_data;
    tanh_read_address = tanh_read_address_1 + 2'b10;
    tanh_read_en = 1'b1;
    x1 = {(indexed_gout_value + 1'b1) << 9};
    y_inter_1 = y0*(x1 - x); 
    y_inter_1_shift = y_inter_1 << 6;
    next_state = s3;
  end

  s3:
  begin
    y1 = tanh_read_data;
    tanh_read_en = 1'b0;
    y_inter_2 = y1*(x - x0);
    y_inter_2_shift = y_inter_2 << 6;
    y = y_inter_1_shift + y_inter_2_shift;
    if(carry == 1'b0)
    begin
      sram_wr_value = {1'b0,y[29:15]};
    end
    else
    begin
      signed_y_value = ~y + 1;
      sram_wr_value = signed_y_value[30:15];
    end
    next_state = s4;
  end
     
  s4:
  begin
    sram_wr_en = 1;
    sram_wr_address = sram_wr_address + 2'd2;  
    count = count + 1;
    if(count == 9'h100)
    begin
      computation_done_ = 1'b1;
    end
    next_state = s0;
  end
  
  
  default:
  begin
    next_state = s0;
  end
endcase
end

endmodule

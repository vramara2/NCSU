module multiplier(reset_b, clk, w_read_address,w_read_en, x_read_address, x_read_en, w_read_data, x_read_data, dut_busy, g_out, matrix_row_done, computation_done, carry);

input reset_b, clk, dut_busy, computation_done;
input signed [15:0] x_read_data, w_read_data;
output [11:0] x_read_address;
output [11:0] w_read_address;
output w_read_en, x_read_en, carry;
output signed [18:0] g_out;
output matrix_row_done;
reg [11:0] x_read_address = 12'd0;
reg [11:0] w_read_address = 12'd0;
reg [12:0] multiplier_computation;
reg w_read_en, x_read_en;
reg carry, carry_;
reg [4:0] w_done;
reg [4:0] x_done;
reg signed [18:0] g_out;
reg signed [19:0] gout_value_;
reg [4:0] matrix_count;
reg signed [30:0] check_multiply;
reg matrix_row_done, matrix_row_done_;

always @(posedge clk)
begin
  if(!reset_b)
  begin
    g_out <= 0;
    x_read_address <= 12'd0;
    w_read_address <= 12'd0;
    matrix_row_done <= 1'b0;
    carry = 1'b0;
  end

  else
  begin
    if(dut_busy == 1'b1)
    begin
      if(matrix_count == 5'd15 && w_done != 5'd16 && (multiplier_computation != 13'd4095 && multiplier_computation != 13'd0))
      begin
        x_read_address <= (x_done)<<5;
        x_read_en <= 1'b1;
        w_read_address <= w_read_address + 2'd2;
        w_read_en <= 1'b1;
      end
      else if(matrix_count == 5'd15 && w_done == 5'd16 && (multiplier_computation != 13'd4095 && multiplier_computation != 13'd0))
      begin
        x_read_address <= (x_done + 1)<<5;
        x_read_en <= 1'b1;
        w_read_address <= 12'd0;
        w_read_en <= 1'b1;
      end
      else if(matrix_count != 5'd0 && (multiplier_computation != 13'd4095 && multiplier_computation != 13'd0))
      begin
        x_read_address <= x_read_address + 2'd2;
        x_read_en <= 1'b1;
        w_read_address <= w_read_address + 2'd2;
        x_read_en <= 1'b1;
      end
      else if(matrix_count == 5'd0 && (multiplier_computation != 13'd4095 && multiplier_computation == 13'd0))
      begin
        x_read_address <= x_read_address + 2'd2;
        x_read_en <= 1'b1;
        w_read_address <= w_read_address + 2'd2;
        x_read_en <= 1'b1;
      end
    end
    if(matrix_row_done_ == 1'b1)
    begin
      g_out <= gout_value_[18:0];
    end
    matrix_row_done <= matrix_row_done_;
  end
end

always @(posedge dut_busy or negedge reset_b)

begin
  if(!reset_b)
  begin
    matrix_count = 0;
    multiplier_computation = 13'd0;
    matrix_row_done_ = 0;
    w_read_en = 0;
    x_read_en = 0;
    gout_value_ = 20'b0;
    w_done = 0;
    x_done = 0; 
  end

  else if(dut_busy == 1'b1)
  begin
    matrix_count = 5'd0;
    matrix_row_done_ = 1'b0;
    x_read_address = 12'b0;
    w_read_address = 12'b0;
    multiplier_computation = 13'b0;
    w_done = 5'b0;
    x_done = 5'd0; 
    x_read_en = 1'b1;
    w_read_en = 1'b1;
    carry_ = 1'b0;
  end
end

always @(x_read_data or w_read_data or computation_done)
begin
    if(computation_done == 1'b1)
    begin
      matrix_row_done_ = 1'b0;
      matrix_count = 5'b0;
      multiplier_computation = 13'b0;
      w_done = 5'b0;
      x_done = 5'b0;
      gout_value_ = 20'd0;
      x_read_en = 1'b0;
      w_read_en = 1'b0;
    end

    else if(dut_busy == 1'b1 && multiplier_computation < 13'd4096)
    begin 
        
        if(matrix_count == 5'd16)
        begin
          if(w_done == 5'd16)
          begin
            w_done = 5'b0;
          end
          matrix_count = 5'd0;
          matrix_row_done_ = 1'b0;
          gout_value_ = 20'd0;
        end

        if(multiplier_computation == ((x_done + 1)*256 - 1))
        begin
          x_done = x_done + 1;
        end

        if(multiplier_computation == ((x_done + 1)*256))
        begin
          gout_value_ = 20'd0;
        end
         
        check_multiply = x_read_data * w_read_data;

        gout_value_ = gout_value_ + {4{check_multiply[30]}, check_multiply[30:15]};  //need to check negative logic too
        
        matrix_count = matrix_count + 1'b1;
          
        if(matrix_count == 5'd1) 
        begin
          w_done = w_done + 5'd1;
        end
        if(matrix_count == 5'd16) 
        begin
          matrix_row_done_ = 1'b1;
          carry = gout_value_[19];
          if(carry == 1'b1)
          begin
            gout_value_[18:0] = ~gout_value_[18:0] + 1;
          end
        end
         
        multiplier_computation = multiplier_computation + 1'b1;
    end
end
endmodule


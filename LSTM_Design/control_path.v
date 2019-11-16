module control_path(reset_b, clk, dut_xxx_run, dut_computation_done, dut_xxx_busy);
  
  input clk, reset_b, dut_xxx_run, dut_computation_done;
  output dut_xxx_busy;
  reg dut_xxx_busy;

  always @(posedge clk)
  begin
    if(!reset_b)
    begin
      dut_xxx_busy <= 1'b0;
    end
 
    else if(dut_xxx_run == 1'b1)
    begin
      dut_xxx_busy <= 1'b1;
    end

    else if(dut_xxx_run == 1'b0 && dut_computation_done == 1'b1)
    begin
      dut_xxx_busy <= 1'b0; 
    end
  end

endmodule

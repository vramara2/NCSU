onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /tb_top/dut/clk
add wave -noupdate /tb_top/dut/xxx__dut__run
add wave -noupdate /tb_top/dut/tanhmem__dut__read_enable
add wave -noupdate /tb_top/dut/tanhmem__dut__read_data
add wave -noupdate /tb_top/dut/t1/tanh_read_address
add wave -noupdate /tb_top/dut/sram__dut__read_enable
add wave -noupdate /tb_top/dut/sram__dut__read_data
add wave -noupdate /tb_top/dut/reset_b
add wave -noupdate /tb_top/dut/matrix_row_done
add wave -noupdate /tb_top/dut/gmem__dut__read_enable
add wave -noupdate /tb_top/dut/gmem__dut__read_data
add wave -noupdate /tb_top/dut/g_out_value
add wave -noupdate /tb_top/dut/dut__xxx__busy
add wave -noupdate /tb_top/dut/computation_done
add wave -noupdate /tb_top/dut/m1/x_read_address
add wave -noupdate /tb_top/dut/m1/w_read_en
add wave -noupdate /tb_top/dut/m1/w_read_address
add wave -noupdate /tb_top/dut/m1/x_read_data
add wave -noupdate /tb_top/dut/m1/x_read_en
add wave -noupdate /tb_top/dut/m1/w_read_data
add wave -noupdate -radix unsigned /tb_top/dut/m1/multiplier_computation
add wave -noupdate /tb_top/dut/m1/matrix_row_done
add wave -noupdate /tb_top/dut/m1/matrix_row_done_
add wave -noupdate /tb_top/dut/m1/matrix_count
add wave -noupdate /tb_top/dut/m1/gout_value
add wave -noupdate /tb_top/dut/m1/g_out
add wave -noupdate /tb_top/dut/m1/d
add wave -noupdate /tb_top/dut/t1/computation_done_
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {81885 ns} 0}
quietly wave cursor active 1
configure wave -namecolwidth 291
configure wave -valuecolwidth 40
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {0 ns} {143360 ns}

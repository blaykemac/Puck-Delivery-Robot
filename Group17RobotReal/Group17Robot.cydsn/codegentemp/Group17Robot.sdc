# THIS FILE IS AUTOMATICALLY GENERATED
<<<<<<< HEAD
# Project: C:\Users\Misha\Documents\PSoC Creator\ECE3091_Group17\Group17RobotReal\Group17Robot.cydsn\Group17Robot.cyprj
# Date: Mon, 21 Oct 2019 02:17:49 GMT
=======
# Project: C:\Users\bamcg3\Documents\ECE3091_Group17\Group17RobotReal\Group17Robot.cydsn\Group17Robot.cyprj
# Date: Mon, 21 Oct 2019 00:49:16 GMT
>>>>>>> master
#set_units -time ns
create_clock -name {ADC_Ext_CP_Clk(routed)} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/dclk_0}]]
create_clock -name {CyILO} -period 1000000 -waveform {0 500000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyIMO} -period 333.33333333333331 -waveform {0 166.666666666667} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyPLL_OUT} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyMASTER_CLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_clock -name {ADC_Ext_CP_Clk} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/dclk_glb_0}]]
create_generated_clock -name {Clock_5} -source [get_pins {ClockBlock/clk_sync}] -edges {1 3 5} [list [get_pins {ClockBlock/dclk_glb_1}]]
create_generated_clock -name {timer_clock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 25 49} [list [get_pins {ClockBlock/dclk_glb_2}]]
create_generated_clock -name {ADC_theACLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 39 77} [list [get_pins {ClockBlock/aclk_glb_0}]]
create_generated_clock -name {UART_1_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 53 105} -nominal_period 2166.6666666666665 [list [get_pins {ClockBlock/dclk_glb_3}]]
create_generated_clock -name {Clock_6} -source [get_pins {ClockBlock/clk_sync}] -edges {1 1201 2401} [list [get_pins {ClockBlock/dclk_glb_4}]]

set_false_path -from [get_pins {__ONE__/q}]

<<<<<<< HEAD
# Component constraints for C:\Users\Misha\Documents\PSoC Creator\ECE3091_Group17\Group17RobotReal\Group17Robot.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\Misha\Documents\PSoC Creator\ECE3091_Group17\Group17RobotReal\Group17Robot.cydsn\Group17Robot.cyprj
# Date: Mon, 21 Oct 2019 02:17:32 GMT
=======
# Component constraints for C:\Users\bamcg3\Documents\ECE3091_Group17\Group17RobotReal\Group17Robot.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\bamcg3\Documents\ECE3091_Group17\Group17RobotReal\Group17Robot.cydsn\Group17Robot.cyprj
# Date: Mon, 21 Oct 2019 00:49:01 GMT
>>>>>>> master

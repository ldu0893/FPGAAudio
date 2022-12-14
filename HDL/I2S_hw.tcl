# TCL File Generated by Component Editor 18.1
# Wed Dec 14 01:58:13 CST 2022
# DO NOT MODIFY


# 
# I2S "I2S" v1.0
#  2022.12.14.01:58:13
# 
# 

# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1


# 
# module I2S
# 
set_module_property DESCRIPTION ""
set_module_property NAME I2S
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME I2S
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL I2S
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file I2S.sv SYSTEM_VERILOG PATH ../SV/I2S.sv TOP_LEVEL_FILE
add_fileset_file lsreg.sv SYSTEM_VERILOG PATH ../SV/lsreg.sv

add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL I2S
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VERILOG ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file I2S.sv SYSTEM_VERILOG PATH ../SV/I2S.sv
add_fileset_file lsreg.sv SYSTEM_VERILOG PATH ../SV/lsreg.sv


# 
# parameters
# 


# 
# display items
# 


# 
# connection point I2S
# 
add_interface I2S conduit end
set_interface_property I2S associatedClock ""
set_interface_property I2S associatedReset ""
set_interface_property I2S ENABLED true
set_interface_property I2S EXPORT_OF ""
set_interface_property I2S PORT_NAME_MAP ""
set_interface_property I2S CMSIS_SVD_VARIABLES ""
set_interface_property I2S SVD_ADDRESS_GROUP ""

add_interface_port I2S SCLK sclk Input 1
add_interface_port I2S LRCLK lrclk Input 1
add_interface_port I2S Din din Input 1
add_interface_port I2S Dout dout Output 1
add_interface_port I2S SW sw Input 10
add_interface_port I2S keycode keycode Input 32
add_interface_port I2S song song Input 64


# 
# connection point CLK
# 
add_interface CLK clock end
set_interface_property CLK clockRate 0
set_interface_property CLK ENABLED true
set_interface_property CLK EXPORT_OF ""
set_interface_property CLK PORT_NAME_MAP ""
set_interface_property CLK CMSIS_SVD_VARIABLES ""
set_interface_property CLK SVD_ADDRESS_GROUP ""

add_interface_port CLK CLK clk Input 1


# 
# connection point RESET
# 
add_interface RESET reset end
set_interface_property RESET associatedClock CLK
set_interface_property RESET synchronousEdges DEASSERT
set_interface_property RESET ENABLED true
set_interface_property RESET EXPORT_OF ""
set_interface_property RESET PORT_NAME_MAP ""
set_interface_property RESET CMSIS_SVD_VARIABLES ""
set_interface_property RESET SVD_ADDRESS_GROUP ""

add_interface_port RESET RESET reset Input 1


# 
# connection point ram
# 
add_interface ram avalon end
set_interface_property ram addressUnits WORDS
set_interface_property ram associatedClock CLK
set_interface_property ram associatedReset RESET
set_interface_property ram bitsPerSymbol 8
set_interface_property ram burstOnBurstBoundariesOnly false
set_interface_property ram burstcountUnits WORDS
set_interface_property ram explicitAddressSpan 0
set_interface_property ram holdTime 0
set_interface_property ram linewrapBursts false
set_interface_property ram maximumPendingReadTransactions 0
set_interface_property ram maximumPendingWriteTransactions 0
set_interface_property ram readLatency 0
set_interface_property ram readWaitTime 1
set_interface_property ram setupTime 0
set_interface_property ram timingUnits Cycles
set_interface_property ram writeWaitTime 0
set_interface_property ram ENABLED true
set_interface_property ram EXPORT_OF ""
set_interface_property ram PORT_NAME_MAP ""
set_interface_property ram CMSIS_SVD_VARIABLES ""
set_interface_property ram SVD_ADDRESS_GROUP ""

add_interface_port ram ram_address address Input 4
add_interface_port ram ram_readdata readdata Output 32
add_interface_port ram ram_writedata writedata Input 32
add_interface_port ram ram_write write Input 1
set_interface_assignment ram embeddedsw.configuration.isFlash 0
set_interface_assignment ram embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment ram embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment ram embeddedsw.configuration.isPrintableDevice 0


# 
# connection point timing
# 
add_interface timing avalon end
set_interface_property timing addressUnits WORDS
set_interface_property timing associatedClock CLK
set_interface_property timing associatedReset RESET
set_interface_property timing bitsPerSymbol 8
set_interface_property timing burstOnBurstBoundariesOnly false
set_interface_property timing burstcountUnits WORDS
set_interface_property timing explicitAddressSpan 0
set_interface_property timing holdTime 0
set_interface_property timing linewrapBursts false
set_interface_property timing maximumPendingReadTransactions 0
set_interface_property timing maximumPendingWriteTransactions 0
set_interface_property timing readLatency 0
set_interface_property timing readWaitTime 1
set_interface_property timing setupTime 0
set_interface_property timing timingUnits Cycles
set_interface_property timing writeWaitTime 0
set_interface_property timing ENABLED true
set_interface_property timing EXPORT_OF ""
set_interface_property timing PORT_NAME_MAP ""
set_interface_property timing CMSIS_SVD_VARIABLES ""
set_interface_property timing SVD_ADDRESS_GROUP ""

add_interface_port timing timing_address address Input 6
add_interface_port timing timing_write write Input 1
add_interface_port timing timing_writedata writedata Input 16
add_interface_port timing timing_readdata readdata Output 16
set_interface_assignment timing embeddedsw.configuration.isFlash 0
set_interface_assignment timing embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment timing embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment timing embeddedsw.configuration.isPrintableDevice 0


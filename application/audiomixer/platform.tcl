# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Users\minji\Desktop\audiomixer_zed\application\audiomixer\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Users\minji\Desktop\audiomixer_zed\application\audiomixer\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {audiomixer}\
-hw {C:\Users\minji\Desktop\audiomixer_zed\design\audiomixer_design\audiomixer_design_1_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -fsbl-target {psu_cortexa53_0} -out {C:/Users/minji/Desktop/audiomixer_zed/application}

platform write
platform generate -domains 
platform active {audiomixer}

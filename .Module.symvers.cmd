cmd_/home/royk/Downloads/fold/bridge_module_kernel_data_structures/Module.symvers := sed 's/\.ko$$/\.o/' /home/royk/Downloads/fold/bridge_module_kernel_data_structures/modules.order | scripts/mod/modpost -m -a  -o /home/royk/Downloads/fold/bridge_module_kernel_data_structures/Module.symvers -e -i Module.symvers   -T -
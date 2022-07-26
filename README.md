# Communication between Kernel module and User space program
This repository contains two systems: i) A program name clientBridge (.h and .c) which is able to establish communication through ioctl with a
ii) Kernel module, bridge (.h and .c) which is in charge to attend different kinds of requests made by the clienBridge program.

I define a kind of protocol to manipulate lists, queues and stacks. The idea is that the program running in the user space (clientBridge)
sends information and commands to the module (of course, running in the kernel space) which will use the functionalities offered by the kernel to store the
information using the kernel data structures (Be cautious with the use of the memory). The protocol is the following (W - Write, R - Read, L - List,
Q - Queue and S - Stack):

  <br />#define BRIDGE_CREATE_Q _IO('p', 1)                     //Create a queue. 
  <br />#define BRIDGE_W_HIGH_PRIOR_Q _IOW('p', 2, char *). 
  <br />#define BRIDGE_W_MIDDLE_PRIOR_Q _IOW('p', 3, char *). 
  <br />#define BRIDGE_W_LOW_PRIOR_Q _IOW('p', 4, char *). 
  <br />#define BRIDGE_R_HIGH_PRIOR_Q _IOR('p', 5, char *). 
  <br />#define BRIDGE_R_MIDDLE_PRIOR_Q _IOR('p', 6, char *). 
  <br />#define BRIDGE_R_LOW_PRIOR_Q _IOR('p', 7, char *). 
  <br />#define BRIDGE_STATE_Q _IO('p', 8). 
  <br />#define BRIDGE_DESTROY_Q _IO('p', 9). 
  <br /> 
  <br />#define BRIDGE_CREATE_S _IO('p', 10)                    //Create a stack. 
  <br />#define BRIDGE_W_S _IOW('p', 11, char *). 
  <br />#define BRIDGE_R_S _IOR('p', 12, char *). 
  <br />#define BRIDGE_STATE_S _IO('p', 13). 
  <br />#define BRIDGE_DESTROY_S _IO('p', 14). 
  <br />  
  <br />#define BRIDGE_CREATE_L _IO('p', 15)                    //Create a list. 
  <br />#define BRIDGE_W_L _IOW('p', 16, char *). 
  <br />#define BRIDGE_R_L _IOR('p', 17, char *). 
  <br />#define BRIDGE_INVERT_L _IO('p', 18). 
  <br />#define BRIDGE_ROTATE_L _IOW('p', 19, int *). 
  <br />#define BRIDGE_CLEAN_L _IO('p', 20). 
  <br />#define BRIDGE_GREATER_VAL_L _IOR('p', 21, char *). 
  <br />#define BRIDGE_END_L _IO('p', 22). 
  <br />#define BRIDGE_CONCAT_L _IO('p', 23). 
  <br />#define BRIDGE_STATE_L _IO('p', 24). 
  <br />#define BRIDGE_DESTROY_L _IO('p', 25). 
  <br />  
  
I plan to evolve this repository while I learn to develope kernel modules. I am open to receive all the advices that you consider.

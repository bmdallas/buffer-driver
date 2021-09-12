Buffer Driver
=============

This is an example device driver that is implemented to work with the Linux kernel. It has been compiled against kernel version 5.4 and 5.14.

This driver supports read and write functionality as a character driver. It holds a small internal buffer in kernel space (1024 bytes). In the future this could be updated to be reallocated based on an IOCTL call.

Building
========
The build system for this project is make. To build the kernel module, ensure that you have the headers installed in /lib/modules/ and run 'make'.

Installing
==========
For now there is no install mechinism, but, if desired, the module can be loaded after building using 'insmod'. It can then be unloaded using 'rmmod'.

MODULE_NAME:=buffer-driver

# If we are running by kernel building system
ifneq ($(KERNELRELEASE),)
	$(MODULE_NAME)-objs := driver.o
	obj-m := $(MODULE_NAME).o
# If we running without kernel build system
else
	BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)
endif

all:
# run kernel build system to make module
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules
clean:
# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

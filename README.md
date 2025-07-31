## Custom Keyboard RGB Controller for Linux

This repository contains a kernel driver and a Python GUI application to control the RGB lighting of a KreoHive keyboard on Linux.
Had to reverse-engineer USB HID protocol by capturing USB traffic in USBPcap.


    * Linux Kernel Driver: A custom hid_driver that co-exists with the standard keyboard driver.

    * Direct Hardware Control: The driver sends raw HID Output Reports to the device to control colors(as of now).

    * Sysfs Interface: Creates a simple sysfs file (/sys/bus/hid/devices/.../rgb_color).

    * You can test the driver with the python script provided.

    * Warning - Is not secure boot compatible. Driver needs to be signed.


This driver is written for a specific keyboard(Kreo Hive RGB) with the following identifiers:

    Vendor ID: 320f

    Product ID: 5055

### Prerequisites

* A Linux system with kernel headers installed.
    - `On Debian/Ubuntu: sudo apt install linux-headers-$(uname -r) build-essential`
    - `On Fedora: sudo dnf install kernel-devel kernel-headers make gcc`

use `make` command to produce kernel module file
> my_rgb_driver.ko
> 

### Load the Kernel Driver

`sudo insmod my_rgb_driver.ko`

### Test it with python script

- `cd py_app`
- `sudo python3 rgb_app.py`

### To unload

`sudo rmmod my_rgb_driver`

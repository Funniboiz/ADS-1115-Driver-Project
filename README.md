# ADS-1115 Driver
Driver used for ADS-1115 Module
# Information 
The ADS1115 16-Bit 4-Channel I2C ADC signal conversion circuit is used to create 4 ADC (Analog to Digital Converter) channels with 16-bit resolution, which communicate with microcontrollers or embedded computers (Raspberry Pi) via a simple I2C interface using only 2 signal pins (Data, Clock). This circuit is applied to read signals from modules or sensors that receive analog signals with high accuracy, or to add additional ADC pins for circuits that only have digital signal pins.
# Connection Diagram
We use a potentiometer 100kΩ for testing.

![image](https://github.com/user-attachments/assets/6a32c212-bdd7-445a-8294-f3ec2c493e12)
# Usage
* Check if the Raspberry Pi has the necessary driver compiler tools installed
  1. Open the Terminal and run the command:
          _uname -r_
     * This will display the version of the Raspberry Pi operating system (e.g., 6.1.21+).
  2. Then, access the Raspberry Pi via VNC and go to:
        _ /lib/modules_
     * Check if there's a folder matching the kernel version you just noted (e.g., 6.1.21+). Open that folder and see if it contains a build folder.
     * If the build folder is missing, install the necessary kernel headers using the following commands:
         _sudo apt update
         sudo apt install raspberrypi-kernel-headers_
  3. Convert ads1115-overlay.dts to ads1115-overlay.dtbo
     * You can compile the Device Tree Source (.dts) file into a Device Tree Blob Overlay (.dtbo) using the following command:
         _dtc -I dts -O dtb -o ads1115-overlay.dtbo ads1115-overlay.dts_
     * Then Copy file ads1115-overlay.dtbo using the following command: sudo cp ads1115-overlay.dtbo /boot/overlays/
     * Make sure you run this command from the directory where your .dts file is located, and using the following command:
        sudo nano /boot/config.txt
     * Add the following line at the end of the file: dtoverlay=ads1115-overlay.
* Compile the Driver File into a Module.
  * This method requires hardware.
  1. Create a folder using the following command: mkdir -p ~/projects/ads1115.
  2. Then using the following command: cd ~/projects/ads1115.
  3. Copy file Makefile, ads1115_test.c, ads1115_ioctl.c, ads1115-overlay.dts file and place it in ads1115 folder.
  4. Run the command:
      _make_
     * This will automatically compile ads1115_ioctl.c into a .ko file (kernel object file), which can then be loaded into the operating system and is ready to run.
* Loading the Driver
     * Once the _.ko file_ is generated, load it into the operating system using the command:
          _sudo insmod ads1115_ioctl.ko_
     * To check if the driver has been loaded successfully, you can use the following command:
          _dmesg_
     * Check whether the driver has appeared or not. Command: ls /dev.
     * After that, use the following command to check whether ads1115 is listed among the loaded modules:
          _lsmod_
     * Run file test:
         gcc ads1115_test.c -o test
         ./test
     * If you want to remove the driver module, use:
          _sudo rmmod ads1115_ioctl_
     * Then using the following the command: _make clean_
     
 # Reference
  https://how2electronics.com/how-to-use-ads1115-16-bit-adc-module-with-raspberry-pi/
  https://www.ti.com/sitesearch/en-us/docs/universalsearch.tsp?langPref=en-US&nr=609&searchTerm=ads1115#q=ads1115
  



     

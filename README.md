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
          uname -r
     This will display the version of the Raspberry Pi operating system (e.g., 6.1.21+).
  3. Then, access the Raspberry Pi via VNC and go to:
         /lib/modules
     Check if there's a folder matching the kernel version you just noted (e.g., 6.1.21+). Open that folder and see if it contains a build folder.
     If the build folder is missing, install the necessary kernel headers using the following commands:
         sudo apt update
         sudo apt install raspberrypi-kernel-headers
  4. Convert ads1115-overlay.dts to ads1115-overlay.dtbo
     You can compile the Device Tree Source (.dts) file into a Device Tree Blob Overlay (.dtbo) using the following command:
         dtc -I dts -O dtb -o ads1115-overlay.dtbo ads1115-overlay.dts
     Make sure you run this command from the directory where your .dts file is located.






     

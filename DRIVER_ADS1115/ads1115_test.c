#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/ads1115"  // Device file for the ADS1115
#define ADS1115_IOCTL_MAGIC 'a'     // Unique magic number for this driver
#define ADS1115_IOCTL_READ_CONVERSION _IOR(ADS1115_IOCTL_MAGIC, 1, int)  // IOCTL command to read conversion

int main() {
    int fd;
    int conversion_value;

    // Open the ADS1115 device file in read-only mode
    fd = open(DEVICE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");  // Print error if device cannot be opened
        return 1;
    }

    // Use ioctl to read the conversion value from the ADS1115
    if (ioctl(fd, ADS1115_IOCTL_READ_CONVERSION, &conversion_value) < 0) {
        perror("Failed to read conversion value");  // Print error if ioctl fails
        close(fd);
        return 1;
    }

    // Print the conversion value to the console
    printf("ADS1115 Conversion Value: %d\n", conversion_value);

    // Close the device file
    close(fd);
    return 0;
}

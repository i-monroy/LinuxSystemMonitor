#include <iostream>      // Standard C++ library for input and output
#include <sys/statvfs.h> // Include for the statvfs structure and statvfs() system call
#include <unistd.h>      // For sleep() function
#include <iomanip>     // For std::setprecision to format output

// Function to get disk usage statistics
void get_disk_usage(const char* path, double& total, double& used, double& free) {
    struct statvfs buffer;  // statvfs structure to store filesystem statistics
    if (statvfs(path, &buffer) != -1) {  // Perform the system call to fill the buffer with data
        total = buffer.f_blocks * buffer.f_frsize;  // Total blocks multiplied by size per block
        free = buffer.f_bfree * buffer.f_frsize;    // Free blocks multiplied by size per block
        used = total - free;                        // Used space is total minus free space

        // Convert from bytes to gigabytes for easier reading
        total /= (1024 * 1024 * 1024);
        used /= (1024 * 1024 * 1024);
        free /= (1024 * 1024 * 1024);
    } else {
        std::cerr << "Failed to get disk statistics for " << path << std::endl;  // Error handling
    }
}

// Function to display disk usage statistics
void display_disk_usage(double total, double used, double free) {
    std::cout << std::setprecision(2) << std::fixed;
    std::cout << "Total Disk Space: " << total << " GB" << std::endl;
    std::cout << "Used Disk Space: " << used << " GB" << std::endl;
    std::cout << "Free Disk Space: " << free << " GB" << std::endl;
}

// Main function to repeatedly check disk usage
int main() {
    while (true) {
        double total = 0, used = 0, free = 0;
        get_disk_usage("/", total, used, free); // Assuming we're looking at the root partition
        display_disk_usage(total, used, free);
        sleep(5);  // Pause for 5 seconds between updates
    }
    return 0;
}

#include <iostream>
#include <sys/statvfs.h>  // for statvfs structure and the statvfs() system call
#include <unistd.h>       // for sleep()

void get_disk_usage(const char* path, double& total, double& used, double& free) {
    struct statvfs buffer;
    if (statvfs(path, &buffer) != -1) {
        total = buffer.f_blocks * buffer.f_frsize;  // Total blocks * size of each block
        free = buffer.f_bfree * buffer.f_frsize;   // Free blocks * size of each block
        used = total - free;                       // Used is total - free
        // Convert from bytes to gigabytes
        total /= (1024 * 1024 * 1024);
        used /= (1024 * 1024 * 1024);
        free /= (1024 * 1024 * 1024);
    } else {
        std::cerr << "Failed to get disk statistics for " << path << std::endl;
    }
}

void display_disk_usage(double total, double used, double free) {
    std::cout << "Total Disk Space: " << total << " GB" << std::endl;
    std::cout << "Used Disk Space: " << used << " GB" << std::endl;
    std::cout << "Free Disk Space: " << free << " GB" << std::endl;
}

int main() {
    while (true) {
        double total = 0, used = 0, free = 0;
        get_disk_usage("/", total, used, free); // Assuming we're looking at the root partition
        display_disk_usage(total, used, free);
        sleep(5);  // Update every 5 seconds
    }
    return 0;
}
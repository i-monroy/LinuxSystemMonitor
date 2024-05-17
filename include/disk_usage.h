// disk_usage.h
#ifndef DISK_USAGE_H
#define DISK_USAGE_H

#include <iostream>
#include <sys/statvfs.h>
#include <string>
#include <iomanip>  // For std::setprecision to format output

// Class to encapsulate disk usage operations
struct DiskUsage {
public:
    // Function to get disk usage statistics
    static std::string get_disk_usage(const std::string& path) {
        struct statvfs buffer;  // statvfs structure to store filesystem statistics
        double total, used, free;

        if (statvfs(path.c_str(), &buffer) != -1) {  // Perform the system call to fill the buffer with data
            total = buffer.f_blocks * buffer.f_frsize;  // Total blocks multiplied by size per block
            free = buffer.f_bfree * buffer.f_frsize;    // Free blocks multiplied by size per block
            used = total - free;                        // Used space is total minus free space

            // Convert from bytes to gigabytes for easier reading
            total /= (1024 * 1024 * 1024);
            used /= (1024 * 1024 * 1024);
            free /= (1024 * 1024 * 1024);

            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "Total Disk Space: " << total << " GB, ";
            ss << "Used Disk Space: " << used << " GB, ";
            ss << "Free Disk Space: " << free << " GB";
            return ss.str();
        } else {
            return "Failed to get disk statistics";
        }
    }
};

#endif // DISK_USAGE_H

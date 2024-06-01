#ifndef DISK_USAGE_H
#define DISK_USAGE_H

#include <iostream>
#include <sys/statvfs.h>  
#include <string>
#include <sstream>     
#include <iomanip>     

// Class to encapsulate disk usage operations
class DiskUsage {
public:
    // Retrieves disk usage statistics for a specified path and returns formatted string.
    // This function calculates the disk space in terms of total, used, and free in Gigabytes.
    static std::string get_disk_usage(const std::string& path) {
        struct statvfs buffer;  // statvfs structure to store filesystem statistics
        double total, used, free;

        // Attempt to get file system statistics; on success, compute space usage
        if (statvfs(path.c_str(), &buffer) != -1) {
            total = static_cast<double>(buffer.f_blocks) * buffer.f_frsize;  // Total blocks multiplied by size per block
            free = static_cast<double>(buffer.f_bfree) * buffer.f_frsize;    // Free blocks multiplied by size per block
            used = total - free;                                            // Used space is total minus free space

            // Convert from bytes to gigabytes for easier readability
            total /= (1024 * 1024 * 1024);
            used /= (1024 * 1024 * 1024);
            free /= (1024 * 1024 * 1024);

            // Format and return the disk usage statistics as a string
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "Total Disk Space: " << total << " GB, ";
            ss << "Used Disk Space: " << used << " GB, ";
            ss << "Free Disk Space: " << free << " GB";
            return ss.str();
        } else {
            return "Failed to get disk statistics";  // Return error message if stats retrieval fails
        }
    }
};

#endif // DISK_USAGE_H

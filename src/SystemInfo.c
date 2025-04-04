#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <pwd.h>
#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#endif

void print_header(const char* title) {
    printf("\n\033[1;36m╔═════════════════════════════════════════════╗\033[0m\n");
    printf("\033[1;36m║\033[0m %-43s \033[1;36m║\033[0m\n", title);
    printf("\033[1;36m╚═════════════════════════════════════════════╝\033[0m\n\n");
}

void print_separator() {
    printf("\033[1;34m--------------------------------------------------\033[0m\n");
}

void print_info(const char* label, const char* value) {
    printf("\033[1;33m%-25s:\033[0m %s\n", label, value);
}

void print_network_info() {
    print_header("Network Information");

    #ifdef __APPLE__
    // Get interfaces and their IP addresses
    FILE *fp = popen("ifconfig | awk '/^[a-z]/ { iface=$1 } /inet / { print iface \": \" $2 }'", "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            line[strcspn(line, "\n")] = 0; // Remove newline
            printf("  • \033[1;36m%s\033[0m\n", line);
        }
        pclose(fp);
    }

    // Get gateway address
    fp = popen("route -n get default | grep gateway | awk '{print $2}'", "r");
    if (fp != NULL) {
        char gateway[256];
        if (fgets(gateway, sizeof(gateway), fp) != NULL) {
            gateway[strcspn(gateway, "\n")] = 0; // Remove newline
            printf("  \033[1;32mGateway:\033[0m %s\n", gateway);
        }
        pclose(fp);
    }
    printf("\n");
    #endif
}

int main() {
    struct utsname sysinfo;
    #ifdef __linux__
        struct sysinfo meminfo;
    #endif

    if (uname(&sysinfo) != 0) {
        perror("uname");
        exit(EXIT_FAILURE);
    }

    print_header("System Information");

    #ifdef __APPLE__
        // Get macOS version instead of Darwin
        FILE *fp = popen("sw_vers -productName", "r");
        if (fp != NULL) {
            char os_name[256];
            if (fgets(os_name, sizeof(os_name), fp) != NULL) {
                os_name[strcspn(os_name, "\n")] = 0;
                
                // Get version
                FILE *fp_ver = popen("sw_vers -productVersion", "r");
                if (fp_ver != NULL) {
                    char version[256];
                    if (fgets(version, sizeof(version), fp_ver) != NULL) {
                        version[strcspn(version, "\n")] = 0;
                        char full_os[512];
                        snprintf(full_os, sizeof(full_os), "%s %s", os_name, version);
                        print_info("Operating System", full_os);
                    }
                    pclose(fp_ver);
                }
            }
            pclose(fp);
        } else {
            print_info("Operating System", sysinfo.sysname);
        }
    #else
        print_info("Operating System", sysinfo.sysname);
    #endif

    print_info("Node Name", sysinfo.nodename);
    print_info("Release", sysinfo.release);
    
    // Extract kernel name and version only
    char *kernel_version = sysinfo.version;
    if (kernel_version != NULL) {
        char *version_start = strstr(kernel_version, "Version");
        if (version_start != NULL) {
            // Skip "Version" and any spaces
            version_start += 7;
            while (*version_start == ' ') version_start++;
            
            // Find the end of version number
            char *version_end = strchr(version_start, ':');
            if (version_end != NULL) {
                *version_end = '\0';
                
                // Create formatted string with just Darwin and version
                char formatted_version[256];
                snprintf(formatted_version, sizeof(formatted_version), 
                        "Darwin %s", version_start);
                print_info("Kernel", formatted_version);
            }
        } else {
            print_info("Kernel", kernel_version);
        }
    } else {
        print_info("Kernel", "Unknown");
    }
    
    print_info("Machine", sysinfo.machine);

    char hostname[1024];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        print_info("Hostname", hostname);
    } else {
        perror("gethostname");
    }

    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        print_info("Current User", pw->pw_name);
    } else {
        perror("getpwuid");
    }

    print_header("Memory Information");

    #ifdef __linux__
        if (sysinfo(&meminfo) != 0) {
            perror("sysinfo");
            exit(EXIT_FAILURE);
        }
        long total_ram = meminfo.totalram / 1024 / 1024;
        long free_ram = meminfo.freeram / 1024 / 1024;
        long used_ram = total_ram - free_ram;
        double used_percentage = (double)used_ram / total_ram * 100;
        printf("\033[1;33m%-25s:\033[0m %ld MB\n", "Total RAM", total_ram);
        printf("\033[1;33m%-25s:\033[0m %ld MB (%.2f%%)\n", "Used RAM", used_ram, used_percentage);
        printf("\033[1;33m%-25s:\033[0m %ld MB\n", "Free RAM", free_ram);
        printf("\033[1;33m%-25s:\033[0m %ld seconds\n", "Uptime", meminfo.uptime);
    #elif __APPLE__
        int mib[2];
        int64_t physical_memory;
        size_t length;

        mib[0] = CTL_HW;
        mib[1] = HW_MEMSIZE;
        length = sizeof(physical_memory);
        if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == 0) {
            mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
            vm_statistics_data_t vmstat;
            if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count) == KERN_SUCCESS) {
                int64_t free_memory = (vmstat.free_count + vmstat.inactive_count) * sysconf(_SC_PAGESIZE);
                int64_t used_memory = physical_memory - free_memory;
                double used_percentage = (double)used_memory / physical_memory * 100;
                printf("\033[1;33m%-25s:\033[0m %lld MB\n", "Total RAM", physical_memory / 1024 / 1024);
                printf("\033[1;33m%-25s:\033[0m %lld MB (%.2f%%)\n", "Used RAM", used_memory / 1024 / 1024, used_percentage);
                printf("\033[1;33m%-25s:\033[0m %lld MB\n", "Free RAM", free_memory / 1024 / 1024);
            } else {
                perror("host_statistics");
            }
        } else {
            perror("sysctl");
        }

        int num_processors;
        mib[0] = CTL_HW;
        mib[1] = HW_NCPU;
        length = sizeof(num_processors);
        if (sysctl(mib, 2, &num_processors, &length, NULL, 0) == 0) {
            printf("\033[1;33m%-25s:\033[0m %d\n", "Number of Processors", num_processors);
        } else {
            perror("sysctl");
        }

        struct timeval boottime;
        size_t size = sizeof(boottime);
        mib[0] = CTL_KERN;
        mib[1] = KERN_BOOTTIME;
        if (sysctl(mib, 2, &boottime, &size, NULL, 0) == 0) {
            time_t bsec = boottime.tv_sec, csec = time(NULL);
            printf("\033[1;33m%-25s:\033[0m %ld seconds\n", "Uptime", csec - bsec);
        } else {
            perror("sysctl");
        }
    #else
        printf("\033[1;33m%-25s:\033[0m Not available on this platform\n", "Total RAM");
        printf("\033[1;33m%-25s:\033[0m Not available on this platform\n", "Uptime");
        printf("\033[1;33m%-25s:\033[0m Not available on this platform\n", "Number of Processors");
    #endif

    print_network_info();

    return 0;
}

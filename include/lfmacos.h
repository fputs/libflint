#ifndef LIBFLINT_MACOS_H
#define LIBFLINT_MACOS_H

#if defined(__APPLE__) || defined(__MACH__)

typedef struct {
    double total_user_time;
    double total_kernel_time;
    double last_total_consumed;
    double percent_cpu;

    uint64_t virtual_memory;
    uint64_t resident_memory;

    time_t timestamp;
    time_t last_timestamp;
} ProcessData;

ProcessData *new_ProcessData();
int update_process(pid_t pid, ProcessData *proc);

#endif /* defined(__APPLE__) || defined(__MACH__) */
#endif /* LIBFLINT_MACOS_H */

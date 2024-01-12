#include <libproc.h>
#include <time.h>
#include <mach/mach_time.h>
#include <stdlib.h>

#include "lfmacos.h"

#define NS_TO_SECONDS 1000000000.0
#define NEW_PROCESS_SENTINEL (-1.0)

ProcessData *new_ProcessData() {
    ProcessData *pd = malloc(sizeof(ProcessData));
    pd->last_total_consumed = NEW_PROCESS_SENTINEL;
    return pd;
}

int update_process(pid_t pid, ProcessData *proc) {
    struct proc_taskinfo taskinfo;
    const int r = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &taskinfo, PROC_PIDTASKINFO_SIZE);
    if (r != PROC_PIDTASKINFO_SIZE) {
        return 1;
    }

    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    const double ns_per_tick = (double)info.numer / (double)info.denom;

    time(&(proc->timestamp));
    proc->total_kernel_time = (taskinfo.pti_total_system * ns_per_tick) / NS_TO_SECONDS;
    proc->total_user_time = (taskinfo.pti_total_user * ns_per_tick) / NS_TO_SECONDS;
    proc->virtual_memory = taskinfo.pti_virtual_size;
    proc->resident_memory = taskinfo.pti_resident_size;

    if (proc->last_total_consumed != NEW_PROCESS_SENTINEL) {
        time_t t = proc->timestamp - proc->last_timestamp;
        proc->percent_cpu = 100.0 * (proc->total_user_time + proc->total_kernel_time - proc->last_total_consumed) / t;
    } else {
        proc->percent_cpu = 0.0;
    }

    proc->last_timestamp = proc->timestamp;
    proc->last_total_consumed = proc->total_kernel_time + proc->total_user_time;

    return 0;
}

#include <libproc.h>
#include <time.h>
#include <mach/mach_time.h>
#include <stdlib.h>
#include <math.h>

#include "lfmacos.h"

ProcessData *new_ProcessData() {
    ProcessData *pd = malloc(sizeof(ProcessData));
    pd->last_total_consumed = -1.0;
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
    proc->total_kernel_time = (taskinfo.pti_total_system * ns_per_tick) / 1000000000.0;
    proc->total_user_time = (taskinfo.pti_total_user * ns_per_tick) / 1000000000.0;
    proc->virtual_memory = taskinfo.pti_virtual_size;
    proc->resident_memory = taskinfo.pti_resident_size;

    if (proc->last_total_consumed > -1.0) {
        proc->percent_cpu = fabs(100.0 *
        (proc->total_user_time + proc->total_kernel_time - proc->last_total_consumed) /
        (proc->last_timestamp - proc->timestamp));
    } else {
        proc->percent_cpu = 0.0;
    }
    proc->last_timestamp = proc->timestamp;
    proc->last_total_consumed = proc->total_kernel_time + proc->total_user_time;

    return 0;
}

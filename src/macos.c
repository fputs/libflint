#if defined(__APPLE__) || defined(__MACH__)

#include <libproc.h>
#include <time.h>
#include <mach/mach_time.h>
#include <sys/errno.h>
#include <stdlib.h>

#include "lfmacos.h"

#define NS_TO_SECONDS 1000000000.0
#define NEW_PROCESS_SENTINEL (-1.0)

ProcessData *new_ProcessData() {
    ProcessData *pd = calloc(1, sizeof(ProcessData));
    if (!pd) return NULL;
    pd->last_total_consumed = NEW_PROCESS_SENTINEL;
    return pd;
}

void destroy_ProcessData(ProcessData *pd) {
    free(pd);
}

int update_process(pid_t pid, ProcessData *proc) {
    struct proc_taskinfo taskinfo;
    const int r = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &taskinfo, PROC_PIDTASKINFO_SIZE);
    if (r != PROC_PIDTASKINFO_SIZE) {
        return -1;
    }

    static mach_timebase_info_data_t info;
    static int info_init = 0;
    if (!info_init) {
        mach_timebase_info(&info);
        info_init = 1;
    }
    const double ns_per_tick = (double)info.numer / (double)info.denom;

    time(&(proc->timestamp));
    proc->total_kernel_time = (taskinfo.pti_total_system * ns_per_tick) / NS_TO_SECONDS;
    proc->total_user_time = (taskinfo.pti_total_user * ns_per_tick) / NS_TO_SECONDS;
    proc->virtual_memory = taskinfo.pti_virtual_size;
    proc->resident_memory = taskinfo.pti_resident_size;

    if (proc->last_total_consumed != NEW_PROCESS_SENTINEL) {
        time_t t = proc->timestamp - proc->last_timestamp;
        if (t > 0) {
            proc->percent_cpu = 100.0 * (proc->total_user_time + proc->total_kernel_time - proc->last_total_consumed) / t;
        } else {
            proc->percent_cpu = 0.0;
        }
    } else {
        proc->percent_cpu = 0.0;
    }

    proc->last_timestamp = proc->timestamp;
    proc->last_total_consumed = proc->total_kernel_time + proc->total_user_time;

    return 0;
}

/* reallocarray is reimplemented here for macOS because Apple doesn't expose
 * their implementation. This is taken straight from the OpenBSD source as
 * shown in the below copyright notice
 */

/*	$OpenBSD: reallocarray.c,v 1.2 2014/12/08 03:45:00 bcook Exp $	*/
/*
 * Copyright (c) 2008 Otto Moerbeek <otto@drijf.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* OPENBSD ORIGINAL: lib/libc/stdlib/reallocarray.c */

#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))

void *reallocarray(void *optr, size_t nmemb, size_t size)
{
    if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
        nmemb > 0 && SIZE_MAX / nmemb < size) {
        errno = ENOMEM;
        return NULL;
    }
    return realloc(optr, size * nmemb);
}

#endif /* defined(__APPLE__) || defined(__MACH__) */

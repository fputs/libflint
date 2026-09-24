Process CPU and memory sampling on macOS. The declarations in `lfmacos.h` are available only on Apple platforms, and CMake builds the implementation only there.

## Usage

Create a sample with `new_ProcessData()`, then call `update_process()` at intervals. The first successful update establishes a baseline and reports zero CPU usage.

```c
ProcessData *pd = new_ProcessData();
if (pd != NULL) {
    for (int i = 0; i < 10; i++) {
        if (update_process(getpid(), pd) != 0) {
            break;
        }
        printf("CPU: %.2f\n", pd->percent_cpu);
        sleep(1);
    }
    destroy_ProcessData(pd);
}
```

Call `destroy_ProcessData()` when finished to free the sample.

## Structs

### ProcessData

CPU and memory measurements for a process. Use the same process ID for successive updates of a sample.

```c
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
```

Members:

- `total_user_time`, `total_kernel_time`: Cumulative CPU time in seconds
- `last_total_consumed`: CPU time at the previous update, or `-1.0` before the first update
- `percent_cpu`: CPU time consumed divided by elapsed wall time, multiplied by 100; can exceed 100 for a process using multiple cores
- `virtual_memory`, `resident_memory`: Memory sizes in bytes
- `timestamp`, `last_timestamp`: Wall-clock timestamps used to measure the interval; both hold the current sample time after a successful update

## Functions

### new_ProcessData

Allocates a sample with `last_total_consumed` set to `-1.0` and all other fields zero. Returns `NULL` on allocation failure.

```c
ProcessData *new_ProcessData(void);
```

### destroy_ProcessData

Frees the sample. Safe to call with `NULL`.

```c
void destroy_ProcessData(ProcessData *pd);
```

### update_process

Updates CPU and memory measurements for `pid`. Returns `0` on success or `-1` if `proc` is NULL or the system queries fail.

CPU usage is measured over whole wall-clock seconds. The first update, or an update with no positive elapsed interval, reports zero CPU usage. Space updates at least one second apart to obtain a useful interval.

```c
int update_process(pid_t pid, ProcessData *proc);
```

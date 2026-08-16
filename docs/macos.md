Process CPU and memory sampling on macOS. The header and implementation are compiled only on Apple platforms:

```c
#if defined(__APPLE__) || defined(__MACH__)
/*
... code ...
*/
#endif
```
## Structs

### ProcessData

ProcessData is a struct that tracks cpu and memory usage of a process. It needs to be regularly updated with calls to `update_process()` in order to provide accurate information

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

## Functions

### new_ProcessData

Returns a pointer to a zero-initialized `ProcessData` struct. Returns `NULL` on allocation failure.

```c
ProcessData *new_ProcessData();
```

### destroy_ProcessData

Frees a `ProcessData` struct. Safe to call with `NULL`.

```c
void destroy_ProcessData(ProcessData *pd);
```

### update_process

Updates a `ProcessData` struct. This should be called in a loop or at specific intervals to measure proper usage data. Returns `0` on success, `-1` on error (e.g. invalid PID). An example is below

```c
int update_process(pid_t pid, ProcessData *proc);

/* Example */
pid_t pid = getpid();
ProcessData *pd = new_ProcessData();
for (int i = 0; i < 10; i++) {
    update_process(pid, pd);
    printf("CPU: %.2f\n", pd->percent_cpu);
    sleep(1);
}
destroy_ProcessData(pd);
```
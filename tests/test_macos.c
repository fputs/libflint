#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lftest.h"

#if defined(__APPLE__) || defined(__MACH__)
#include "lfmacos.h"

int main() {
    pid_t pid = getpid();

    /* new_ProcessData returns zeroed memory with sentinel */
    ProcessData *pd = new_ProcessData();
    ASSERT_NOT_NULL(pd);
    ASSERT_EQ(pd->percent_cpu, 0.0);
    ASSERT_EQ(pd->virtual_memory, 0);
    ASSERT_EQ(pd->resident_memory, 0);
    ASSERT_EQ(pd->total_user_time, 0.0);
    ASSERT_EQ(pd->total_kernel_time, 0.0);

    /* first update succeeds and populates fields */
    int rc = update_process(pid, pd);
    ASSERT_EQ(rc, 0);
    ASSERT_TRUE(pd->resident_memory > 0);
    ASSERT_TRUE(pd->virtual_memory > 0);
    ASSERT_TRUE(pd->percent_cpu == 0.0);

    sleep(1);

    /* second update succeeds */
    rc = update_process(pid, pd);
    ASSERT_EQ(rc, 0);
    ASSERT_TRUE(pd->percent_cpu >= 0.0);

    /* error path: invalid PID returns -1 */
    rc = update_process(-1, pd);
    ASSERT_EQ(rc, -1);

    destroy_ProcessData(pd);

    /* destroy_ProcessData handles NULL safely */
    destroy_ProcessData(NULL);

    TEST_REPORT();
}
#else
int main() {
    printf("Skipped (not macOS)\n");
    return 0;
}
#endif

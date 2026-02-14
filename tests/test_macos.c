#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lftest.h"

#if defined(__APPLE__) || defined(__MACH__)
#include "lfmacos.h"

int main() {
    pid_t pid = getpid();
    ProcessData *pd = new_ProcessData();
    ASSERT_NOT_NULL(pd);

    for (int i = 0; i < 2; i++) {
        update_process(pid, pd);
        sleep(1);
    }
    ASSERT_TRUE(pd->percent_cpu >= 0.0);

    free(pd);
    TEST_REPORT();
}
#else
int main() {
    printf("Skipped (not macOS)\n");
    return 0;
}
#endif

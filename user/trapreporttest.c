#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "kernel/spinlock.h"
#include "kernel/proc.h"
#include "user/user.h"

char *getProcState(enum procstate state);  // Renamed for uniqueness

int main(int argc, char *argv[]) {
    for (int iteration = 0; iteration < 2; iteration++) {
        int pid1 = fork();
        if (pid1 == 0) {
            int pid2 = fork();
            if (pid2 == 0) {
                int *null_ptr = 0;
                *null_ptr = 4;  // Trigger a trap
                sleep(4);
            } else {
                // Trigger a trap in the parent process
                int *null_ptr = 0;
                *null_ptr = 4;
                sleep(3);
            }
            exit(0);
        }
    }

    sleep(5);  // Give time for child processes to trap

    struct report_traps trapReports;
    int error_code = trapreport(&trapReports);
    printf("Traps from child processes (%d):\nPID\tPNAME\tscause\tsepc\tstval\n", trapReports.count);

    for (struct report *report = trapReports.reports; report < &trapReports.reports[trapReports.count]; report++) {
        printf("%d\t%s\t%ld\t%ld\t%ld\n", report->pid, report->pname, report->scause, report->sepc, report->stval);
    }

    // Wait for all child processes to finish
    wait((int *)0);
    wait((int *)0);

    exit(error_code);
}

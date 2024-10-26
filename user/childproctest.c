// C Program to Track Child Processes
#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "kernel/spinlock.h"
#include "kernel/proc.h"
#include "user/user.h"

// Custom function to get process state as a string
char* getProcState(enum procstate state);

int main(int argc, char *argv[]) {
    // Loop to create two process chains
    for (int loopIdx = 0; loopIdx < 2; loopIdx++) {
        int child1 = fork();
        if (child1 == 0) {
            int child2 = fork();
            if (child2 == 0) {
                sleep(4);  // Inner child sleeps
            } else {
                // Wait for inner child and then continue
                wait((int *)0);
                sleep(3);  // Mid-level child sleeps
            }
            exit(0);
        }
    }

    // Give the parent process a chance to gather child info
    sleep(1);

    struct child_processes childInfo;
    int statusCode = childproc(&childInfo);
    printf("List of child processes (%d):\nPID\tPPID\tSTATE\tNAME\n", childInfo.count);

    for (int idx = 0; idx < childInfo.count; idx++) {
        printf("%d\t%d\t%s\t%s\n", 
               childInfo.processes[idx].pid, 
               childInfo.processes[idx].ppid, 
               getProcState(childInfo.processes[idx].state), 
               childInfo.processes[idx].name);
    }

    // Wait for all first-level child processes
    wait((int*)0);
    wait((int*)0);

    exit(statusCode);
    return statusCode;
}

// Function to convert enum procstate to a readable string
char* getProcState(enum procstate state) {
    switch (state) {
        case UNUSED:   return "UNUSED";
        case USED:     return "USED";
        case SLEEPING: return "SLEEPING";
        case RUNNABLE: return "RUNNABLE";
        case RUNNING:  return "RUNNING";
        case ZOMBIE:   return "ZOMBIE";
        default:       return "UNKNOWN";
    }
}

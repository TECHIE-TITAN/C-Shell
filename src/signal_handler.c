#include "header.h"

/* ###################################### LLM Generated Code Begins ###################################### */

void handle_sigchld(int signo) {
    int status;
    pid_t pid;
    
    // Only handle background jobs - avoid consuming foreground processes
    // This prevents race conditions with the executor's waitpid calls
    for (int i = 0; i < job_count; i++) {
        pid = waitpid(job_list[i].pid, &status, WNOHANG | WUNTRACED);
        if (pid == job_list[i].pid) {
            if (WIFEXITED(status)) {
                printf("\n[%d] %s with pid %d exited normally\n", job_list[i].job_number, job_list[i].command, job_list[i].pid);
                remove_job(pid);
                i--; // Adjust index after removal
            } else if (WIFSIGNALED(status)) {
                printf("\n[%d] %s with pid %d exited abnormally\n", job_list[i].job_number, job_list[i].command, job_list[i].pid);
                remove_job(pid);
                i--; // Adjust index after removal
            } else if (WIFSTOPPED(status)) {
                job_list[i].status = 1; // Mark as stopped
                printf("\n[%d] Stopped %s\n", job_list[i].job_number, job_list[i].command);
            }
        }
    }
    
    // Re-register the handler for the next signal
    signal(SIGCHLD, handle_sigchld);
}

void handle_sigint(int signo) {
    if (foreground_pid > 0) {
        // Send SIGINT to the entire process group
        kill(-foreground_pid, SIGINT);
    }
    // Re-register the handler for the next signal
    signal(SIGINT, handle_sigint);
}

void handle_sigtstp(int signo) {
    if (foreground_pid > 0) {
        // Send SIGTSTP to the entire process group
        kill(-foreground_pid, SIGTSTP);
        
        // Update job status to 'Stopped' and add to the job list
        Job* job = get_job_by_pid(foreground_pid);
        if (job) {
            job->status = 1; // Mark as stopped
            printf("\n[%d] Stopped %s\n", job->job_number, job->command);
        }
        foreground_pid = 0; // Clear foreground PID
    }
    // Re-register the handler for the next signal
    signal(SIGTSTP, handle_sigtstp);
}

// This function sets up all the signal handlers for the shell.
// It uses the simpler signal() function as requested.
void setup_signal_handlers() {
    // Set up SIGINT handler for Ctrl+C
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal for SIGINT failed");
    }

    // Set up SIGTSTP handler for Ctrl+Z
    if (signal(SIGTSTP, handle_sigtstp) == SIG_ERR) {
        perror("signal for SIGTSTP failed");
    }

    // Set up SIGCHLD handler to reap completed background processes
    if (signal(SIGCHLD, handle_sigchld) == SIG_ERR) {
        perror("signal for SIGCHLD failed");
    }
}

/* ###################################### LLM Generated Code Endss ###################################### */
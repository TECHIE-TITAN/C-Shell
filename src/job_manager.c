#include "header.h"

/* ###################################### LLM Generated Code Begins ###################################### */

void add_job(pid_t pid, const char* command, int status) {
    if (job_count < MAX_JOBS) {
        job_list[job_count].pid = pid;
        strcpy(job_list[job_count].command, command);
        job_list[job_count].status = status;
        job_list[job_count].job_number = ++latest_job_number;
        job_count++;
    }
}

void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (job_list[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) {
                job_list[j] = job_list[j+1];
            }
            job_count--;
            break;
        }
    }
}

Job* get_job_by_pid(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (job_list[i].pid == pid) {
            return &job_list[i];
        }
    }
    return NULL;
}

Job* get_job_by_number(int job_number) {
    for (int i = 0; i < job_count; i++) {
        if (job_list[i].job_number == job_number) {
            return &job_list[i];
        }
    }
    return NULL;
}

void update_job_status(pid_t pid, int status) {
    Job* job = get_job_by_pid(pid);
    if (job) {
        job->status = status;
    }
}

/* ###################################### LLM Generated Code Ends ###################################### */

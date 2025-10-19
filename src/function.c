#include "header.h"

void hop_function(TokenStream* ts){
    if(!strcmp(ts->Tokens[ts->pos].token, "END")){
        getcwd(prev_dir, MAX_LENGTH);
        chdir(home_dir);
    } else if(!strcmp(ts->Tokens[ts->pos].token, "~")){
        getcwd(prev_dir, MAX_LENGTH);
        chdir(home_dir);
    } else if(!strcmp(ts->Tokens[ts->pos].token, ".")){
        // Do nothing
    } else if(!strcmp(ts->Tokens[ts->pos].token, "..")){
        getcwd(prev_dir, MAX_LENGTH);
        chdir("..");
    } else if(!strcmp(ts->Tokens[ts->pos].token, "-")){
        char current_dir[MAX_LENGTH];
        getcwd(current_dir, MAX_LENGTH);
        if (strlen(prev_dir) == 0) {
            printf("No previous directory!\n");
        } else {
            chdir(prev_dir);
            strcpy(prev_dir, current_dir);
        }
    } else{
        getcwd(prev_dir, MAX_LENGTH);
        if (chdir(ts->Tokens[ts->pos].token) == -1) {
            printf("No such directory!\n");
        }
    }
}

void reveal_function(TokenStream* ts){
    int show_hidden = 0;
    int show_line_by_line = 0;

    // Parse flags
    while(ts->Tokens[ts->pos].type == NAME && ts->Tokens[ts->pos].token[0] == '-'){
        int index = 1;
        while(ts->Tokens[ts->pos].token[index] != '\0'){
            if(ts->Tokens[ts->pos].token[index] == 'a'){
                show_hidden = 1;
            } else if(ts->Tokens[ts->pos].token[index] == 'l'){
                show_line_by_line = 1;
            }
            index++;
        }
        ts->pos++;
    }

    // Determine target directory
    char folder[MAX_LENGTH];
    if(!strcmp(ts->Tokens[ts->pos].token, "END") || !strcmp(ts->Tokens[ts->pos].token, ".")){
        getcwd(folder, MAX_LENGTH);
    } else if(!strcmp(ts->Tokens[ts->pos].token, "~")){
        strcpy(folder, home_dir);
    } else if(!strcmp(ts->Tokens[ts->pos].token, "..")){
        char cwd[MAX_LENGTH];
        getcwd(cwd, MAX_LENGTH);
        char* parent = strrchr(cwd, '/');
        if (parent == cwd) {
            strcpy(folder, "/");
        } else if (parent) {
            strncpy(folder, cwd, parent - cwd);
            folder[parent-cwd] = '\0';
        } else {
            // Not absolute path
            strcpy(folder, cwd);
        }
    } else if(!strcmp(ts->Tokens[ts->pos].token, "-")){
        if (strlen(prev_dir) == 0) {
            printf("No such directory!\n");
            return;
        }
        strcpy(folder, prev_dir);
    } else{
        strcpy(folder, ts->Tokens[ts->pos].token);
    }

    if(ts->Tokens[ts->pos+1].type == NAME){
        printf("reveal: Invalid Syntax!\n");
        return;
    }

    DIR* dir = opendir(folder);
    if(!dir){
        printf("No such directory!\n");
        return;
    }

    struct dirent* entry;
    char* files[1024];
    int count = 0;

    // Collect entries
    while((entry = readdir(dir)) != NULL){
        if(!show_hidden && entry->d_name[0] == '.') 
            continue; // skip hidden files
        files[count] = strdup(entry->d_name); 
        count++;
    }
    closedir(dir);

    // Sort lexicographically (ASCII order)
    for(int i=0;i<count-1;i++){
        for(int j=i+1;j<count;j++){
            if(strcmp(files[i], files[j]) > 0){
                char* tmp = files[i];
                files[i] = files[j];
                files[j] = tmp;
            }
        }
    }

    // Print
    for(int i=0;i<count;i++){
        if(show_line_by_line) printf("%s\n", files[i]);
        else printf("%s ", files[i]);
        free(files[i]); // free strdup'd strings
    }
    if(!show_line_by_line && count > 0) printf("\n");
}

void log_function(TokenStream* ts){
    if(!strcmp(ts->Tokens[ts->pos].token, "END")){
        log_print();
    } else if(!strcmp(ts->Tokens[ts->pos].token, "purge")){
        log_purge();
    } else if(!strcmp(ts->Tokens[ts->pos].token, "execute")){
        ts->pos++;
        log_execute(ts->Tokens[ts->pos].token);
    } else {
        printf("log: Invalid Syntax!\n");
    }
}

/* ###################################### LLM Generated Code Begins ###################################### */

void activities_function() {
    // Sort the job list lexicographically by command name
    for (int i = 0; i < job_count - 1; i++) {
        for (int j = i + 1; j < job_count; j++) {
            if (strcmp(job_list[i].command, job_list[j].command) > 0) {
                Job temp = job_list[i];
                job_list[i] = job_list[j];
                job_list[j] = temp;
            }
        }
    }
    
    // Print the sorted list
    for (int i = 0; i < job_count; i++) {
        printf("[%d] : %s - %s\n", job_list[i].job_number, job_list[i].command, job_list[i].status ? "Stopped" : "Running");
    }
}

void ping_function(TokenStream* ts) {
    if (ts->Tokens[ts->pos].type != NAME || ts->Tokens[ts->pos + 1].type != NAME) {
        printf("ping: Invalid Syntax!\n");
        return;
    }
    
    pid_t pid = (pid_t)strToInt(ts->Tokens[ts->pos].token);
    int signal_number = strToInt(ts->Tokens[ts->pos + 1].token) % 32;

    if (kill(pid, 0) == -1) { // Check if process exists
        if (errno == ESRCH) {
            printf("No such process found\n");
            return;
        }
    }

    if (kill(pid, signal_number) == 0) {
        printf("Sent signal %d to process with pid %d\n", signal_number, pid);
    } else {
        perror("ping failed");
    }
}

void fg_function(TokenStream* ts) {
    int job_number = -1;
    if (ts->Tokens[ts->pos].type == NAME) {
        job_number = strToInt(ts->Tokens[ts->pos].token);
    } else {
        // Use most recent job
        if (job_count > 0) {
            job_number = job_list[job_count - 1].job_number;
        }
    }

    Job* job = get_job_by_number(job_number);
    if (!job) {
        printf("No such job\n");
        return;
    }

    printf("%s\n", job->command);
    if (job->status == 1) { // If stopped, continue
        kill(job->pid, SIGCONT);
        update_job_status(job->pid, 0); // Mark as running
    }
    
    foreground_pid = job->pid;
    int status;
    waitpid(job->pid, &status, WUNTRACED);
    
    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        remove_job(job->pid);
    } else if (WIFSTOPPED(status)) {
        update_job_status(job->pid, 1); // Mark as stopped
        printf("\n[%d] Stopped %s\n", job->job_number, job->command);
    }
}

void bg_function(TokenStream* ts) {
    int job_number = -1;
    if (ts->Tokens[ts->pos].type == NAME) {
        job_number = strToInt(ts->Tokens[ts->pos].token);
    } else {
        if (job_count > 0) {
            job_number = job_list[job_count - 1].job_number;
        }
    }

    Job* job = get_job_by_number(job_number);
    if (!job) {
        printf("No such job\n");
        return;
    }

    if (job->status == 0) {
        printf("Job already running\n");
        return;
    }
    
    kill(job->pid, SIGCONT);
    update_job_status(job->pid, 0); // Mark as running
    printf("[%d] %s &\n", job->job_number, job->command);
}

/* ###################################### LLM Generated Code Ends ###################################### */
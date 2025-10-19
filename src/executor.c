#include "header.h"

/* ###################################### LLM Generated Code Begins ###################################### */

// Checks for completed background processes and prints a status message
void check_background_jobs() {
    int status;
    pid_t pid;
    for (int i = 0; i < job_count; i++) {
        pid = waitpid(job_list[i].pid, &status, WNOHANG);
        if (pid == job_list[i].pid) {
            // Job has completed
            printf("[%d] Done %s\n", job_list[i].job_number, job_list[i].command);
            remove_job(pid);
            i--; // Adjust index after removal
        }
    }
}

/* ###################################### LLM Generated Code Ends ###################################### */

// Executes a single command group, which may include pipes and I/O redirection
int execute_command_group(TOKEN* Tokens, int start_pos, int end_pos, int is_background) {
    // Check for special intrinsic commands first
    if (!strcmp(Tokens[start_pos].token, "hop")) {
        // printf("%d %d\n", start_pos, end_pos);
        for(int i=1; i<end_pos; i++){
            TokenStream ts = {Tokens, i};
            hop_function(&ts);
        }
        return 0;
    } else if (!strcmp(Tokens[start_pos].token, "reveal")) {
        TokenStream ts = {Tokens, start_pos + 1};
        reveal_function(&ts);
        return 0;
    } else if (!strcmp(Tokens[start_pos].token, "log")) {
        TokenStream ts = {Tokens, start_pos + 1};
        log_function(&ts);
        return 0;
    } else if (!strcmp(Tokens[start_pos].token, "activities")) {
        activities_function();
        return 0;
    } else if (!strcmp(Tokens[start_pos].token, "ping")) {
        TokenStream ts = {Tokens, start_pos + 1};
        ping_function(&ts);
        return 0;
    } else if (!strcmp(Tokens[start_pos].token, "fg")) {
        TokenStream ts = {Tokens, start_pos + 1};
        fg_function(&ts);
        return 0;
    } else if (!strcmp(Tokens[start_pos].token, "bg")) {
        TokenStream ts = {Tokens, start_pos + 1};
        bg_function(&ts);
        return 0;
    }

/* ###################################### LLM Generated Code Begins ###################################### */

    int pipe_count = 0;
    for (int i = start_pos; i < end_pos; i++) {
        if (Tokens[i].type == PIPE) {
            pipe_count++;
        }
    }
    
    int pipes[pipe_count][2];
    for (int i = 0; i < pipe_count; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            return 1;
        }
    }
    
    int cmd_start = start_pos;
    int pipe_index = 0;
    pid_t pids[pipe_count + 1];
    
    for (int i = start_pos; i <= end_pos; i++) {
        if (i == end_pos || Tokens[i].type == PIPE) {
            char *args[MAX_LENGTH];
            char *input_file = NULL;
            char *output_file = NULL;
            TokenType output_type = NAME;
            
            int arg_pos = 0;
            int current_cmd_end = i;
            
            // Parse arguments and redirections for the current command
            for (int j = cmd_start; j < current_cmd_end; j++) {
                if (Tokens[j].type == READ) {
                    input_file = Tokens[j+1].token;
                    j++;
                } else if (Tokens[j].type == OVERWRITE || Tokens[j].type == APPEND) {
                    output_type = Tokens[j].type;
                    output_file = Tokens[j+1].token;
                    j++;
                } else {
                    args[arg_pos++] = Tokens[j].token;
                }
            }
            args[arg_pos] = NULL;
            
            if (arg_pos == 0) continue; // Skip if no command name
            
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork failed");
                return 1;
            }
            
            if (pid == 0) { // Child process
                // Create a new process group for the child
                setpgid(0, 0);
                
                // Handle I/O redirection
                // printf("Child Process ID: %d\n", getpid());
                if (input_file) {
                    int fd = open(input_file, O_RDONLY);
                    if (fd == -1) {
                        perror("open input file failed");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                if (output_file) {
                    int fd;
                    if (output_type == OVERWRITE) {
                        fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    } else {
                        fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    }
                    if (fd == -1) {
                        perror("open output file failed");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                
                // Handle pipes
                if (pipe_index > 0) { // Not the first command
                    dup2(pipes[pipe_index-1][0], STDIN_FILENO);
                }
                if (pipe_index < pipe_count) { // Not the last command
                    dup2(pipes[pipe_index][1], STDOUT_FILENO);
                }
                
                // Close all pipe file descriptors in the child
                for (int j = 0; j < pipe_count; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                
                execvp(args[0], args);
                
                // perror("execvp failed");
                // printf("Invalid Command!\n");
                // exit(127);
                _exit(127);
            } else if(pid>0) { // Parent process
                // show_shell_prompt();
                // wait(NULL); // Wait for the child to prevent zombies
                // printf("Parent Process ID: %d\n", getpid());
                pids[pipe_index] = pid;
                cmd_start = i + 1;
                pipe_index++;
            }
        }
    }
    
    // Parent closes all pipe descriptors
    for (int i = 0; i < pipe_count; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all child processes in the pipeline
    if (!is_background) {
        for (int i = 0; i < pipe_count + 1; i++) {
            foreground_pid = pids[i];
            int status;
            waitpid(pids[i], &status, 0);
        }
        foreground_pid = 0; // Clear foreground PID after all processes complete
    } else {
        // Add all jobs in the pipeline to the background job list
        char full_cmd[MAX_LENGTH] = "";
        for (int i = start_pos; i < end_pos; i++) {
            strcat(full_cmd, Tokens[i].token);
            strcat(full_cmd, " ");
        }
        for (int i = 0; i < pipe_count + 1; i++) {
            add_job(pids[i], full_cmd, 0);
            printf("[%d] %d\n", latest_job_number, pids[i]);
        }
    }

    return 0;
}

// Top-level execute function to handle sequential and background commands
void execute(TOKEN* Tokens) {
    int current_pos = 0;
    while (Tokens[current_pos].type != END) {
        int next_group_start = current_pos;
        int next_separator = -1;
        int is_background = 0;

        // Find the next separator token (SEMICOLON, BACKGROUND, or END)
        while (Tokens[next_group_start].type != SEMICOLON && Tokens[next_group_start].type != BACKGROUND && Tokens[next_group_start].type != END) {
            next_group_start++;
        }
        
        next_separator = next_group_start;
        
        // Handle background token
        if (Tokens[next_separator].type == BACKGROUND) {
            is_background = 1;
            next_separator++; // Move past the '&'
        }
        
        // Execute the command group
        execute_command_group(Tokens, current_pos, next_group_start, is_background);
        
        if (Tokens[next_separator].type != END) {
            current_pos = next_separator + 1;
        } else {
            current_pos = next_separator;
        }
    }
}

/* ###################################### LLM Generated Code Ends ###################################### */

#include "header.h"

char* home_dir = NULL;
char* prev_dir = NULL;
volatile sig_atomic_t foreground_pid = 0;

logger* log_tracker = NULL;
Job job_list[MAX_JOBS];
int job_count = 0;
int latest_job_number = 0;

int main(){
    home_dir = (char*)malloc(MAX_LENGTH * sizeof(char));
    prev_dir = (char*)calloc(MAX_LENGTH, sizeof(char));
    getcwd(home_dir, MAX_LENGTH);
    prev_dir[0] = '\0';
    char command[MAX_LENGTH];
    
    log_tracker = (logger*)malloc(sizeof(logger));
    load_log_data();
    
    setup_signal_handlers();

    while(1){
        check_background_jobs();
        show_shell_prompt();
        fflush(stdout);
        
        if(fgets(command, sizeof(command), stdin) == NULL) {
            // Handle EOF or error
            if (feof(stdin)) {
                printf("logout\n");
                // Kill all child processes before exiting
                for(int i = 0; i < job_count; i++){
                    kill(job_list[i].pid, SIGKILL);
                }
                exit(0);
            } else if (ferror(stdin)) {
                perror("fgets error");
                clearerr(stdin);
                continue;
            }
        }
        
        // Handle Ctrl-D (EOF) - this check is now redundant but keeping for safety
        if (feof(stdin)) {
            printf("logout\n");
            // Kill all child processes before exiting
            for(int i = 0; i < job_count; i++){
                kill(job_list[i].pid, SIGKILL);
            }
            exit(0);
        }
        
        TOKEN* Tokens = (TOKEN*)malloc(256 * sizeof(TOKEN));
        tokeniser(command, Tokens);
        
        if(Tokens[0].type == END){
            free(Tokens);
            continue;
        }
        
        // Debug output - remove after fixing
        // fprintf(stderr, "DEBUG: Processing command: %s\n", command);
        // fflush(stderr);
        // int i = 0;
        // while(Tokens[i].type != END){
        //     printf("Token: %s, Type: %d\n", Tokens[i].token, Tokens[i].type);
        //     i++;
        // }

        if(parse(Tokens)){
            execute(Tokens);
            if(strcmp(Tokens[0].token, "log") != 0 && (log_tracker->latest_log_index == -1 || strcmp(command, log_tracker->logs[log_tracker->latest_log_index]))){
                log_into_file(command);
            }
        } else {
            printf("Invalid Syntax!\n");
        }
        // show_shell_prompt();
        // fflush(stdout);
        free(Tokens);
    }
    return 0;
}

#ifndef HEADER
#define HEADER

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h> // Now using signal() which is defined here

#define MAX_LENGTH 1024
#define MAX_LOGS 15
#define MAX_JOBS 256

// Global variables for shell state
extern char* home_dir;
extern char* prev_dir;
extern volatile sig_atomic_t foreground_pid;

/* Struct Definitions */
typedef enum {
    NAME, 
    PIPE, 
    OVERWRITE,
    APPEND,
    READ,
    INPUT,
    OUTPUT,
    AND,        // Corresponds to &&
    SEMICOLON,  // Corresponds to ;
    BACKGROUND, // Corresponds to &
    END
} TokenType;

typedef struct{
    TokenType type;
    char token[MAX_LENGTH];
} TOKEN;

typedef struct{
    TOKEN* Tokens;
    int pos;
} TokenStream;

typedef struct{
    FILE* file;
    FILE* logs_stat_file;
    char logs[MAX_LOGS][MAX_LENGTH];
    int log_total;
    int latest_log_index;
} logger;

// Struct to hold information about a background or stopped job
typedef struct {
    pid_t pid;
    char command[MAX_LENGTH];
    int job_number;
    int status; // 0 for running, 1 for stopped
} Job;

extern logger* log_tracker;
extern Job job_list[MAX_JOBS];
extern int job_count;
extern int latest_job_number;

/* prompter.c */
void show_shell_prompt();

/* utility.c */
void skip_spaces(char** cmd_ptr);
int strToInt(char* str);

/* tokeniser.c */
void add_token(TOKEN* Tokens, int token_count, TokenType type, char* cmd_ptr, int len);
void tokeniser(char* command, TOKEN* Tokens);

/* parser.c */
int parse(TOKEN* Tokens);
int parse_shell_cmd(TokenStream* ts);
int parse_cmd_group(TokenStream* ts);
int parse_atomic(TokenStream* ts);

/* executor.c */
void execute(TOKEN* Tokens);
int execute_command_group(TOKEN* Tokens, int start_pos, int end_pos, int is_background);
void check_background_jobs();

/* function.c */
void hop_function(TokenStream* ts);
void reveal_function(TokenStream* ts);
void log_function(TokenStream* ts);
void log_purge();
void log_execute(char* index_str);
void activities_function();
void ping_function(TokenStream* ts);
void fg_function(TokenStream* ts);
void bg_function(TokenStream* ts);

/* log_ops.c */
int isFileEmpty(const char *filename);
void load_log_data();
void log_into_file(char* command);
void log_print();
void log_purge();
void log_execute(char* index_str);

/* signal_handler.c */
// Now only contains the handlers, as setup is in main.c
void handle_sigchld(int signo);
void handle_sigint(int signo);
void handle_sigtstp(int signo);
void setup_signal_handlers();

/* job_manager.c */
void add_job(pid_t pid, const char* command, int status);
void remove_job(pid_t pid);
Job* get_job_by_pid(pid_t pid);
Job* get_job_by_number(int job_number);
void update_job_status(pid_t pid, int status);

#endif

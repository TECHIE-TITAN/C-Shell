#include "header.h"

void show_shell_prompt(){
    char* cwd = (char*)calloc(MAX_LENGTH, sizeof(char));
    char* username = getenv("USER");

    char hostname[MAX_LENGTH];
    gethostname(hostname, sizeof(hostname));

    getcwd(cwd, MAX_LENGTH);

    if(!strncmp(cwd, home_dir, strlen(home_dir))){
        printf("<%s@%s:~%s> ", username, hostname, cwd + strlen(home_dir));
    }else{
        printf("<%s@%s:%s> ", username, hostname, cwd);
    }

    fflush(stdout);
}

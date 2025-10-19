#include "header.h"

int isFileEmpty(const char *filename) {
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fclose(file);

    return (fileSize == 0);
}

void load_log_data(){
    char str[MAX_LENGTH];
    char logs_stat_filepath[MAX_LENGTH];
    char logs_filepath[MAX_LENGTH];
    strcpy(logs_stat_filepath, home_dir);
    strcpy(logs_filepath, home_dir);

    strcat(logs_stat_filepath, "/data/logs_stat.txt");
    strcat(logs_filepath, "/data/logs.txt");

    if(isFileEmpty(logs_stat_filepath)){
        log_tracker->log_total = 0;
        log_tracker->latest_log_index = -1;
    } else {
        log_tracker->logs_stat_file = fopen(logs_stat_filepath, "r");
        fgets(str, MAX_LENGTH, log_tracker->logs_stat_file);
        str[strcspn(str, "\n")] = '\0';
        log_tracker->log_total = strToInt(str);
        fgets(str, MAX_LENGTH, log_tracker->logs_stat_file);
        str[strcspn(str, "\n")] = '\0';
        log_tracker->latest_log_index = strToInt(str);
        fclose(log_tracker->logs_stat_file);
    }

    if(!isFileEmpty(logs_filepath)){
        log_tracker->file = fopen(logs_filepath, "r");
        for(int i=0;i<log_tracker->log_total;i++){
            fgets(str, MAX_LENGTH, log_tracker->file);
            strcpy(log_tracker->logs[i], str);
        }
        fclose(log_tracker->file);
    }
}

void log_into_file(char* command){
    if(log_tracker->latest_log_index == 14){
        log_tracker->latest_log_index = -1;
    }
    strcpy(log_tracker->logs[++log_tracker->latest_log_index], command);

    if(log_tracker->log_total != 15){
        log_tracker->log_total++;
    }

    char logs_stat_filepath[MAX_LENGTH];
    char logs_filepath[MAX_LENGTH];
    strcpy(logs_stat_filepath, home_dir);
    strcpy(logs_filepath, home_dir);

    strcat(logs_stat_filepath, "/data/logs_stat.txt");
    strcat(logs_filepath, "/data/logs.txt");

    // Create data directory if it doesn't exist
    char data_dir[MAX_LENGTH];
    strcpy(data_dir, home_dir);
    strcat(data_dir, "/data");
    mkdir(data_dir, 0755);

    log_tracker->logs_stat_file = fopen(logs_stat_filepath, "w");
    if (log_tracker->logs_stat_file) {
        fprintf(log_tracker->logs_stat_file, "%d\n%d\n", log_tracker->log_total, log_tracker->latest_log_index);
        fclose(log_tracker->logs_stat_file);
    }

    log_tracker->file = fopen(logs_filepath, "w");
    if (log_tracker->file) {
        for(int i=0;i<log_tracker->log_total;i++){
            fprintf(log_tracker->file, "%s", log_tracker->logs[i]);
        }
        fclose(log_tracker->file);
    }
}

void log_print(){
    if (log_tracker->log_total == 0) return;
    int i = (log_tracker->latest_log_index + 1) % log_tracker->log_total;
    int count = 0;
    while(count < log_tracker->log_total){
        printf("%s", log_tracker->logs[i]);
        i = (i + 1) % log_tracker->log_total;
        count++;
    }
}

void log_purge(){
    char logs_stat_filepath[MAX_LENGTH];
    char logs_filepath[MAX_LENGTH];
    strcpy(logs_stat_filepath, home_dir);
    strcpy(logs_filepath, home_dir);

    strcat(logs_stat_filepath, "/data/logs_stat.txt");
    strcat(logs_filepath, "/data/logs.txt");

    log_tracker->logs_stat_file = fopen(logs_stat_filepath, "w");
    fprintf(log_tracker->logs_stat_file, "0\n-1\n");
    fclose(log_tracker->logs_stat_file);

    log_tracker->file = fopen(logs_filepath, "w");
    fclose(log_tracker->file);
    log_tracker->log_total = 0;
    log_tracker->latest_log_index = -1;
}

void log_execute(char* index_str){
    int index = strToInt(index_str);
    if (index <= 0 || index > log_tracker->log_total) {
        printf("log: execute: Index out of range\n");
        return;
    }

    int log_index = (log_tracker->latest_log_index - (index - 1) + log_tracker->log_total) % log_tracker->log_total;
    
    char command[MAX_LENGTH];
    strcpy(command, log_tracker->logs[log_index]);
    
    TOKEN* log_tokens = (TOKEN*)malloc(256*sizeof(TOKEN));
    tokeniser(command, log_tokens);
    if(parse(log_tokens)){
        execute(log_tokens);
    } else {
        printf("Invalid Syntax!\n");
    }
    free(log_tokens);
}

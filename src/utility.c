#include "header.h"

void skip_spaces(char** cmd_ptr){
    while(**cmd_ptr == ' '){
        (*cmd_ptr)++;
    }
}

int strToInt(char* str){
    int num = 0;
    char ch = '\0';
    if(*str == '-'){
        ch = *str;
        str++;
    }
    while(*str != '\0'){
        num*=10;
        num = num + (*str - '0');
        str++;
    }
    return (ch=='\0') ? num : (-1)*num;
}
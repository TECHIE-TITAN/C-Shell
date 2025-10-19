#include "header.h"

void add_token(TOKEN* Tokens, int token_count, TokenType type, char* cmd_ptr, int len){
    Tokens[token_count].type = type;
    strncpy(Tokens[token_count].token, cmd_ptr, len);
    Tokens[token_count].token[len] = '\0';
}

void tokeniser(char* command, TOKEN* Tokens){
    int token_count = 0;
    char* cmd_ptr = command;
    char* start = NULL;
    while(*cmd_ptr && *cmd_ptr!='\n'){
        skip_spaces(&cmd_ptr);
        if (*cmd_ptr == '\0') break; // Handle end of string after skipping spaces
        
        switch(*cmd_ptr){
            case '|':   add_token(Tokens, token_count, PIPE, cmd_ptr, 1);
                        token_count++;
                        cmd_ptr++;
                        break;
            case '>':   if(*(cmd_ptr+1) == '>'){
                            add_token(Tokens, token_count, APPEND, cmd_ptr, 2);
                            token_count++;
                            cmd_ptr+=2;
                        } else{
                            add_token(Tokens, token_count, OVERWRITE, cmd_ptr, 1);
                            token_count++;
                            cmd_ptr++;
                        }
                        break;
            case '<':   add_token(Tokens, token_count, READ, cmd_ptr, 1);
                        token_count++;
                        cmd_ptr++;
                        break;
            case '&':   add_token(Tokens, token_count, BACKGROUND, cmd_ptr, 1);
                        token_count++;
                        cmd_ptr++;
                        break;
            case ';':   add_token(Tokens, token_count, SEMICOLON, cmd_ptr, 1);
                        token_count++;
                        cmd_ptr++;
                        break;
            default:    start = cmd_ptr;
                        while(*cmd_ptr && *cmd_ptr!=' ' && *cmd_ptr!='\n' && *cmd_ptr!='|' && *cmd_ptr!='&' && *cmd_ptr!='>' && *cmd_ptr!='<' && *cmd_ptr!=';'){
                            cmd_ptr++;
                        }
                        if (start == cmd_ptr) { // To prevent infinite loop on invalid character
                            cmd_ptr++;
                            break;
                        }
                        add_token(Tokens, token_count, NAME, start, cmd_ptr-start);
                        token_count++;
                        break;
        }
    }
    add_token(Tokens, token_count, END, "END", 3);
}

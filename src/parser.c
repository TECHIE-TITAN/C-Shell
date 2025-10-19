#include "header.h"

int parse_atomic(TokenStream* ts){
    if((ts->Tokens[ts->pos++].type) != NAME){
        return 0;
    }

    while(ts->Tokens[ts->pos].type != PIPE && ts->Tokens[ts->pos].type != SEMICOLON && ts->Tokens[ts->pos].type != BACKGROUND && ts->Tokens[ts->pos].type != END){
        if((ts->Tokens[ts->pos].type) == NAME){
            ts->pos++;
            continue;
        } else if((ts->Tokens[ts->pos].type) == READ){
            ts->pos++;
            if((ts->Tokens[ts->pos].type) != NAME){
                return 0;
            }
        } else if((ts->Tokens[ts->pos].type) == OVERWRITE || (ts->Tokens[ts->pos].type) == APPEND){
            ts->pos++;
            if((ts->Tokens[ts->pos].type) != NAME){
                return 0;
            }
        } else {
            // Invalid token
            return 0;
        }
        ts->pos++;
    }

    return 1;
}

int parse_cmd_group(TokenStream* ts){
    if(!parse_atomic(ts)){
        return 0;
    }

    while((ts->Tokens[ts->pos]).type == PIPE){
        ts->pos++;
        if(!parse_atomic(ts)){
            return 0;
        }
    }
    return 1;
}

int parse_shell_cmd(TokenStream* ts){
    if(!parse_cmd_group(ts)){
        return 0;
    }

    while(ts->Tokens[ts->pos].type == SEMICOLON || ts->Tokens[ts->pos].type == BACKGROUND){
        TokenType separator = ts->Tokens[ts->pos].type;
        ts->pos++;
        if (separator == BACKGROUND && ts->Tokens[ts->pos].type == END) {
            return 1; // Trailing & is valid
        }
        if(!parse_cmd_group(ts)){
            return 0;
        }
    }
    
    // Check if there is a single '&' at the end
    if (ts->Tokens[ts->pos].type == END) {
        return 1;
    } else if (ts->Tokens[ts->pos].type == BACKGROUND && ts->Tokens[ts->pos + 1].type == END) {
        ts->pos++;
        return 1;
    } else {
        return 0;
    }
}

int parse(TOKEN* Tokens){
    TokenStream ts = {Tokens, 0};
    if(parse_shell_cmd(&ts)){
        return 1;
    } else return 0;
}

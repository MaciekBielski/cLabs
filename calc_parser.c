/* glib 2.4 */
/* TODO:
 * [+] describe Token type,
 * [+] newToken, freeToken
 * [+] stack- first version,
 * [+] popStack, pushStack,
 * [+] tokenization- part1, no logic, all pushed on stack,
 * [+] operator levels assignement,
 * [-] operator workers assignement
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <glib.h>

#define OP_SET "+-*/="
#define OP_L1 "+-="
#define OP_L2 "*/"

typedef enum{ OP, DIG } TokenType;
typedef struct Token
{
    TokenType type;
    double val;
    uint8_t level;
    double (*work)(int lVal, int rVal);
}Token;

/* Create a new Token object from provided character.
 * Numbers have level 0, operators have level >0
 */
static Token* newToken(char c)
{
    Token *out = (Token*) malloc(sizeof(*out));
    if(isdigit(c))
    {
        *out = (Token){
            .type=DIG,
            .val = (double)atoi(&c),
            .work = NULL,
        };
    }
    else
    {
        *out = (Token){
            .type=OP,
            .val = (double)c,
            .level= ( strchr(OP_L1,c) ? 1 : ( strchr(OP_L2,c) ? 2 : 255)),
            .work=NULL,
        };
    }
    return out;
}

static void freeToken(Token *ptr)
{
    free(ptr);
}

static Token* tokenStack[50];
static size_t stackIdx=0;

static void pushStack(const Token * const t)
{
    tokenStack[stackIdx++] = (Token*)t;
}

static Token* popStack()
{
    stackIdx--;
    if(0==stackIdx)
        return NULL;
    Token *out = tokenStack[stackIdx];
    tokenStack[stackIdx] = NULL;
    return out;
}

static void printStack()
{
    printf("idx: %lu\n", stackIdx);
    for(size_t i=0; i<stackIdx; i++)
    {
        Token *t = tokenStack[i];
        printf("%lu: ",i);
        if(!t)
        {
            printf("--NULL--\n");
            continue;
        }
        switch(t->type)
        {
            case OP: printf("type: OP, val: [%c],", (char)t->val); break;
            case DIG: printf("type: DIG, val: %lf,", t->val); break;

        }
        printf("level: %u\n", t->level);
    }
    puts("-----------------------");
}

static bool isValid(char c)
{
    return (bool)strchr(OP_SET, c) ? true : isdigit(c);
}

static void parseChar(char c)
{
    static bool fragmDigit=false;
    while(!isValid(c));
        //wait asynchronously for the next character
    if(isdigit(c))
    {
        if(fragmDigit)
        {
            Token *t = popStack();
            t->val = t->val*10 + atoi(&c);
            pushStack(t);
        }
        else
        {
            fragmDigit=true;
            Token *tok = newToken(c);
            pushStack(tok);
        }
    }
    else
    {
        fragmDigit=false;
        Token *t = newToken(c);
        pushStack(t);
    }
    printStack();
}

const char *test="3-44*2+9/1/3+10-5=";
int main(int argc, char**argv)
{
    for(const char *ptr=test; *ptr; ptr++)
    {
        parseChar(*ptr);
    }
    return 0;
}

__attribute__((constructor)) void initParser()
{
    tokenStack[0]=NULL;
    stackIdx=1;
}

__attribute__((destructor)) void destroyParser()
{
    Token *t=NULL;
    size_t i=0;
    while(t=popStack())
    {
        freeToken(t);
        i++;
    }
    printf("\ndestroyed: %lu\n", i);
    printStack();
}
    //printf("isValid: %u\n", isValid('+'));

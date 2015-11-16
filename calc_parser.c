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
#include <assert.h>

#define OP_SET "+-*/="
#define OP_L1 "+-="
#define OP_L2 "*/"

typedef enum{ OP, DIG } TokenType;
typedef struct Token
{
    TokenType type;
    double val;
    uint8_t level;
    double (*work)(double l, double r);
}Token;

static Token* new_token(char c);
static inline void free_token(Token *ptr) { free(ptr); ptr=NULL; }
static void push_stack(const Token * const t);
static Token* pop_stack();
static void print_stack();
static bool is_valid(char c);

static GHashTable *work_ops = NULL;
static Token* token_stack[50];
static Token* init_op = NULL;
static Token* null_token = NULL;
static size_t stack_idx=0;

static void parse_char(char c)
{
    static bool fragm_digit=false;
    //initial previous token
    static Token *prev_op = NULL;
    static uint8_t curr_level = 0;
    while(!is_valid(c));
        //wait asynchronously for the next character
    if(isdigit(c))
    {
        Token *tok = NULL;
        if(fragm_digit)
        {
            tok = pop_stack();
            tok->val = tok->val*10 + atoi(&c);
        }
        else
        {
            fragm_digit=true;
            tok = new_token(c);
        }
        push_stack(tok);
    }
    else
    {
        fragm_digit=false;
        print_stack();
        if(prev_op==NULL)
        {
            puts("prev_op init");
            prev_op = init_op;
            curr_level = init_op->level;
        }
        Token *next_op = new_token(c);
        printf("\tprev_op: %c, level: %d\n", (char)prev_op->val, prev_op->level);
        printf("\tnext_op: %c, level: %d\n", (char)next_op->val, next_op->level);
        while( next_op->level <= prev_op->level )
        {
            printf("\tR: prev_op: %c, level: %d\n", (char)prev_op->val, prev_op->level);
            printf("\tR: next_op: %c, level: %d\n", (char)next_op->val, next_op->level);
            Token *r_tok = pop_stack();
            Token *l_tok = pop_stack();
            assert(r_tok->level==0 && l_tok->level==0);
            Token *tmp = new_token('0');
            tmp->val = prev_op->work(l_tok->val, r_tok->val);
            printf("\tR: tmp val: %lf, level: %d\n", tmp->val, tmp->level);
            prev_op = pop_stack();
            if(prev_op->val == (double)'\0')
            {
                push_stack(prev_op);
                puts("null pop-push");
            }
            push_stack(tmp);
            free_token(r_tok);
            free_token(l_tok);
            print_stack();
            if( prev_op->val == (double)'\0')
            {
                //reduced completely on the left
                puts("left side completely reduced");
                prev_op = next_op;
                curr_level = prev_op->level;
                if(prev_op->val==(double)'=')
                    puts("end");
                return;
            }
            puts("further reduction");
            //else reduce further to the left
        }
        if( next_op->level > prev_op->level )
        {
            puts("\t Order exchange");
            Token *tmp = pop_stack();
            push_stack(prev_op);
            push_stack(tmp);
            //update
            prev_op = next_op;
            curr_level = prev_op->level;
        }
    }
}

//const char *test="3-44*2+9/1/3+10-5=";
const char *test="66/13+44*6-39-1005+33*7=";    //-543.923077
int main(int argc, char**argv)
{
    for(const char *ptr=test; *ptr; ptr++)
    {
        parse_char(*ptr);
    }
    Token *ret = pop_stack();
    printf("RESULT: %lf\n", ret->val);
    free_token(ret);
    return 0;
}

static double op_add(double l_val, double r_val) { return l_val+r_val; }
static double op_sub(double l_val, double r_val) { return l_val-r_val; }
static double op_mul(double l_val, double r_val){ return l_val*r_val; }
static double op_div(double l_val, double r_val)
{
    if(r_val!=0)
        return l_val/r_val;
}

__attribute__((constructor)) void init_parser()
{
    work_ops = g_hash_table_new(g_direct_hash, NULL);
    g_hash_table_insert(work_ops, GINT_TO_POINTER('+'), op_add);
    g_hash_table_insert(work_ops, GINT_TO_POINTER('-'), op_sub);
    g_hash_table_insert(work_ops, GINT_TO_POINTER('*'), op_mul);
    g_hash_table_insert(work_ops, GINT_TO_POINTER('/'), op_div);
    g_hash_table_insert(work_ops, GINT_TO_POINTER('\0'), NULL);
    null_token = new_token('\0');
    token_stack[0]=null_token;
    token_stack[1]=new_token('0');
    stack_idx=2;
    init_op = new_token('+');
}

__attribute__((destructor)) void destroy_parser()
{
    Token *t=NULL;
    /* in fact, stack should contain only null_token if the result has been
     * popped at the end*/
    for(;;)
    {
        t=pop_stack();
        if(t->val == (double)'\0')
            break;
        free_token(t);
    }
    free_token(init_op);
    free_token(null_token);
    g_hash_table_destroy(work_ops);
}
    //printf("isValid: %u\n", isValid('+'));
static Token* new_token(char c)
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
            .work=(double (*)(double, double)) g_hash_table_lookup(work_ops, GINT_TO_POINTER(c)),
        };
    }
    return out;
}

static void push_stack(const Token * const t)
{
    token_stack[stack_idx++] = (Token*)t;
}

static Token* pop_stack()
{
    Token *out = token_stack[--stack_idx];
    token_stack[stack_idx] = NULL;
    return out;
}

static void print_stack()
{
    /* TODO: this function uses 'raw' access to the stack, it should be changed */
    printf("idx: %lu\n", stack_idx);
    for(size_t i=0; i<stack_idx; i++)
    {
        Token *t = token_stack[i];
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

static bool is_valid(char c)
{
    return (bool)strchr(OP_SET, c) ? true : isdigit(c);
}

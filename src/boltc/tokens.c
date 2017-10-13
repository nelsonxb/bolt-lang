
#include <stdlib.h>
#include <stdint.h>

#include "tokens.h"


struct token_list_node {
    token_t *token;
    struct token_list_node *next;
};

struct token_list {
    struct token_list_node *first;
    struct token_list_node *last;
    token_t *prev;
};

struct tokenizer {
    struct token_list tokens;
};


tkn_t *tkn_new()
{
    tkn_t *t = malloc(sizeof (t));
    t->tokens.first = NULL;
    t->tokens.last = NULL;
    t->tokens.prev = NULL;
    return t;
}

void tkn_free(tkn_t *t)
{
    free(t);
}

token_t *tkn_next(tkn_t *t)
{
    if (t->tokens.prev) {
        free(t->tokens.prev);
        t->tokens.prev = NULL;
    }
    struct token_list_node *node = t->tokens.first;
    if (node) {
        token_t *token = node->token;
        t->tokens.first = node->next;
        free(node);
        t->tokens.prev = token;
        return token;
    } else {
        return NULL;
    }
}

token_t *tkn_push(tkn_t *t, token_type_t type)
{
    struct token_list_node *node = malloc(sizeof (node));
    node->token = malloc(sizeof (node->token));
    node->next = NULL;

    if (!t->tokens.first) {
        t->tokens.first = node;
    }

    if (t->tokens.last) {
        t->tokens.last->next = node;
    } else {
        t->tokens.last = node;
    }

    t->tokens.last = node;

    token_t *token = node->token;
    token->header.type = type;

    if (type >= TOKEN_VALUE && type < _TOKEN_VALUE_MAX)
        token->header.category = TOKEN_CATEG_VALUE;
    else if (type == TOKEN_IDENT)
        token->header.category = TOKEN_CATEG_IDENT;
    else if (type >= TOKEN_KW && type < _TOKEN_KW_MAX)
        token->header.category = TOKEN_CATEG_KEYWORD;
    else if (type >= TOKEN_KWV && type < _TOKEN_KWV_MAX)
        token->header.category = TOKEN_CATEG_KEYWORD | TOKEN_CATEG_VALUE;
    else if (type >= TOKEN_KWI && type < _TOKEN_KWI_MAX)
        token->header.category = TOKEN_CATEG_KEYWORD | TOKEN_CATEG_IDENT;
    else if (type >= TOKEN_OP && type < _TOKEN_OP_MAX)
        token->header.category = TOKEN_CATEG_OPERATOR;
    else
        token->header.category = 0;

    return token;
}

token_t *tkn_push_comment(tkn_t *t, const char *comment)
{
    token_t *token = tkn_push(t, TOKEN_COMMENT);
    token->comment.body = comment;
    return token;
}

token_t *tkn_push_vint(
    tkn_t *t, long long value, int size, enum _token_vint_repr repr
)
{
    if (size == 0 && value) {
        if (value > 0)
            size = value <= INT8_MAX ? 1
                : value <= INT16_MAX - 1 ? 2
                : value <= INT32_MAX - 1 ? 4
                : value <= INT64_MAX - 1 ? 8
                : -1;
        else
            size = value >= INT8_MIN ? 1
                : value >= INT16_MIN ? 2
                : value >= INT32_MIN ? 4
                : value >= INT64_MIN ? 8
                : -1;
    }

    token_t *token = tkn_push(t, TOKEN_VINT);
    token->vint.value = value;
    token->vint.size = size;
    token->vint.repr = repr;
    return token;
}

token_t *tkn_push_vfloat(tkn_t *t, long double value)
{
    token_t *token = tkn_push(t, TOKEN_VFLOAT);
    token->vfloat.value = value;
    return token;
}

token_t *tkn_push_vstring(tkn_t *t, size_t length, const char *value)
{
    token_t *token = tkn_push(t, TOKEN_VSTRING);
    token->vstring.length = length;
    token->vstring.data = value;
    return token;
}

token_t *tkn_push_ident(tkn_t *t, const char *ident)
{
    token_t *token = tkn_push(t, TOKEN_IDENT);
    token->ident.ident = ident;
    return token;
}

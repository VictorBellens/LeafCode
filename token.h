#ifndef TOKEN_H
#define TOKEN_H

// Token types
typedef enum {
    BLOOM, WITHER, PETAL, VINE, TREE, STUMP, LILLY,
    WATER, NUM, VAR, POPPY, ROSE, BUSH, SPROUT, SHED, BRANCH, DECAY, EQUAL,
    NOTEQUAL, RIGHTCROC, LEFTCROC, MYCENA, PLANT, POT,
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char *lexeme;
} Token;

#endif

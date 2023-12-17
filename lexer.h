#ifndef LEXER_H
#define LEXER_H

#include "token.h"

Token processWord(const char *word);
const char *getNameOfToken(TokenType type);
Token tokenizeWord(const char *word);

#endif

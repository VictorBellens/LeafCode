#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "token.h"

// Function to get the name of a token based on its type
const char *getNameOfToken(TokenType type) {
    static const char *names[] = {
        "BLOOM", "WITHER", "PETAL", "VINE", "TREE", "STUMP", 
        "LILLY", "WATER", "NUM", "VAR", "POPPY", "ROSE", 
        "BUSH", "SPROUT", "SHED", "BRANCH", "DECAY", "EQUAL", 
        "NOTEQUAL", "RIGHTCROC", "LEFTCROC", "MYCENA", 
        "LEGGI", "BASTA", "PLANT", "POT"
    };

    return (type >= BLOOM && type <= POT) ? names[type] : "UNKNOWN";
}

// Function to determine if a string is a keyword
TokenType identifyKeyword(const char *word) {
    static struct {
        const char *keyword;
        TokenType type;
    } keywords[] = {
        {"BLOOM", BLOOM}, {"wither", WITHER}, {"petal", PETAL}, 
        {"vine", VINE}, {"tree", TREE}, {"stump", STUMP}, 
        {":", LILLY}, {"\"", WATER}, {"Poppy", POPPY}, 
        {"Rose", ROSE}, {"Bush", BUSH}, {"sprout", SPROUT}, 
        {"shed", SHED}, {"branch", BRANCH}, {"decay", DECAY}, 
        {"=", EQUAL}, {"!=", NOTEQUAL}, {"rightcroc", RIGHTCROC}, 
        {"leftcroc", LEFTCROC}, {"stampa", MYCENA}, 
        {"plant", PLANT}, {"pot", POT}, {NULL, VAR}
    };

    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(word, keywords[i].keyword) == 0) {
            return keywords[i].type;
        }
    }
    return strspn(word, "0123456789") == strlen(word) ? NUM : VAR;
}



// Function to convert a word to a token
Token processWord(const char *word) {
    Token token;
    token.type = identifyKeyword(word);
    token.lexeme = strdup(word);
    return token;
}

// Main lexer function
void processInput(const char *inputStr) {
    char *inputCopy = strdup(inputStr);
    const char *separators = " \t\n";
    for (char *word = strtok(inputCopy, separators); word; word = strtok(NULL, separators)) {
        Token token = processWord(word);
        printf("Lexeme: %s, TokenType: %s, TokenValue: %d\n", token.lexeme, getNameOfToken(token.type), token.type);
    }
    free(inputCopy);
}

int runLexerDemo() {
    const char *testInput = "Bloom Leaf Thorn Petal Wither";
    processInput(testInput);
    return 0;
}
// int main() { return runLexerDemo(); }

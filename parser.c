#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"

// AST Node types
typedef enum {
    NodeTypeProgram,
    NodeTypeExp,
    NodeTypeOp,
    NodeTypeLit,
    NodeTypeVar,
    NodeTypePrint,
} NodeType;

// AST Node structure
typedef struct TreeNode {
    NodeType type;
    Token token; // Token associated with the node (for literals and operators)
    struct TreeNode *left; // Left child
    struct TreeNode *right; // Right child
} TreeNode;

// Function to create a new AST node
TreeNode* createNode(NodeType type, Token token) {
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for TreeNode\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->token = token;
    node->left = NULL;
    node->right = NULL;
    return node;
}

//////////// Parse functions ////////////
TreeNode* parseExpression(Token tokens[], int *currentToken);
TreeNode* parseTerm(Token tokens[], int *currentToken);
TreeNode* parseFactor(Token tokens[], int *currentToken);
TreeNode* parsePrint(Token tokens[], int *currentToken); // Added for MYCENA

// Parse a literal
TreeNode* parseLiteral(Token tokens[], int *currentToken) {
    Token literalToken = tokens[(*currentToken)++];

    if (literalToken.type == NUM) {
        return createNode(NodeTypeLit, literalToken);
    } else {
        return createNode(NodeTypeVar, literalToken);
    }
}

// Parse an entire program
TreeNode* parseProgram(Token tokens[]) {
    int currentToken = 0;
    return parseExpression(tokens, &currentToken);
}

// Parse an expression
TreeNode* parseExpression(Token tokens[], int *currentToken) {
    if (tokens[*currentToken].type == MYCENA) {
        return parsePrint(tokens, currentToken);
    }
    return parseTerm(tokens, currentToken);
}

// Parse a print statement
TreeNode* parsePrint(Token tokens[], int *currentToken) {
    (*currentToken)++; // Skip the MYCENA token
    TreeNode *valueToPrint = parseExpression(tokens, currentToken);
    TreeNode *printNode = createNode(NodeTypePrint, (Token){MYCENA, strdup("MYCENA")});
    printNode->left = valueToPrint;
    return printNode;
}

// Parse a term (handles addition and subtraction)
TreeNode* parseTerm(Token tokens[], int *currentToken) {
    TreeNode *left = parseFactor(tokens, currentToken);

    while (tokens[*currentToken].type == SPROUT || tokens[*currentToken].type == SHED) {
        Token operatorToken = tokens[(*currentToken)++];
        TreeNode *right = parseFactor(tokens, currentToken);

        TreeNode *operationNode = createNode(NodeTypeOp, operatorToken);
        operationNode->left = left;
        operationNode->right = right;

        left = operationNode;
    }

    return left;
}

// Parse a factor (handles multiplication and division)
TreeNode* parseFactor(Token tokens[], int *currentToken) {
    TreeNode *left = parseLiteral(tokens, currentToken);

    while (tokens[*currentToken].type == BRANCH || tokens[*currentToken].type == DECAY) {
        Token operatorToken = tokens[(*currentToken)++];
        TreeNode *right = parseLiteral(tokens, currentToken);

        TreeNode *operationNode = createNode(NodeTypeOp, operatorToken);
        operationNode->left = left;
        operationNode->right = right;

        left = operationNode;
    }

    return left;
}



// Print AST
void printAST(TreeNode *root) {
    if (root == NULL) {
        return;
    }

    switch (root->type) {
        case NodeTypeProgram:
        case NodeTypeExp:
            printAST(root->left);
            break;

        case NodeTypeOp:
            printf("(");
            printAST(root->left);
            printf(" %s ", getNameOfToken(root->token.type));
            printAST(root->right);
            printf(")");
            break;

        case NodeTypeLit:
            printf("%s", root->token.lexeme);
            break;

        case NodeTypeVar:
            printf(" | var:  %s", root->token.lexeme);
            break;

        case NodeTypePrint:
            printf("MYCENA(");
            printAST(root->left);  // Print the content of the expression
            printf(")");
            break;

        default:
            printf("Node Unrecognized");
    }
}

// Print AST with indentation
void printASTIndented(TreeNode* root, int level) {
    if (root == NULL) {
        return;
    }

    // Print spaces for indentation
    for (int i = 0; i < level; ++i) {
        printf("  ");
    }

    switch (root->type) {
        case NodeTypeProgram:
        case NodeTypeExp:
            printf("Expression:\n");
            printASTIndented(root->left, level + 1);
            break;

        case NodeTypeOp:
            printf(getNameOfToken(root->token.type));
            printASTIndented(root->left, level + 1);
            printASTIndented(root->right, level + 1);
            break;

        case NodeTypeLit:
            printf("stdin: %s | ", root->token.lexeme);
            break;

        case NodeTypeVar:
            printf(root->token.lexeme);
            break;

        case NodeTypePrint:
            printf("MYCENA\n");
            printASTIndented(root->left, level + 1);
            break;

        default:
            printf("Node Unrecognized\n");
    }
}

// Function to evaluate an AST node
char* evaluate(TreeNode *node) {
    char* leftValueStr = NULL;
    char* rightValueStr = NULL;
    char *result;

    if (node == NULL) {
        char *emptyString = malloc(1);
        emptyString[0] = '\0';
        return emptyString;
    }

    switch (node->type) {
        case NodeTypeLit:
        case NodeTypeVar:  // Assuming var returns a string as well
            result = malloc(strlen(node->token.lexeme) + 1);
            strcpy(result, node->token.lexeme);
            return result;

        case NodeTypeOp: {
            leftValueStr = evaluate(node->left);
            rightValueStr = evaluate(node->right);

            // Error handling for memory allocation
            if (!leftValueStr || !rightValueStr) {
                char *errorResult = strdup("Error: Memory allocation failure");
                free(leftValueStr);
                free(rightValueStr);
                return errorResult;
            }

            int leftValue = atoi(leftValueStr);
            int rightValue = atoi(rightValueStr);
            int resultValue;

            switch (node->token.type) {
                case SPROUT:
                    resultValue = leftValue + rightValue;
                    break;
                case SHED:
                    resultValue = leftValue - rightValue;
                    break;
                case BRANCH:
                    resultValue = leftValue * rightValue;
                    break;
                case DECAY:
                    if (rightValue == 0) {
                        fprintf(stderr, "Error: divide by zero\n");
                        free(leftValueStr);
                        free(rightValueStr);
                        return strdup("Error: divide by zero");
                    }
                    resultValue = leftValue / rightValue;
                    break;
                default:
                    fprintf(stderr, "Error: Unknown operation\n");
                    free(leftValueStr);
                    free(rightValueStr);
                    return strdup("Error: Unknown operation");
            }

            result = malloc(12);  // Enough for int representation
            sprintf(result, "%d", resultValue);
            free(leftValueStr);
            free(rightValueStr);
            return result;
        }

        case NodeTypePrint:
            return evaluate(node->left);

        default:
            fprintf(stderr, "Error: Unrecognized node type in evaluation\n");
            return strdup("Error: Unrecognized node type");
    }
}

int main() {
    printf("\nLEAFCODE LANG\n");
    printf("Nature is so cool\n\n");
    FILE *file;
    char buffer[1024]; // Buffer to store each line of the file

    file = fopen("input.txt", "r"); // Open the file
    if (file == NULL) {
        perror("Couldn't open file");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        size_t len = strlen(buffer);

        // Trim trailing whitespace (optional)
        while (len > 0 && isspace((unsigned char)buffer[len - 1])) {
            len--;
            buffer[len] = '\0';
        }

        // Check for semicolon at the end
        if (len == 0 || buffer[len - 1] != ';') {
            fprintf(stderr, "Error: lines must end with ';'\n");
            fclose(file);
            return 1;
        }

        char *copy = strdup(buffer); // Make a copy of the buffer
        char *word = strtok(copy, " ;"); // Tokenize the first word
        Token tokens[100]; // Array of tokens
        int tokenCount = 0; // Counter for the number of tokens

        while (word != NULL) {
            tokens[tokenCount++] = processWord(word);
            word = strtok(NULL, " ;");
        }
        for (int i = tokenCount; i < sizeof(tokens) / sizeof(tokens[0]); ++i) { 
            tokens[i] = (Token){0, ""}; // Clear the rest of the memory
        }

        free(copy);

        TreeNode *ast = parseProgram(tokens);

        printAST(ast);
        printf("\n");
        printASTIndented(ast, 0);
        char *result = evaluate(ast);
        printf("stdout: %s", result);

        free(result); // Free the dynamically allocated memory
    }

    fclose(file); // Close the file
    return 0;
}

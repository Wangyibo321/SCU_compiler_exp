%{
#include <stdio.h>  
#include <string.h> 
#define MAXTOKENLEN 50   
  
char tokenString[MAXTOKENLEN + 1];
int lineno = 1;

typedef enum   
{  
    ENDFILE, ERROR,  
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,  
    ID, NUM,  
    ASSIGN, EQ, LT, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI  
} TokenType;

TokenType getToken(void);
void printToken(TokenType token, const char* tokenString);

%}

%x COMMENT 

%%
<INITIAL>"{"          { BEGIN COMMENT; }
<COMMENT>"}"          { BEGIN INITIAL; printf("\n");}
<COMMENT>\n           { ++lineno; printf("\n");}
<COMMENT>.            {ECHO;}
\n                   {lineno++;} 

[ \t]+               ;          
"if"                 {return IF;}
"then"               {return THEN;}
"else"               {return ELSE;}
"end"                {return END;}
"repeat"             {return REPEAT;}
"until"              {return UNTIL;}
"read"               {return READ;}
"write"              {return WRITE;}
":="                 {return ASSIGN;}
"="                  {return EQ;}
"<"                  {return LT;}
"+"                  {return PLUS;}
"-"                  {return MINUS;}
"*"                  {return TIMES;}
"/"                  {return OVER;}
"("                  {return LPAREN;}
")"                  {return RPAREN;}
";"                  {return SEMI;}
[a-zA-Z]+            {return ID;} 
[0-9]+               {return NUM;} 
.|\n                 {return ERROR;} 

%%

TokenType getToken(void)  
{  
    TokenType currentToken;  
    currentToken = yylex();  
    if (currentToken == 0)  currentToken=ENDFILE;
    strncpy(tokenString, yytext, MAXTOKENLEN);
    printToken(currentToken, tokenString);
    if (currentToken == 0)  return ENDFILE;
    return currentToken;  
} 

void printToken(TokenType token, const char* tokenString)  
{  
    char* tokenType;
    switch (token) {
        case IF:
        case THEN:
        case ELSE:
        case END:
        case REPEAT:
        case UNTIL:
        case READ:
        case WRITE:
            tokenType = "reserved word";
            break;
        case ASSIGN:
        case EQ:
        case LT:
        case PLUS:
        case MINUS:
        case TIMES:
        case OVER:
        case LPAREN:
        case RPAREN:
        case SEMI:
            tokenType = "special Symbol";
            break;
        case ID:
            tokenType = "identifier";
            break;
        case NUM:
            tokenType = "number";
            break;
        case ENDFILE:
            tokenType = "EOF";
            break;
        default:
            tokenType = "ERROR";
    }
    printf("%d: <%s, %s> \n", lineno, tokenType, tokenString);
}  

int main(int argc, char* argv[])  
{  
    TokenType token;  
    lineno = 1;
    do  
    {  
        token = getToken();  
    } while (token != ENDFILE);  
    return 0;  
}

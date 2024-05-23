#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define BUFLEN 256
#define MAXTOKENLEN 40
#define MAXRESERVED 8

typedef enum 
    /* book-keeping tokens */
   {ENDFILE,ERROR,
    /* reserved words */
    IF,THEN,ELSE,END,REPEAT,UNTIL,READ,WRITE,
    /* multicharacter tokens */
    ID,NUM,
    /* special symbols */
    ASSIGN,EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI
   } TokenType;

/* states in scanner DFA */
typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID,DONE }
   StateType;

char tokenString[MAXTOKENLEN+1];
char lineBuf[BUFLEN]; /* holds the current line */
int linepos = 0; /* current position in LineBuf */
int bufsize = 0; /* current size of buffer string */
int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */
int lineno = 0;
FILE * source;
FILE * listing;


TokenType getToken(void);

int main( int argc, char * argv[] )
{ 
  char pgm[120]; /* source code file name */
  strcpy(pgm,argv[1]) ;
  source = fopen(pgm,"r");
  printf("%s",pgm);
  listing = stdout; /* send listing to screen */
  fprintf(listing,"\nTINY COMPILATION: %s\n",pgm);
  while (getToken()!=ENDFILE);
  fclose(source);
  return 0;
}


/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { 
    if (fgets(lineBuf,BUFLEN-1,source))
    { 
	  lineno++;
      //fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* lookup table of reserved words */
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}


/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
static void printToken( TokenType token, const char* tokenString )
{ 
char* tokenType;
switch (token)
  {
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
    case ERROR:
        tokenType = "ERROR";
        break;
    default:
        tokenType = "Unknown token";
  }
  fprintf(listing, "%d: <%s, %s> \n", lineno, tokenType, tokenString);
}


/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{  /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   TokenType currentToken;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
        if (isdigit(c)) {
            state = INNUM;
        } else if (isalpha(c)) {
            state = INID;
        } else if ((c == ' ') || (c == '\t') || (c == '\n')) {
            save = FALSE;
            if (c == '\n') lineno++;
        } else if (c == ':') {
            state = INASSIGN;
        } else if(c == '{'){
          save = FALSE;
          state = INCOMMENT;
        } else if(c == '}'){
          state = START;
        }
        else {
            state = DONE;
            switch (c) {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case '=':
                    currentToken = EQ;
                    break;
                case '<':
                    currentToken = LT;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '/':
                    currentToken = OVER;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                default:
                    currentToken = ERROR;
                    break;
            }
        }
         break;
       case INCOMMENT:
        save = FALSE;
        if (c == '}') {
            state = START;
            printf("\n");
        } else if (c == EOF) {
            state = DONE;
            currentToken = ENDFILE;
        } else if (c == '\n') {
            lineno++;
        } else{
          printf("%c", c);  // print comment
        }
         break;
       case INASSIGN:
        state = DONE;
        if (c == '=') {
            currentToken = ASSIGN;
        } else {
            ungetNextChar();
            save = FALSE;
            currentToken = ERROR;
        }
         break;
       case INNUM:
        if (!isdigit(c)) {
            ungetNextChar();
            save = FALSE;
            state = DONE;
            currentToken = NUM;
        }
         break;
       case INID:
        if (!isalpha(c)) {
            ungetNextChar();
            save = FALSE;
            state = DONE;
            currentToken = ID;
        }
        break;
       case DONE:
       default: /* should never happen */
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   printToken(currentToken,tokenString);
   return currentToken;
} /* end getToken */



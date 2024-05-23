# SCU_TINY_YACC

## 5
 `bison --o y.tab.c -d tiny.y`
 
 然后删除y.tab.h的 `extern YYSTYPE yylval;` 
 
 将y.tab.c的`yylex ()`改为`getToken ()`
 
`make`

 `./tiny < /home/flex/4/yacc_struct/sample.tny`
 
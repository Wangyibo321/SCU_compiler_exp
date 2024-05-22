#include <stdio.h>


// Follow(E') = {')', '#'}
// Follow(T') = {'+', ')', '#'}

void E(); 
void T(); 
void E1(); 
void T1(); 
void F();

char s[100];
int i = 0, SIGN = 0;

void error() {
    SIGN = 1;
}

void advance() {
    i++;
}


int check_follow(char token, const char *follow) {
    while (*follow != '\0') {
        if (token == *follow) {
            return 1;
        }
        follow++;
    }
    return 0;
}

int main() {
    printf("Please input a statement that ends with '#'.\n"); 
    while (1) { 
        SIGN = 0;
        i = 0; 
        scanf("%s", s); 
        if (s[0] == '#') 
            return 0; 
        E();
        if (SIGN == 0 && s[i] == '#') 
            printf("The statement is correct.\n");
        else
            printf("The statement is incorrect.\n");
        printf("Please input the next statement that ends with '#'.\n"); 
    }
    return 1;
}

void E() {
    if (SIGN == 0) {
        T();
        E1();
    }
}

void E1() {
    if (SIGN == 0) {
        if (s[i] == '+') {
            advance();
            T();
            E1();
        } else if (!check_follow(s[i], ")#")) {
            error();
        }
    }
}

void T() {
    if (SIGN == 0) {
        F();
        T1();
    }
}

void T1() {
    if (SIGN == 0) {
        if (s[i] == '*') {
            advance();
            F();
            T1();
        } else if (!check_follow(s[i], "+)#")) {
            error();
        }
    }
}

void F() {
    if (SIGN == 0) {
        if (s[i] == '(') {
            advance();
            E();
            if (s[i] == ')')
                advance();
            else
                error();
        } else if (s[i] == 'i') {
            advance();
        } else {
            error();
        }
    }
}

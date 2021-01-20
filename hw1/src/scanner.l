%{
#include <stdint.h>

#define LIST                strCat(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
char buffer[MAX_LINE_LENG];

static int32_t opt_src = 1;
static int32_t opt_tok = 1;
static char *buffer_ptr = buffer;

static void strCat(const char *Text);

%}

%x CONT

%%
"(" { TOKEN_CHAR('('); }
")" { TOKEN_CHAR(')'); }
"," { TOKEN_CHAR(','); }
";" { TOKEN_CHAR(';'); }
":" { TOKEN_CHAR(':'); }
"[" { TOKEN_CHAR('['); }
"]" { TOKEN_CHAR(']'); }

"+" { TOKEN(+); }
"-" { TOKEN(-); }
"*" { TOKEN(*); }
"/" { TOKEN(/); }
"mod" { TOKEN(mod); }
":=" { TOKEN(:=); }
"<" { TOKEN(<); }
"<=" { TOKEN(<=); }
"<>" { TOKEN(<>); }
">=" { TOKEN(>=); }
">" { TOKEN(>); }
"=" { TOKEN(=); }
"and" { TOKEN(and); }
"or" { TOKEN(or); }
"not" { TOKEN(not); }

[a][r][r][a][y] {
    TOKEN(KWarray);
}
[b][e][g][i][n] {
    TOKEN(KWbegin);
}
[b][o][o][l][e][a][n] {
    TOKEN(KWboolean);
}
[d][e][f] {
    TOKEN(KWdef);
}
[d][o] {
    TOKEN(KWdo);
}
[e][l][s][e] {
    TOKEN(KWelse);
}
[e][n][d] {
    TOKEN(KWend);
}
[f][a][l][s][e] {
    TOKEN(KWfalse);
}
[f][o][r] {
    TOKEN(KWfor);
}
[i][n][t][e][g][e][r] {
    TOKEN(KWinteger);
}
[i][f] {
    TOKEN(KWif);
}
[o][f] {
    TOKEN(KWof);
}
[p][r][i][n][t] {
    TOKEN(KWprint);
}
[r][e][a][d] {
    TOKEN(KWread);
}
[r][e][a][l] {
    TOKEN(KWreal);
}
[s][t][r][i][n][g] {
    TOKEN(KWstring);
}
[t][h][e][n] {
    TOKEN(KWthen);
}
[t][o] {
    TOKEN(KWto);
}
[t][r][u][e] {
    TOKEN(KWtrue);
}
[r][e][t][u][r][n] {
    TOKEN(KWreturn);
}
[v][a][r] {
    TOKEN(KWvar);
}
[w][h][i][l][e] {
    TOKEN(KWwhile);
}

[a-zA-Z]([a-zA-Z]|[0-9])* { TOKEN_STRING(id, yytext); }
0|[1-9][0-9]* { TOKEN_STRING(integer, yytext); }
0+[1-7]* { TOKEN_STRING(oct_integer, yytext); }
0+\.[0-9]+ { TOKEN_STRING(float, yytext); }
[1-9][0-9]*\.[0-9]*[1-9] { TOKEN_STRING(float, yytext); }
[1-9][0-9]*+[eE][+-]?(0|[1-9][0-9]*) { TOKEN_STRING(scientific, yytext); }
0+\.[0-9]+[eE][+-]?(0|[1-9][0-9]*) { TOKEN_STRING(scientific, yytext); }
[0-9]+\.[0-9]+[eE][+-]?(0|[1-9][0-9]*) { TOKEN_STRING(scientific, yytext); }

[ \t] LIST;

["]([^\n"]|["]["])*["] {
    char str[yyleng];
    memset(str, '\0', yyleng);
    int i = 1;
    int idx = 0;
    while(i < yyleng){
        if(yytext[i] == '"'){
            if(i == yyleng-2){
                break;
            }
            else if(yytext[i+1] == '"'){
                str[idx] = yytext[i];
                i+=2;
                idx++;
            }
            else{
                i++;
            }
        }
        else{
            str[idx] = yytext[i];
            i++;
            idx++;
        }
    }
    TOKEN_STRING(string, str);
}




[/][*] {
    BEGIN CONT;
    LIST;
}

<CONT>[*][/] {
    BEGIN 0;
    LIST;
}

<CONT>. {
    LIST;
}

<CONT>[\n] {
    printf("%d: %s\n", line_num, buffer);
    line_num++;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}

[/][/][&][S][+].* {
    LIST;
    opt_src = 1;
}

[/][/][&][S][-].* {
    LIST;
    opt_src = 0;
}

[/][/][&][T][+].* {
    LIST;
    opt_tok = 1;
}

[/][/][&][T][-].* {
    LIST;
    opt_tok = 0;
}

[/][/]([^\n])* {
    LIST;
}



\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void strCat(const char *Text) {
    while (*Text) {
        *buffer_ptr++ = *Text++;
    }
    *buffer_ptr = '\0';
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}

#include <bits/stdc++.h>

#include <string>

using namespace std;

#define TOKEN_ID 1
#define TOKEN_CONST 2
#define TOKEN_IGUAL 3
#define TOKEN_ATRIB 4
#define TOKEN_ABRE_PAREN 5
#define TOKEN_FECHA_PAREN 6
#define TOKEN_MENOR 7
#define TOKEN_MAIOR 8
#define TOKEN_DIF 9
#define TOKEN_MAIOR_IGUAL 10
#define TOKEN_MENOR_IGUAL 11
#define TOKEN_VIRG 12
#define TOKEN_MAIS 13
#define TOKEN_MENOS 14
#define TOKEN_ASTER 15
#define TOKEN_BARRA 16
#define TOKEN_PONTO_VIRG 17
#define TOKEN_ABRE_CHAVE 18
#define TOKEN_FECHA_CHAVE 19
#define TOKEN_MOD 20
#define TOKEN_ABRE_COLCH 21
#define TOKEN_FECHA_COLCH 22
#define TOKEN_EOF 23
#define TOKEN_FINAL 24
#define TOKEN_INT 25
#define TOKEN_CHAR 26
#define TOKEN_FOR 27
#define TOKEN_IF 28
#define TOKEN_TRUE 29
#define TOKEN_ELSE 30
#define TOKEN_AND 31
#define TOKEN_OR 32
#define TOKEN_NOT 33
#define TOKEN_THEN 34
#define TOKEN_READLN 35
#define TOKEN_FALSE 36
#define TOKEN_WRITE 37
#define TOKEN_WRITELN 38
#define TOKEN_MAIN 39


struct Simbolo {
    string lexema;
    int token;  // Short provisorio
};

struct RegLex {
    string lexema;
    int token;
    int posicao;
    string tipo;
}; // registro léxico

class TabelaSimbolos {
    int num_posicoes;

    list<Simbolo> *tabela;

   public:
    TabelaSimbolos(int n);  // Construtor
    int inserir(string lex, int token);
    int pesquisar(string lex);
    int hash(string lex);
    void mostrar();
};

TabelaSimbolos::TabelaSimbolos(int n) {
    this->num_posicoes = n;
    tabela = new list<Simbolo>[num_posicoes];
}

void TabelaSimbolos::mostrar(){
    for (int i = 0; i < num_posicoes; i++) {
        cout << i;
        list<Simbolo>::iterator x;
        for (x = tabela[i].begin(); x != tabela[i].end();
             x++){ 

            cout << " --> " << x->lexema << " " << x->token;
        }
        cout << endl;
    }
}

int TabelaSimbolos::inserir(string lex, int token) {
    Simbolo s = {lex, token};
    int pos = hash(lex);
    tabela[pos].push_back(s);
    return pos;
}

int TabelaSimbolos::pesquisar(string lex) {
    // Pega a posicao inicial
    int pos = -1;
    int pos_inicial = hash(lex);

    // Verifica se a lista da posição encontrada é vazia
    if (tabela[pos_inicial].size() != 0) {
        // Pesquisa o lexema na tabela de simbolos
        list<Simbolo>::iterator i;
        for (i = tabela[pos_inicial].begin(); i != tabela[pos_inicial].end();
             i++) {
            // Se encontrado, retorna a posição na tabela
            if (!i->lexema.compare(lex)) {
                pos = pos_inicial;
                break;
            }
        }
    }

    return pos;
}

int TabelaSimbolos::hash(string lex) {
    int soma;
    int pos;

    // Converter cada char pra int e somar
    for (int i = 0; i < lex.size(); i++) {
        soma += (int)lex[i];
    }
    // Fazer o mod num_posicoes para achar a posicao
    pos = soma % num_posicoes;
    return pos;
}


// inicializar tabela de simbolos (para ser variável global)
TabelaSimbolos t(100);

string analisadorLexico() {
    char c;
    int S = 0;

    /* 
        feito:  cases
                inserção de token ID

        falta:  elses (erros lexicos e EOF)
                demais tokens
                registro lexico
    
    
    */

    // iniciando o registro lexico
    RegLex reg;

    // criando variaveis para criação do reglex e do token
    string lex = "";
    string tipo = "";
    int tok = 0;
    int pos = -1;

    while (S != 1) {
        if (c != EOF) {
            c = cin.get();
            // if(c == ""){
            //   // ERRO
            //   break;
            // //Se invalido = ERRO
            // }
            // Senao = EOF
        } else {
            return "EOF";
        }

        // Automato usando switch
        switch (S) {
            case 0:
                lex = "";
                if (c == ' ' || c == '\n') {
                    S = 0;
                } else if (c == '_') {
                    tok = TOKEN_ID;
                    S = 2;
                    lex += c;
                } else if (c == '\'') {
                    S = 4;
                    lex += c;
                } else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
                    tok = TOKEN_ID;
                    S = 3;
                    lex += c;
                } else if (c >= '1' && c <= '9') {
                    S = 11;
                    lex += c;
                } else if (c == '0') {
                    S = 6;
                    lex += c;
                } else if (c == '=' || c == '+' || c == '*' || c == '[' ||
                           c == ']' || c == '(' || c == ')' || c == '{' ||
                           c == '}' || c == '.' || c == ';' || c == ',' ||
                           c == '%') {
                    S = 1;
                    lex += c;
                } else if (c == ':') {
                    S = 12;
                    lex += c;
                } else if (c == '<') {
                    S = 13;
                    lex += c;
                } else if (c == '>') {
                    S = 14;
                    lex += c;
                } else if (c == '\"') {
                    S = 15;
                    lex += c;
                } else if (c == '/') {
                    S = 17;
                    lex += c;
                }
                break;

            case 1:
                S = 0;
                // Aceita token
                break;
            case 2:
                S = 0;
                if (c == '_') {
                    S = 2;
                    lex += c;
                } else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' ||
                           c >= '0' && c <= '9') {
                    S = 3;
                    lex += c;
                }
                break;
            case 3:
                if (c == '_' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' ||
                    c >= '0' && c <= '9') {
                    S = 3;
                    lex += c;
                } else {
                    /* 
                        busca pelo token,
                        se não achar, insere
                        se achar, retorna a sua posição
                    */
                    pos = t.pesquisar(lex);
                    if(pos != -1){
                        // colocar o touken aqui
                    } else{
                        t.inserir(lex, tok);
                        t.mostrar();
                    }


                    S = 1;
                    cin.unget();
                }
                break;
            case 4:
                if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'){
                    lex+=c;
                    S = 5;
                }
                break;
            case 5:
                if (c == '\'') {
                    lex+=c;
                    S = 1;
                }
                break;
            case 6:
                if(c >= 0 && c <= 9){
                    lex+=c;
                    S=9;
                } else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')){
                    lex+=c;
                    S=7;
                } else {
                    S=1;
                    cin.unget();
                }
                break;
            case 7:
                if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')){
                    lex+=c;
                    S=8;
                }
                break;
            case 8:
                if(c == 'h'){
                    S=1;
                    lex+=c;
                }
                break;
            case 9:
                if(c >= 0 && c <= 9){
                    lex+=c;
                    S=10;
                } else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')){
                    lex+=c;
                    S=8;
                } else {
                    S=1;
                    cin.unget();
                }
                break;
            case 10:
                if(c == 'h'){
                    lex+=c;
                    S = 1;
                } else if (c >= 0 && c <= 9) {
                    lex+=c;
                    S=11;
                } else {
                    S = 1;
                    cin.unget();
                }
                break;
            case 11:
                if (c >= 0 && c <= 9){
                    lex+=c;
                    S=11;
                } else {
                    S=1;
                    cin.unget();
                }
                break;
            case 12:
                if(c == '='){
                    lex+=c;
                    S=1;
                }
                break;
            case 13:
                if(c == '>' || c == '='){
                    lex+=c;
                    S=1;
                } else {
                    S=1;
                    cin.unget();
                }
                break;
            case 14:
                if(c=='='){
                    lex+=c;
                    S=1;
                } else {
                    S=1;
                    cin.unget();
                }
                break;
            case 15:
                if(c != '\"' && c != '$' && c != '\n'){
                    lex+=c;
                    S=16;
                }
                break;
            case 16:
                if(c != '\"' && c != '$' && c != '\n'){
                    lex+=c;
                    S=16;
                } else if (c == '\"'){
                    lex+=c;
                    S=1;
                }
                break;
            case 17:
                if(c=='*'){
                    lex+=c;
                    S=18;
                }
                break;
            case 18:
                if(c != '*'){
                    lex+=c;
                    S=18;
                } else {
                    lex+=c;
                    S=19;
                }
                break;
            case 19:
                if(c == '*'){
                    lex+=c;
                    S=19;
                } else if (c == '/'){
                    lex+=c;
                    S=0;
                } else if (c != '*' && c != '/') {
                    lex+=c;
                    S=18;
                }
                break;
        }
        cout << c << " → " << S << endl;
    }
    cout << lex << endl;
    cout << "fim do lexema" << endl;
    return lex;
}

int main() {
    // iniciando tabela de simbolos
    t.inserir("final", TOKEN_FINAL);
    t.inserir("int", TOKEN_INT);
    t.inserir("char", TOKEN_CHAR);
    t.inserir("for", TOKEN_FOR);
    t.inserir("if", TOKEN_IF);
    t.inserir("TRUE", TOKEN_TRUE);
    t.inserir("else", TOKEN_ELSE);
    t.inserir("and", TOKEN_AND);
    t.inserir("or", TOKEN_OR);
    t.inserir("not", TOKEN_NOT);
    t.inserir(":=", TOKEN_ATRIB);
    t.inserir("=", TOKEN_IGUAL);
    t.inserir("(", TOKEN_ABRE_PAREN);
    t.inserir(")", TOKEN_FECHA_PAREN);
    t.inserir("<", TOKEN_MENOR);
    t.inserir(">", TOKEN_MAIOR);
    t.inserir("<>", TOKEN_DIF);
    t.inserir(">=", TOKEN_MAIOR_IGUAL);
    t.inserir("<=", TOKEN_MENOR_IGUAL);
    t.inserir(",", TOKEN_VIRG);
    t.inserir("+", TOKEN_MAIS);
    t.inserir("-", TOKEN_MENOS);
    t.inserir("*", TOKEN_ASTER);
    t.inserir("/", TOKEN_BARRA);
    t.inserir(";", TOKEN_PONTO_VIRG);
    t.inserir("{", TOKEN_ABRE_CHAVE);
    t.inserir("}", TOKEN_FECHA_CHAVE);
    t.inserir("then", TOKEN_THEN);
    t.inserir("readln", TOKEN_READLN);
    t.inserir("FALSE", TOKEN_FALSE);
    t.inserir("write", TOKEN_WRITE);
    t.inserir("writeln", TOKEN_WRITELN);
    t.inserir("%", TOKEN_MOD);
    t.inserir("[", TOKEN_ABRE_COLCH);
    t.inserir("]", TOKEN_FECHA_COLCH);
    t.inserir("main", TOKEN_MAIN);
    t.inserir("EOF", TOKEN_EOF); // CONSERTAR !!!!!

    string token = "";
    while (token != "EOF") {
        token = analisadorLexico();
        cout << "token " << token << " identificado" << endl;
        // analise sintatica...
    }

    /*  // Testes da Tabela se Simbolos
    TabelaSimbolos t(10);
    t.inserir("a", TOKEN_ID);
    t.inserir("b", TOKEN_ID);
    t.inserir("c", TOKEN_ID);
    t.inserir("d", TOKEN_ID);
    t.inserir("e", TOKEN_ID);
    t.inserir("f", TOKEN_ID);
    t.inserir("g", TOKEN_ID);
    t.inserir("h", TOKEN_ID);
    t.inserir("i", TOKEN_ID);
    t.inserir("j", TOKEN_ID);
    t.inserir("k", TOKEN_ID);
    t.inserir("l", TOKEN_ID);
    int a = t.pesquisar("a");
    cout << a << endl;
    a = t.pesquisar("b");
    cout << a << endl;
    a = t.pesquisar("c");
    cout << a << endl;
    a = t.pesquisar("d");
    cout << a << endl;
    a = t.pesquisar("e");
    cout << a << endl;
    a = t.pesquisar("f");
    cout << a << endl;
    a = t.pesquisar("g");
    cout << a << endl;
    a = t.pesquisar("h");
    cout << a << endl;
    a = t.pesquisar("i");
    cout << a << endl;
    a = t.pesquisar("j");
    cout << a << endl;
    a = t.pesquisar("k");
    cout << a << endl;
    a = t.pesquisar("l");
    cout << a << endl;
        a = t.pesquisar("m");
    cout << a << endl;
*/
    return 0;
}

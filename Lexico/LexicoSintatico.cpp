/*
Implementação do Analisador Sintático e Léxico para a Linguagem L
Disciplina COMPILADORES (6806100) 
PUC-MG
Professor Alexei Machado

Grupo:
Pedro Díógenes - 605549
João Rene - 606821
Tulio Nunes - 605286
*/

#include <bits/stdc++.h>

#include <string>

using namespace std;

// Definição/numeração dos tokens da linguagem.

#define TOKEN_ID 1            // identificadores
#define TOKEN_CONST 2         // constantes
#define TOKEN_IGUAL 3         // =
#define TOKEN_ATRIB 4         // :=
#define TOKEN_ABRE_PAREN 5    // (
#define TOKEN_FECHA_PAREN 6   // )
#define TOKEN_MENOR 7         // <
#define TOKEN_MAIOR 8         // >
#define TOKEN_DIF 9           // <>
#define TOKEN_MAIOR_IGUAL 10  // >=
#define TOKEN_MENOR_IGUAL 11  // <=
#define TOKEN_VIRG 12         // ,
#define TOKEN_MAIS 13         // +
#define TOKEN_MENOS 14        // -
#define TOKEN_ASTER 15        // *
#define TOKEN_BARRA 16        // /
#define TOKEN_PONTO_VIRG 17   // ;
#define TOKEN_ABRE_CHAVE 18   // {
#define TOKEN_FECHA_CHAVE 19  // }
#define TOKEN_MOD 20          // %
#define TOKEN_ABRE_COLCH 21   // [
#define TOKEN_FECHA_COLCH 22  // ]
#define TOKEN_EOF 23          // EOF ou -1
#define TOKEN_FINAL 24        // final
#define TOKEN_INT 25          // int
#define TOKEN_CHAR 26         // char
#define TOKEN_FOR 27          // for
#define TOKEN_IF 28           // if
#define TOKEN_TRUE 29         // true
#define TOKEN_ELSE 30         // else
#define TOKEN_AND 31          // and
#define TOKEN_OR 32           // or
#define TOKEN_NOT 33          // not
#define TOKEN_THEN 34         // then
#define TOKEN_READLN 35       // readln
#define TOKEN_FALSE 36        // false
#define TOKEN_WRITE 37        // write
#define TOKEN_WRITELN 38      // writeln
#define TOKEN_MAIN 39         // main
#define TOKEN_BOOL 40         // boolean

// definição de classes e tipos
#define CLASSE_VAR 101
#define CLASSE_CONST 102

#define TIPO_INT 103
#define TIPO_CHAR 104
#define TIPO_STRING 105
#define TIPO_BOOLEAN 106
#define TIPO_VAZIO 107

// definição de erros para o analisador
#define ERR_CHAR -1   // usado para erro de caractere não esperado
#define ERR_EOF -2    // usado para erro de EOF nao esperado
#define ERR_TOKEN -3  // usado para erro de token não esperado
#define ERR_EXISTS -4 // usado para erro de id já declarado
#define ERR_NOT_EXISTS -5 // usado para erro de id não declarado
#define ERR_TIPO -6 // usado para erro de tipo nao compativel
#define ERR_TAMANHO -7 //usado para erro de tamanho maior que o esperado
#define ERR_CLASSE -8 // usado para erro de classe errada



// Definição da estrutura dos símbolos para serem inseridos na tabela de
// símbolos
struct Simbolo {
    string lexema;
    int token;
    int tipo;
    int classe;
    int tamanho;
};

// definição da estrutura do registro léxico para o retorno do analisador léxico
struct RegLex {
    string lexema;
    int token;
    int posicao;
    int tipo;
    size_t tamanho;
};

// caracteres aceitos pelo compilador para verificação de caracteres invalidos
string alfabeto = "_.,;:(){}[]+-/%@!?><=*$";

// iniciando o registro lexico variável global
RegLex reg;

// classe da tabela de simbolos
class TabelaSimbolos {
    int num_posicoes;

    list<Simbolo> *tabela;

   public:
    TabelaSimbolos(int n);  // Construtor
    int inserir(string lex, int token);
    int pesquisar(string lex);
    int hash(string lex);
    int getToken(string lex, int pos);
    int getTamanho(string lex, int pos);
    int getTipo(string lex, int pos);
    int getClasse(string lex, int pos);
    void atualizar(string lex, int tipo, int tamanho, int classe);
    void mostrar();
};

TabelaSimbolos::TabelaSimbolos(int n) {
    this->num_posicoes = n;
    tabela = new list<Simbolo>[num_posicoes];
}

// metodo para atualizar tabela com os tipos dos identificadores
void TabelaSimbolos::atualizar(string lex, int tipo, int tamanho, int classe){
    int pos = pesquisar(lex);
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            i->tamanho = tamanho;
            i->classe = classe;
            i->tipo = tipo;
        }
    }
}

// retorna o token de um registro da tabela de símbolos
int TabelaSimbolos::getToken(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->token;
        }
    }
    return -1;
}

//
int TabelaSimbolos::getTamanho(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->tamanho;
        }
    }
    return -1;
}

//
int TabelaSimbolos::getTipo(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->tipo;
        }
    }
    return -1;
}

//
int TabelaSimbolos::getClasse(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->classe;
        }
    }
    return -1;
}

// apenas para teste, mostra a tabela de símbolos inteira
void TabelaSimbolos::mostrar() {
    for (int i = 0; i < num_posicoes; i++) {
        cout << i;
        list<Simbolo>::iterator x;
        for (x = tabela[i].begin(); x != tabela[i].end(); x++) {
            cout << " --> " << x->lexema << " " << x->token;
        }
        cout << endl;
    }
}

// insere um registro na tabela de símbolos e retorna sua posição
int TabelaSimbolos::inserir(string lex, int token) {
    Simbolo s = {};
    s.lexema = lex;
    s.token = token;
    s.tipo = TIPO_VAZIO;
    int pos = hash(lex);
    tabela[pos].push_back(s);
    return pos;
}

// pesquisa se existe uma entrada na tabela de símbolos
// a tabela é encadeada para evitar colisões
// retorna a posição, caso encontrado, e -1 caso contrário
int TabelaSimbolos::pesquisar(string lex) {
    // Pega a posicao inicial
    int pos = -1;
    int pos_inicial = hash(lex);

    // Verifica se a lista da posição encontrada é vazia
    // cout << "pos_inicial: " << pos_inicial << endl;
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

// função de hash: multiplica o valor do char de cada posição do lexema pelo
// indice da sua posição na string então pega o resultado dessa soma mod numero
// de posições da tabela
int TabelaSimbolos::hash(string lex) {
    int soma = 0;
    int pos;

    // Converter cada char pra int e somar
    for (int i = 0; i < lex.size(); i++) {
        soma += ((int)lex[i] * (i + 1));
    }
    // Fazer o mod num_posicoes para achar a posicao
    pos = soma % num_posicoes;
    return pos;
}

// inicializar tabela de simbolos (para ser variável global)
TabelaSimbolos t(127);

// metodo para inserir token
void atualizarTabela(string lex, int tipo, int token, size_t tamanho) {
    if (token == TOKEN_CONST) {
        // CONST NAO ENTRA NA TABELA
        reg.lexema = lex;
        reg.tipo = tipo;
        reg.token = token;
        reg.tamanho = tamanho;
    } else {
        /*
            busca pelo token,
            se não achar, insere
            se achar, retorna a sua posição
        */
        int pos = t.pesquisar(lex);
        if (pos != -1) {
            reg.posicao = pos;
            reg.token = t.getToken(lex, pos);
            reg.lexema = lex;
        } else {
            pos = t.inserir(lex, token);
            reg.token = token;
            reg.posicao = pos;
            reg.lexema = lex;
        }
    }
}


// iniciando a contagem de linhas para a mensagem de resultado ou de erro
int linha = 1;

// inicializar o char;
char c;

/*
----------------------------------------------------
Analisador Léxico
Automato feito por meio de um switch case simples
Cada case é um estado do automato
Preenche a tabela de símbolos e o registro léxico a cada chamada
----------------------------------------------------
*/
void analisadorLexico() {
    // limpa o registro léxico
    reg.lexema = "";
    reg.posicao = -1;
    reg.tipo = 0;
    reg.token = 0;
    reg.tamanho = 0;

    // c = caractere lido
    // S = estado do automato

    int S = 0;

    // criando variaveis para criação do reglex e do token
    string lex = "";
    int tipo = 0;
    int tok = 0;
    int pos = -1;
    size_t tamanho = 0;

    // S = 1 é o estado final, então o automato roda até chegar nele
    while (S != 1) {
        /*
        verifica se o caractere lido é fim de arquivo
        se for, retorna o token do fim de arquivo para o registro léxico
        se não, verifica se o caractere é valido.
        */
        if (c != EOF) {
            c = cin.get();
            if (!(c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
                  c == '\'' || c == EOF || c == '\"' ||
                  (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                  (c >= '0' && c <= '9') ||
                  alfabeto.find(c) != std::string::npos)) {
                throw ERR_CHAR;
            }
        } else {
            atualizarTabela("EOF", tipo, TOKEN_EOF, tamanho);
            break;
        }

        // Automato usando switch
        switch (S) {
            case 0:
                lex = "";
                if (c == ' ' || c == '\t' || c == '\r') {
                    S = 0;
                } else if (c == '\n') {
                    S = 0;
                    linha++;
                } else if (c == '_') {
                    tok = TOKEN_ID;
                    S = 2;
                    lex += c;
                } else if (c == '\'') {
                    tok = TOKEN_CONST;
                    tipo = TIPO_CHAR;
                    lex += c;
                    S = 4;
                } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    tok = TOKEN_ID;
                    S = 3;
                    lex += c;
                } else if (c >= '1' && c <= '9') {
                    tok = TOKEN_CONST;
                    tipo = TIPO_INT;
                    S = 11;
                    lex += c;
                } else if (c == '0') {
                    tok = TOKEN_CONST;
                    lex += c;
                    S = 6;
                } else if (c == '=' || c == '+' || c == '*' || c == '[' ||
                           c == '-' || c == ']' || c == '(' || c == ')' ||
                           c == '{' || c == '}' || c == ';' ||
                           c == ',' || c == '%') {
                    lex += c;
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
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
                    tok = TOKEN_CONST;
                    lex += c;
                    tipo = TIPO_STRING;
                    S = 15;
                } else if (c == '/') {
                    S = 17;
                    lex += c;
                } else if (c == EOF) {
                    S = 0;
                } else {
                    throw lex;
                    break;
                }
                break;

            case 1:
                S = 0;
                // Aceita token
                break;

            case 2:  // IDENTIFICADORES
                S = 0;
                if (c == '_') {
                    S = 2;
                    lex += c;
                } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                           (c >= '0' && c <= '9')) {
                    S = 3;
                    lex += c;
                } else if (c == EOF) {
                    throw ERR_EOF;

                    S = 0;
                } else {
                    throw lex;
                }
                break;

            case 3:  // IDENTIFICADORES
                if (c == '_' || (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                    S = 3;
                    lex += c;
                } else {
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;

            case 4:  // CHAR
                if (c != EOF) {
                    lex += c;
                    S = 5;
                } else if (c == EOF) {
                    throw ERR_EOF;
                    S = 0;
                } else {
                    throw lex;
                }
                break;

            case 5:  // CHAR
                if (c == '\'') {
                    lex += c;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c == EOF) {
                    throw ERR_EOF;
                    S = 0;
                } else {
                    throw lex;
                }
                break;

            case 6:  // INTS OU CHARS HEXADECIMAIS
                if (c >= '0' && c <= '9') {
                    lex += c;
                    S = 9;
                } else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    tipo = TIPO_CHAR;
                    lex += c;
                    S = 7;
                } else {
                    tipo = TIPO_INT;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;

            case 7:  // CHARS HEXADECIMAIS
                if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') ||
                    (c >= '0' && c <= '9')) {
                    lex += c;
                    S = 8;
                } else if (c == EOF) {
                    throw ERR_EOF;
                    S = 0;
                } else {
                    throw lex;
                }
                break;

            case 8:  // CHARS HEXADECIMAIS
                if (c == 'h') {
                    lex += c;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c == EOF) {
                    throw ERR_EOF;
                    S = 0;
                } else {
                    throw lex;
                }
                break;

            case 9:  // INTS OU CHARS HEXADECIMAIS
                if (c >= '0' && c <= '9') {
                    lex += c;
                    S = 10;
                } else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    tipo = TIPO_CHAR;
                    lex += c;
                    S = 8;
                } else {
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;

            case 10:  // INTS OU CHARS HEXADECIMAIS
                if (c == 'h') {
                    tipo = TIPO_CHAR;
                    lex += c;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c >= '0' && c <= '9') {
                    tipo = TIPO_INT;
                    lex += c;
                    S = 11;
                } else {
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;

            case 11:  // INTS
                if (c >= '0' && c <= '9') {
                    lex += c;
                    S = 11;
                } else {
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;

            case 12:  // :=
                if (c == '=') {
                    lex += c;
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c == EOF) {
                    throw ERR_EOF;

                    S = 0;
                } else {
                    // cout << S << endl;

                    throw lex;
                }
                break;

            case 13:  // <> ou <=
                if (c == '>' || c == '=') {
                    lex += c;
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else {
                    S = 1;
                    atualizarTabela(lex, tipo, tok, tamanho);
                    cin.unget();
                }
                break;

            case 14:  // >=
                if (c == '=') {
                    lex += c;
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else {
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;

            case 15:  // STRINGS
                if (c != '\"' && c != '$' && c != '\n' && c != EOF) {
                    lex += c;
                    S = 15;
                } else if (c == EOF) {
                    throw ERR_EOF;
                    S = 0;
                } else if (c == '\"') {
                    lex += c;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else {
                    throw lex;
                }
                break;

            case 17:  // comentário
                if (c == '*') {
                    lex += c;
                    S = 18;
                } else {
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;

            case 18:
                if (c == '\n') {
                    lex += c;
                    S = 18;
                    linha++;
                } else if (c == EOF) {
                    throw ERR_EOF;
                } else if (c != '*' && c != '\n') {
                    lex += c;
                    S = 18;
                } else {
                    lex += c;
                    S = 19;
                }
                break;

            case 19:
                if (c == '*') {
                    lex += c;
                    S = 19;
                } else if (c == '/') {
                    lex += c;
                    S = 0;
                } else if (c == '\n') {
                    linha++;
                    lex += c;
                    S = 18;
                } else if (c == EOF) {
                    throw ERR_EOF;
                } else if (c != '*' && c != '/' && c != '\n') {
                    lex += c;
                    S = 18;
                }
                break;
        }
    }
}

//Metodo para verificar a compatibilidade de tipos nas operacoes *, /, %, , +, -, =, >, <, >=, <=, <> OR e AND
void verificaOps(int f1_tipo, int f2_tipo, int f1_tamanho, int f2_tamanho, int t_op) {
    switch(t_op) {
        case TOKEN_ASTER:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_BARRA:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_MOD:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            }
            break;

        case TOKEN_AND:
            if (f1_tipo == TIPO_BOOLEAN && f1_tamanho == 0) { // se tipo 1 for boolean
                if(f2_tipo != TIPO_BOOLEAN || f2_tamanho != 0) throw ERR_TIPO;
            } 
            break;

        case TOKEN_MAIS:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_MENOS:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_OR:
            if (f1_tipo == TIPO_BOOLEAN && f1_tamanho == 0) { // se tipo 1 for boolean
                if(f2_tipo != TIPO_BOOLEAN || f2_tamanho != 0) throw ERR_TIPO;
            } 
            break;

        //ADICIONAR STRING
        case TOKEN_IGUAL:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho != 0) { // se tipo 1 for vetor de char
                if((f2_tipo != TIPO_CHAR && f2_tipo != TIPO_STRING) || f2_tamanho == 0) throw ERR_TIPO; // se tipo 2 nao for vetor de char ou string ERRO
            } else if (f1_tipo == TIPO_BOOLEAN && f1_tamanho == 0) { // se tipo 1 for boolean
                if(f2_tipo != TIPO_BOOLEAN || f2_tamanho != 0) throw ERR_TIPO;
            } else if (f1_tipo == TIPO_STRING){ // se tipo 1 for string
                if((f2_tipo != TIPO_STRING && f2_tipo != TIPO_CHAR) || f2_tamanho == 0) throw ERR_TIPO; // se tipo 2 nao for vetor de char ou string ERRO
            } 
            else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_MAIOR:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_MENOR:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_MENOR_IGUAL:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_MAIOR_IGUAL:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_DIF:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_CHAR && f1_tamanho == 0) { // se tipo 1 for char escalar
                if(f2_tipo != TIPO_CHAR || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for char ou nao for escalar ERRO
            } else if (f1_tipo == TIPO_BOOLEAN && f1_tamanho == 0) { // se tipo 1 for boolean
                if(f2_tipo != TIPO_BOOLEAN || f2_tamanho != 0) throw ERR_TIPO;
            } else {
                throw ERR_TIPO;
            }
            break;
    }
}

bool verificaCompatibDec(int id_tipo, int const_tipo){
    if((id_tipo == TIPO_INT && const_tipo == TIPO_INT) 
        || (id_tipo == TIPO_CHAR && const_tipo == TIPO_CHAR)
        || (id_tipo == TIPO_BOOLEAN && const_tipo == 0)){
        return true;
    }
        return false;
}

// int -> int[]
// int[] -> int
// int -> int

// char id -> string
// char id -> char id
// char[] -> char
// char -> char

// boolean -> boolean
// boolean[] -> boolean
// boolean -> boolean[]

bool verificaCompatibAtr(int id_tipo, int exp_tipo, int id_tamanho, int exp_tamanho){
    // cout << id_tipo << " " << exp_tipo << " " << id_tamanho << " " << exp_tamanho << endl;
    if(id_tipo == TIPO_INT){
        //confere se os inteiros manipulados sao escalares
        if(exp_tipo == TIPO_INT && id_tamanho == 0 && exp_tamanho == 0) return true;
    } else if (id_tipo == TIPO_CHAR){
        if(id_tamanho > 0) { // se ID for vetor
            if(exp_tipo == TIPO_STRING && exp_tamanho <= (id_tamanho-1)) return true;
            else if(exp_tipo == TIPO_CHAR && exp_tamanho <= id_tamanho) return true; 
        } else { // se ID for escalar
            if(exp_tipo == TIPO_CHAR && exp_tamanho == 0) return true;
        }
    } else if (id_tipo == TIPO_BOOLEAN) {
        //confere se os boolean manipulados sao escalares
        if(exp_tipo == TIPO_BOOLEAN && id_tamanho == 0 && exp_tamanho == 0) return true;
    }
}

bool verificaTamanho(int size, int tipo){
    if(tipo == TIPO_INT || tipo == TIPO_BOOLEAN){
        if(size*2 <= 8000) return true;
    } else {
        if(size <= 8000) return true;
    }
    return false;
}

/*
    método casa token
    recebe um token e compara com o token do registro léxico
    se for igual, chama o analisador léxico novamente
    se não, retorna erro de token não esperado
*/
void casaToken(int token_esperado) {
    if (reg.token == token_esperado) {
        analisadorLexico();
    } else {
        throw ERR_TOKEN;
    }
}

/*
----------------------------------------------------
Analisador Sintático
Cada método a seguir representa um símbolo da gramática
Os comentários representam o símbolo representado pelo método
----------------------------------------------------
*/

/* pré declarando a função Exp() para o compilador aceitar
 as demais funções declaradas posteriormente */
void Exp(int &exp_tipo, int &exp_tamanho);

// F -> not F | "(" Exp ")" | CONST | ID [ "[" Exp "]" ]

// F -> not F1 (1) | "(" Exp (2) ")" | CONST (3) | ID (4) [ "[" Exp (5) "]" ]

// (1) { se (f1.tipo != boolean) entao ERRO; f.tipo = f1.tipo; f.valor = f1.valor; f.tamanho = f1.tamanho }
// (2) { f.tipo = exp.tipo; f.tamanho = exp.tamanho; f.valor = exp.valor }
// (3) { f.tipo = const.tipo; f.valor = const.valor;  se (const.tipo == inteiro) f.tamanho = 0 senao f.tamanho = const.tamanho }
// (4) { f.tipo = id.tipo; f.tamanho = id.tamanho }
// (5) { se (exp.tipo != inteiro || exp.tamanho > 0) entao ERRO; f.tamanho = exp.tamanho }
void F(int &f_tipo, int &f_tamanho) {
    int f1_tipo, f1_tamanho; 
    int exp_tipo, exp_tamanho;
    int const_tipo, const_tamanho, id_tipo, id_tamanho;

    if (reg.token == TOKEN_NOT) {
        casaToken(TOKEN_NOT);
        F(f1_tipo, f1_tamanho);
        if(f1_tipo != TIPO_BOOLEAN){
            throw ERR_TIPO;
        }
        f_tamanho = f1_tamanho;
    } else if (reg.token == TOKEN_ABRE_PAREN) {
        casaToken(TOKEN_ABRE_PAREN);
        Exp(exp_tipo, exp_tamanho);
        f_tipo = exp_tipo;
        f_tamanho = exp_tamanho;
        casaToken(TOKEN_FECHA_PAREN);
    } else if (reg.token == TOKEN_CONST) {
        const_tipo = reg.tipo;
        const_tamanho = reg.tamanho;
        f_tipo = const_tipo;
        if(const_tipo != TIPO_STRING) f_tamanho = 0;
        else f_tamanho = const_tamanho; 
        
        casaToken(TOKEN_CONST);
    } else {
        int id_pos, id_tipo;
        string id_lex;
        id_pos = reg.posicao;
        id_lex = reg.lexema;
        id_tipo = t.getTipo(id_lex, id_pos);
        id_tamanho = t.getTamanho(id_lex, id_pos);
        f_tipo = id_tipo;
        f_tamanho = id_tamanho;
        casaToken(TOKEN_ID);
        if (reg.token == TOKEN_ABRE_COLCH) {
            casaToken(TOKEN_ABRE_COLCH);
            Exp(exp_tipo, exp_tamanho);
            if((exp_tipo != TIPO_INT) || exp_tamanho > 0){
                throw ERR_TIPO;
            }
            f_tamanho = exp_tamanho;
            casaToken(TOKEN_FECHA_COLCH);
        }
    }
}

// T ->  F { ( * | / | % | and ) F }

// T ->  F1 (1) { ( * (2) | / (3) | % (4) | and (5) ) F2 (6) }

// (1) { t.tipo = f1.tipo; t.tamanho = f1.tamanho }
// (2) { t.op = mult }
// (3) { t.op = div }
// (4) { t.op = mod }
// (5) { t.op = and; t.tipo = boolean }
// (6) { se não(verificaOps) entao ERRO }
void T(int &t_tipo, int &t_tamanho) {
    int t_op;
    int f1_tipo, f1_tamanho, f2_tipo, f2_tamanho;

    F(f1_tipo, f1_tamanho);
   
    t_tipo = f1_tipo;
    t_tamanho = f1_tamanho;

    while (reg.token == TOKEN_ASTER || reg.token == TOKEN_BARRA ||
           reg.token == TOKEN_MOD || reg.token == TOKEN_AND) {
        if (reg.token == TOKEN_ASTER) {
            t_op = reg.token;
            casaToken(TOKEN_ASTER);
        } else if (reg.token == TOKEN_BARRA) {
            t_op = reg.token;
            casaToken(TOKEN_BARRA);
        } else if (reg.token == TOKEN_MOD) {
            t_op = reg.token;
            casaToken(TOKEN_MOD);
        } else {
            t_op = reg.token;
            t_tipo = TIPO_BOOLEAN;
            casaToken(TOKEN_AND);
        }
        F(f2_tipo, f2_tamanho);
        verificaOps(f1_tipo, f2_tipo, f1_tamanho, f2_tamanho, t_op);
        
    }
    
}

// ExpS -> [ + | - ] T { ( + | - | or ) T }

// ExpS -> [ + | - ] T1 (1) { ( + (2) | - (3) | or (4) ) T2 (5) }

// (1) { exps.tipo = t1.tipo; exps_tamanho = t1.tamanho }
// (2) { exps.op = add }
// (3) { exps.op = sub }
// (4) { exps.op = or; exps.tipo = boolean }
// (5) {verificaOps}

void ExpS(int &exps_tipo, int &exps_tamanho) {
    int exps_op;
    int t1_tipo, t1_tamanho, t2_tipo, t2_tamanho;

    if (reg.token == TOKEN_MAIS) {
        casaToken(TOKEN_MAIS);
    } else if (reg.token == TOKEN_MENOS) {
        casaToken(TOKEN_MENOS);
    }

    T(t1_tipo, t1_tamanho);
    exps_tipo = t1_tipo;
    exps_tamanho = t1_tamanho;

    while (reg.token == TOKEN_MAIS || reg.token == TOKEN_MENOS ||
           reg.token == TOKEN_OR) {
        if (reg.token == TOKEN_MAIS) {
            exps_op = TOKEN_MAIS;
            casaToken(TOKEN_MAIS);
        } else if (reg.token == TOKEN_MENOS) {
            exps_op = TOKEN_MENOS;
            casaToken(TOKEN_MENOS);
        } else {
            exps_op = TOKEN_OR;
            exps_tipo = TIPO_BOOLEAN;
            casaToken(TOKEN_OR);
        }
        T(t2_tipo, t2_tamanho);
        verificaOps(t1_tipo, t2_tipo, t1_tamanho, t2_tamanho, exps_op);
    }
}

// Exp -> ExpS [ ( = | > | < | <> | <= | >= ) ExpS ]
// Exp -> ExpS1 (1) [ ( = | > | < | <> | <= | >= ) (2) ExpS2 (3) ]

// (1) { exp_tipo = exps_tipo; exp.tamanho = exps.tamanho;  }
// (2) { exp_tipo = boolean; exp.tamanho = 0 }
// (3) { se nao(VerificaOps) entao ERRO }
void Exp(int &exp_tipo, int &exp_tamanho) {
    // declarando variaveis que serão necessarias para o semantico
    int exps1_tipo, exps1_tamanho, exps2_tipo, exps2_tamanho;
    int exp_op;
    
    ExpS(exps1_tipo, exps1_tamanho);

    exp_tipo = exps1_tipo;
    exp_tamanho = exps1_tamanho;

    if (reg.token == TOKEN_IGUAL || reg.token == TOKEN_MAIOR ||
        reg.token == TOKEN_MENOR || reg.token == TOKEN_DIF ||
        reg.token == TOKEN_MENOR_IGUAL || reg.token == TOKEN_MAIOR_IGUAL) {
        if (reg.token == TOKEN_IGUAL) {
            exp_op = TOKEN_IGUAL;
            casaToken(TOKEN_IGUAL);
        } else if (reg.token == TOKEN_MAIOR) {
            exp_op = TOKEN_MAIOR;
            casaToken(TOKEN_MAIOR);
        } else if (reg.token == TOKEN_MENOR) {
            exp_op = TOKEN_MENOR;
            casaToken(TOKEN_MENOR);
        } else if (reg.token == TOKEN_DIF) {
            exp_op = TOKEN_DIF;
            casaToken(TOKEN_DIF);
        } else if (reg.token == TOKEN_MENOR_IGUAL) {
            exp_op = TOKEN_MENOR_IGUAL;
            casaToken(TOKEN_MENOR_IGUAL);
        } else {
            exp_op = TOKEN_MAIOR_IGUAL;
            casaToken(TOKEN_MAIOR_IGUAL);
        }

        exp_tipo = TIPO_BOOLEAN;
        exp_tamanho = 0;

        ExpS(exps2_tipo, exps2_tamanho);
        
        verificaOps(exps1_tipo, exps2_tipo, exps1_tamanho, exps2_tamanho, exp_op);
    }
}

// Dec -> ( int | boolean | char ) ID [:= [-]CONST | "[" CONST "]"] {, ID [:=
// [-]CONST | "[" CONST "]"] } ; | final ID = [-]CONST ;
void Dec() {
    // iniciando variaveis regra DEC
    int dec_tipo, dec_classe;
    int id_tipo, id_classe, id_pos, id_tamanho;
    string id_lex;
    int const_tipo, const_tam; 
    string const_val;

    if (reg.token == TOKEN_FINAL) {
        dec_classe = CLASSE_CONST;
        casaToken(TOKEN_FINAL);
        
		// retorna os dados do identificador lido
        id_pos = reg.posicao;
        id_lex = reg.lexema;
        id_tamanho = t.getTamanho(id_lex, id_pos);
        id_tipo = t.getTipo(id_lex, id_pos);
        id_classe = t.getClasse(id_lex, id_pos);
        
		//verifica a unicidade
        if(id_tipo != TIPO_VAZIO){
            throw ERR_EXISTS;
        } else {
            id_classe = dec_classe;
        }
        casaToken(TOKEN_ID);
        casaToken(TOKEN_IGUAL);
        if (reg.token == TOKEN_MENOS) {
            casaToken(TOKEN_MENOS);
        }
        
		// retorna os dados da constante
        const_val = reg.lexema;
        const_tipo = reg.tipo;
        const_tam = reg.tamanho;
        id_tipo = const_tipo;
        casaToken(TOKEN_CONST);
        casaToken(TOKEN_PONTO_VIRG);
        t.atualizar(id_lex, id_tipo, id_tamanho, id_classe);
    } else {
        if (reg.token == TOKEN_INT) {
            dec_tipo = TIPO_INT;
            dec_classe = CLASSE_VAR;
            casaToken(TOKEN_INT);
        } else if (reg.token == TOKEN_CHAR) {
            dec_tipo = TIPO_CHAR;
            dec_classe = CLASSE_VAR;
            casaToken(TOKEN_CHAR);
        } else {
            dec_tipo = TIPO_BOOLEAN;
            dec_classe = CLASSE_VAR;
            casaToken(TOKEN_BOOL);
        }

        // retorna os dados do identificador lido
        id_pos = reg.posicao;
        id_lex = reg.lexema;
        id_tamanho = t.getTamanho(id_lex, id_pos);
        id_tipo = t.getTipo(id_lex, id_pos);
        id_classe = t.getClasse(id_lex, id_pos);
        
		//verifica a unicidade
        if(id_tipo != TIPO_VAZIO){
            throw ERR_EXISTS;
        } else { 
            id_tipo = dec_tipo;
            id_classe = dec_classe;
        }

        casaToken(TOKEN_ID);
        if (reg.token == TOKEN_ATRIB) {
            casaToken(TOKEN_ATRIB);
            if (reg.token == TOKEN_MENOS) {
                casaToken(TOKEN_MENOS);
            }
            
			// retorna os dados da constante
            const_val = reg.lexema;
            const_tipo = reg.tipo;
            const_tam = reg.tamanho;
            
			// verifica compatibilidade
            if(!verificaCompatibDec(id_tipo, const_tipo)){ // INCOMPLETO
                throw ERR_TIPO;
            }
            casaToken(TOKEN_CONST);

        } else if (reg.token == TOKEN_ABRE_COLCH) {
            casaToken(TOKEN_ABRE_COLCH);
            
			// retorna os dados da constante
            const_val = reg.lexema;
            const_tipo = reg.tipo;
            const_tam = reg.tamanho;
            
			// verifica tamanho do vetor
            if(!verificaTamanho(stoi(const_val), id_tipo)){ //INCOMPLETO
                throw ERR_TAMANHO;
            }
            casaToken(TOKEN_CONST);
            casaToken(TOKEN_FECHA_COLCH);
        }
        t.atualizar(id_lex, id_tipo, id_tamanho, id_classe);
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            
			// retorna os dados do identificador lido
            id_pos = reg.posicao;
            id_lex = reg.lexema;
            id_tamanho = t.getTamanho(id_lex, id_pos);
            id_tipo = t.getTipo(id_lex, id_pos);
            id_classe = t.getClasse(id_lex, id_pos);

            //verifica a unicidade
            if(id_tipo != TIPO_VAZIO){
                throw ERR_EXISTS;
            } else { 
                id_tipo = dec_tipo;
                id_classe = dec_classe;
            }
            casaToken(TOKEN_ID);
            if (reg.token == TOKEN_ATRIB) {
                casaToken(TOKEN_ATRIB);
                if (reg.token == TOKEN_MENOS) {
                    casaToken(TOKEN_MENOS);
                }
                
				// retorna os dados da constante
                const_val = reg.lexema;
                const_tipo = reg.tipo;
                const_tam = reg.tamanho;
                
				// verifica compatibilidade
                if(!verificaCompatibDec(id_tipo, const_tipo)){ // INCOMPLETO
                    throw ERR_TIPO;
                }
                casaToken(TOKEN_CONST);
            } else if (reg.token == TOKEN_ABRE_COLCH) {
                casaToken(TOKEN_ABRE_COLCH);
                
				// retorna os dados da constante
                const_val = reg.lexema;
                const_tipo = reg.tipo;
                const_tam = reg.tamanho;
                
				// verifica tamanho do vetor
                if(!verificaTamanho(stoi(const_val), id_tipo)){ //INCOMPLETO
                    throw ERR_TAMANHO;
                }
                casaToken(TOKEN_CONST);
                casaToken(TOKEN_FECHA_COLCH);
            }
            t.atualizar(id_lex, id_tipo, id_tamanho, id_classe);
        }
        casaToken(TOKEN_PONTO_VIRG);
    }
}

// pré declarando comando Cmd(); e CmdP();
void Cmd();
void CmdP();

// BlocoCmd -> "{" { Cmd } "}"
void BlocoCmd() {
    casaToken(TOKEN_ABRE_CHAVE);
    while (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
           reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
           reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
           reg.token == TOKEN_WRITELN) {
        Cmd();
    }
    casaToken(TOKEN_FECHA_CHAVE);
}

// CmdAtr -> ID ["["Exp"]"] := Exp
// CmdAtr -> ID (1)(2) ["[" Exp (3) "]"] := Exp1 (4)

// (1) { if id.tipo == vazio then ERRO else atr.tipo = id_tipo; }
// (2) { if id.classe == classe_const then ERRO }
// (3) { se nao(exp.tipo == tipo_int) entao erro }
// (4) { se (nao compativel(atr.tipo, exp1.tipo)) entao ERRO }
void CmdAtr() {
    int atr_tipo, exp1_tipo, exp1_tamanho, exp2_tipo, exp2_tamanho, id_tipo, id_tamanho, id_pos, id_classe; // variaveis necessarias para semantico
    string id_lex;
    
    //verificando existencia do identificador
    id_pos = reg.posicao;
    id_lex = reg.lexema;
    id_tipo = t.getTipo(id_lex, id_pos);
    id_classe = t.getClasse(id_lex, id_pos);
    id_tamanho = t.getTamanho(id_lex, id_pos);


    if(id_tipo == TIPO_VAZIO){ // se tipo vazio, id não declarado
        throw ERR_NOT_EXISTS;
    } else { // senão, atribui tipo do id
        atr_tipo = id_tipo;
    }

    if(id_classe == CLASSE_CONST) {
        throw ERR_CLASSE;
    }

    casaToken(TOKEN_ID);
    if (reg.token == TOKEN_ABRE_COLCH) {
        casaToken(TOKEN_ABRE_COLCH);
        Exp(exp1_tipo, exp1_tamanho);

        if(exp1_tipo != TIPO_INT){
            throw ERR_TIPO;
        }
        id_tamanho = 0; //tamanho do id assume 0 pois agora ele sera um escalar
        casaToken(TOKEN_FECHA_COLCH);
    }
    casaToken(TOKEN_ATRIB);

    Exp(exp2_tipo, exp2_tamanho);
    

    if(!(verificaCompatibAtr(id_tipo, exp2_tipo, id_tamanho, exp2_tamanho))) throw ERR_TIPO;
}

// CmdRep -> for"(" [CmdP {, CmdP}]; Exp; [CmdP {, CmdP}] ")" (Cmd | BlocoCmd)
void CmdRep() {

    int exp_tipo, exp_tamanho;

    casaToken(TOKEN_FOR);
    casaToken(TOKEN_ABRE_PAREN);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
        reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN) {
        CmdP();
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
                reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN) {
                CmdP();
            }
        }
    }

    casaToken(TOKEN_PONTO_VIRG);
    Exp(exp_tipo, exp_tamanho);
    casaToken(TOKEN_PONTO_VIRG);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
        reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN) {
        CmdP();
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
                reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN) {
                CmdP();
            }
        }
    }

    casaToken(TOKEN_FECHA_PAREN);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
        reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
        reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
        reg.token == TOKEN_WRITELN) {
        Cmd();
    } else {
        BlocoCmd();
    }
}

// CmdIf -> if"(" Exp ")" then (Cmd | BlocoCmd) [else (Cmd | BlocoCmd)]
void CmdIf() {

    int exp_tipo, exp_tamanho;

    casaToken(TOKEN_IF);
    casaToken(TOKEN_ABRE_PAREN);
    Exp(exp_tipo, exp_tamanho);
    casaToken(TOKEN_FECHA_PAREN);
    casaToken(TOKEN_THEN);

    if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
        reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
        reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
        reg.token == TOKEN_WRITELN) {
        Cmd();
    } else {
        BlocoCmd();
    }

    if (reg.token == TOKEN_ELSE) {
        casaToken(TOKEN_ELSE);
        if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
            reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
            reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
            reg.token == TOKEN_WRITELN) {
            Cmd();
        } else {
            BlocoCmd();
        }
    }
}

// CmdNull -> ;
void CmdNull() { casaToken(TOKEN_PONTO_VIRG); }

// CmdRead -> readln "(" ID ["[" Exp "]"] ")"
void CmdRead() {

    int exp_tipo, exp_tamanho;
    casaToken(TOKEN_READLN);
    casaToken(TOKEN_ABRE_PAREN);
    casaToken(TOKEN_ID);
    if (reg.token == TOKEN_ABRE_COLCH) {
        casaToken(TOKEN_ABRE_COLCH);
        Exp(exp_tipo, exp_tamanho);
        casaToken(TOKEN_FECHA_COLCH);
    }
    casaToken(TOKEN_FECHA_PAREN);
}

// CmdWrite -> (write|writeln)"(" Exp {, Exp} ")"
void CmdWrite() {
    int exp_tipo, exp1_tipo, exp_tamanho, exp1_tamanho;

    if (reg.token == TOKEN_WRITE) {
        casaToken(TOKEN_WRITE);
    } else {
        casaToken(TOKEN_WRITELN);
    }

    casaToken(TOKEN_ABRE_PAREN);
    Exp(exp_tipo, exp_tamanho);
    while (reg.token == TOKEN_VIRG) {
        casaToken(TOKEN_VIRG);
        Exp(exp1_tipo, exp1_tamanho);
    }
    casaToken(TOKEN_FECHA_PAREN);
}

// CmdP -> CmdAtr | CmdWrite | CmdRead
void CmdP() {
    if (reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN) {
        CmdWrite();
    } else if (reg.token == TOKEN_READLN) {
        CmdRead();
    } else {
        CmdAtr();
    }
}

// Cmd -→ CmdP ; | CmdFor | CmdIf | CmdNull
void Cmd() {
    if (reg.token == TOKEN_FOR) {
        CmdRep();
    } else if (reg.token == TOKEN_IF) {
        CmdIf();
    } else if (reg.token == TOKEN_PONTO_VIRG) {
        CmdNull();
    } else /*if(reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN ||
              reg.token == TOKEN_READLN || reg.token == TOKEN_ID || reg.token ==
              TOKEN_PONTO_VIRG)*/
    {
        CmdP();
        casaToken(TOKEN_PONTO_VIRG);
    }
}

// Prog -> { Dec } main BlocoCmd EOF
void Prog() {
    while (reg.token == TOKEN_INT || reg.token == TOKEN_CHAR ||
           reg.token == TOKEN_BOOL || reg.token == TOKEN_FINAL) {
        Dec();
    }
    casaToken(TOKEN_MAIN);
    BlocoCmd();
}

/*
----------------------------------------------------
Método principal
Inicializa a tabela de tokens com os tokens e palavras reservadas
Executa os analisadores a partir do primeiro token recebido
----------------------------------------------------
*/
int main() {
    // iniciando tabela de simbolos
    t.inserir("final", TOKEN_FINAL);
    t.inserir("int", TOKEN_INT);
    t.inserir("char", TOKEN_CHAR);
    t.inserir("for", TOKEN_FOR);
    t.inserir("if", TOKEN_IF);
    t.inserir("TRUE", TOKEN_CONST);
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
    t.inserir("FALSE", TOKEN_CONST);
    t.inserir("write", TOKEN_WRITE);
    t.inserir("writeln", TOKEN_WRITELN);
    t.inserir("%", TOKEN_MOD);
    t.inserir("[", TOKEN_ABRE_COLCH);
    t.inserir("]", TOKEN_FECHA_COLCH);
    t.inserir("main", TOKEN_MAIN);
    t.inserir("EOF", TOKEN_EOF);  // CONSERTAR !!!!!
    t.inserir("boolean", TOKEN_BOOL);

    try {
        analisadorLexico();
        Prog();
        casaToken(TOKEN_EOF);
    } catch (int err) {
        switch(err){
            case ERR_TOKEN:
                cout << linha << endl << "token nao esperado [" << reg.lexema << "].";
                break;
            case ERR_EOF:
                cout << linha << endl << "fim de arquivo nao esperado.";
                break;
            case ERR_CHAR:
                cout << linha << endl << "caractere invalido.";
                break;
            case ERR_TIPO:
                cout << linha << endl << "tipos incompativeis.";
                break;
            case ERR_TAMANHO:
                cout << linha << endl << "tamanho do vetor excede o maximo permitido.";
                break;
            case ERR_EXISTS:
                cout << linha << endl << "identificador ja declarado [" << reg.lexema << "].";
                break;
            case ERR_NOT_EXISTS:
                cout << linha << endl << "identificador nao declarado [" << reg.lexema << "].";
                break;
            case ERR_CLASSE:
                cout << linha << endl << "classe de identificador incompatível [" << reg.lexema << "].";
                break;
        }
        return 0;
    } catch (string err) {
        cout << linha << endl << "lexema nao identificado [" << err << "].";
        return 0;
    }

    cout << linha << " linhas compiladas.";
    return 0;
}

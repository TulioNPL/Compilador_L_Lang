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

#define POS_TEMPORARIOS 0



// Definição da estrutura dos símbolos para serem inseridos na tabela de
// símbolos
struct Simbolo {
    string lexema;
    int token;
    int tipo;
    int classe;
    int tamanho;
    int endereco;
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

// iniciando variavel global para memoria
int contadorDados;

//iniciando variavel global para temporarios
int contadorTemp = POS_TEMPORARIOS; // inicia em 0

//iniciando variavel global para rotulos
int contadorRot = 0;

// iniciando variavel global para o arquivo de saida
ofstream saida;

string novoRot(){
    return "R" + std::to_string(contadorRot++);
}


// metodo para criar temporario para expressoes
int novoTemp(int tipo){
    int temp = contadorTemp;
    if(tipo == TIPO_CHAR){
        contadorTemp += 1;
    } else {
        contadorTemp += 2;
    }
    return temp;
}

// metodo para criar temporarios para o buffer de entrada
int novoBuffer(int tipo, int tamanho){
    int temp = contadorTemp;
    if(tipo == TIPO_CHAR){
        contadorTemp += 1*tamanho;
    } else {
        contadorTemp += 2*tamanho;
    }
    return temp;
}

// classe da tabela de simbolos
class TabelaSimbolos {
    int num_posicoes;

    list<Simbolo> *tabela;
    int pos_inicial = 0x4000;

   public:
    TabelaSimbolos(int n);  // Construtor
    int inserir(string lex, int token);
    int pesquisar(string lex);
    int hash(string lex);
    int getToken(string lex, int pos);
    int getTamanho(string lex, int pos);
    int getTipo(string lex, int pos);
    int getClasse(string lex, int pos);
    int getEnd(string lex, int pos);
    void atualizar(string lex, int tipo, int tamanho, int classe, int end);
    void mostrar();
};


//Construtor
TabelaSimbolos::TabelaSimbolos(int n) {
    this->num_posicoes = n;
    tabela = new list<Simbolo>[num_posicoes];
}


// metodo para atualizar tabela com os tipos dos identificadores
void TabelaSimbolos::atualizar(string lex, int tipo, int tamanho, int classe, int end){
    int pos = pesquisar(lex);
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            i->tamanho = tamanho;
            i->classe = classe;
            i->tipo = tipo;
            i->endereco = end;
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


//Retorna o tamanho de um simbolo pesquisando pelo lexema
int TabelaSimbolos::getTamanho(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->tamanho;
        }
    }
    return -1;
}


//Retorna o tipo de um simbolo pesquisando pelo lexema
int TabelaSimbolos::getTipo(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->tipo;
        }
    }
    return -1;
}


//Retorna a classe de um simbolo pesquisando pelo lexema
int TabelaSimbolos::getClasse(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->classe;
        }
    }
    return -1;
}

//Retorna o endereco de um simbolo pesquisando pelo lexema
int TabelaSimbolos::getEnd(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->endereco;
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
            cout << " --> " << x->lexema << " " << x->token << " , endereco: " << x->endereco;
        }
    }
}


// insere um registro na tabela de símbolos e retorna sua posição
int TabelaSimbolos::inserir(string lex, int token) {
    Simbolo s = {};
    s.lexema = lex;
    s.token = token;
    s.tipo = TIPO_VAZIO;
    s.tamanho = 0;
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
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_BARRA:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
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
            } else {
                throw ERR_TIPO;
            }
            break;

        case TOKEN_MENOS:
            if(f1_tipo == TIPO_INT && f1_tamanho == 0) { // se tipo 1 for int escalar
                if(f2_tipo != TIPO_INT || f2_tamanho != 0) throw ERR_TIPO; // se tipo 2 nao for int ou nao for escalar ERRO
            }else {
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


//Verifica compatibilidade de tipos na declaracao
bool verificaCompatibDec(int id_tipo, int const_tipo){
    if((id_tipo == TIPO_INT && const_tipo == TIPO_INT) 
        || (id_tipo == TIPO_CHAR && const_tipo == TIPO_CHAR)
        || (id_tipo == TIPO_BOOLEAN && const_tipo == 0)){
        return true;
    }
        return false;
}


//Verifica a compatibilidade de tipos no comando ReadLn
bool verificaCompatibRead(int id_tipo, int id_tamanho) {
    if (id_tipo == TIPO_INT){
        if(id_tamanho == 0) return true;
    } else if (id_tipo == TIPO_CHAR) {
        return true;
    }  
    return false;
}


//Verifica a compatibilidade de tipos do comando Write
bool verificaCompatibWrite(int exp_tipo, int exp_tamanho){
    if(exp_tipo == TIPO_INT){
        if(exp_tamanho == 0) return true;
    } else if (exp_tipo == TIPO_CHAR || exp_tipo == TIPO_STRING) {
        return true;
    } 

    return false;
}


//Verifica a compatibilidade de tipos na atribuicao
bool verificaCompatibAtr(int id_tipo, int exp_tipo, int id_tamanho, int exp_tamanho){
    if(id_tipo == TIPO_INT){
        //confere se os inteiros manipulados sao escalares
        // aqui demtro ten qui ver si eh vetor o nao
        if(exp_tipo == TIPO_INT && id_tamanho == 0 && exp_tamanho == 0) return true;
    } else if (id_tipo == TIPO_CHAR){
        if(id_tamanho > 0) { // se ID for vetor
            if(exp_tipo == TIPO_STRING && exp_tamanho -1 <= id_tamanho)return true;
            else if(exp_tipo == TIPO_CHAR && id_tamanho <= exp_tamanho) return true; 
        } else { // se ID for escalar
            if(exp_tipo == TIPO_CHAR && exp_tamanho == 0) return true;
        }
    } else if (id_tipo == TIPO_BOOLEAN) {
        //confere se os boolean manipulados sao escalares
        if(exp_tipo == TIPO_BOOLEAN && id_tamanho == 0 && exp_tamanho == 0) return true;
    }
    return false;
}


//Verifica se o tamanho do vetor excede o permitido
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
	metodo printDec
	recebe o tipo, classe, valor e tamanho da variavel declarada
	e converte sua declaracao para masm
*/
void printDec(int tipo, int classe, string val, int tamanho, bool ehNeg){

    if(classe == CLASSE_CONST){
        if(tipo == TIPO_INT) {
            saida << "\tsword " << (ehNeg?"-":"") << val << "\t;const. Int em "  << contadorDados << endl;
            contadorDados += 2;

        } else if(tipo == TIPO_CHAR) {
            saida << "\tbyte " << val << "\t;const. Char em "  << contadorDados << endl;
            contadorDados += 1;

        } else if(tipo == TIPO_STRING) {
            string s = val.substr(1, val.length() - 2);
            saida << "\tbyte \"" << s << "$\" \t;const. String em "  << contadorDados << endl;
            contadorDados += tamanho + 1;

        } else { //Bool
            saida << "\tsword " << (!val.compare("TRUE")?"1":"0") << "\t;const. Bool em "  << contadorDados << endl;
            contadorDados += 2;
        }
    } else {
        if(tamanho > 0){
            if(tipo == TIPO_INT) {
                saida << "\tsword " << tamanho << " DUP(?) \t;var. Vet Int em "  << contadorDados << endl;
                contadorDados += 2*tamanho;

            } else if(tipo == TIPO_CHAR) {
                saida << "\tbyte " << tamanho << " DUP(?) \t;var. Vet Char em "  << contadorDados << endl;
                contadorDados += tamanho;

            } else { //Bool
                saida << "\tsword " << tamanho << " DUP(?) \t;var. Vet Bool em " << contadorDados << endl;
                contadorDados += 2*tamanho; 
            }
            
        } else {
            if(tipo == TIPO_INT) {
                saida << "\tsword " << (ehNeg?"-":"") << (!val.compare("VAZIO")?"?":val) << " \t;var. Int em " << contadorDados << endl;
                contadorDados += 2;

            } else if(tipo == TIPO_CHAR) {
                saida << "\tbyte " << (!val.compare("VAZIO")?"?":val) << " \t;var. Char em " << contadorDados << endl;
                contadorDados += 1;
                
            } else { //Bool
                saida << "\tsword " << (!val.compare("VAZIO")?"?":(!val.compare("TRUE")?"1":"0")) << " \t;var. Bool em " << contadorDados << endl;
                contadorDados += 2;
            }
        }
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
void Exp(int &exp_tipo, int &exp_tamanho, int &exp_end);


// F -> not F1 (1) | "(" Exp (2) ")" | CONST (3) | ID (6)(4) [ "[" Exp (5) "]" ]
// (1) { se (f1.tipo != boolean) entao ERRO; f.tipo = f1.tipo; f.valor = f1.valor; f.tamanho = f1.tamanho }
// (2) { f.tipo = exp.tipo; f.tamanho = exp.tamanho; f.valor = exp.valor }
// (3) { f.tipo = const.tipo; f.valor = const.valor;  se (const.tipo == inteiro) f.tamanho = 0 senao f.tamanho = const.tamanho }
// (4) { f.tipo = id.tipo; f.tamanho = id.tamanho }
// (5) { se (exp.tipo != inteiro || exp.tamanho > 0) entao ERRO; f.tamanho = exp.tamanho }
// (6) { se id.tipo == TIPO_VAZIO entao ERRO }
void F(int &f_tipo, int &f_tamanho, int &f_end) {
    int f1_tipo, f1_tamanho, f1_end; 
    int exp_tipo, exp_tamanho, exp_end;
    int const_tipo, const_tamanho, id_tipo, id_tamanho, id_end;

    if (reg.token == TOKEN_NOT) {
        casaToken(TOKEN_NOT);
        F(f1_tipo, f1_tamanho, f1_end);

        if(f1_tipo != TIPO_BOOLEAN){
            throw ERR_TIPO;
        }
        f_tamanho = f1_tamanho;
        f_tipo = f1_tipo; 
        f_end = novoTemp(f_tipo);
        saida << "\tmov ax, DS:[" << dec << f1_end << "]" << endl;
        saida << "\tneg ax" << endl;
        saida << "\tadd ax, 1" << endl;
        saida << "\tmov DS:[" << dec << f_end << "], ax" << endl;

    } else if (reg.token == TOKEN_ABRE_PAREN) {
        casaToken(TOKEN_ABRE_PAREN);
        Exp(exp_tipo, exp_tamanho, exp_end);
        f_tipo = exp_tipo;
        f_tamanho = exp_tamanho;
        f_end = exp_end;
        casaToken(TOKEN_FECHA_PAREN);

    } else if (reg.token == TOKEN_CONST) {
        const_tipo = reg.tipo;
        const_tamanho = reg.tamanho;
        f_tipo = const_tipo;

        if (reg.lexema == "TRUE") {
            f_tipo = TIPO_BOOLEAN;
            reg.lexema = "1";
        } else if(reg.lexema == "FALSE"){
            reg.lexema = "0";
            f_tipo = TIPO_BOOLEAN;
        } 
            
        if(const_tipo != TIPO_STRING){
            f_tamanho = 0;
            f_end = novoTemp(f_tipo);
            saida << "\tmov ax, " << reg.lexema << endl;
            saida << "\tmov DS:[" << dec << f_end << "], ax" << endl;
        } 
        else { //Const eh string
            f_tamanho = const_tamanho; 
            int tamanho_string = f_tamanho - 2 + 1; // -2 das aspas, +1 do $
            saida << "dseg SEGMENT PUBLIC" << endl;
            saida << "\tbyte " << "\"" << reg.lexema.substr(1,reg.lexema.length() - 2) << "$\" \t;const. String em "  << contadorDados << endl; 
            saida << "dseg ENDS" << endl;
            f_end = contadorDados;
            contadorDados += tamanho_string;
        }

        casaToken(TOKEN_CONST);
    } else {
        int id_pos, id_tipo;
        string id_lex;
        id_pos = reg.posicao;
        id_lex = reg.lexema;
        id_tipo = t.getTipo(id_lex, id_pos);
        id_tamanho = t.getTamanho(id_lex, id_pos);
        id_end = t.getEnd(id_lex, id_pos);
        f_tipo = id_tipo;
        f_tamanho = id_tamanho;
        f_end = id_end;

        if(f_tipo == TIPO_VAZIO) throw ERR_NOT_EXISTS;

        casaToken(TOKEN_ID);
        if (reg.token == TOKEN_ABRE_COLCH) {
            if(f_tamanho == 0) {
                throw ERR_TIPO;
            }
            casaToken(TOKEN_ABRE_COLCH);
            Exp(exp_tipo, exp_tamanho, exp_end);
            if((exp_tipo != TIPO_INT) || exp_tamanho > 0){
                throw ERR_TIPO;
            }
            f_tamanho = exp_tamanho;
            f_end = novoTemp(f_tipo);
            saida << "\tmov bx, DS:[" << exp_end<< "]" << endl;
            if(f_tipo == TIPO_INT) 
                saida << "\tadd bx, DS:[" << exp_end << "]" << endl;
            saida << "\tadd bx, "  << id_end << "" << endl;
            saida << "\tmov ax, DS:[bx]" << endl;
            saida << "\tmov DS:[" << f_end << "], ax" << endl;
            casaToken(TOKEN_FECHA_COLCH);
        }
    }
}


// T ->  F1 (1) { ( * (2) | / (3) | % (4) | and (5) ) F2 (6) }
// (1) { t.tipo = f1.tipo; t.tamanho = f1.tamanho }
// (2) { t.op = mult }
// (3) { t.op = div }
// (4) { t.op = mod }
// (5) { t.op = and; t.tipo = boolean }
// (6) { se não(verificaOps) entao ERRO }
void T(int &t_tipo, int &t_tamanho, int &t_end) {
    int t_op;
    int f1_tipo, f1_tamanho, f1_end, f2_tipo, f2_tamanho, f2_end;

    F(f1_tipo, f1_tamanho,f1_end);
   
    t_tipo = f1_tipo;
    t_tamanho = f1_tamanho;
    t_end = f1_end;

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
        F(f2_tipo, f2_tamanho, f2_end);
        verificaOps(f1_tipo, f2_tipo, f1_tamanho, f2_tamanho, t_op);
        
        saida << "\tmov ax, DS:[" << t_end << "]" << endl;
        saida << "\tmov bx, DS:[" << f2_end<< "]" << endl;
        if (t_op == TOKEN_ASTER) saida << "\timul bx" << endl;
        else if(t_op == TOKEN_BARRA) { 
            saida << "\tcwd" << endl;
            saida << "\tidiv bx" << endl;
        } else if(t_op == TOKEN_MOD) {
            saida << "\tcwd" << endl;
            saida << "\tidiv bx" << endl;
            saida << "\tmov ax, dx" << endl; // move resto para o registrador Ax
        } else saida << "\timul bx" << endl;
            

        t_end = novoTemp(t_tipo);
        saida << "\tmov DS:[" << t_end << "], ax" << endl;

    }
    
}


// ExpS -> [ + | - ] T1 (1) { ( + (2) | - (3) | or (4) ) T2 (5) }
// (1) { exps.tipo = t1.tipo; exps_tamanho = t1.tamanho }
// (2) { exps.op = add }
// (3) { exps.op = sub }
// (4) { exps.op = or; exps.tipo = boolean }
// (5) {verificaOps}
void ExpS(int &exps_tipo, int &exps_tamanho, int &exps_end) {
    int exps_op;
    int t1_tipo, t1_tamanho, t1_end, t2_tipo, t2_tamanho, t2_end;
    bool neg = false;
    bool sinal = false;

    if (reg.token == TOKEN_MAIS) {
        sinal = true;
        casaToken(TOKEN_MAIS);
    } else if (reg.token == TOKEN_MENOS) {
        sinal = true;
        neg = true;
        casaToken(TOKEN_MENOS);
    }

    T(t1_tipo, t1_tamanho, t1_end);
    exps_tipo = t1_tipo;
    exps_tamanho = t1_tamanho;
    exps_end = t1_end;

    if(sinal){
        if(exps_tipo != TIPO_INT) throw ERR_TIPO;
    }

    if(neg) {
        saida << "\tmov ax,DS:[" << exps_end << "]" << endl;
        saida << "\tneg ax" << endl;
        saida << "\tmov DS:[" << exps_end << "], ax" << endl;
    }

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
        T(t2_tipo, t2_tamanho, t2_end);
        verificaOps(t1_tipo, t2_tipo, t1_tamanho, t2_tamanho, exps_op);

        saida << "\tmov ax, DS:[" << exps_end << "]" << endl;
        saida << "\tmov bx, DS:[" << t2_end << "]" << endl;
        if(exps_op == TOKEN_MAIS) saida << "\tadd ax, bx" << endl;
        else if(exps_op == TOKEN_MENOS) saida << "\tsub ax, bx" << endl;
        else {
            //OR => a + b - (a * b)
            saida << "\tmov cx,ax" << endl; // salva valor de AX em CX
            saida << "\timul bx" << endl; // ax = a * b
            saida << "\tadd bx, cx" << endl; // bx = a + b
            saida << "\tsub bx, ax" << endl; // a + b - (a*b)
            saida << "\tmov ax, bx" << endl; // move pro Ax o resultado
        }
        
        exps_end = novoTemp(exps_tipo);
        saida << "\tmov DS:[" << exps_end << "], ax" << endl;
    }
}


// Exp -> ExpS1 (1) [ ( = | > | < | <> | <= | >= ) (2) ExpS2 (3) ]
// (1) { exp_tipo = exps_tipo; exp.tamanho = exps.tamanho;  }
// (2) { exp_tipo = boolean; exp.tamanho = 0 }
// (3) { se nao(VerificaOps) entao ERRO }
void Exp(int &exp_tipo, int &exp_tamanho, int &exp_end) {
    // declarando variaveis que serão necessarias para o semantico
    int exps1_tipo, exps1_tamanho, exps2_tipo, exps2_tamanho, exps1_end, exps2_end;
    int exp_op;

    ExpS(exps1_tipo, exps1_tamanho, exps1_end);

    exp_tipo = exps1_tipo;
    exp_tamanho = exps1_tamanho;
    exp_end = exps1_end;

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

        ExpS(exps2_tipo, exps2_tamanho, exps2_end);
        verificaOps(exps1_tipo, exps2_tipo, exps1_tamanho, exps2_tamanho, exp_op);

        if((exps1_tipo == TIPO_STRING || (exps1_tipo == TIPO_CHAR && exps1_tamanho > 0)) &&
           (exps2_tipo == TIPO_STRING || (exps2_tipo == TIPO_CHAR && exps1_tamanho > 0))) {
            
            string comecoCmp, iguais, fimCmp;
            comecoCmp = novoRot();
            iguais = novoRot();
            fimCmp = novoRot();
            saida << "\tmov ax, 1" << endl; //ax recebe true
            saida << "\tmov cx, 0" << endl; //contador de posicao em 0
            saida << "\tmov bl, DS:[" << exps1_end << "]" << endl; //carrega em bx o primeiro char da string 1
            saida << "\tmov dl, DS:[" << exps2_end << "]" << endl; //carrega em dx o primeiro char da string 2
            saida << comecoCmp << ":" << endl; //loop de comparacao
            saida << "\tcmp bl, dl" << endl; //compara dx e bx
            saida << "\tje " << iguais << endl;
            saida << "\tmov ax, 0" << endl; //se forem diferentes, ax recebe false e acaba o loop
            saida << "\tjmp " << fimCmp << endl;
            saida << iguais << ":" << endl; // se forem iguais carrega os proximos chars
            saida << "\tcmp bl, \'$\'" << endl;
            saida << "\tje " << fimCmp << endl;
            saida << "\tcmp dl, \'$\'" << endl;
            saida << "\tje " << fimCmp << endl;
            saida << "\tadd cx, 1" << endl; //incrementa a posicao
            saida << "\tmov bx, "  << exps2_end  << endl; //move o end pra bx
            saida << "\tadd bx, cx" << endl; // soma posicao
            saida << "\tmov dl, DS:[bx]" << endl; // atualiza o char de dx
            saida << "\tmov bx, "  << exps1_end  << endl; //mov end pra bx
            saida << "\tadd bx, cx" << endl; //soma a posicao
            saida << "\tmov bl, DS:[bx]" << endl; //atualiza o char de bx
            saida << "\tjmp " << comecoCmp << endl;
            saida << fimCmp << ":" << endl;
        } else {
            saida << "\tmov ax, DS:[" <<  exps1_end<< "]" << endl;
            saida << "\tmov bx, DS:[" <<  exps2_end<< "]" << endl;
            saida << "\tcmp ax, bx" << endl;
            string RotVerdadeiro = novoRot();

            if (exp_op == TOKEN_IGUAL) saida << "\tje " << RotVerdadeiro << endl;
            else if (exp_op == TOKEN_DIF) saida << "\tjne " << RotVerdadeiro << endl;
            else if(exp_op == TOKEN_MENOR) saida << "\tjl " << RotVerdadeiro << endl;
            else if(exp_op == TOKEN_MAIOR) saida << "\tjg " << RotVerdadeiro << endl;
            else if(exp_op == TOKEN_MAIOR_IGUAL) saida << "\tjge " << RotVerdadeiro << endl;
            else saida << "\tjle " << RotVerdadeiro << endl;

            saida << "\tmov ax, 0" << endl;
            string RotFim = novoRot();
            saida << "\tjmp " << RotFim << endl;
            saida << RotVerdadeiro << ":" << endl;
            saida << "\tmov ax, 1" << endl;
            saida << RotFim << ":" << endl;
        }
        
        exp_tipo = TIPO_BOOLEAN;
        exp_end = novoTemp(exp_tipo);
        saida << "\tmov DS:[" << exp_end<< "], ax" << endl;  
    }
}


// Dec -> ( int (1)| boolean (2)| char (3)) ID (4) [:= [-]CONST (5) | "[" CONST (6) "]" ] (8) {, ID (4) [:= [-]CONST (5) | "[" CONST (6) "]" ] (8)} ; | final (7) ID (10) = [-]CONST (9) ; (8)
// (1) { Dec.tipo = inteiro; dec.classe = classe_var }
// (2) { Dec.tipo = boolean; dec.classe = classe_var }
// (3) { Dec.tipo = char; dec.classe = classe_var }
// (4) { se (id.tipo == vazio) entao {id.tipo = dec.tipo; id.classe = dec.classe} senao erro }
// (5) { se (nao compativel(dec.tipo, const.tipo)) entao erro }
// (6) { se (const.val <= 8kbytes) entao (id.tamanho = const.val) senao erro}
// (7) { Dec.classe = classe_const }
// (8) { atualizarTabelaSimbolos();}
// (9) { dec_tipo = const_tipo }
// (10) { se (id.tipo == vazio) entao {id.classe = dec.classe} senao erro }
void Dec() {
    // iniciando variaveis regra DEC
    int dec_tipo, dec_classe;
    int id_tipo, id_classe, id_pos, id_tamanho;
    int const_tipo, const_tam; 
    bool ehNeg = false; //flag que é ativada caso o valor seja negativo
    string id_lex;
    string const_val = "VAZIO";

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
            ehNeg=true;
            casaToken(TOKEN_MENOS);
            if(reg.tipo != TIPO_INT) throw ERR_TIPO;
        }
        
		// retorna os dados da constante
        const_val = reg.lexema;
        const_tipo = reg.tipo;
        const_tam = reg.tamanho;
        id_tipo = const_tipo == 0?TIPO_BOOLEAN:const_tipo;
        casaToken(TOKEN_CONST);
        casaToken(TOKEN_PONTO_VIRG);

        t.atualizar(id_lex, id_tipo, id_tamanho, id_classe, contadorDados);
        printDec(id_tipo, id_classe, const_val, const_tam,ehNeg);

        
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
                ehNeg = true;
                casaToken(TOKEN_MENOS);
                if(reg.tipo != TIPO_INT) throw ERR_TIPO;
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
            id_tamanho = stoi(const_val);
            casaToken(TOKEN_CONST);
            casaToken(TOKEN_FECHA_COLCH);
        }

        t.atualizar(id_lex, id_tipo, id_tamanho, id_classe, contadorDados);
        printDec(id_tipo, id_classe, const_val, id_tamanho, ehNeg);

        while (reg.token == TOKEN_VIRG) {
            const_val = "VAZIO"; // resetando o buffer;
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
                    ehNeg = true;
                    casaToken(TOKEN_MENOS);
                    if(reg.tipo != TIPO_INT) throw ERR_TIPO;
                }
                
				// retorna os dados da constante
                const_val = reg.lexema;
                const_tipo = reg.tipo;
                const_tam = reg.tamanho;
                
				// verifica compatibilidade
                if(!verificaCompatibDec(id_tipo, const_tipo)){
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
                if(!verificaTamanho(stoi(const_val), id_tipo)){
                    throw ERR_TAMANHO;
                }
                id_tamanho = stoi(const_val);
                casaToken(TOKEN_CONST);
                casaToken(TOKEN_FECHA_COLCH);
            }
            t.atualizar(id_lex, id_tipo, id_tamanho, id_classe, contadorDados);
            printDec(id_tipo, id_classe, const_val, id_tamanho, ehNeg);
        }
        casaToken(TOKEN_PONTO_VIRG);
    }    
}

// pré declarando comando Cmd(); e CmdP();
void Cmd();
void CmdP();
void CmdFor();


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


// CmdAtr -> ID (1)(2) ["[" Exp (3) "]"] := Exp1 (4)
// (1) { if id.tipo == vazio then ERRO else atr.tipo = id_tipo; }
// (2) { if id.classe == classe_const then ERRO }
// (3) { se nao(exp.tipo == tipo_int) entao erro }
// (4) { se (nao compativel(atr.tipo, exp1.tipo)) entao ERRO }
void CmdAtr() {
    saida << ";cmdAtr()"<< endl;
    int atr_tipo, exp1_tipo, exp1_tamanho, exp2_tipo, exp2_tamanho, id_tipo, 
        id_tamanho, id_pos, id_classe, exp1_end, exp2_end, id_end; // variaveis necessarias para semantico
    string id_lex;
    
    //verificando existencia do identificador
    id_pos = reg.posicao;
    id_lex = reg.lexema;
    id_tipo = t.getTipo(id_lex, id_pos);
    id_classe = t.getClasse(id_lex, id_pos);
    id_tamanho = t.getTamanho(id_lex, id_pos);
    id_end = t.getEnd(id_lex, id_pos);
    
    
    if(id_tipo == TIPO_VAZIO){ // se tipo vazio, id não declarado
        throw ERR_NOT_EXISTS;
    } else { // senão, atribui tipo do id
        atr_tipo = id_tipo;
    }

    if(id_classe == CLASSE_CONST) {
        throw ERR_CLASSE;
    }
    
    saida << "\tmov si, " << id_end << endl;
    casaToken(TOKEN_ID);
    if (reg.token == TOKEN_ABRE_COLCH) {
        casaToken(TOKEN_ABRE_COLCH);
        Exp(exp1_tipo, exp1_tamanho, exp1_end);
        contadorTemp = POS_TEMPORARIOS;

        if(exp1_tipo != TIPO_INT){
            throw ERR_TIPO;
        }
        id_tamanho = 0; //tamanho do id assume 0 pois agora ele sera um escalar

        // carrega posicao do id
        saida << "\tadd si, DS:[" << exp1_end << "]" << endl;
        if (atr_tipo == TIPO_INT) saida << "\tadd si, DS:[" << exp1_end << "]" << endl;

        casaToken(TOKEN_FECHA_COLCH);
    }
    casaToken(TOKEN_ATRIB);
    Exp(exp2_tipo, exp2_tamanho, exp2_end);
    contadorTemp = POS_TEMPORARIOS;
    

    if(!(verificaCompatibAtr(atr_tipo, exp2_tipo, id_tamanho, exp2_tamanho))) throw ERR_TIPO;
    
    if(id_tipo == TIPO_CHAR && id_tamanho > 0) { // vetor de char
        if(exp2_tipo == TIPO_STRING || (exp2_tipo == TIPO_CHAR && exp2_tamanho > 0)){ // se exp2 for string ou vetor de char
            string rotInicio = novoRot();
            string rotFim = novoRot();
            saida << "\tmov di, " << id_end << endl; //
            saida << "\tmov si, " << exp2_end << endl;
            saida << "\tmov cx, " << exp2_tamanho - 1 << endl;
            saida << rotInicio << ":" << endl;
            saida << "\tcmp cx, 0" << endl;
            saida << "\tmov al, DS:[si]" << endl;
            saida << "\tmov DS:[di], al" << endl;
            saida << "\tje " << rotFim << endl;
            saida << "\tadd si, 1" << endl;
            saida << "\tadd di, 1" << endl;
            saida << "\tadd cx, -1" << endl;
            saida << "\tjmp " << rotInicio << endl;
            saida << rotFim << ":" << endl;
        }
    } else {
        saida << "\tmov ax, DS:[" << exp2_end << "]" << endl;
        saida << "\tmov DS:[si], ax" << endl;
    }
}


//CmdRep -> for"(" [CmdFor {, CmdFor}]; Exp (1); [CmdFor {, CmdFor}] ")" (Cmd | BlocoCmd)
//(1) -> { se exp.tipo != BOOLEAN entao ERRO}
void CmdRep() {
    saida << ";cmdRep()" << endl;

    int exp_tipo, exp_tamanho, exp_end;
    string rotInicio, rotFim, rotComandes, rotComandos;

    casaToken(TOKEN_FOR);
    casaToken(TOKEN_ABRE_PAREN);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
        reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN || 
        reg.token == TOKEN_IF || reg.token == TOKEN_FOR) {
        CmdFor();
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
                reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN || 
                reg.token == TOKEN_IF || reg.token == TOKEN_FOR) {
                CmdFor();
            }
        }
    }

    casaToken(TOKEN_PONTO_VIRG);
    rotInicio = novoRot();
    saida << rotInicio << ":" << endl;
    Exp(exp_tipo, exp_tamanho, exp_end);
    
    contadorTemp = POS_TEMPORARIOS;

    if(exp_tipo != TIPO_BOOLEAN || exp_tamanho > 0) throw ERR_TIPO;
    
    
    rotFim = novoRot();
    rotComandes = novoRot();
    rotComandos = novoRot();

    
    saida << "\tmov dx, ds:[" << exp_end << "]" << endl; //carregar conteúdo de Exp.end em dx
    saida << "\tcmp dx, 0" << endl;
    saida << "\tje " << rotFim << endl;
    saida << "\tjmp " << rotComandos << endl;
    saida << rotComandes << ":" << endl;

    casaToken(TOKEN_PONTO_VIRG);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
        reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN || 
        reg.token == TOKEN_IF || reg.token == TOKEN_FOR) {
        CmdFor();
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            if (reg.token == TOKEN_ID || reg.token == TOKEN_READLN ||
                reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN || 
                reg.token == TOKEN_IF || reg.token == TOKEN_FOR) {
                CmdFor();
            }
        }
    }

    saida << "\tjmp " << rotInicio << endl;
    saida << rotComandos << ":" << endl;
    
    casaToken(TOKEN_FECHA_PAREN);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
        reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
        reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
        reg.token == TOKEN_WRITELN) {
        Cmd();
    } else {
        BlocoCmd();
    }

    saida << "\tjmp " << rotComandes << endl;
    saida << rotFim << ":" << endl;
}


// CmdIf -> if"(" Exp (1) ")" then (Cmd | BlocoCmd) [else (Cmd | BlocoCmd)]
// (1) -> { se exp.tipo != BOOLEAN || exp.tamanho > 0 entao ERRO }
void CmdIf() {
    saida << ";cmdIf()" << endl;
    int exp_tipo, exp_tamanho, exp_end;
    string RotFalso, RotFim;
    RotFalso = novoRot();
    RotFim = novoRot();

    casaToken(TOKEN_IF);
    casaToken(TOKEN_ABRE_PAREN);
    Exp(exp_tipo, exp_tamanho, exp_end);
    contadorTemp = POS_TEMPORARIOS;

    saida << "\tmov DX, ds:["<<exp_end<<"]" << endl;
    saida << "\tcmp DX, 0" << endl;
    saida << "\tje " << RotFalso << endl;

    if(exp_tipo != TIPO_BOOLEAN || exp_tamanho > 0) throw ERR_TIPO;

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

    saida << "jmp " << RotFim << endl;
    saida << RotFalso << ":" << endl;
    
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
    saida << RotFim << ":" << endl;
}


// CmdNull -> ;
void CmdNull() { casaToken(TOKEN_PONTO_VIRG); }


// CmdRead -> readln "(" ID (2) ["[" Exp (3) "]"] (1) ")"
// (1) -> { se id.tipo != int || char || string e for vetor entao ERRO }
// (2) -> { se id.tipo == VAZIO entao ERRO}
// (3) -> { se exp.tipo != int && exp.tamanho > 0 entao ERRO }
void CmdRead() {
    saida <<";cmdRead()" << endl;
    int exp_tipo, exp_tamanho, id_tipo, id_tamanho, id_end, id_classe, exp_end;
    casaToken(TOKEN_READLN);
    casaToken(TOKEN_ABRE_PAREN);
    id_tipo = t.getTipo(reg.lexema, reg.posicao);
    id_tamanho = t.getTamanho(reg.lexema, reg.posicao);
    id_classe = t.getClasse(reg.lexema, reg.posicao);
    id_end = t.getEnd(reg.lexema, reg.posicao);

    if(id_tipo == TIPO_VAZIO) throw ERR_NOT_EXISTS;
    else if(id_classe == CLASSE_CONST) throw ERR_CLASSE;

    casaToken(TOKEN_ID);
    saida << "\tmov si, " << id_end << endl;

    if (reg.token == TOKEN_ABRE_COLCH) {
        casaToken(TOKEN_ABRE_COLCH);
        Exp(exp_tipo, exp_tamanho, exp_end);
        contadorTemp = POS_TEMPORARIOS;

        if(exp_tipo != TIPO_INT || exp_tamanho > 0) throw ERR_TIPO;
        id_tamanho = 0; //tamanho do id assume 0 pois agora ele sera um escalar

        
        saida << "\tadd si, DS:[" << exp_end << "]" << endl;
        if (id_tipo == TIPO_INT) saida << "\tadd si, DS:[" << exp_end << "]" << endl;
        casaToken(TOKEN_FECHA_COLCH);
    }


    if(!verificaCompatibRead(id_tipo,id_tamanho)){
        throw ERR_TIPO;
    }
    
    int buffer_tam = 0; // inicia o tamanho do buffer
    if(id_tipo == TIPO_INT && id_tamanho == 0) buffer_tam = 255; // se o tamanho do id for 1, é escalar, portanto o buffer tem tamanho 255
    else if(id_tipo == TIPO_CHAR && id_tamanho == 0) buffer_tam = 1+3;
    else buffer_tam = id_tamanho+3; // se nao o tamanho do buffer é o tamanho do id
    // buffer_tam = (buffer_tam < 255) ? buffer_tam : 255; // seleciona entre buffer_tam e 2 o que for menor para o tamanho do buffer
    
    // define o tamanho do buffer e cria o endereço
    int buffer_end = novoBuffer(id_tipo, buffer_tam);
    
    saida << "\tmov dx, " << buffer_end << endl;
    saida << "\tmov al, " << buffer_tam-3 << endl;
    saida << "\tmov ds:[" << buffer_end << "], al" << endl;
    saida << "\tmov ah, 0Ah" << endl;
    saida << "\tint 21h" << endl;

    if(id_tipo == TIPO_INT){
        string R0, R1, R2;
        R0 = novoRot();
        R1 = novoRot();
        R2 = novoRot();

        saida << "\tmov di, " << (buffer_end+2) << endl; //posição do string
        saida << "\tmov ax, 0" << endl; //acumulador
        saida << "\tmov cx, 10" << endl; //base decimal
        saida << "\tmov dx, 1" << endl; //valor sinal +
        saida << "\tmov bh, 0" << endl;
        saida << "\tmov bl, ds:[di]" << endl; //caractere
        saida << "\tcmp bx, 2Dh" << endl; //verifica sinal
        saida << "\tjne " << R0 << endl; //se não negativo
        saida << "\tmov dx, -1" << endl; //valor sinal -
        saida << "\tadd di, 1" << endl; //incrementa base
        saida << "\tmov bl, ds:[di]" << endl; //próximo caractere
        saida << R0 << ":" << endl;
        saida << "\tpush dx" << endl; 
        saida << "\tmov dx, 0" << endl; 
        saida << R1 << ":" << endl;
        saida << "\tcmp bx, 0dh" << endl; 
        saida << "\tje " << R2 << " ;salta se fim string" << endl; 
        saida << "\timul cx ;mult. 10" << endl; 
        saida << "\tadd bx, -48  ;converte caractere" << endl;
        saida << "\tadd ax, bx ;soma valor caractere" << endl;
        saida << "\tadd di, 1 ;incrementa base" << endl;
        saida << "\tmov bh, 0" << endl;
        saida << "\tmov bl, ds:[di] ;próximo caractere" << endl;
        saida << "\tjmp " << R1 << " ;loop" << endl;
        saida << R2 << ":" << endl;
        saida << "\tpop cx ;desempilha sinal" << endl;
        saida << "\timul cx ;mult. sinal" << endl;
        saida << "\tmov ds:[si], ax ;move pro id o valor acumulado" << endl;
    } else {
        //pega string
        string inicio, fim;
        inicio = novoRot();
        fim = novoRot();

        saida << "\tmov di, " << (buffer_end+2) << endl; //posição do string
        saida << inicio << ":" << endl;
        saida << "\tmov bl, ds:[di]" << endl; //caractere
        saida << "\tcmp bx, 0dh" << endl; 
        saida << "\tje " << fim << " ;salta se fim string" << endl;
        saida << "\tmov ds:[si], bl" << endl;
        saida << "\tadd di, 1 ;incrementa base" << endl;
        saida << "\tadd si, 1 ;incrementa base" << endl;
        saida << "\tjmp " << inicio << endl;
        saida << fim << ":" << endl;
        saida << "\tmov ax, 024h" << endl;
        saida << "\tmov DS:[si], ax" << endl;
    }
    
    saida << "\tmov ah, 02h" << endl;
    saida << "\tmov dl, 0Dh" << endl;
    saida << "\tint 21h" << endl;
    saida << "\tmov DL, 0Ah" << endl;
    saida << "\tint 21h" << endl;

    casaToken(TOKEN_FECHA_PAREN);
}


void printWrite(int tipo, int tamanho, int end) {
    string R0, R1, R2;

    if (tipo != TIPO_INT && tipo != TIPO_CHAR && tipo != TIPO_STRING) throw  ERR_TIPO;
    else {
        if(tipo == TIPO_INT) {
            R0 = novoRot();
            R1 = novoRot();
            R2 = novoRot();
            int string_temp = novoTemp(TIPO_CHAR);
            saida << "\tmov ax, DS:[" << end << "]" << endl;
            saida << "\tmov di, " << string_temp << endl; //end. string temp.
            saida << "\tmov cx, 0" << endl; //contador
            saida << "\tcmp ax,0" << endl; //verifica sinal
            saida << "\tjge " << R0 << endl; //salta se número positivo
            saida << "\tmov bl, 2Dh" << endl; //senão, escreve sinal –
            saida << "\tmov ds:[di], bl" << endl;
            saida << "\tadd di, 1" << endl; //incrementa índice
            saida << "\tneg ax" << endl; //toma módulo do número
            saida << R0 << ":" << endl;
            saida << "\tmov bx, 10" << endl; //divisor
            saida << R1 << ":" << endl;
            saida << "\tadd cx, 1" << endl; //incrementa contador
            saida << "\tmov dx, 0" << endl; //estende 32bits p/ div.
            saida << "\tidiv bx" << endl; //divide DXAX por BX
            saida << "\tpush dx" << endl; //empilha valor do resto
            saida << "\tcmp ax, 0" << endl; //verifica se quoc. é 0
            saida << "\tjne " << R1 << endl; //se não é 0, continua

            //agora, desemp. os valores e escreve o string
            saida << R2 << ":" << endl; 
            saida << "\tpop dx" << endl;  //desempilha valor
            saida << "\tadd dx, 30h" << endl;  //transforma em caractere
            saida << "\tmov ds:[di],dl" << endl;  //escreve caractere
            saida << "\tadd di, 1" << endl;  //incrementa base
            saida << "\tadd cx, -1" << endl;  //decrementa contador
            saida << "\tcmp cx, 0" << endl;  //verifica pilha vazia
            saida << "\tjne " << R2 << endl;  //se não pilha vazia, loop

            //grava fim de string
            saida << "\tmov dl, 024h" << endl;  //fim de string
            saida << "\tmov ds:[di], dl" << endl;  //grava '$'

            //exibe string
            saida << "\tmov dx, " << string_temp << endl; 
            
            
        } else {
            //imprimir string normalmente
            saida << "\tmov dx, " << end << endl; 
        }

        saida << "\tmov ah, 09h" << endl; 
        saida << "\tint 21h" << endl; 
        contadorTemp = POS_TEMPORARIOS;
    }
}

// CmdWrite -> (write|writeln)"(" Exp (1) {, Exp (1) } ")"
// (1) { if exp.tipo != int || char || string }
void CmdWrite() {
    saida << ";cmdWrite()" << endl;
    int exp_tipo, exp1_tipo, exp_tamanho, exp1_tamanho, exp_end, exp1_end;
    bool wln = false;

    if (reg.token == TOKEN_WRITE) {
        casaToken(TOKEN_WRITE);
    } else {
        casaToken(TOKEN_WRITELN);
        wln = true;
    }

    casaToken(TOKEN_ABRE_PAREN);
    Exp(exp_tipo, exp_tamanho, exp_end);
    contadorTemp = POS_TEMPORARIOS;
    if(!verificaCompatibWrite(exp_tipo, exp_tamanho)){
        throw ERR_TIPO;
    }
    printWrite(exp_tipo, exp_tamanho, exp_end);

    while (reg.token == TOKEN_VIRG) {
        casaToken(TOKEN_VIRG);
        Exp(exp1_tipo, exp1_tamanho, exp1_end);
        contadorTemp = POS_TEMPORARIOS;
        if(!verificaCompatibWrite(exp1_tipo, exp1_tamanho)){
            throw ERR_TIPO;
        }   
        printWrite(exp1_tipo, exp1_tamanho, exp1_end);
    }
    casaToken(TOKEN_FECHA_PAREN);

    if(wln) { //imprime quebra de linha
        saida << "\tmov ah, 02h" << endl;
        saida << "\tmov dl, 0Dh" << endl;
        saida << "\tint 21h" << endl;
        saida << "\tmov DL, 0Ah" << endl;
        saida << "\tint 21h" << endl;
    }
}


//CmdFor -> CmdAtr | CmdWrite | CmdRead | CmdFor | CmdIf
void CmdFor() {
    if(reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN){
        CmdWrite();
    } else if (reg.token == TOKEN_READLN) {
        CmdRead();
    } else if (reg.token == TOKEN_FOR) {
        CmdRep();
    } else if (reg.token == TOKEN_IF) {
        CmdIf();
    } else {
        CmdAtr();
    }
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
    } else /*if(reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN ||
              reg.token == TOKEN_READLN || reg.token == TOKEN_ID || reg.token ==
              TOKEN_PONTO_VIRG)*/
    {
        if(reg.token == TOKEN_WRITE || reg.token == TOKEN_WRITELN ||
           reg.token == TOKEN_READLN || reg.token == TOKEN_ID){
            CmdP();
        }
        casaToken(TOKEN_PONTO_VIRG);
    }
}


// Prog -> { Dec } main BlocoCmd EOF
void Prog() {
    //Escrever inicio do segmento de dados

    // saida << "sseg SEGMENT STACK\t;início seg. pilha" << endl;
    // saida << "byte " << dec << contadorDados <<" DUP(?)\t;dimensiona pilha" << endl;
    // saida << "sseg ENDS\t;fim seg. pilha" << endl;

    contadorDados += 0x4000;

    saida << "dseg SEGMENT PUBLIC\n";
    saida << "\tbyte " << dec << contadorDados << " DUP (?)\n";
    
    //Ciclo de declaracoes
    while (reg.token == TOKEN_INT || reg.token == TOKEN_CHAR ||
           reg.token == TOKEN_BOOL || reg.token == TOKEN_FINAL) {
        Dec();
    }
    //Escrever fim do segmento de dados
    saida << "dseg ENDS" << endl;

    // inicio segmento codigo
    saida << "cseg SEGMENT PUBLIC" << endl;
    saida << "\tASSUME CS:cseg, DS:dseg" << endl << endl;
    saida << "strt:" << endl;
    saida << "\tmov ax, dseg" << endl << "\tmov ds, ax" << endl;



    casaToken(TOKEN_MAIN);
    BlocoCmd();

    saida << ";fimProg" << endl;
    saida << "\tmov ah, 4Ch" << endl;
    saida << "\tint 21h" << endl;
    saida << "cseg ENDS" << endl;
    saida << "END strt" << endl;
}


/*
----------------------------------------------------
Método principal
Inicializa a tabela de tokens com os tokens e palavras reservadas
Executa os analisadores a partir do primeiro token recebido
----------------------------------------------------
*/
int main() {
    //inicializando memoria
    contadorDados = 0;

	//inicializando contador de rotulos e de temporarios
	contadorRot = 0;
    contadorTemp = POS_TEMPORARIOS;

    //inicializando arquivo de saida
    saida.open("saida.asm");

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
    t.inserir("EOF", TOKEN_EOF);
    t.inserir("boolean", TOKEN_BOOL);

    try {
        analisadorLexico();
        Prog();
        casaToken(TOKEN_EOF);
        saida.close();
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
                cout << linha << endl << "classe de identificador incompativel [" << reg.lexema << "].";
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
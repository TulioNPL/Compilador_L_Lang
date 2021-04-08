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
#define TOKEN_BOOL 40

// definição de errors
#define ERR_CHAR -1
#define ERR_EOF -2
#define ERR_TOKEN -3

struct Simbolo {
    string lexema;
    int token;
};

struct RegLex {
    string lexema;
    int token;
    int posicao;
    string tipo;
    size_t tamanho;
};  // registro léxico

string alfabeto = "_.,;:(){}[]+-/%@!?><=*";

// iniciando o registro lexico variável global
RegLex reg;

class TabelaSimbolos {
    int num_posicoes;

    list<Simbolo> *tabela;

   public:
    TabelaSimbolos(int n);  // Construtor
    int inserir(string lex, int token);
    int pesquisar(string lex);
    int hash(string lex);
    int getToken(string lex, int pos);
    void mostrar();
};

TabelaSimbolos::TabelaSimbolos(int n) {
    this->num_posicoes = n;
    tabela = new list<Simbolo>[num_posicoes];
}

int TabelaSimbolos::getToken(string lex, int pos) {
    list<Simbolo>::iterator i;
    for (i = tabela[pos].begin(); i != tabela[pos].end(); i++) {
        if (!lex.compare(i->lexema)) {
            return i->token;
        }
    }
    return -1;
}

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
void atualizarTabela(string lex, string tipo, int token, size_t tamanho) {
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
            // cout << "já encontrou!" << endl;
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

int linha = 1;

void analisadorLexico() {
    // limpa o registro léxico
    reg.lexema = "";
    reg.posicao = -1;
    reg.tipo = "";
    reg.token = 0;
    reg.tamanho = 0;

    char c;
    int S = 0;

    /*
        feito:  cases
                inserção de token ID

        falta:  elses (erros lexicos e EOF)
                demais tokens


    */

    // criando variaveis para criação do reglex e do token
    string lex = "";
    string tipo = "";
    int tok = 0;
    int pos = -1;
    size_t tamanho = 0;

    while (S != 1) {
        // cout << "caractere atual: " << (int)c << endl;

        if (c != EOF) {
            c = cin.get();
            if (!(c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
                  c == '\'' || c == EOF || c == '\"' ||
                  (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                  (c >= '0' && c <= '9') ||
                  alfabeto.find(c) != std::string::npos)) {
                throw ERR_CHAR;
                break;
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
                    tipo = "char";
                    lex += c;
                    S = 4;
                } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    tok = TOKEN_ID;
                    S = 3;
                    lex += c;
                } else if (c >= '1' && c <= '9') {
                    tok = TOKEN_CONST;
                    tipo = "int";
                    S = 11;
                    lex += c;
                } else if (c == '0') {
                    tok = TOKEN_CONST;
                    lex += c;
                    S = 6;
                } else if (c == '=' || c == '+' || c == '*' || c == '[' ||
                           c == '-' || c == ']' || c == '(' || c == ')' ||
                           c == '{' || c == '}' || c == '.' || c == ';' ||
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
                    tipo = "string";
                    S = 15;
                } else if (c == '/') {
                    S = 17;
                    lex += c;
                } else if (c == EOF) {
                    S = 0;
                } else {
                    lex += c;
                    // cout << S << endl;
                    // cout << (int)c << " " << c << endl;
                    throw lex;
                    break;
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
                } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                           (c >= '0' && c <= '9')) {
                    S = 3;
                    lex += c;
                } else if (c == EOF) {
                    throw ERR_EOF;

                    S = 0;
                } else {
                    lex += c;
                    cout << S << endl;

                    throw lex;
                }
                break;
            case 3:
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
            case 4:
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                    (c >= '0' && c <= '9')) {
                    lex += c;
                    S = 5;
                } else if (c == EOF) {
                    throw ERR_EOF;

                    S = 0;
                } else {
                    lex += c;
                    // cout << S << endl;

                    throw lex;
                }
                break;
            case 5:
                if (c == '\'') {
                    lex += c;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c == EOF) {
                    throw ERR_EOF;

                    S = 0;
                } else {
                    lex += c;
                    // cout << S << endl;

                    throw lex;
                }
                break;
            case 6:
                if (c >= '0' && c <= '9') {
                    lex += c;
                    S = 9;
                } else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    tipo = "char";
                    lex += c;
                    S = 7;
                } else {
                    tipo = "int";
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;
            case 7:
                if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') ||
                    (c >= '0' && c <= '9')) {
                    lex += c;
                    S = 8;
                } else if (c == EOF) {
                    throw ERR_EOF;

                    S = 0;
                } else {
                    lex += c;
                    // cout << S << endl;

                    throw lex;
                }
                break;
            case 8:
                if (c == 'h') {
                    lex += c;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c == EOF) {
                    S = 0;
                } else {
                    lex += c;
                    // cout << S << endl;

                    throw lex;
                }
                break;
            case 9:
                if (c >= '0' && c <= '9') {
                    lex += c;
                    S = 10;
                } else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    tipo = "char";
                    lex += c;
                    S = 8;
                } else {
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;
            case 10:
                if (c == 'h') {
                    tipo = "char";
                    lex += c;
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c >= '0' && c <= '9') {
                    tipo = "int";
                    lex += c;
                    S = 11;
                } else {
                    tamanho = lex.size();
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                    cin.unget();
                }
                break;
            case 11:
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
            case 12:
                if (c == '=') {
                    lex += c;
                    atualizarTabela(lex, tipo, tok, tamanho);
                    S = 1;
                } else if (c == EOF) {
                    throw ERR_EOF;

                    S = 0;
                } else {
                    lex += c;
                    // cout << S << endl;

                    throw lex;
                }
                break;
            case 13:
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
            case 14:
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
            case 15:
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
                    lex += c;
                    // cout << S << endl;

                    throw lex;
                }
                break;
            // case 16:
            //     if (c != '\"' && c != '$' && c != '\n' && c != EOF) {
            //         lex += c;
            //         S = 16;
            //     } else if (c == '\"') {
            //         atualizarTabela(lex, tipo, tok);
            //         S = 1;
            //     } else if (c == EOF) {
            //         cout << linha << endl << "fim de arquivo nao esperado.";
            //
            //         S = 0;
            //     }
            //     break;
            case 17:
                if (c == '*') {
                    lex += c;
                    S = 18;
                } else {
                    S = 1;
                    cin.unget();
                }
                break;
            case 18:
                if (c != '*') {
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
                } else if (c != '*' && c != '/') {
                    lex += c;
                    S = 18;
                }
                break;
        }
        // cout << c << " → " << S << endl;
    }
    // cout << lex << endl;
    // cout << "fim do lexema" << endl;
}

// void casaToken(int token_esperado) {
//     if (reg.token == token_esperado) {
//         cout << "Casa Token casou " << reg.lexema << " com TOKEN "
//              << token_esperado << endl;
//         analisadorLexico();
//     } else {
//         cout << linha << endl << "token não esperado [" << reg.lexema <<
//         "]."; ERR = true;
//     }
// }

void casaToken(int token_esperado) {
    if (reg.token == token_esperado) {
        // cout << "token_esperado: " << token_esperado << " token encontrado: "
        // << reg.token << "(" << reg.lexema << ")" << endl; cout << "Casa Token
        // casou " << reg.lexema << " com TOKEN "
        //      << token_esperado << endl;
        analisadorLexico();
    } else {
        // cout << "token_esperado: " << token_esperado << endl;
        throw ERR_TOKEN;
    }
}

/* pré declarando a função Exp() para o compilador aceitar
 as demais funções declaradas posteriormente */
void Exp();

// F -> not F | "(" Exp ")" | CONST | ID [ "[" Exp "]" ]
void F() {
    if (reg.token == TOKEN_NOT) {
        casaToken(TOKEN_NOT);
        F();
    } else if (reg.token == TOKEN_ABRE_PAREN) {
        casaToken(TOKEN_ABRE_PAREN);
        Exp();
        casaToken(TOKEN_FECHA_PAREN);
    } else if (reg.token == TOKEN_CONST) {
        casaToken(TOKEN_CONST);
    } else if (reg.token == TOKEN_ID) {
        casaToken(TOKEN_ID);
        if (reg.token == TOKEN_ABRE_COLCH) {
            casaToken(TOKEN_ABRE_COLCH);
            Exp();
            casaToken(TOKEN_FECHA_COLCH);
        }
    }
}

// T ->  F { ( * | / | % | and ) F }
void T() {
    if (reg.token == TOKEN_NOT || reg.token == TOKEN_ABRE_PAREN ||
        reg.token == TOKEN_CONST || reg.token == TOKEN_ID) {
        F();
        while (reg.token == TOKEN_ASTER || reg.token == TOKEN_BARRA ||
               reg.token == TOKEN_MOD || reg.token == TOKEN_AND) {
            if (reg.token == TOKEN_ASTER) {
                casaToken(TOKEN_ASTER);
            } else if (reg.token == TOKEN_BARRA) {
                casaToken(TOKEN_BARRA);
            } else if (reg.token == TOKEN_MOD) {
                casaToken(TOKEN_MOD);
            } else {
                casaToken(TOKEN_AND);
            }
            F();
        }
    }
}

// ExpS -> [ + | - ] T { ( + | - | or ) T }
void ExpS() {
    if (reg.token == TOKEN_MAIS) {
        casaToken(TOKEN_MAIS);
    } else if (reg.token == TOKEN_MENOS) {
        casaToken(TOKEN_MENOS);
    }

    T();
    while (reg.token == TOKEN_MAIS || reg.token == TOKEN_MENOS ||
           reg.token == TOKEN_OR) {
        if (reg.token == TOKEN_MAIS) {
            casaToken(TOKEN_MAIS);
        } else if (reg.token == TOKEN_MENOS) {
            casaToken(TOKEN_MENOS);
        } else {
            casaToken(TOKEN_OR);
        }
        T();
    }
}

// Exp -> ExpS [ ( = | > | < | <> | <= | >= ) ExpS ]
void Exp() {
    if (reg.token == TOKEN_NOT || reg.token == TOKEN_ABRE_PAREN ||
        reg.token == TOKEN_CONST || reg.token == TOKEN_ID ||
        reg.token == TOKEN_MAIS || reg.token == TOKEN_MENOS) {
        ExpS();
        if (reg.token == TOKEN_IGUAL || reg.token == TOKEN_MAIOR ||
            reg.token == TOKEN_MENOR || reg.token == TOKEN_DIF ||
            reg.token == TOKEN_MENOR_IGUAL || reg.token == TOKEN_MAIOR_IGUAL) {
            if (reg.token == TOKEN_IGUAL) {
                casaToken(TOKEN_IGUAL);
            } else if (reg.token == TOKEN_MAIOR) {
                casaToken(TOKEN_MAIOR);
            } else if (reg.token == TOKEN_MENOR) {
                casaToken(TOKEN_MENOR);
            } else if (reg.token == TOKEN_DIF) {
                casaToken(TOKEN_DIF);
            } else if (reg.token == TOKEN_MENOR_IGUAL) {
                casaToken(TOKEN_MENOR_IGUAL);
            } else {
                casaToken(TOKEN_MAIOR_IGUAL);
            }
            ExpS();
        }
    }
}

// void Prog(){
//     if (reg.token == TOKEN_INT){
//         cout << "Token INT encontrado" << endl;
//         analisadorLexico();
//         cout << "Proximo token: " << reg.lexema << " → numero do token: " <<
//         reg.token << endl;
//     }
// }

// Dec -> ( int | boolean | char ) ID [:= [-]CONST | "[" CONST "]"] {, ID [:=
// [-]CONST | "[" CONST "]"] } ; | final ID = [-]CONST ;
void Dec() {
    if (reg.token == TOKEN_FINAL) {
        casaToken(TOKEN_FINAL);
        casaToken(TOKEN_ID);
        casaToken(TOKEN_IGUAL);
        if (reg.token == TOKEN_MENOS) {
            casaToken(TOKEN_MENOS);
        }
        casaToken(TOKEN_CONST);
        casaToken(TOKEN_PONTO_VIRG);
    } else {
        if (reg.token == TOKEN_INT) {
            casaToken(TOKEN_INT);
        } else if (reg.token == TOKEN_CHAR) {
            casaToken(TOKEN_CHAR);
        } else {
            casaToken(TOKEN_BOOL);
        }
        casaToken(TOKEN_ID);
        if (reg.token == TOKEN_ATRIB) {
            casaToken(TOKEN_ATRIB);
            if (reg.token == TOKEN_MENOS) {
                casaToken(TOKEN_MENOS);
            }
            casaToken(TOKEN_CONST);
        } else if (reg.token == TOKEN_ABRE_COLCH) {
            casaToken(TOKEN_ABRE_COLCH);
            casaToken(TOKEN_CONST);
            casaToken(TOKEN_FECHA_COLCH);
        }
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            casaToken(TOKEN_ID);
            if (reg.token == TOKEN_ATRIB) {
                casaToken(TOKEN_ATRIB);
                if (reg.token == TOKEN_MENOS) {
                    casaToken(TOKEN_MENOS);
                }
                casaToken(TOKEN_CONST);
            } else if (reg.token == TOKEN_ABRE_COLCH) {
                casaToken(TOKEN_ABRE_COLCH);
                casaToken(TOKEN_CONST);
                casaToken(TOKEN_FECHA_COLCH);
            }
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
void CmdAtr() {
    casaToken(TOKEN_ID);
    if (reg.token == TOKEN_ABRE_COLCH) {
        casaToken(TOKEN_ABRE_COLCH);
        Exp();
        casaToken(TOKEN_FECHA_COLCH);
    }
    casaToken(TOKEN_ATRIB);
    Exp();
}

// CmdRep -> for"(" [CmdP {, CmdP}]; Exp; [CmdP {, CmdP}] ")" (Cmd | BlocoCmd)
void CmdRep() {
    casaToken(TOKEN_FOR);
    casaToken(TOKEN_ABRE_PAREN);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
        reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
        reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
        reg.token == TOKEN_WRITELN) {
        CmdP();
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
                reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
                reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
                reg.token == TOKEN_WRITELN) {
                CmdP();
            }
        }
    }

    casaToken(TOKEN_PONTO_VIRG);
    Exp();
    casaToken(TOKEN_PONTO_VIRG);
    if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
        reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
        reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
        reg.token == TOKEN_WRITELN) {
        CmdP();
        while (reg.token == TOKEN_VIRG) {
            casaToken(TOKEN_VIRG);
            if (reg.token == TOKEN_ID || reg.token == TOKEN_FOR ||
                reg.token == TOKEN_IF || reg.token == TOKEN_PONTO_VIRG ||
                reg.token == TOKEN_READLN || reg.token == TOKEN_WRITE ||
                reg.token == TOKEN_WRITELN) {
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
    casaToken(TOKEN_IF);
    casaToken(TOKEN_ABRE_PAREN);
    Exp();
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
    casaToken(TOKEN_READLN);
    casaToken(TOKEN_ABRE_PAREN);
    casaToken(TOKEN_ID);
    if (reg.token == TOKEN_ABRE_COLCH) {
        casaToken(TOKEN_ABRE_COLCH);
        Exp();
        casaToken(TOKEN_FECHA_COLCH);
    }
    casaToken(TOKEN_FECHA_PAREN);
}
// CmdWrite -> (write|writeln)"(" Exp {, Exp} ")"
void CmdWrite() {
    if (reg.token == TOKEN_WRITE) {
        casaToken(TOKEN_WRITE);
    } else {
        casaToken(TOKEN_WRITELN);
    }

    casaToken(TOKEN_ABRE_PAREN);
    Exp();
    while (reg.token == TOKEN_VIRG) {
        casaToken(TOKEN_VIRG);
        Exp();
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
    t.inserir("EOF", TOKEN_EOF);  // CONSERTAR !!!!!
    t.inserir("boolean", TOKEN_BOOL);

    try {
        analisadorLexico();
        Prog();
        casaToken(TOKEN_EOF);
    } catch (int err) {
        if (err == ERR_TOKEN) {
            cout << linha << endl
                 << "token não esperado [" << reg.lexema << "].";
        } else if (err == ERR_CHAR) {
            cout << linha << endl << "caractere invalido.";
        } else if (err == ERR_EOF) {
            cout << linha << endl << "fim de arquivo nao esperado.";
        }
        return 0;
    } catch (string err) {
        cout << linha << endl << "lexema nao identificado [" << err << "].";
        return 0;
    }

    // while (reg.token != TOKEN_EOF && !ERR) {
    //     analisadorLexico();
    //     // cout << "token " << reg.lexema << " identificado" << endl;
    //     // cout << endl << "-- registro lexico atual --" << endl;
    //     // cout << "posição → " << reg.posicao << ", lexema → " << reg.lexema
    //     //      << ", tipo → " << reg.tipo << ", tamanho → " << reg.tamanho
    //     << ", token → " << reg.token << endl;

    //     // analise sintatica...
    // }
    // if (!ERR) cout << linha << " linhas compiladas.";
    // t.mostrar();
    cout << linha << " linhas compiladas.";
    return 0;
}
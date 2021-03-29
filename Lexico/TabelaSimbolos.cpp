#include <bits/stdc++.h>

using namespace std;

#define TOKEN_ID 1
#define TOKEN_CONST 2
#define TOKEN_IGUAL 3
#define TOKEN_2_PTS_IGUAL 4
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

//Definir valores pro restante dos tokens e pros tipos das variaveis

struct Simbolo{
    string lexema;
    int token; //Short provisorio
};

class TabelaSimbolos {
    int num_posicoes;
    
    list<Simbolo> *tabela;

    public:
        TabelaSimbolos(int n); //Construtor     
        int inserir(string lex, int token);
        int pesquisar(string lex);
        int hash(string lex);
};

TabelaSimbolos::TabelaSimbolos(int n) { 
  this->num_posicoes = n;
  tabela = new list<Simbolo>[num_posicoes];
}

int TabelaSimbolos::inserir(string lex, int token){
    Simbolo s = {lex, token};
    int pos = hash(lex);
    tabela[pos].push_back(s);
	return pos;
}

int TabelaSimbolos::pesquisar(string lex){
    //Pega a posicao inicial
    int pos = -1;
    int pos_inicial = hash(lex);

    // Verifica se a lista da posição encontrada é vazia
    if(tabela[pos_inicial].size() != 0) {

        //Pesquisa o lexema na tabela de simbolos
        list<Simbolo>::iterator i;
        for(i = tabela[pos_inicial].begin(); i != tabela[pos_inicial].end(); i++){
            //Se encontrado, retorna a posição na tabela
            if(!i->lexema.compare(lex)){
                pos = pos_inicial;
                break;
            } 
        }
    }

    return pos;
}

int TabelaSimbolos::hash(string lex){
    int soma;
    int pos;
    
    //Converter cada char pra int e somar
    for(int i = 0; i < lex.size(); i++){
        soma += (int)lex[i];
    }
    //Fazer o mod num_posicoes para achar a posicao
    pos = soma % num_posicoes;
    return pos;
}

void analisadorLexico() {

}

int main(){

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

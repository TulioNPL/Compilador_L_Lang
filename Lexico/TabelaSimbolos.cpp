#include <bits/stdc++.h>

using namespace std;

#DEFINE TOKEN_ID 1
#DEFINE TOKEN_CONST 2
//Definir valores pro restante dos tokens e pros tipos das variaveis

struct Simbolo{
    string lexema;
    short int token; //Short provisorio
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
    
}

int TabelaSimbolos::pesquisar(string lex){
    //Pega a posicao inicial
    int pos;
    int pos_inicial = hash(lex);

    //Verifica se a lista nessa posicao esta vazia, 
        //se estiver retorna -1
    
        //Senao, Procura na lista referente a posicao inicial

            //se encontrar retorna inteiro representando a posicao na tabela
            //Senao retorna -1; 
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
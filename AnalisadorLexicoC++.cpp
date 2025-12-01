#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <fstream>
#include <sstream>

// Definicoes de tipos
enum class TipoToken {
    FimArquivo,
    Erro,
    // Palavras reservadas
    CmVar, CmIf, CmElse, CmWhile, TipoInt, TipoReal,
    // Identificadores e literais
    Id, Num,
    // Operadores e delimitadores
    OpSoma, OpSub, OpMult, OpDiv, OpPot, CmAtrib,
    OpMaior, OpMenor, OpMaiorIgual, OpMenorIgual, OpIgual, OpDiferente,
    DelimAbrePar, DelimFechaPar, DelimAbreChave, DelimFechaChave,
    DelimVirgula, DelimPontoVirgula, DelimPonto
};

struct Token {
    TipoToken Tipo;
    std::string Lexema;
};

// Classe de analisador lexico
class AnalisadorLexico {
private:
    std::string CodigoFonte;
    std::regex RegexMestra;
    std::sregex_iterator IteradorAtual;
    std::sregex_iterator FimIterador;
    
    // Mapeia a expressao regular para o tipo do token
    std::vector<std::pair<std::string, TipoToken>> RegrasLexicas;

    void ConfigurarRegras() {
        
        // Palavras reservadas
        RegrasLexicas.push_back({ "if\\b",     TipoToken::CmIf });
        RegrasLexicas.push_back({ "else\\b",   TipoToken::CmElse });
        RegrasLexicas.push_back({ "while\\b",  TipoToken::CmWhile });
        RegrasLexicas.push_back({ "var\\b",    TipoToken::CmVar });
        RegrasLexicas.push_back({ "int\\b",    TipoToken::TipoInt });
        RegrasLexicas.push_back({ "real\\b",   TipoToken::TipoReal });

        // Numeros (inteiros ou reais)
        RegrasLexicas.push_back({ "[0-9]+(?:\\.[0-9]+)?", TipoToken::Num });

        // Identificadores (genericos)
        RegrasLexicas.push_back({ "[a-zA-Z][a-zA-Z0-9]*", TipoToken::Id });

        // Operadores compostos
        RegrasLexicas.push_back({ ">=", TipoToken::OpMaiorIgual });
        RegrasLexicas.push_back({ "<=", TipoToken::OpMenorIgual });
        RegrasLexicas.push_back({ "==", TipoToken::OpIgual });
        RegrasLexicas.push_back({ "!=", TipoToken::OpDiferente });

        // Caracteres simples e operadores
        RegrasLexicas.push_back({ "\\+", TipoToken::OpSoma });
        RegrasLexicas.push_back({ "-",   TipoToken::OpSub });
        RegrasLexicas.push_back({ "\\*", TipoToken::OpMult });
        RegrasLexicas.push_back({ "/",   TipoToken::OpDiv });
        RegrasLexicas.push_back({ "\\^", TipoToken::OpPot });
        RegrasLexicas.push_back({ "=",   TipoToken::CmAtrib });
        RegrasLexicas.push_back({ ">",   TipoToken::OpMaior });
        RegrasLexicas.push_back({ "<",   TipoToken::OpMenor });
        
        // Delimitadores
        RegrasLexicas.push_back({ "\\(", TipoToken::DelimAbrePar });
        RegrasLexicas.push_back({ "\\)", TipoToken::DelimFechaPar });
        RegrasLexicas.push_back({ "\\{", TipoToken::DelimAbreChave });
        RegrasLexicas.push_back({ "\\}", TipoToken::DelimFechaChave });
        RegrasLexicas.push_back({ ";",   TipoToken::DelimPontoVirgula });
        RegrasLexicas.push_back({ ",",   TipoToken::DelimVirgula });
        RegrasLexicas.push_back({ "\\.", TipoToken::DelimPonto });
    }

    void CompilarRegex() {
        std::string regexCombinada;
        // Constroi uma unica regex
        for (const auto& regra : RegrasLexicas) {
            regexCombinada += "(" + regra.first + ")|";
        }
        
        // Remove o ultimo pipe '|'
        if (!regexCombinada.empty()) {
            regexCombinada.pop_back();
        }

        RegexMestra = std::regex(regexCombinada);
    }

public:
    AnalisadorLexico(std::string codigoFonte) {
        CodigoFonte = codigoFonte;
        ConfigurarRegras();
        CompilarRegex();
        
        // Inicializa o iterador sobre o codigo fonte
        IteradorAtual = std::sregex_iterator(CodigoFonte.begin(), CodigoFonte.end(), RegexMestra);
        FimIterador = std::sregex_iterator();
    }

    Token ObterProximoToken() {
        if (IteradorAtual == FimIterador) {
            return { TipoToken::FimArquivo, "" };
        }

        std::smatch match = *IteradorAtual;
        size_t indiceRegra = 0;
        
        // Identifica qual subgrupo da regex capturou o texto
        for (size_t i = 1; i < match.size(); ++i) {
            if (match[i].length() > 0) {
                indiceRegra = i - 1;
                break;
            }
        }

        // Verificacao de seguranca
        if (indiceRegra >= RegrasLexicas.size()) {
             IteradorAtual++;
             return { TipoToken::Erro, match.str() };
        }

        Token tokenEncontrado;
        tokenEncontrado.Lexema = match.str();
        tokenEncontrado.Tipo = RegrasLexicas[indiceRegra].second;

        // Avanca para o proximo match
        IteradorAtual++;

        return tokenEncontrado;
    }
};

// Funcao auxiliar para impressao
std::string ObterNomeToken(TipoToken tipo) {
    switch (tipo) {
        case TipoToken::CmVar: return "CM_VAR";
        case TipoToken::CmIf: return "CM_IF";
        case TipoToken::CmElse: return "CM_ELSE";
        case TipoToken::CmWhile: return "CM_WHILE";
        case TipoToken::TipoInt: return "TYPE_INT";
        case TipoToken::TipoReal: return "TYPE_REAL";
        case TipoToken::Id: return "ID";
        case TipoToken::Num: return "NUM";
        case TipoToken::OpSoma: return "OP_ADD";
        case TipoToken::OpSub: return "OP_SUB";
        case TipoToken::OpMult: return "OP_MULT";
        case TipoToken::OpDiv: return "OP_DIV";
        case TipoToken::OpMaior: return "OP_GT";
        case TipoToken::OpMenor: return "OP_LT";
        case TipoToken::OpMaiorIgual: return "OP_GE";
        case TipoToken::OpMenorIgual: return "OP_LE";
        case TipoToken::OpIgual: return "OP_EQ";
        case TipoToken::OpDiferente: return "OP_NE";
        case TipoToken::CmAtrib: return "CM_ATRIB";
        case TipoToken::DelimAbreChave: return "DELIM";
        case TipoToken::DelimFechaChave: return "DELIM";
        case TipoToken::DelimAbrePar: return "DELIM";
        case TipoToken::DelimFechaPar: return "DELIM";
        case TipoToken::DelimPontoVirgula: return "DELIM";
        case TipoToken::DelimVirgula: return "DELIM";
        case TipoToken::DelimPonto: return "DELIM";
        default: return "TOKEN_DESCONHECIDO";
    }
}

// Main -----------------------------------------------------------------------------------------------------------------------------
int main() {
    std::string nomeArquivo;
    
    std::cout << "[_-=-_] Analisador Lexico [_-=-_]\n";
    std::cout << "Digite o nome do arquivo (ex: Codigo.txt): ";
    std::cin >> nomeArquivo;

    // Tenta abrir o arquivo
    std::ifstream arquivo(nomeArquivo);

    if (!arquivo.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo '" << nomeArquivo << "'\n";
        return 1;
    }

    // Le todo o conteudo do arquivo para uma string
    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    std::string codigoFonte = buffer.str();

    std::cout << "\n--- Conteudo do Arquivo ---\n" << codigoFonte << "\n---------------------------\n\nSaida:\n";

    // Passa o conteudo do arquivo para o analisador
    AnalisadorLexico lexer(codigoFonte);
    
    Token t;
    do {
        t = lexer.ObterProximoToken();
        if (t.Tipo != TipoToken::FimArquivo) {
            std::cout << "'" << t.Lexema << "' -> " << ObterNomeToken(t.Tipo) << std::endl;
        }
    } while (t.Tipo != TipoToken::FimArquivo);

    arquivo.close();
    return 0;
}
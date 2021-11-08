#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <unistd.h>
#include <sys/types.h>
#include <locale.h>
#include <time.h> //contar tempo
#include <sys/stat.h>
#include "json.h" //Trabalhar com objetos JSON

#define nDisplayWidth 21
#define nDisplayHeight 7
#define nImpressaoWidth 40
#define nImpressaoHeight 50
#define FTerminal "terminal.json"
#define FProdutos "produtos.json"
#define FVendas "vendas.json"
#define FImpressao "impressao.txt"
#define FPS 60

//define das teclas
#define teclaDEL 0xC
#define KeyCANCEL teclaDEL
#define KeyBACKSPACE 0xA
#define KeyENTER 0xB
#define Key01 0x1
#define Key02 0x2
#define Key03 0x3
#define Key04 0x4
#define Key05 0x5
#define Key06 0x6
#define Key07 0x7
#define Key08 0x8
#define Key09 0x9
#define Key00 0x0

//define dos IDs de Erros
#define ERRO_NADA 0x00
#define ERRO_ESTADO_NAO_REC 0x01
#define ERRO_VALOR_MIN 0x02
#define ERRO_VALOR_MAX 0x03
#define ERRO_CARTAO_INVALIDO 0x04
#define ERRO_OPERACAO_CANCELADA 0x05
#define ERRO_PARCELA_INVALIDA 0x06


const int TamDisplay = nDisplayWidth*nDisplayHeight;
const int TamImpressao = nImpressaoWidth*nImpressaoHeight;
char *display, *displayBuffer, **terminal, **produtos, **idProduto, *impressao, *impressaoBuffer;
time_t t;
struct tm tm;
clock_t t1;
HANDLE hDisplay;
DWORD dwBytesWritten;
COORD origin = {0,0};

//Variaveis do terminal
json_char* JCterminal;
json_value* JVterminal;
json_char* JCprodutos;
json_value* JVprodutos;
uint8_t STATE = 0;
uint8_t PRODUTO = 0;
uint8_t idERRO = 0;
unsigned __int64 VENDA = 0;
unsigned __int64 VMIN = 0;
unsigned __int64 VMAX = 0;
unsigned int PARCELAS = 0;
char *ROTULO;
char *IDPRODUTO;
char *sERRO;
char VALOR[] = "                0,00";
char CARTAO[] = "                     ";
char sCARTAO[] = "                     ";
char CNPJ[] = "99.999.999/9999-99";

//FUNCOES TERMINAL
void iniciarPOS(void);
void configTerminal(void);
void maquinaEstados(void);
void displayTerminal(void);
void getNumVendas(void);
void ResetVar(void);
void freeVariaveis(void);

//ESTADOS
//define dos estados
#define ESTADO_TELA_PRINCIPAL 0x00
void TelaPrincipal(void);

#define ESTADO_MENU_VENDA 0x01
void TelaMenuVenda(void);

#define ESTADO_MENU_ESTORNO 0x02
void TelaMenuEstorno(void);

#define ESTADO_VALOR_VENDA 0x03
void TelaValorVenda(void);

#define ESTADO_NUM_PARCELAS 0x04
void TelaNumParcelas(void);

#define ESTADO_NUM_CARTAO 0x05
void TelaNumCartao(void);

#define ESTADO_CONFIRM_VENDA 0x06
void TelaComfirmVenda(void);

#define ESTADO_CONFIRM_ESTORNO 0x07
void TelaComfirmEstorno(void);

#define ESTADO_RELAT_DATA 0x08
void TelaRelatData(void);

#define ESTADO_ERRO 0x09
void TelaErro(void);

#define ESTADO_PRINT_VENDA 0x10
void PrintVenda(void);

#define ESTADO_PRINT_ESTORNO 0x11
void PrintEstorno(void);

#define ESTADO_PRINT_RELATORIO 0x12
void PrintRelatorio(void);

//FUNCOES AUXILIARES
void cDisplay(char* printScreen, char* screen, int nScreenWidth, int TamScreen);
int ReadKey(void);
void converterInt2Notacao(char *str, unsigned __int64 num);
void converterCartao2Notacao(char *src, char *dst);
void converterCNPJ2Notacao(char *src, char *dst);


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

//FUNCOES AUXILIARES
void cDisplay(char* printScreen, char* screen, int nScreenWidth, int TamScreen);
int ReadKey(void);
void converterInt2Notacao(char *str, unsigned __int64 num);

//FUNCOES TERMINAL
void ResetVar(void);
void iniciarPOS(void);
void configTerminal(void);
void freeVariaveis(void);
void maquinaEstados(void);
void displayTerminal(void);

//ESTADOS
void TelaPrincipal(void);
void TelaMenuVenda(void);
void TelaMenuEstorno(void);
void TelaValorVenda(void);
void TelaNumParcelas(void);
void TelaNumCartao(void);
void TelaComfirmVenda(void);
void TelaComfirmEstorno(void);
void TelaRelatData(void);
void TelaErro(void);
void PrintVenda(void);
void PrintEstorno(void);
void PrintRelatorio(void);
void converterCartao2Notacao(char *str, unsigned __int64 num);

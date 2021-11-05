#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <Windows.h> //ler input de teclas
#include <locale.h>
#include <time.h> //contar tempo
#include <sys/stat.h>
#include "json.h" //Trabalhar com objetos JSON

// Compile (static linking) with
//  gcc -o POS -I.. POS.c json.c -lm

#define nDisplayWidth 21
#define nDisplayHeight 7
#define nImpressaoWidth 40
#define nImpressaoHeight 50
#define FTerminal "terminal.json"
#define FProdutos "produtos.json"
#define FVendas "vendas.json"
#define FImpressao "impressao.txt"
#define FPS 60

//FUNCOES AUXILIARES
void cDisplay(char* printScreen, char* screen, int nScreenWidth, int TamScreen);
int ReadKey(void);
void converterInt2Notacao(char *str, unsigned __int64 num);
void ResetVar(void);

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

const int TamDisplay = nDisplayWidth*nDisplayHeight;
const int TamImpressao = nImpressaoWidth*nImpressaoHeight;
char *display, *displayBuffer, **terminal, **produtos, **idProduto, *impressao, *impressaoBuffer;
json_char* JCterminal;
json_value* JVterminal;
json_char* JCprodutos;
json_value* JVprodutos;
time_t t;
struct tm tm;

//Variaveis do terminal
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


int main(){
    //Create Display Buffer
    display = (char*) malloc(TamDisplay*sizeof(char));
    displayBuffer = (char*) malloc(TamDisplay*sizeof(char));
    impressao = (char*) malloc(TamImpressao*sizeof(char));
    impressaoBuffer = (char*) malloc(TamImpressao*sizeof(char));
    HANDLE hDisplay = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                                CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hDisplay);
    DWORD dwBytesWritten = 0;
    COORD origin = {0,0};

    //CONFIG FILE---------------------
    FILE *fpT, *fpP;
    int file_size_T, file_size_P;
    char* file_contents_T;
    char* file_contents_P;
    struct stat filestatus_T, filestatus_P;
    //Read file
    fpT = fopen(FTerminal,"rb");
    fpP = fopen(FProdutos,"rb");;
    if ( (stat(FTerminal, &filestatus_T) != 0) || (stat(FProdutos, &filestatus_P) != 0) ) {
            return 1;
    }
    file_size_T = filestatus_T.st_size;
    file_size_P = filestatus_P.st_size;
    file_contents_T = (char*) malloc(filestatus_T.st_size);
    file_contents_P = (char*) malloc(filestatus_P.st_size);
    if ( (fpT == NULL) || (fpT == NULL) ) {
        fclose(fpT);
        fclose(fpP);
        free(file_contents_T);
        free(file_contents_P);
        return 1;
    }
    if ( (fread(file_contents_T, file_size_T, 1, fpT) != 1) || fread(file_contents_P, file_size_P, 1, fpP) != 1 ) {
        fclose(fpT);
        fclose(fpP);
        free(file_contents_T);
        free(file_contents_P);
        return 1;
    }
    fclose(fpT);
    fclose(fpP);
    
    JCterminal = (json_char*)file_contents_T;
    JVterminal = json_parse(JCterminal,file_size_T);
    JCprodutos = (json_char*)file_contents_P;
    JVprodutos = json_parse(JCprodutos,file_size_P);

    free(file_contents_T);
    free(file_contents_P);

    //Ler terminal.json
    int *length;
    length = (int*) malloc(sizeof(int));
    length[0] = JVterminal->u.object.values[0].value->u.object.length;
    terminal = (char**) malloc(length[0]*sizeof(char*));
    for (int x = 0; x < length[0]; x++) {
        terminal[x] = (char*) malloc( ( strlen(JVterminal->u.object.values[0].value->u.object.values[x].value->u.string.ptr) + 1 )*sizeof(char) );
        sprintf(terminal[x],JVterminal->u.object.values[0].value->u.object.values[x].value->u.string.ptr);
    }
    
    //Ler produtos.json
    length[0] = JVprodutos->u.object.values[0].value->u.object.length;
    produtos = (char**) malloc(length[0]*sizeof(char*));
    idProduto = (char**) malloc(length[0]*sizeof(char*));
    for (int x = 0; x < length[0]; x++) {
        produtos[x] = (char*) malloc((strlen(JVprodutos->u.object.values[0].value->u.object.values[x].value->u.object.values[2].value->u.string.ptr)+1)*sizeof(char));
        sprintf(produtos[x],JVprodutos->u.object.values[0].value->u.object.values[x].value->u.object.values[2].value->u.string.ptr);
        idProduto[x] = (char*) malloc((strlen(JVprodutos->u.object.values[0].value->u.object.values[x].value->u.object.values[0].value->u.string.ptr)+1)*sizeof(char));
        sprintf(idProduto[x],JVprodutos->u.object.values[0].value->u.object.values[x].value->u.object.values[0].value->u.string.ptr);
    }
    free(length);

    
    ROTULO = (char*) malloc(TamDisplay*sizeof(char));
    IDPRODUTO = (char*) malloc(TamDisplay*sizeof(char));
    sERRO = (char*) malloc(TamDisplay*sizeof(char));
    ResetVar();
    clock_t t1;
    while(1){
        t1 = clock();
        switch (STATE){
            case 0://Tela Principal
                TelaPrincipal();
            break;
            case 1://Tela Menu de Vendas
                TelaMenuVenda();
            break;
            case 2://Tela Menu de Transações para Estorno
                TelaMenuEstorno();
            break;
            case 3://Tela Valor da Venda
                TelaValorVenda();
            break;
            case 4://Tela Número de Parcelas
                TelaNumParcelas();
            break;
            case 5://Tela Número do Cartão
                TelaNumCartao();
            break;
            case 6://Tela Confirmação de Venda
                TelaComfirmVenda();
            break;
            case 7://Tela Confirmação de Estorno
                TelaComfirmEstorno();
            break;
            case 8://Tela Data do Relatorio
                TelaRelatData();
            break;
            case 9://Tela de Erros
                TelaErro();
            break;
            case 10://Imprimir Comprovante de Venda
                PrintVenda();
            break;
            case 11://Imprimir Comprovante de Estorno
                PrintEstorno();
            break;
            case 12://Imprimir Relatório
                PrintRelatorio();
            break;
            case 13://RESET
                PrintRelatorio();
            break;
            default:
                TelaPrincipal();
                STATE = 0;
            break;
            }
        
        display[TamDisplay] = '\0';
        WriteConsoleOutputCharacter(hDisplay, display, TamDisplay, origin, &dwBytesWritten);
        while( ((float)(clock()-t1)/CLOCKS_PER_SEC) < ((float)1/FPS)){
            //Wait frame period to finish
        }
    }
    return 0;
}

void cDisplay(char* printScreen, char* screen, int nScreenWidth, int TamScreen){
    int i = 0;//Posição na string
    int s = 0;//Posição no display
    int wordSize = 0;
    int pad = 0;
    int padC = 0;
    int sizeOfPrint = strlen(printScreen);
    for(i = 0; i < TamScreen; i++){
        //Limpar tela
        screen[i] = ' ';
    }
    for(i = 0; (i < sizeOfPrint) && (s < TamScreen); i++){
        switch (printScreen[i]){
            case '\n':
            //Alinhar a ESQUERDA------Inicio==================================================================================
                i++;
                for( ; (printScreen[i] != '\n') && (i < sizeOfPrint) && (s < TamScreen) ; ){
                //caso teja dentro da string
                    switch (printScreen[i]){
                        case ' ':
                        //Se for um ' '--------------Inicio
                            if( (s%nScreenWidth) == 0 ){
                            //Se tiver no inicio da linha
                                for( ; (printScreen[i] == ' ') && (i < sizeOfPrint); i++){
                                    //Remover todos os ' ' antes de uma palavra
                                }
                            }else{
                            //Se não for o inicio da linha
                                for( ; (printScreen[i] == ' ') && ((s%nScreenWidth) != 0) &&
                                    (i < sizeOfPrint) && (s < TamScreen); i++, s++){
                                    //Imprimir todos os ' ' antes de uma palavra ou até chegar no fim da linha
                                }
                            }
                        break;
                        //Se for um ' '--------------Fim

                        case '\n':
                        //Se for um '\n'--------------Inicio
                            //nada
                        break;
                        //Se for um '\n'--------------Fim

                        default:
                        //Se for uma palavra----------Inicio
                            for( wordSize = 0; (printScreen[i + wordSize] != ' ') && (printScreen[i + wordSize] != '\n') &&
                                ( (i + wordSize) < sizeOfPrint); wordSize++){
                                //Contar tamanho da palavra
                            }
                            if( wordSize >= nScreenWidth ){
                            //Caso a palavra for maior que uma linha inteira
                                if( (s+1)%nScreenWidth == 0 ){
                                //Caso a posição do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posição não seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamScreen); s++){
                                    if ( (s+1)%nScreenWidth == 0 && wordSize > 1){
                                    //Caso seja o ultimo caracter usar hífen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                        i++;
                                        wordSize--;
                                    }
                                }
                            }else{
                            //Caso não for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }else{
                                //Se não couber quebrar linha
                                    pad = nScreenWidth - (s%nScreenWidth);
                                    s += pad;
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }
                            }
                        break;
                        //Se for uma palavra----------Fim
                    }
                }
                //Quebrar linha quando acabar alinhamento
                if(s < TamScreen){
                    pad = nScreenWidth - (s%nScreenWidth);
                    s += pad;
                }else{
                    //Nada
                }
            break;
            //Alinhar a ESQUERDA------Fim=====================================================================================

            case '\t':
            //Alinhar ao CENTRO-------Inicio==================================================================================
                i++;
                for( ; (printScreen[i] != '\t') && (i < sizeOfPrint) && (s < TamScreen) ; ){
                //caso teja dentro da string
                    switch (printScreen[i]){
                        case ' ':
                        //Se for um ' '--------------Inicio
                            if( (s%nScreenWidth) == 0 ){
                            //Se tiver no inicio da linha
                                for( ; (printScreen[i] == ' ') && (i < sizeOfPrint); i++){
                                    //Remover todos os ' ' antes de uma palavra
                                }
                            }else{
                            //Se não for o inicio da linha
                                for( ; (printScreen[i] == ' ') && ((s%nScreenWidth) != 0) &&
                                    (i < sizeOfPrint) && (s < TamScreen); i++, s++){
                                    //Imprimir todos os ' ' antes de uma palavra ou até chegar no fim da linha
                                }
                            }
                        break;
                        //Se for um ' '--------------Fim

                        case '\t':
                        //Se for um '\n'--------------Inicio
                            //nada
                        break;
                        //Se for um '\n'--------------Fim

                        default:
                        //Se for uma palavra----------Inicio
                            for( wordSize = 0; (printScreen[i + wordSize] != ' ') && (printScreen[i + wordSize] != '\t') &&
                                ( (i + wordSize) < sizeOfPrint); wordSize++){
                                //Contar tamanho da palavra
                            }
                            if( wordSize >= nScreenWidth ){
                            //Caso a palavra for maior que uma linha inteira
                                if( (s+1)%nScreenWidth == 0 ){
                                //Caso a posição do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posição não seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamScreen); s++){
                                    if ( (s+1)%nScreenWidth == 0 && wordSize > 1){
                                    //Caso seja o ultimo caracter usar hífen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                        i++;
                                        wordSize--;
                                    }
                                }
                            }else{
                            //Caso não for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0 ){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }else{
                                //Se não couber quebrar linha
                                    pad = nScreenWidth - (s%nScreenWidth);
                                    for(int j = 0; j < (nScreenWidth - pad); j++){
                                        screen[s-1-j+pad/2] = screen[s-1-j];
                                    }
                                    for(int j = 0, padC = s - (s%nScreenWidth); (j < pad/2); j++){
                                        screen[padC+j] = ' ';
                                    }
                                    s += pad;
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }
                            }
                        break;
                        //Se for uma palavra----------Fim
                    }
                }
                //Quebrar linha quando acabar alinhamento
                if(s < TamScreen){
                    //Deslocar linha para direita
                    pad = nScreenWidth - (s%nScreenWidth);
                    for(int j = 0; j < (nScreenWidth - pad); j++){
                        screen[s-1-j+pad/2] = screen[s-1-j];
                    }
                    for(int j = 0, padC = s - (s%nScreenWidth); (j < pad/2); j++){
                        screen[padC+j] = ' ';
                    }
                    s += pad;
                }else{
                    //Nada
                }
            //Alinhar ao CENTRO-------Fim=====================================================================================
            break;

            case '\r':
            //Alinhar a DIREITA-------Inicio==================================================================================
                i++;
                for( ; (printScreen[i] != '\r') && (i < sizeOfPrint) && (s < TamScreen) ; ){
                //caso teja dentro da string
                    switch (printScreen[i]){
                        case ' ':
                        //Se for um ' '--------------Inicio
                            if( (s%nScreenWidth) == 0 ){
                            //Se tiver no inicio da linha
                                for( ; (printScreen[i] == ' ') && (i < sizeOfPrint); i++){
                                    //Remover todos os ' ' antes de uma palavra
                                }
                            }else{
                            //Se não for o inicio da linha
                                for( ; (printScreen[i] == ' ') && ((s%nScreenWidth) != 0) &&
                                    (i < sizeOfPrint) && (s < TamScreen); i++, s++){
                                    //Imprimir todos os ' ' antes de uma palavra ou até chegar no fim da linha
                                }
                            }
                        break;
                        //Se for um ' '--------------Fim

                        case '\r':
                        //Se for um '\n'--------------Inicio
                            //nada
                        break;
                        //Se for um '\n'--------------Fim

                        default:
                        //Se for uma palavra----------Inicio
                            for( wordSize = 0; (printScreen[i + wordSize] != ' ') && (printScreen[i + wordSize] != '\r') &&
                                ( (i + wordSize) < sizeOfPrint); wordSize++){
                                //Contar tamanho da palavra
                            }
                            if( wordSize >= nScreenWidth ){
                            //Caso a palavra for maior que uma linha inteira
                                if( (s+1)%nScreenWidth == 0 ){
                                //Caso a posição do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posição não seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamScreen); s++){
                                    if ( (s+1)%nScreenWidth == 0 && wordSize > 1){
                                    //Caso seja o ultimo caracter usar hífen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                        i++;
                                        wordSize--;
                                    }
                                }
                            }else{
                            //Caso não for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0 ){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }else{
                                //Se não couber quebrar linha
                                    //Deslocar linha para direita
                                    pad = nScreenWidth - (s%nScreenWidth);
                                    for(int j = 0; j < (nScreenWidth - pad); j++){
                                        screen[s-1-j+pad] = screen[s-1-j];
                                    }
                                    for(int j = 0, padC = s - (s%nScreenWidth); (j < pad); j++){
                                        screen[padC+j] = ' ';
                                    }
                                    s += pad;
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }
                            }
                        break;
                        //Se for uma palavra----------Fim
                    }
                }
                //Quebrar linha quando acabar alinhamento
                if(s < TamScreen){
                    //Deslocar linha para direita
                    pad = nScreenWidth - (s%nScreenWidth);
                    for(int j = 0; j < (nScreenWidth - pad); j++){
                        screen[s-1-j+pad] = screen[s-1-j];
                    }
                    for(int j = 0, padC = s - (s%nScreenWidth); (j < pad); j++){
                        screen[padC+j] = ' ';
                    }
                    s += pad;
                }else{
                    //Nada
                }
            break;
            //Alinhar a DIREITA-------Fim=====================================================================================

            case '\b':
            //Linha em branco---------Inicio==================================================================================
                pad = nScreenWidth - (s%nScreenWidth);
                s += pad;
            //Linha em branco---------Fim=====================================================================================
            break;

            default:
                //Não fazer nada
            break;
        }
    }
}

int ReadKey(void){
    int iKey = 0xF;
    if(GetAsyncKeyState((unsigned  short)'1') & 0x01){
        iKey = 0x1;
    }
    if(GetAsyncKeyState((unsigned  short)'2') & 0x01){
        iKey = 0x2;
    }
    if(GetAsyncKeyState((unsigned  short)'3') & 0x01){
        iKey = 0x3;
    }
    if(GetAsyncKeyState((unsigned  short)'4') & 0x01){
        iKey = 0x4;
    }
    if(GetAsyncKeyState((unsigned  short)'5') & 0x01){
        iKey = 0x5;
    }
    if(GetAsyncKeyState((unsigned  short)'6') & 0x01){
        iKey = 0x6;
    }
    if(GetAsyncKeyState((unsigned  short)'7') & 0x01){
        iKey = 0x7;
    }
    if(GetAsyncKeyState((unsigned  short)'8') & 0x01){
        iKey = 0x8;
    }
    if(GetAsyncKeyState((unsigned  short)'9') & 0x01){
        iKey = 0x9;
    }
    if(GetAsyncKeyState((unsigned  short)'0') & 0x01){
        iKey = 0x0;
    }
    if(GetAsyncKeyState(0x08) & 0x01){//BACKSPACE
        iKey = 0xA;
    }
    if(GetAsyncKeyState(0x0D) & 0x01){//ENTER
        iKey = 0xB;
    }
    if(GetAsyncKeyState(0x2E) & 0x01){//DEL / CANCEL
        iKey = 0xC;
    }
    return iKey;
}

void TelaPrincipal(void){//STATE = 00
    t = time(NULL);
    tm = *localtime(&t);
    sprintf(displayBuffer, "\t%s %02d/%02d %02d:%02d\t\t%s\t\b\tTecle ENTER\t\tpara vender\t\b\t1-ESTORNO   2-RELAT\t",
            terminal[0], tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, terminal[3]);

    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            case 0xB://Key = ENTER
                STATE = 1;//ValorVenda
                break;
            case 0x1://Key = 1
                STATE = 2;//MenuEstorno
                break;
            case 0x2://Key = 2
                STATE = 8;//RelatData
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaMenuVenda(void){//STATE = 01
    sprintf(displayBuffer, "\tESCOLHA A VENDA\t\n1-CREDITO A VISTA\n\n2-CREDITO PARCELADO\n\n3-DEBITO\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            case 0x1://Key = CANCEL
                PRODUTO = 1;//Escolheu credito a vista
                sprintf(ROTULO,"%s",produtos[PRODUTO-1]);
                sprintf(IDPRODUTO,"%s",idProduto[PRODUTO-1]);
                VMIN = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[3].value->u.dbl);
                VMAX = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[4].value->u.dbl);
                STATE = 3;
                break;
            case 0x2://Key = CANCEL
                PRODUTO = 2;//Escolheu credito parcelado
                sprintf(ROTULO,"%s",produtos[PRODUTO-1]);
                sprintf(IDPRODUTO,"%s",idProduto[PRODUTO-1]);
                VMIN = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[3].value->u.dbl);
                VMAX = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[4].value->u.dbl);
                STATE = 3;
                break;
            case 0x3://Key = CANCEL
                PRODUTO = 3;//Escolheu debito
                sprintf(ROTULO,"%s",produtos[PRODUTO-1]);
                sprintf(IDPRODUTO,"%s",idProduto[PRODUTO-1]);
                VMIN = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[3].value->u.dbl);
                VMAX = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[4].value->u.dbl);
                STATE = 3;
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaMenuEstorno(void){//STATE = 02
    sprintf(displayBuffer, "\nFalta implementar Tela Menu de Estronos\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaValorVenda(void){//STATE = 03
    if(VENDA > 999999999999){
        VENDA = 999999999999;
    }
    converterInt2Notacao(VALOR, VENDA);
    sprintf(displayBuffer, "\t%s\t\b\rVALOR (R$)\r\b\r%s\r", ROTULO, VALOR);
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        int KeyPressed = ReadKey();
        if(KeyPressed == 0xC){
            ResetVar();
        }else if(KeyPressed == 0xB){//Key = ENTER
            if(VENDA >= VMIN && VENDA <= VMAX){
                switch (PRODUTO){
                    case 0x1://CREDITO A VIISTA
                        STATE = 5;//Tela Num Cartao
                    break;
                    case 0x2://CREDITO PARCELADO
                        STATE = 4;//Tela num parcela
                    break;
                    case 0x3://DEBITO
                        STATE = 5;//Tela Num Cartao
                    break;
                    
                    default:

                    break;
                }
            }else if(VENDA > VMAX){
                idERRO = 2;//valor>mmax
                STATE = 9;//Tela de Erro
                VENDA = 0;
            }else{
                idERRO = 1;//valor<min
                STATE = 9;//Tela de Erro
                VENDA = 0;
            }
        }else if(KeyPressed == 0xA){//Key = BACKSPACE
            VENDA /= 10;
        }else if(KeyPressed >= 0x0 && KeyPressed <= 0x9){
            VENDA *= 10;
            VENDA += KeyPressed;
        }else{//Nothing pressed
        }
    }
}

void TelaNumParcelas(void){//STATE = 04
    if(PARCELAS>999){
        PARCELAS = 999;
    } 
    sprintf(displayBuffer, "\t%s\t\b\rPARCELAS\r\b\t%dx\t", ROTULO, PARCELAS);
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        int KeyPressed = ReadKey();
        if(KeyPressed == 0xC){
            ResetVar();
        }else if(KeyPressed == 0xB){//Key = ENTER
            if(PARCELAS <= 1){
                STATE = 9;
                idERRO = 5;
                PARCELAS = 0;
            }else{
                STATE = 5;
            }
        }else if(KeyPressed == 0xA){//Key = BACKSPACE
            PARCELAS /= 10;
        }else if(KeyPressed >= 0x0 && KeyPressed <= 0x9){
            PARCELAS *= 10;
            PARCELAS += KeyPressed;
        }else{//Nothing pressed
        }
    }
}

void TelaNumCartao(void){//STATE = 05
    sprintf(sCARTAO,"");
    int len = strlen(CARTAO);
    char c;
    for(int i = 0; i < len; i++){
        if( (i > 0) && (i%4 == 0) ){
            c = '.';
            strncat(sCARTAO, &c, 1);
        }
        c = CARTAO[i];
        strncat(sCARTAO, &c, 1);
    }
    sprintf(displayBuffer, "\t%s\t\b\rCARTAO\r\b\t%s\t", ROTULO, sCARTAO);
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        int KeyPressed = ReadKey();
        switch (KeyPressed){
            case 0xC://Key = CANCEL
                ResetVar();
            break;
            case 0xB://Key = ENTER
                if( len<=19 && len>=11 ){
                    STATE = 6;//Tela Confirmar venda
                }else{
                    idERRO = 3;//Cartao Invalido
                    STATE = 9;//Tela de Erro
                    sprintf(CARTAO,"");
                }
            break;
            case 0xA://Key = BACKSPACE
                CARTAO[len-1] = '\0';
            break;
            case 0x1://1
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x2:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x3:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x4:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x5:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x6:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x7:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x8:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x9:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            case 0x0:
                c = '0'+KeyPressed;
                strncat(CARTAO, &c, 1);
            break;
            default://Nothing pressed
                //nada
            break;
        }
    }
}

void TelaComfirmVenda(void){//STATE = 06
    char aux[] = "                     ";
    char aux2[] = "                     ";
    sprintf(aux,"R$");
    int auxint = nDisplayWidth - strlen(VALOR) - 4;
    char c = ' ';
    for(int i = 0; i < auxint; i++){
        strncat(aux, &c, 1);
    }
    strncat(aux, VALOR, strlen(VALOR));
    sprintf(aux2,CARTAO);
    for (int i = 4; i < strlen(aux2)-4; i++){
        aux2[i] = '*';
    }
    sprintf(displayBuffer, "\t%s\t\tConfirma Venda?\t\b\t%s\t\t%s\t\b\tSIM-1         2-NAO\t",ROTULO, aux2, aux);
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            case 0x1://Key = CANCEL
                STATE = 10;
                break;
            case 0x2://Key = CANCEL
                STATE = 9;
                idERRO = 4;
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaComfirmEstorno(void){//STATE = 07
    sprintf(displayBuffer, "\nFalta implementar Tela Comfirmacao de Estorno\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaRelatData(void){//STATE = 08
    sprintf(displayBuffer, "\nFalta implementar Tela Data de Relatorio\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaErro(void){//STATE = 09
    switch (idERRO){
        case 0:
            sprintf(sERRO, "\tNAO HA ERRO\t");
        break;

        case 1://erro de Valor minimo
            converterInt2Notacao(VALOR,VMIN);
            sprintf(sERRO,"\t\"VALOR MENOR QUE O MINIMO DE R$ %s\"\t",VALOR);
        break;

        case 2://erro de Valor maximo
            converterInt2Notacao(VALOR,VMAX);
            sprintf(sERRO,"\t\"VALOR MENOR QUE O MAXIMO DE R$ %s\"\t",VALOR);
        break;

        case 3://erro de Cartao Invalido
            sprintf(sERRO,"\t\"CARTAO INVALIDO\"\t");
        break;

        case 4://erro de Operacao Cancelada
            sprintf(sERRO,"\t\"OPERACAO CANCELADA\"\t");
        break;

        case 5://num de parcelas incorreto
            sprintf(sERRO,"\t\"NUMERO INCORRETO DE PARCELAS\"\t");
        break;
        
        default:
            sprintf(sERRO, "\nERRO NAO RECONHECIDO.\n");
        break;
    }
    sprintf(displayBuffer, "\tERRO %d\t\b%s", idERRO,sERRO);
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        int KeyPressed = ReadKey();
        switch (idERRO){
            case 1:
                if(KeyPressed == 0xC){
                    ResetVar();
                }else if(KeyPressed >= 0x0 && KeyPressed <= 0xB){
                    STATE = 3;
                    VENDA = 0;
                }else{//Nothing pressed

                }
            break;
            case 2:
                if(KeyPressed == 0xC){
                    ResetVar();
                }else if(KeyPressed >= 0x0 && KeyPressed <= 0xB){
                    STATE = 3;
                    VENDA = 0;
                }else{//Nothing pressed

                }
            break;
            case 3:
                if(KeyPressed == 0xC){
                    ResetVar();
                }else if(KeyPressed >= 0x0 && KeyPressed <= 0xB){
                    STATE = 5;
                    VENDA = 0;
                }else{//Nothing pressed

                }
            break;
            case 5:
                if(KeyPressed == 0xC){
                    ResetVar();
                }else if(KeyPressed >= 0x0 && KeyPressed <= 0xB){
                    STATE = 4;
                    PARCELAS = 0;
                }else{//Nothing pressed

                }
            break;
            default:
                if(KeyPressed >= 0x0 && KeyPressed <= 0xC){
                    ResetVar();
                }else{//Nothing pressed

                }
            break;
        }
    }
}

void PrintVenda(void){//STATE = 10
    char dataToAppend[500];
    unsigned __int64 numVend = 0;
    t = time(NULL);
    tm = *localtime(&t);
    FILE *fpV;
    fpV = fopen(FVendas,"rb+");
    if( fpV == NULL ){
        fpV = fopen(FVendas,"wb");
        sprintf(dataToAppend,"{\n\t\"*Venda* #%I64u\": {\n\t\t\"tipo\": %d,\n\t\t\"rotulo\": \"%s\",\n\t\t\"valor\": %lld.%02d,\n\t\t\"parcelas\": %d,\n\t\t\"cartao\": %s,\n\t\t\"data\":{\n\t\t\t\"dia\": %d,\n\t\t\t\"mes\": %d,\n\t\t\t\"ano\": %d\n\t\t},\n\t\t\"horario\":{\n\t\t\t\"hora\": %d,\n\t\t\t\"min\": %d,\n\t\t\t\"seg\": %d\n\t\t},\n\t\t\"estornada\": false\n\t}\n}",numVend,PRODUTO,ROTULO,VENDA/100,VENDA%100,PARCELAS,CARTAO,tm.tm_mday,tm.tm_mon,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec);
    }else{
        char wrd[] = "\"*Venda* #";
        char *aux;
        char buf = ' ';
        aux = (char*) malloc( (strlen(wrd) + 1)*sizeof(char) );
        sprintf(aux, "          ");
        fseek(fpV,0,SEEK_END);
        for(int i = 0;  i<1000000; i++, fseek(fpV,-i,SEEK_END)){
            buf = fgetc(fpV);
            for(int j = strlen(aux)-1; j > 0 ; j--){
                aux[j] = aux[j-1];
            }
            aux[0] = buf;
            if(strcmp(aux,wrd) == 0)break;
        }
        free(aux);
        while(fgetc(fpV) != '#');
        while(1){
            buf = fgetc(fpV);
            if(buf > '9' || buf < '0')break;
            numVend *= 10;
            numVend += buf-'0';
        }
        numVend++;
        sprintf(dataToAppend,",\n\t\"*Venda* #%I64u\": {\n\t\t\"tipo\": %d,\n\t\t\"rotulo\": \"%s\",\n\t\t\"valor\": %lld.%02d,\n\t\t\"parcelas\": %d,\n\t\t\"cartao\": %s,\n\t\t\"data\":{\n\t\t\t\"dia\": %d,\n\t\t\t\"mes\": %d,\n\t\t\t\"ano\": %d\n\t\t},\n\t\t\"horario\":{\n\t\t\t\"hora\": %d,\n\t\t\t\"min\": %d,\n\t\t\t\"seg\": %d\n\t\t},\n\t\t\"estornada\": false\n\t}\n}",numVend,PRODUTO,ROTULO,VENDA/100,VENDA%100,PARCELAS,CARTAO,tm.tm_mday,tm.tm_mon,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec);
        //remover final do arquivo
        fseeko(fpV,-2,SEEK_END);
        off_t position = ftello(fpV);
        ftruncate(fileno(fpV), position);
        fpV = freopen(FVendas,"ab+",fpV);
    }
    fprintf(fpV,dataToAppend);
    fclose(fpV);
    sprintf(dataToAppend,"");
    char c[] = "   ";
    char cnpj[] = "99.999.999/9999-99";
    sprintf(cnpj,"");
    sprintf(c," ");
    for(int i = 0; i < strlen(terminal[2]); i++){
        if(i == 2 || i == 5){
            c[0] = '.';
            strncat(cnpj,c,1);
        }
        if(i == 8){
            c[0] = '/';
            strncat(cnpj,c,1);
        }
        if(i == 12){
            c[0] = '-';
            strncat(cnpj,c,1);
        }
        c[0] = terminal[2][i];
        strncat(cnpj,c,1);
    }
    sprintf(c," ");
    for( int i = 0; i < nImpressaoWidth - strlen(VALOR) - 16 - 4;i++){
        strncat(dataToAppend,c,1);
    }
    sprintf(c,"R$ ");
    strncat(dataToAppend,c,strlen(c));
    strncat(dataToAppend,VALOR,strlen(VALOR));
    for(int i = 0; i < strlen(CARTAO) - 4; i++){
        CARTAO[i] = '*';
    }
    fpV = fopen(FImpressao,"wb");
    sprintf(impressaoBuffer,"\t%s\t\t%s\t\tCNPJ: %s\t\b\tDATA: %02d/%02d/%d TERMINAL: %s\t\b\n%s\n\n%s\n\b\tVALOR APROVADO: %s\t\b\n%s\n",
            terminal[3],terminal[1],cnpj,tm.tm_mday,tm.tm_mon,tm.tm_year+1900,terminal[0],ROTULO,
            CARTAO,dataToAppend,terminal[4]);
    cDisplay(impressaoBuffer, impressao, nImpressaoWidth, TamImpressao);
    for(int i = 0; i < strlen(impressao); i++){
        char c[] = " ";
        c[0] = impressao[i];
        fprintf(fpV,c);
        if ( (i+1)%nImpressaoWidth == 0 ){
            c[0] = '\n';
            fprintf(fpV,c);
        }
    }
    fclose(fpV);
    ResetVar();
}

void PrintEstorno(void){//STATE = 11
    sprintf(displayBuffer, "\nFalta implementar Tela Impressao de Estorno\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            default://Nothing pressed
                break;
        }
    }
}

void PrintRelatorio(void){//STATE = 12
    sprintf(displayBuffer, "\nFalta implementar Tela Impressao de Relatorio\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                ResetVar();
                break;
            default://Nothing pressed
                break;
        }
    }
}

void ResetVar(void){
    STATE = 0;
    PRODUTO = 0;
    PARCELAS = 0;
    idERRO = 0;
    VENDA = 0;
    VMIN = 0;
    VMAX = 0;
    sprintf(ROTULO,"");
    sprintf(IDPRODUTO,"");
    sprintf(sERRO,"");
    sprintf(sCARTAO,"");
    sprintf(CARTAO,"");
}

void converterInt2Notacao(char *str, unsigned __int64 num){
    char aux[] = "                    ";
    unsigned __int64 auxValor = num;
    sprintf(aux,"%I64u", num);
    int tamValor = strlen(aux);
    int i = 0;
    char c;
    sprintf(str,"00,0");
    for(i = 0; (i < tamValor) && auxValor > 0 ; i++){
        if(i < 2){
            str[i] = auxValor%10+'0';
            auxValor /= 10;
        }else if(i == 2){
            str[i+1] = auxValor%10+'0';
            auxValor /= 10;
        }else if(i>3 && ((i-2)%3 == 0)){
            c = '.';
            strncat(str, &c, 1);
            c = auxValor%10+'0';
            strncat(str, &c, 1);
            auxValor /= 10;
        }else{
            c = auxValor%10+'0';
            strncat(str, &c, 1);
            auxValor /= 10;
        }
    }
    tamValor = strlen(str);
    sprintf(aux,str);
    for(i = 0; i < tamValor; i++){
        str[i] = aux[tamValor-1-i];
    }
}



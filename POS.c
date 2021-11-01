#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <sys/stat.h>
#include "json.h"

// Compile (static linking) with
//  gcc -o POS -I.. POS.c json.c -lm

#define nScreenWidth 21
#define nScreenHeight 7
#define FileTerminal "terminal.json"
#define FPS 60

void cDisplay(char* printScreen);
int ReadKey(void);
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

char *screen, *screenBuffer, **terminal;
json_char* JCterminal;
json_value* JVterminal;
time_t t;
struct tm tm;
uint8_t STATE = 0;

int main(){
    //Create Display Buffer
    screen = (char*) malloc(nScreenWidth*nScreenHeight*sizeof(char));
    screenBuffer = (char*) malloc(nScreenWidth*nScreenHeight*sizeof(char));
    HANDLE hDisplay = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                                CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hDisplay);
    DWORD dwBytesWritten = 0;
    COORD origin = {0,0};

    //CONFIG FILE---------------------
    FILE *fp;
    int file_size;
    char* file_contents;
    struct stat filestatus;
    char FTerminal[] = FileTerminal;
    //Read file
    fp = fopen(FTerminal,"rb");
    if ( stat(FTerminal, &filestatus) != 0) {
            fprintf(stderr, "File %s not found\n", FTerminal);
            printf("EOF: %d\n",feof(fp));
            printf("ERROR: %d\n",ferror(fp));
            return 1;
    }
    file_size = filestatus.st_size;
    file_contents = (char*) malloc(filestatus.st_size);
    if (fp == NULL) {
            fprintf(stderr, "Unable to open %s\n", FTerminal);
            printf("EOF: %d\n",feof(fp));
            printf("ERROR: %d\n",ferror(fp));
            fclose(fp);
            free(file_contents);
            return 1;
    }
    if ( fread(file_contents, file_size, 1, fp) != 1 ) {
            fprintf(stderr, "Unable to read content of %s\n", FTerminal);
            printf("EOF: %d\n",feof(fp));
            printf("ERROR: %d\n",ferror(fp));
            fclose(fp);
            free(file_contents);
            return 1;
    }
    fclose(fp);

    //printf("%s\n", file_contents);
    JCterminal = (json_char*)file_contents;
    JVterminal = json_parse(JCterminal,file_size);

    int length = JVterminal->u.object.values[0].value->u.object.length;
    terminal = (char**) malloc(length*sizeof(char*));
    for (int x = 0; x < length; x++) {
        terminal[x] = (char*) malloc((strlen(JVterminal->u.object.values[0].value->u.object.values[x].value->u.string.ptr)+1)*sizeof(char));
        terminal[x] = JVterminal->u.object.values[0].value->u.object.values[x].value->u.string.ptr;
    }
    
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
            default:
                TelaPrincipal();
                STATE = 0;
            break;
            }
        
        screen[nScreenWidth*nScreenHeight] = '\0';
        WriteConsoleOutputCharacter(hDisplay, screen, nScreenWidth*nScreenHeight, origin, &dwBytesWritten);
        while( ((float)(clock()-t1)/CLOCKS_PER_SEC) < ((float)1/FPS)){
            //Wait frame period to finish
        }
    }
    return 0;
}

void cDisplay(char* printScreen){
    for(int i = 0; i < nScreenWidth*nScreenHeight; i++){
        screen[i] = ' ';
    }
    int s = 0;
    int aux = 0;
    int auxLB = 0;
    int c = 0;
    int pad = 0;
    for(int i = 0; i <= strlen(printScreen); i++){
        aux = 0;
        auxLB = 0;
        c = 0;
        pad = 0;
        //Allign LEFT-------
        if(printScreen[i] == '\n'){
            i++;
            //write left
            while(printScreen[i] != '\n' && s < (nScreenWidth*nScreenWidth)){
                //if = ' '
                if((printScreen[i] == ' ')){
                    //Check if begining of line -> remove
                    if( (s%nScreenWidth) == 0){
                        while((printScreen[i] == ' ') && (printScreen[i] != '\n') && (i <= strlen(printScreen))){
                            i++;
                        }
                    }else{
                        //print ' ' elsewhere and stop if at end of line
                        while((printScreen[i] == ' ') && (printScreen[i] != '\n') && (i <= strlen(printScreen)) && ((s%nScreenWidth) != 0) ){
                            s++;
                            i++;
                        }
                    }
                //Not ' ' i.e. word
                }else{
                    //Count word size
                    auxLB = 0;
                    while(printScreen[i+auxLB] != ' ' && printScreen[i] != '\n' && (i + auxLB) <= strlen(printScreen)){
                        auxLB++;
                    }
                    //If word (size > ScreenWidth) -> hyphenate
                    if((auxLB+1) > nScreenWidth){
                        //If pointer is at 1 char to LB
                        pad = nScreenWidth-(s+1)%nScreenWidth;
                        if( pad > 1){
                            s += pad;
                        }
                        while( (auxLB >= 0) && (s < nScreenWidth*nScreenHeight)){
                            //Check if rest of word dsn't fit
                            if(!( ((s+1)%nScreenWidth) == 0 ) && auxLB > 0){
                                screen[s] = '-';
                                s++;
                            }else{
                                screen[s] = printScreen[i];
                                s++;
                                i++;
                                auxLB--;
                            }
                        }
                    }else{
                        //If word size fits screen
                        if( ( ((s%nScreenWidth) + auxLB)/nScreenWidth ) == 0){
                            while(auxLB >= 0 && s < nScreenWidth*nScreenHeight){
                                screen[s] = printScreen[i];
                                s++;
                                i++;
                                auxLB--;
                            }
                        //If not -> pad then write
                        }else{
                            pad = nScreenWidth - (s%nScreenWidth);
                            s += pad;
                            while(auxLB >= 0 && s < nScreenWidth*nScreenHeight){
                                screen[s] = printScreen[i];
                                s++;
                                i++;
                                auxLB--;
                            }
                        }
                    }
                }
            }
            //pad right
            if(s%nScreenWidth){
                pad = nScreenWidth - (s%nScreenWidth);
                s += pad;
                if(s >= (nScreenWidth*nScreenWidth))break;
            }
        }
        //Allign CENTER-----
        if(printScreen[i] == '\t'){
            i++;
            //Count num of char inside \t...\t
            while(printScreen[i+aux] != '\t' && (s+aux) < (nScreenWidth*nScreenWidth)){
                aux++;
            }
            c = aux%nScreenWidth;
            aux -= c;
            pad = nScreenWidth - c;
            //if string > screen width
            for(int j = 0; j < aux && s < (nScreenWidth*nScreenWidth); j++){
                screen[s] = printScreen[i];
                s++;
                i++;
            }
            //pad left
            for(int j = 0; j < pad/2 && (s+j) < (nScreenWidth*nScreenWidth); j++){
                screen[s + j] = ' ';
            }
            s += pad/2;
            //Write center
            for(int j = 0; j < c && s < (nScreenWidth*nScreenWidth); j++){
                screen[s] = printScreen[i];
                s++;
                i++;
            }
            //pad right
            pad += pad%2;
            pad /= 2;
            for(int j = 0; j < pad && (s+j) < (nScreenWidth*nScreenWidth); j++){
                screen[s + j] = ' ';
            }
            s += pad;
            if(s >= (nScreenWidth*nScreenWidth))break;
        }
        //Empty line
        if(printScreen[i] == '\b'){
            s += nScreenWidth;
        }
        if(s >= (nScreenWidth*nScreenWidth))break;
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
    sprintf(screenBuffer, "\n%s %02d/%02d %02d:%02d\n\t%s\t\b\tTecle ENTER\t\tpara vender\t\b\t1-ESTORNO   2-RELAT\t",
            terminal[0], tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, terminal[3]);
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            case 0xB://Key = ENTER
                STATE = 3;//ValorVenda
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
    sprintf(screenBuffer, "\nFalta implementar Tela Menu de Vendas\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaMenuEstorno(void){//STATE = 02
    sprintf(screenBuffer, "\nFalta implementar Tela Menu de Estronos\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaValorVenda(void){//STATE = 03
    sprintf(screenBuffer, "\nFalta implementar Tela Valor da Venda\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaNumParcelas(void){//STATE = 04
    sprintf(screenBuffer, "\nFalta implementar Tela Numero de Parcelas\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaNumCartao(void){//STATE = 05
    sprintf(screenBuffer, "\nFalta implementar Tela Numero do Cartao\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaComfirmVenda(void){//STATE = 06
    sprintf(screenBuffer, "\nFalta implementar Tela Cofirmacao de Venda\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaComfirmEstorno(void){//STATE = 07
    sprintf(screenBuffer, "\nFalta implementar Tela Comfirmacao de Estorno\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaRelatData(void){//STATE = 08
    sprintf(screenBuffer, "\nFalta implementar Tela Data de Relatorio\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void TelaErro(void){//STATE = 09
    sprintf(screenBuffer, "\nFalta implementar Tela Erros\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void PrintVenda(void){//STATE = 10
    sprintf(screenBuffer, "\nFalta implementar Tela Impressao de Venda\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void PrintEstorno(void){//STATE = 11
    sprintf(screenBuffer, "\nFalta implementar Tela Impressao de Estorno\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}

void PrintRelatorio(void){//STATE = 12
    sprintf(screenBuffer, "\nFalta implementar Tela Impressao de Relatorio\n");
    cDisplay(screenBuffer);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case 0xC://Key = CANCEL
                STATE = 0;//TPrincipal
                break;
            default://Nothing pressed
                break;
        }
    }
}
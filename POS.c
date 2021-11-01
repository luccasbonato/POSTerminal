#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> //ler input de teclas
#include <locale.h>
#include <time.h> //contar tempo
#include <sys/stat.h>
#include "json.h" //Trabalhar com objetos JSON

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
const int TamDisplay = nScreenWidth*nScreenHeight;

int main(){
    //Create Display Buffer
    screen = (char*) malloc(TamDisplay*sizeof(char));
    screenBuffer = (char*) malloc(TamDisplay*sizeof(char));
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
        
        screen[TamDisplay] = '\0';
        WriteConsoleOutputCharacter(hDisplay, screen, TamDisplay, origin, &dwBytesWritten);
        while( ((float)(clock()-t1)/CLOCKS_PER_SEC) < ((float)1/FPS)){
            //Wait frame period to finish
        }
    }
    return 0;
}

void cDisplay(char* printScreen){
    int i = 0;//Posição na string
    int s = 0;//Posição no display
    int wordSize = 0;
    int pad = 0;
    int padC = 0;
    int sizeOfPrint = strlen(printScreen);
    for(i = 0; i < TamDisplay; i++){
        //Limpar tela
        screen[i] = ' ';
    }
    for(i = 0; (i < sizeOfPrint) && (s < TamDisplay); i++){
        switch (printScreen[i]){
            case '\n':
            //Alinhar a ESQUERDA------Inicio==================================================================================
                i++;
                for( ; (printScreen[i] != '\n') && (i < sizeOfPrint) && (s < TamDisplay) ; ){
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
                                    (i < sizeOfPrint) && (s < TamDisplay); i++, s++){
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
                            if( wordSize > nScreenWidth ){
                            //Caso a palavra for maior que uma linha inteira
                                if( (s+1)%nScreenWidth == 0 ){
                                //Caso a posição do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posição não seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamDisplay); i++, s++, wordSize--){
                                    if ( (s+1)%nScreenWidth == 0 ){
                                    //Caso seja o ultimo caracter usar hífen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }
                            }else{
                            //Caso não for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0 ){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamDisplay); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }else{
                                //Se não couber quebrar linha
                                    pad = nScreenWidth - (s%nScreenWidth);
                                    s += pad;
                                    for( ; (wordSize > 0) && (s < TamDisplay); i++, s++, wordSize--){
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
                if(s < TamDisplay){
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
                for( ; (printScreen[i] != '\t') && (i < sizeOfPrint) && (s < TamDisplay) ; ){
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
                                    (i < sizeOfPrint) && (s < TamDisplay); i++, s++){
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
                            if( wordSize > nScreenWidth ){
                            //Caso a palavra for maior que uma linha inteira
                                if( (s+1)%nScreenWidth == 0 ){
                                //Caso a posição do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posição não seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamDisplay); i++, s++, wordSize--){
                                    if ( (s+1)%nScreenWidth == 0 ){
                                    //Caso seja o ultimo caracter usar hífen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }
                            }else{
                            //Caso não for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0 ){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamDisplay); i++, s++, wordSize--){
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
                                    for( ; (wordSize > 0) && (s < TamDisplay); i++, s++, wordSize--){
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
                if(s < TamDisplay){
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

    // //Testar o '\n' - alinhar a esquerda
    // sprintf(screenBuffer, "\n%s %02d/%02d %02d:%02d\n\n%s\n\n\n\nTecle ENTER\n\npara vender\n\n\n\n1-ESTORNO   2-RELAT\n",
    //         terminal[0], tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, terminal[3]);

    // //Testar o '\t' - alinhar ao centro
    // sprintf(screenBuffer, "\t%s %02d/%02d %02d:%02d\t\t%s\t\n\n\tTecle ENTER\t\tpara vender\t\n\n\t1-ESTORNO   2-RELAT\t",
    //         terminal[0], tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, terminal[3]);

    //Testar o '\b' - quebra de linha
    sprintf(screenBuffer, "\t%s %02d/%02d %02d:%02d\t\t%s\t\b\tTecle ENTER\t\tpara vender\t\b\t1-ESTORNO   2-RELAT\t",
            terminal[0], tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, terminal[3]);

    // //Testar o '\n' - alinhar a esquerda - com string grande
    // sprintf(screenBuffer, "\n%s\n", terminal[5]);

    // //Testar o '\t' - alinhar ao centro - com string grande
    // sprintf(screenBuffer, "\t%s\t", terminal[4]);

    // //Testar - palavra grande
    // sprintf(screenBuffer, "\n%s\n\t%s\t", "astrolopitecoastrolopitecoastrolopiteco", "astrolopitecoastrolopitecoastrolopiteco");

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
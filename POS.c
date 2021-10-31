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
#define BufferSize 1024

void cDisplay(char* printScreen);
void TelaPrincipal(void);
int ReadKey(void);

char *screen, *screenBuffer, **terminal;
json_char* JCterminal;
json_value* JVterminal;
time_t t;
struct tm tm;

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
    
    for(int i = 0; i < nScreenWidth*nScreenHeight; i++){
        screen[i] = ' ';
    }
    while(1){
        TelaPrincipal();
        
        screen[nScreenWidth*nScreenHeight] = '\0';
        WriteConsoleOutputCharacter(hDisplay, screen, nScreenWidth*nScreenHeight, origin, &dwBytesWritten);
    }
    return 0;
}

void cDisplay(char* printScreen){
    for(int i = 0; i < nScreenWidth*nScreenHeight; i++){
        screen[i] = 'x';
    }
    int s = 0;
    for(int i = 0; i <= strlen(printScreen); i++){
        //Allign LEFT-------
        if(printScreen[i] == '\n'){
            i++;
            //write left
            while(printScreen[i] != '\n' && s < (nScreenWidth*nScreenWidth)){
                screen[s] = printScreen[i];
                s++;
                i++;
            }
            //pad right
            int pad = nScreenWidth - (s%nScreenWidth);
            for(int j = 0; j < pad && (s+j) < (nScreenWidth*nScreenWidth); j++){
                screen[s + j] = 'x';
            }
            s += pad;
            if(s >= (nScreenWidth*nScreenWidth))break;
        }
    }
}

void TelaPrincipal(void){
    t = time(NULL);
    tm = *localtime(&t);
    // sprintf(screen, "%d", JVterminal->u.object.values[0].value->u.object.length);
    // sprintf(screen, "%s", terminal[1]);
    // sprintf(screen, "%d %d %d %d", strlen(terminal[0]),
    //  strlen(terminal[1]), strlen(terminal[2]), strlen(terminal[3]));
    sprintf(screenBuffer, "\n%s %02d/%02d %02d:%02d\n\n%s\n\n\n\nTecle ENTER\n\npara vender\n\n\n\n1-ESTORNO     2-RELAT\n",
            terminal[0], tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, terminal[3]);
    cDisplay(screenBuffer);
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
    if(GetAsyncKeyState(0x2E) & 0x01){//DEL
        iKey = 0xC;
    }
    return iKey;
}
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include "json.h"

#define nScreenWidth 21
#define nScreenHeight 7
#define FileTerminal "terminal.json"

void TelaPrincipal(void);
int ReadKey(void);

char *screen, **terminal;
json_char* JCterminal;
json_value* JVterminal;

int main(){
    //Create Display Buffer
    screen = (char*) malloc(nScreenWidth*nScreenHeight*sizeof(char));
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
    //Read file
    fp = fopen(FileTerminal,"rb");
    file_size = filestatus.st_size;
    char *buffer = (char*) malloc(filestatus.st_size);
    
    if ( stat(FileTerminal, &filestatus) != 0) {
            fprintf(stderr, "File %s not found\n", FileTerminal);
            return 1;
    }
    if (fp == NULL) {
            fprintf(stderr, "Unable to open %s\n", FileTerminal);
            printf("EOF: %d\n",feof(fp));
            printf("ERROR: %d\n",ferror(fp));
            fclose(fp);
            free(file_contents);
            return 1;
    }
    if ( fread(file_contents, (size_t)file_size, 1, fp) != 1 ) {
            fprintf(stderr, "Unable to read content of %s\n", FileTerminal);
            printf("EOF: %d\n",feof(fp));
            printf("ERROR: %d\n",ferror(fp));
            fclose(fp);
            free(file_contents);
            return 1;
    }
    fclose(fp);

    printf("%s\n", file_contents);
    JCterminal = (json_char*)file_contents;
    JVterminal = json_parse(JCterminal,file_size);

    int length = JVterminal->u.object.length;
    terminal = (char**) malloc(length*sizeof(char*));
    for (int x = 0; x < length; x++) {
        *terminal = (char*) malloc(sizeof(JVterminal->u.object.values[0].value->u.object.values[x].value->u.string.ptr)*sizeof(char));
        terminal[x] = JVterminal->u.object.values[0].value->u.object.values[x].value->u.string.ptr;
    }

    while(1){
        TelaPrincipal();
        
        screen[nScreenWidth*nScreenHeight] = '\0';
        WriteConsoleOutputCharacter(hDisplay, screen, nScreenWidth*nScreenHeight, origin, &dwBytesWritten);
    }

    return 0;
}

void TelaPrincipal(void){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(screen, "%s %s/%s %s:%s\n", terminal[0], tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min);
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
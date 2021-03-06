#include "POS.h"

int main(){
    iniciarPOS();
    while(1){
        t1 = clock();
        maquinaEstados();
        displayTerminal();
        while( ((float)(clock()-t1)/CLOCKS_PER_SEC) < ((float)1/FPS)){
            //Wait frame period to finish
        }
    }
    freeVariaveis();
    return 0;
}




//=====================================================FUNCOES TERMINAL=====================================================
void iniciarPOS(void){
    //Alloc Display Buffer
    display = (char*) malloc(TamDisplay*sizeof(char));
    displayBuffer = (char*) malloc(TamDisplay*sizeof(char));
    impressao = (char*) malloc( ( TamImpressao + nImpressaoHeight )*sizeof(char));
    impressaoBuffer = (char*) malloc( ( TamImpressao + nImpressaoHeight )*sizeof(char));
    hDisplay = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                                CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hDisplay);
    dwBytesWritten = 0;

    configTerminal();

    ROTULO = (char*) malloc(TamDisplay*sizeof(char));
    IDPRODUTO = (char*) malloc(TamDisplay*sizeof(char));
    sERRO = (char*) malloc(TamDisplay*sizeof(char));
    ResetVar();
    char displayLines[nDisplayHeight][nDisplayWidth];
}

void configTerminal(void){
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
    }
    if ( (fread(file_contents_T, file_size_T, 1, fpT) != 1) || fread(file_contents_P, file_size_P, 1, fpP) != 1 ) {
        fclose(fpT);
        fclose(fpP);
        free(file_contents_T);
        free(file_contents_P);
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
    converterCNPJ2Notacao(terminal[2],CNPJ);
}

void maquinaEstados(void){
    switch (STATE){
        case ESTADO_TELA_PRINCIPAL:
            TelaPrincipal();
        break;
        case ESTADO_MENU_VENDA:
            TelaMenuVenda();
        break;
        case ESTADO_MENU_ESTORNO:
            TelaMenuEstorno();
        break;
        case ESTADO_VALOR_VENDA:
            TelaValorVenda();
        break;
        case ESTADO_NUM_PARCELAS:
            TelaNumParcelas();
        break;
        case ESTADO_NUM_CARTAO:
            TelaNumCartao();
        break;
        case ESTADO_CONFIRM_VENDA:
            TelaComfirmVenda();
        break;
        case ESTADO_CONFIRM_ESTORNO:
            TelaComfirmEstorno();
        break;
        case ESTADO_RELAT_DATA:
            TelaRelatData();
        break;
        case ESTADO_ERRO:
            TelaErro();
        break;
        case ESTADO_PRINT_VENDA:
            PrintVenda();
        break;
        case ESTADO_PRINT_ESTORNO:
            PrintEstorno();
        break;
        case ESTADO_PRINT_RELATORIO:
            PrintRelatorio();
        break;
        default:
            idERRO = ERRO_ESTADO_NAO_REC;
            STATE = ESTADO_ERRO;
        break;
    }
}

void displayTerminal(void){
    display[strlen(display)-1] = '\0';
    char displayLine[nDisplayWidth];
    for(int i = 0; i < nDisplayHeight; i++){
        for(int j = 0; j < nDisplayWidth; j++){
            displayLine[j] = display[i*nDisplayWidth+j];
        }
        origin = (COORD) {0,i};
        WriteConsoleOutputCharacter(hDisplay, displayLine, nDisplayWidth, origin, &dwBytesWritten);
    }
}

void getNumVendas(void){

}

void ResetVar(void){
    STATE = ESTADO_TELA_PRINCIPAL;
    PRODUTO = 0;
    PARCELAS = 0;
    idERRO = ERRO_NADA;
    VENDA = 0;
    VMIN = 0;
    VMAX = 0;
    sprintf(ROTULO,"");
    sprintf(IDPRODUTO,"");
    sprintf(sERRO,"");
    sprintf(sCARTAO,"");
    sprintf(CARTAO,"");
}

void freeVariaveis(void){
    free(display);
    free(displayBuffer);
    free(impressao);
    free(impressaoBuffer);
    free(ROTULO);
    free(IDPRODUTO);
    free(sERRO);
    free(JCterminal);
    free(JVterminal);
    free(JCprodutos);
    free(JVprodutos);
}




//==========================================================ESTADOS==========================================================
void TelaPrincipal(void){//STATE = 00
    t = time(NULL);
    tm = *localtime(&t);
    sprintf(displayBuffer, "\t%s %02d/%02d %02d:%02d\t\t%s\t\b\tTecle ENTER\t\tpara vender\t\b\t1-ESTORNO   2-RELAT\t",
            terminal[0], tm.tm_mday, tm.tm_mon+1, tm.tm_hour, tm.tm_min, terminal[3]);

    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case KeyCANCEL:
                ResetVar();
            break;
            case KeyENTER:
                STATE = ESTADO_MENU_VENDA;//ValorVenda
            break;
            case Key01:
                STATE = ESTADO_MENU_ESTORNO;//MenuEstorno
            break;
            case Key02:
                STATE = ESTADO_RELAT_DATA;//RelatData
            break;
            default:
                //Nothing pressed
            break;
        }
    }
}

void TelaMenuVenda(void){//STATE = 01
    sprintf(displayBuffer, "\tESCOLHA A VENDA\t\n1-CREDITO A VISTA\n\n2-CREDITO PARCELADO\n\n3-DEBITO\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        int keyPressed = ReadKey();
        if(keyPressed == KeyCANCEL){
            ResetVar();
        }else if(keyPressed >= Key00 && keyPressed <= Key09){
            PRODUTO = keyPressed;//Escolheu credito a vista
            sprintf(ROTULO,"%s",produtos[PRODUTO-1]);
            sprintf(IDPRODUTO,"%s",idProduto[PRODUTO-1]);
            VMIN = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[3].value->u.dbl);
            VMAX = (unsigned __int64)100*(JVprodutos->u.object.values[0].value->u.object.values[PRODUTO-1].value->u.object.values[4].value->u.dbl);
            STATE = ESTADO_VALOR_VENDA;
        }else {
            //Fazer nada
        }
    }
}

void TelaMenuEstorno(void){//STATE = 02
    sprintf(displayBuffer, "\nFalta implementar Tela Menu de Estronos\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case KeyCANCEL:
                ResetVar();
            break;
            default:
                //Nothing pressed
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
        if(KeyPressed == KeyCANCEL){
            ResetVar();
        }else if(KeyPressed == KeyENTER){
            if(VENDA >= VMIN && VENDA <= VMAX){
                switch (PRODUTO){
                    case Key01://CREDITO A VIISTA
                        STATE = ESTADO_NUM_CARTAO;
                    break;
                    case Key02://CREDITO PARCELADO
                        STATE = ESTADO_NUM_PARCELAS;
                    break;
                    case Key03://DEBITO
                        STATE = ESTADO_NUM_CARTAO;
                    break;
                    
                    default:
                        //Fazer nada
                    break;
                }
            }else{
                if(VENDA > VMAX){
                    idERRO = ERRO_VALOR_MAX;
                }else{
                    idERRO = ERRO_VALOR_MIN;
                }
                STATE = ESTADO_ERRO;
                VENDA = 0;
            }
        }else if(KeyPressed == KeyBACKSPACE){
            VENDA /= 10;
        }else if(KeyPressed >= Key00 && KeyPressed <= Key09){
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
        if(KeyPressed == KeyCANCEL){
            ResetVar();
        }else if(KeyPressed == KeyENTER){
            if(PARCELAS <= 1){
                STATE = ESTADO_ERRO;
                idERRO = ERRO_PARCELA_INVALIDA;
                PARCELAS = 0;
            }else{
                STATE = ESTADO_NUM_CARTAO;
            }
        }else if(KeyPressed == KeyBACKSPACE){
            PARCELAS /= 10;
        }else if(KeyPressed >= Key00 && KeyPressed <= Key09){
            PARCELAS *= 10;
            PARCELAS += KeyPressed;
        }else{//Nothing pressed
        }
    }
}

void TelaNumCartao(void){//STATE = 05
    int len = strlen(CARTAO);
    char c;
    converterCartao2Notacao(CARTAO, sCARTAO);
    sprintf(displayBuffer, "\t%s\t\b\rCARTAO\r\b\t%s\t", ROTULO, sCARTAO);
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        int KeyPressed = ReadKey();
        if(KeyPressed == KeyCANCEL){
            ResetVar();
        }else if(KeyPressed == KeyENTER){
            if( len<=19 && len>=11 ){
                STATE = ESTADO_CONFIRM_VENDA;
            }else{
                idERRO = ERRO_CARTAO_INVALIDO;
                STATE = ESTADO_ERRO;
                sprintf(CARTAO,"");
            }
        }else if(KeyPressed == KeyBACKSPACE){
            CARTAO[len-1] = '\0';
        }else if(KeyPressed >= Key00 && KeyPressed <= Key09){
            c = '0'+KeyPressed;
            strncat(CARTAO, &c, 1);
        }else {
            //Fazer nada
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
            case KeyCANCEL:
                ResetVar();
                break;
            case Key01:
                STATE = ESTADO_PRINT_VENDA;
                break;
            case Key02:
                STATE = ESTADO_ERRO;
                idERRO = ERRO_OPERACAO_CANCELADA;
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
            case KeyCANCEL:
                ResetVar();
            break;
            default:
                //Nothing pressed
            break;
        }
    }
}

void TelaRelatData(void){//STATE = 08
    sprintf(displayBuffer, "\nFalta implementar Tela Data de Relatorio\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case KeyCANCEL:
                ResetVar();
            break;
            default:
                //Nothing pressed
            break;
        }
    }
}

void TelaErro(void){//STATE = 09
    switch (idERRO){
        case ERRO_NADA:
            sprintf(sERRO, "\tNAO HA ERRO\t");
        break;

        case ERRO_ESTADO_NAO_REC:
            sprintf(sERRO, "\tESTADO NAO RECONHECIDO\t");
        break;

        case ERRO_VALOR_MIN:
            converterInt2Notacao(VALOR,VMIN);
            sprintf(sERRO,"\t\"VALOR MENOR QUE O MINIMO DE R$ %s\"\t",VALOR);
        break;

        case ERRO_VALOR_MAX:
            converterInt2Notacao(VALOR,VMAX);
            sprintf(sERRO,"\t\"VALOR MENOR QUE O MAXIMO DE R$ %s\"\t",VALOR);
        break;

        case ERRO_CARTAO_INVALIDO:
            sprintf(sERRO,"\t\"CARTAO INVALIDO\"\t");
        break;

        case ERRO_OPERACAO_CANCELADA:
            sprintf(sERRO,"\t\"OPERACAO CANCELADA\"\t");
        break;

        case ERRO_PARCELA_INVALIDA:
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
            case ERRO_VALOR_MIN:
                if(KeyPressed == KeyCANCEL){
                    ResetVar();
                }else if(KeyPressed >= Key00 && KeyPressed <= KeyENTER){
                    STATE = ESTADO_VALOR_VENDA;
                    VENDA = 0;
                }else{
                    //Nothing pressed
                }
            break;
            case ERRO_VALOR_MAX:
                if(KeyPressed == KeyCANCEL){
                    ResetVar();
                }else if(KeyPressed >= Key00 && KeyPressed <= KeyENTER){
                    STATE = ESTADO_VALOR_VENDA;
                    VENDA = 0;
                }else{
                    //Nothing pressed
                }
            break;
            case ERRO_CARTAO_INVALIDO:
                if(KeyPressed == KeyCANCEL){
                    ResetVar();
                }else if(KeyPressed >= Key00 && KeyPressed <= KeyENTER){
                    STATE = ESTADO_NUM_CARTAO;
                    VENDA = 0;
                }else{
                    //Nothing pressed
                }
            break;
            case ERRO_OPERACAO_CANCELADA:
                if(KeyPressed >= Key00 && KeyPressed <= KeyCANCEL){
                    ResetVar();
                }else{
                    //Nothing pressed
                }
            break;
            case ERRO_PARCELA_INVALIDA:
                if(KeyPressed == KeyCANCEL){
                    ResetVar();
                }else if(KeyPressed >= Key00 && KeyPressed <= KeyENTER){
                    STATE = ESTADO_NUM_PARCELAS;
                    PARCELAS = 0;
                }else{
                    //Nothing pressed
                }
            break;
            default://Qualquer outro erro
                if(KeyPressed >= Key00 && KeyPressed <= KeyCANCEL){
                    ResetVar();
                }else{
                    //Nothing pressed
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
    char c[] = "   ";
    fpV = fopen(FVendas,"rb+");
    if( fpV == NULL ){
        fpV = fopen(FVendas,"wb");
        sprintf(dataToAppend,"{\n\t\"*Venda* #%I64u\": {\n\t\t\"tipo\": %d,\n\t\t\"rotulo\": \"%s\",\n\t\t\"valor\": %lld.%02d,\n\t\t\"parcelas\": %d,\n\t\t\"cartao\": %s,\n\t\t\"data\":{\n\t\t\t\"dia\": %d,\n\t\t\t\"mes\": %d,\n\t\t\t\"ano\": %d\n\t\t},\n\t\t\"horario\":{\n\t\t\t\"hora\": %d,\n\t\t\t\"min\": %d,\n\t\t\t\"seg\": %d\n\t\t},\n\t\t\"estornada\": false\n\t}\n}",numVend,PRODUTO,ROTULO,VENDA/100,VENDA%100,PARCELAS,CARTAO,tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec);
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
        sprintf(dataToAppend,",\n\t\"*Venda* #%I64u\": {\n\t\t\"tipo\": %d,\n\t\t\"rotulo\": \"%s\",\n\t\t\"valor\": %lld.%02d,\n\t\t\"parcelas\": %d,\n\t\t\"cartao\": %s,\n\t\t\"data\":{\n\t\t\t\"dia\": %d,\n\t\t\t\"mes\": %d,\n\t\t\t\"ano\": %d\n\t\t},\n\t\t\"horario\":{\n\t\t\t\"hora\": %d,\n\t\t\t\"min\": %d,\n\t\t\t\"seg\": %d\n\t\t},\n\t\t\"estornada\": false\n\t}\n}",numVend,PRODUTO,ROTULO,VENDA/100,VENDA%100,PARCELAS,CARTAO,tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec);
        //remover final do arquivo
        fseeko(fpV,-2,SEEK_END);
        off_t position = ftello(fpV);
        ftruncate(fileno(fpV), position);
        fpV = freopen(FVendas,"ab+",fpV);
    }
    fprintf(fpV,dataToAppend);
    fclose(fpV);
    sprintf(dataToAppend,"");
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
            terminal[3],terminal[1],CNPJ,tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,terminal[0],ROTULO,
            CARTAO,dataToAppend,terminal[4]);
    cDisplay(impressaoBuffer, impressao, nImpressaoWidth, TamImpressao);
    sprintf(c," ");
    for(int i = 0; i < strlen(impressao); i++){
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
            case KeyCANCEL:
                ResetVar();
            break;
            default:
                //Nothing pressed
            break;
        }
    }
}

void PrintRelatorio(void){//STATE = 12
    sprintf(displayBuffer, "\nFalta implementar Tela Impressao de Relatorio\n");
    cDisplay(displayBuffer, display, nDisplayWidth, TamDisplay);
    if(WM_KEYDOWN){
        switch (ReadKey()){
            case KeyCANCEL:
                ResetVar();
            break;
            default:
                //Nothing pressed
            break;
        }
    }
}




//=====================================================FUNCOES AUXILIARES=====================================================
void cDisplay(char* printScreen, char* screen, int nScreenWidth, int TamScreen){
    int i = 0;//Posi????o na string
    int s = 0;//Posi????o no display
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
                            //Se n??o for o inicio da linha
                                for( ; (printScreen[i] == ' ') && ((s%nScreenWidth) != 0) &&
                                    (i < sizeOfPrint) && (s < TamScreen); i++, s++){
                                    //Imprimir todos os ' ' antes de uma palavra ou at?? chegar no fim da linha
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
                                //Caso a posi????o do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posi????o n??o seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamScreen); s++){
                                    if ( (s+1)%nScreenWidth == 0 && wordSize > 1){
                                    //Caso seja o ultimo caracter usar h??fen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                        i++;
                                        wordSize--;
                                    }
                                }
                            }else{
                            //Caso n??o for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }else{
                                //Se n??o couber quebrar linha
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
                            //Se n??o for o inicio da linha
                                for( ; (printScreen[i] == ' ') && ((s%nScreenWidth) != 0) &&
                                    (i < sizeOfPrint) && (s < TamScreen); i++, s++){
                                    //Imprimir todos os ' ' antes de uma palavra ou at?? chegar no fim da linha
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
                                //Caso a posi????o do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posi????o n??o seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamScreen); s++){
                                    if ( (s+1)%nScreenWidth == 0 && wordSize > 1){
                                    //Caso seja o ultimo caracter usar h??fen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                        i++;
                                        wordSize--;
                                    }
                                }
                            }else{
                            //Caso n??o for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0 ){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }else{
                                //Se n??o couber quebrar linha
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
                            //Se n??o for o inicio da linha
                                for( ; (printScreen[i] == ' ') && ((s%nScreenWidth) != 0) &&
                                    (i < sizeOfPrint) && (s < TamScreen); i++, s++){
                                    //Imprimir todos os ' ' antes de uma palavra ou at?? chegar no fim da linha
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
                                //Caso a posi????o do caracter do display for a ultima da linha
                                    s++;
                                }else{
                                //Caso a posi????o n??o seja a ultima da linha
                                    //nada
                                }
                                for( ; (wordSize > 0) && (s < TamScreen); s++){
                                    if ( (s+1)%nScreenWidth == 0 && wordSize > 1){
                                    //Caso seja o ultimo caracter usar h??fen
                                        screen[s] = '-';
                                    }else{
                                    //Imprimir palavra
                                        screen[s] = printScreen[i];
                                        i++;
                                        wordSize--;
                                    }
                                }
                            }else{
                            //Caso n??o for maior que uma linha inteira
                                if( ( ((s%nScreenWidth) + wordSize)/nScreenWidth ) == 0 ){
                                //Caso a palavra caber dentro da linha
                                    for( ; (wordSize > 0) && (s < TamScreen); i++, s++, wordSize--){
                                        //Imprimir palavra
                                        screen[s] = printScreen[i];
                                    }
                                }else{
                                //Se n??o couber quebrar linha
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
                //N??o fazer nada
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

void converterInt2Notacao(char *str, unsigned __int64 num){
    char aux[] = "                     ";
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

void converterCartao2Notacao(char *src, char *dst){
    sprintf(dst,"");
    int len = strlen(src);
    char c;
    for(int i = 0; i < len; i++){
        if( (i > 0) && (i%4 == 0) ){
            c = '.';
            strncat(dst, &c, 1);
        }
        c = src[i];
        strncat(dst, &c, 1);
    }    
}

void converterCNPJ2Notacao(char *src, char *dst){
    char c = ' ';
    sprintf(dst,"");
    for(int i = 0; i < strlen(src); i++){
        if(i == 2 || i == 5){
            c = '.';
            strncat(dst,&c,1);
        }
        if(i == 8){
            c = '/';
            strncat(dst,&c,1);
        }
        if(i == 12){
            c = '-';
            strncat(dst,&c,1);
        }
        c = src[i];
        strncat(dst,&c,1);
    }
}



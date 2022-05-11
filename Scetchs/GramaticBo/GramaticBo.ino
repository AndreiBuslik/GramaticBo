////////////////////////////////////////////// BIBLIOTECAS //////////////////////////////////////////

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <EEPROM.h>

////////////////////////////////////////////// DEFINES /////////////////////////////////////////////

#define MINPRESSURE 400
#define MAXPRESSURE 2000
#define PRETO     0x0000
#define VERMELHO  0xF800
#define VERDE     0x07E0
#define BRANCO    0xFFFF
#define AZUL      0x001F
#define ROXO      0x9339
#define AMARELO   0xFFE0
#define CIANO     0x07FF
#define CIANO_2   0x03EF
#define MAGENTA   0xF81F
#define XP 8
#define XM A2
#define YP A3
#define YM 9
#define TS_LEFT 78
#define TS_RT 889
#define TS_TOP 896
#define TS_BOT 126
#define ROTATION 1
#define TOTAL_BOTOES 6

///////////////////////////////////////////////// OBJETOS ////////////////////////////////////

TouchScreen ts = TouchScreen(XP, YP, XM, YM, MINPRESSURE);  // Objeto para captar toques na tela.
Adafruit_GFX_Button botoes[TOTAL_BOTOES];                   // Objeto para desenhar os botões na tela.
MCUFRIEND_kbv tft;                                          // Objeto usado para manipular as informações de acordo com os toques.
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;                // Objeto para usar a biclioteca com os caracteres latinos.

//////////////////////////////////////////////// ESTRUTURAS ////////////////////////////////////
// As frases em cada um dos jogos foram estruturadas em 2, 3 ou 4 partes para poder manipular //
// a cor dos caracteres e comparar com as respostas corretas.                                 //
////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct frase_2_partes{  // Estrutura para frases que são decompostas em duas partes.

  String parte_1, parte_2;  // Variáveis para guardar as duas partes da frase.
  byte resp_1, resp_2;      // Variáveis para guardar as respostas para o exercício de análise sintática. Para o exercício
                            // de estudo de vírgula uma das variáveis guarda a resposta correta, a outra marca qual das partes
}fp2;                       // da frase terá uma cor diferente para os caracteres.

typedef struct frase_3_partes{ // Estrutura para frases que são decompostas em três partes.

  String parte_1, parte_2, parte_3; // Variáveis para guardar as três partes da frase.
  byte resp_1, resp_2, resp_3;      // Variáveis para guardar as respostas para o exercício de análise sintática. Para o exercício
                                    // de estudo de vírgula uma das variáveis guarda a resposta correta, a outra marca qual das partes
}fp3;                               // da frase terá uma cor diferente para os caracteres.

typedef struct frase_4_partes{ // Estrutura para frases que são decompostas em quatro partes.

  String parte_1, parte_2, parte_3, parte_4; // Variáveis para guardar as quatro partes da frase.
  byte resp_1, resp_2, resp_3, resp_4;       // Variáveis para guardar as respostas para o exercício de análise sintática. Para o exercício
                                             // de estudo de vírgula uma das variáveis guarda a resposta correta, a outra marca qual das partes
}fp4;                                        // da frase terá uma cor diferente para os caracteres.

//////////////////////////////////////////////// VARIÁVEIS GLOBAIS //////////////////////////////////

bool pressionado = false, enunciado_Analise = true, enunciado_Virgula = true;
int posX;
int posY;
byte estado = 4;
byte pontos_N_1 = 0, pontos_N_2 = 0, pontos_N_3 = 0, pontos_Virgula_N_1 = 0, pontos_Virgula_N_2 = 0; // Pontuação em cada um dos jogos.

//////////////////// Análise Sintática Nível 1
     fp2 vet_N1_p2[2];  // Vetor para receber as frases divididas em duas partes. Primeiro nível do exercício de análise sintática.
     fp3 vet_N1_p3[9];  // Vetor para receber as frases divididas em três partes. Primeiro nível do exercício de análise sintática.
     fp4 vet_N1_p4[4];  // Vetor para receber as frases divididas em quatro partes. Primeiro nível do exercício de análise sintática.

  ////////////////// Análise Sintática Nível 2
     fp2 vet_N2_p2[1]; // Vetor para receber as frases divididas em duas partes. Segundo nível do exercício de análise sintática.
     fp3 vet_N2_p3[5]; // Vetor para receber as frases divididas em três partes. Segundo nível do exercício de análise sintática.
     fp4 vet_N2_p4[9]; // Vetor para receber as frases divididas em quatro partes. Segundo nível do exercício de análise sintática.

  ///////////////// Análise Sintática Nível 3
     fp2 vet_N3_p2[1]; // Vetor para receber as frases divididas em duas partes. Terceiro nível do exercício de análise sintática.
     fp3 vet_N3_p3[9]; // Vetor para receber as frases divididas em três partes. Terceiro nível do exercício de análise sintática.
     fp4 vet_N3_p4[5]; // Vetor para receber as frases divididas em quatro partes. Terceiro nível do exercício de análise sintática.

  //////////////// Vírgula nível 1
     fp2 vet_V1_p2[10]; // Vetor para receber as frases divididas em duas partes. Primeiro nível do exercício de estudo da vírgula.
     fp3 vet_V1_p3[18]; // Vetor para receber as frases divididas em três partes. Primeiro nível do exercício de estudo da vírgula.

  ///////////////// Vírgula nível 2
     fp2 vet_V2_p2[4]; // Vetor para receber as frases divididas em duas partes. Segundo nível do exercício de estudo da vírgula.
     fp3 vet_V2_p3[1]; // Vetor para receber as frases divididas em três partes. Segundo nível do exercício de estudo da vírgula.

/////////////////////////////////////////////// CORPO DE FUNÇÕES ////////////////////////////////////

////////////////////////////////////////// Opções iniciais ///////////////////////////////////////////
// Função que mostra na tela as opções de escolha de jogo. O jogador poderá escolher entre os exer- //
// cícios de Análise ou Vírgula.                                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////

byte tela_Opcoes(){

  byte i, opcao_Escolhida;    // Variáveis para o laço de desenho dos botões na tela (i) e para indicar qual dos botões foi pressionado na tela (opcao_Escolhida).  
  bool toque_Opcoes = false;  // Variável para indicar que houve toque na tela inicia como FALSO.
  tft.fillScreen(PRETO);      // A tela é preenchida completamente pela cor indicada em parenteses.

  char bt2[8] = {'V', char(161), 'r', 'g', 'u', 'l', 'a', '\0'};
  char bt1[8] = {'A', 'n', char(134), 'l', 'i', 's', 'e', '\0' };
  botoes[0].initButton(&tft, 160,  95, 255, 50, BRANCO, CIANO, PRETO, (char *)bt1, 2);  // O botão 0 recebe a frase "Análise".
  botoes[1].initButton(&tft, 160, 153, 255, 50, BRANCO, AZUL, PRETO, (char *)bt2, 2);   // O botão 1 recebe a frase "Vírgula".
  botoes[2].initButton(&tft, 160, 210, 255, 50, BRANCO, ROXO, PRETO, (char *)"Encerrar", 2); // O botão 2 recebe a frase "Encerrar".

  for(i = 0 ; i < 3 ; i++){  // Laço para desenhar 3 botoões na tela.
  
      botoes[i].drawButton(false); // Desenha os botões na tela e indica que nenhum deles foi pressionado ainda.
      
  }
  
  tft.fillRect(0, 0, 320, 60, BRANCO);  // Desenha um retângulo com fundo branco na parte superior da tela.
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB12_te);  // Seleciona a fonte para as letras que serão escritas na tela.
  u8g2_for_adafruit_gfx.setFontMode(1);              // Seleciona o tamanho das letras.
  u8g2_for_adafruit_gfx.setForegroundColor(PRETO);   // Seleciona a cor das letras. 
  u8g2_for_adafruit_gfx.setCursor(65,32);            // Posiciona o cursor para indicar onde começar a escrever na tela.
  u8g2_for_adafruit_gfx.print(F("ESCOLHA O EXERCÍCIO"));  // Escreve a frase "ESCOLHA O EXERCÍCIO" na tela.

  while(toque_Opcoes == false){  // Laço para esperar que o jogador selecione um dos botões na tela.
  
      toque_Opcoes = obter_Toque();  // A variável toque_opcoes recebe o retorno da função obter_Toque() para saber se a tela foi pressionada.
      opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 3); // A variável opcao_Escolhida espera retorno da função tecla_Pressionada() para saber qual botão o jogador pressionou.

      if(opcao_Escolhida == 7){   // A tela suporta até 6 botões desenhados com frases, caso o toque tenha sido fora de qualquer botão desenhado na tela, indica que o toque deve ser ignorado.

            toque_Opcoes = false;
                
      }
  
  }

  botoes[opcao_Escolhida].drawButton(true);  // Indica qual dos botões na tela foi pressionado.
  espera_Soltar();    // Função para esperar que o jogador deixe de pressionar a tela.
  botoes[opcao_Escolhida].drawButton(); // Desenha novamente o botão que foi pressionado mas agora com uma animação para mostrar que foi pressionado.

  return opcao_Escolhida; // Retorna qual foi o botão pressionado na tela.
  
}

////////////////////////////////////////// Opções de níveis ///////////////////////////////////////////
// Função que mostra na tela as opções de níveis de dificuldade para os exercícios de Análise Sintá- //
// tica e Vírgula. O exercício de Análise possui 3 níveis de dificuldade, já o exercício de Vírgula  //
// possui 2 níveis. A função recebe a qual dos dois exercícios foi selecionado nas Opções iniciais.  //
///////////////////////////////////////////////////////////////////////////////////////////////////////

byte tela_Escolha_Nivel(byte exr){

    byte i, opcao_Escolhida;   // Variáveis para o laço de desenho dos botões na tela (i) e para indicar qual dos botões foi pressionado na tela (opcao_Escolhida).  
    bool toque_Opcoes = false; // Variável para indicar que houve toque na tela inicia como FALSO.
    
    tft.fillScreen(PRETO);  // A tela é preenchida completamente pela cor indicada em parenteses.
    tft.fillRect(0, 0, 320, 60, BRANCO);  // Desenha um retângulo com fundo branco na parte superior da tela.
    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB12_te);  // Seleciona a fonte para as letras que serão escritas na tela.
    u8g2_for_adafruit_gfx.setFontMode(1);  // Seleciona o tamanho das letras.
    u8g2_for_adafruit_gfx.setCursor(80,32);  // Posiciona o cursor para indicar onde começar a escrever na tela.
    u8g2_for_adafruit_gfx.setForegroundColor(PRETO); // Seleciona a cor das letras.
    u8g2_for_adafruit_gfx.print(F("ESCOLHA SEU NÍVEL")); // Escreve a frase "ESCOLHA SEU NÍVEL" na tela.
    
    char bt1[8] = {'N', char(161), 'v', 'e', 'l', ' ', '1', '\0'};
    char bt2[8] = {'N', char(161), 'v', 'e', 'l', ' ', '2', '\0'};
    char bt3[8] = {'N', char(161), 'v', 'e', 'l', ' ', '3', '\0'};
    char bt4[7] = {'P', 'o', 'n', 't', 'o', 's', '\0'};
    
    botoes[0].initButton(&tft,  70, 110, 140, 40, BRANCO, CIANO, PRETO, (char *)bt1, 2); // O botão 0 recebe a frase "Nível 1".
    botoes[1].initButton(&tft, 240, 110, 150, 40, BRANCO, AZUL, PRETO, (char *)bt2, 2);  // O botão 1 recebe a frase "Nível 2".
    botoes[2].initButton(&tft,  70, 160, 140, 40, BRANCO, VERMELHO, PRETO, (char *)bt3, 2); // O botão 2 recebe a frase "Nível 3".
    botoes[3].initButton(&tft, 240, 160, 150, 40, BRANCO, ROXO, PRETO, (char *)"Voltar", 2); // O botão 3 recebe a frase "Voltar".
    botoes[4].initButton(&tft, 240, 210, 150, 40, BRANCO, MAGENTA, PRETO, (char *)bt4, 2); // O botão 4 recebe a frase "Pontos".

    for(i = 0 ; i < 5 ; i++){ // Laço para desenhar os botões na tela.

        if((exr == 1) && (i == 2)){ // Caso o exercício escolhido seja o de Vírgula, não deve desenhar o botão 2 na tela.
              continue;
        }

        else{
              botoes[i].drawButton(false); // Desenha um botão na tela
        }
      
    }

    while(toque_Opcoes == false){ // Laço para esperar que o jogador selecione um dos botões na tela.
  
        toque_Opcoes = obter_Toque(); // A variável toque_opcoes recebe o retorno da função obter_Toque() para saber se a tela foi pressionada.
        opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 5); // A variável opcao_Escolhida espera retorno da função tecla_Pressionada() para saber qual botão o jogador pressionou.

        if(opcao_Escolhida == 7){ // A tela suporta até 6 botões desenhados com frases, caso o toque tenha sido fora de qualquer botão desenhado na tela, indica que o toque deve ser ignorado.

            toque_Opcoes = false;
                
        }
  
    }
  
    botoes[opcao_Escolhida].drawButton(true);  // Indica qual dos botões na tela foi pressionado.
    espera_Soltar();    // Função para esperar que o jogador deixe de pressionar a tela.
    botoes[opcao_Escolhida].drawButton(); // Desenha novamente o botão que foi pressionado mas agora com uma animação para mostrar que foi pressionado.

    return opcao_Escolhida; // Retorna qual foi o botão pressionado na tela.

}

//////////////////////////////////////// Exercícios de análise sintática ///////////////////////////////
// Função que roda o jogo de Análise Sintática. Recebe o nível de dificuldade, os vetores com as fra- //
// ses dividas em 2, 3 e 4 partes e também recebe o tamanho de cada um desses vetores.                //
////////////////////////////////////////////////////////////////////////////////////////////////////////

void tela_Ex_Func_Sint(byte nivel, fp2 vet_p2[], fp3 vet_p3[], fp4 vet_p4[], byte tamanho_p2, byte tamanho_p3, byte tamanho_p4){

    byte opcao_Escolhida, j, chamado = 0, cont = 1; // Variáveis para receber qual o botão foi pressionado na tela (opcao_Escolhida), 
                                                    //rodar os laços de leitura dos vetores de frases (j), saber quantas vezes dese-
                                                    //nhamos a mesma frase na tela (chamado) e em qual frase estamos no jogo atual (cont).
    bool toque_Opcoes = false; // Variável para indicar que houve toque na tela inicia como FALSO.
    
    
        for(j = 0 ; j < tamanho_p2 ; j++){ // Laço para percorrer o vetor com as frases divididas em duas partes.

            while(chamado < 2){ // Laço para utilizar a frase atual do vetor duas vezes. Frases divididas em duas partes devem ser escritas duas vezes para o jogador analisar cada uma das partes.

                desenha_botoes_frases(cont);  // Função que desenha os botões para o jogador escolher uma opção, levando a informação de qual frase estamos.
                escreve_frases(vet_p2[j], vet_p3[0], vet_p4[0], chamado, 2, 'A'); // Chama a função que escreve a frase na tela.

                while(toque_Opcoes == false){

                    toque_Opcoes = obter_Toque();
                    opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 6);

                    if(opcao_Escolhida == 7){

                        toque_Opcoes = false;
                
                    }
  
                }
  
                botoes[opcao_Escolhida].drawButton(true);
                espera_Soltar();
                botoes[opcao_Escolhida].drawButton();

                if(opcao_Escolhida == 5){  // Caso o jogador tenha escolhido sair do nível de dificuldade atual, entra neste caso.

                    zerar_Pontos_Nivel(nivel); // Chama a função para zerar os pontos deste nível de dificuldade.

                    return;

                }

                if( ((chamado == 0) && (opcao_Escolhida == vet_p2[j].resp_1)) || ((chamado == 1) && (opcao_Escolhida == vet_p2[j].resp_2)) ){ // Compara a opção escolhida pelo jogador com as respostas corretas.

                          correto(nivel); // Caso tenha acertado, chama a função para mostrar que o jogador acertou.
                          chamado++; // Incrementa a variável que indica quantas vezes desenhamos a frase na tela.
                  
                }

                else{
                     
                          chamado += incorreto();  // Caso tenha errado, chama a função para mostrar que o jogador errou. A variável "chamado" será incrementada somente se o jogador decidir não tentar responder novamente a questão.
                
                }

                toque_Opcoes = false; // A variável para indicar toque na tela deve ser reiniciada com o valor FALSO.

           }

           cont++;  // Incrementa o contador para que indica quantas frases foram mostradas para o jogador até o momento.
           chamado = 0; // Reinicia a variável que indica quantas vezes desenhamos a frase na tela.
      }


      for(j = 0 ; j < tamanho_p3 ; j++){ // Laço para percorrer o vetor com as frases divididas em três partes.

            while(chamado < 3){ // Laço para utilizar a frase atual do vetor três vezes. Frases em três partes devem ser escritas três vezes para o jogador analisar cada uma das partes.

                  desenha_botoes_frases(cont); // Função que desenha os botões para o jogador escolher uma opção, levando a informação de qual frase estamos.
                  escreve_frases(vet_p2[0], vet_p3[j], vet_p4[0], chamado, 3, 'A'); // Chama a função que escreve a frase na tela.

                  while(toque_Opcoes == false){
  
                    toque_Opcoes = obter_Toque();
                    opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 6);

                    if(opcao_Escolhida == 7){

                        toque_Opcoes = false;
                
                    }
  
                  }
  
                  botoes[opcao_Escolhida].drawButton(true);
                  espera_Soltar();
                  botoes[opcao_Escolhida].drawButton();

                  if(opcao_Escolhida == 5){

                    zerar_Pontos_Nivel(nivel);

                    return;
                    
                  }

                  if( ((chamado == 0) && (opcao_Escolhida == vet_p3[j].resp_1)) || ((chamado == 1) && (opcao_Escolhida == vet_p3[j].resp_2)) || ((chamado == 2) && (opcao_Escolhida == vet_p3[j].resp_3)) ){ // Compara a opção escolhida pelo jogador com as respostas corretas.

                          correto(nivel); // Caso tenha acertado, chama a função para mostrar que o jogador acertou.
                          chamado++; // Incrementa a variável que indica quantas vezes desenhamos a frase na tela.
                  }

                  else{

                          chamado += incorreto(); // Caso tenha errado, chama a função para mostrar que o jogador errou. A variável "chamado" será incrementada somente se o jogador decidir não tentar responder novamente a questão.
                
                  } 

                  toque_Opcoes = false;

            }

            cont++; // Incrementa o contador para que indica quantas frases foram mostradas para o jogador até o momento.
            chamado = 0; // Reinicia a variável que indica quantas vezes desenhamos a frase na tela.
            
        }


        for(j = 0 ; j < tamanho_p4 ; j++){  // Laço para percorrer o vetor com as frases divididas em quatro partes.

            while(chamado < 4){ // Laço para utilizar a frase atual do vetor quatro vezes. Frases em quatro partes devem ser escritas quatro vezes para o jogador analisar cada uma das partes.

                  desenha_botoes_frases(cont);
                  escreve_frases(vet_p2[0], vet_p3[0], vet_p4[j], chamado, 4, 'A');

                  while(toque_Opcoes == false){
  
                    toque_Opcoes = obter_Toque();
                    opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 6);

                    if(opcao_Escolhida == 7){

                        toque_Opcoes = false;
                
                    }
  
                  }
  
                  botoes[opcao_Escolhida].drawButton(true);
                  espera_Soltar();
                  botoes[opcao_Escolhida].drawButton();

                  if(opcao_Escolhida == 5){

                    zerar_Pontos_Nivel(nivel);

                    return;
                    
                  }

                  if( ((chamado == 0) && (opcao_Escolhida == vet_p4[j].resp_1)) || ((chamado == 1) && (opcao_Escolhida == vet_p4[j].resp_2)) || ((chamado == 2) && (opcao_Escolhida == vet_p4[j].resp_3)) || ((chamado == 3) && (opcao_Escolhida == vet_p4[j].resp_4)) ){

                          correto(nivel);
                          chamado++;
                  }

                  else{

                          chamado += incorreto();
                
                  } 

                  toque_Opcoes = false;

            }

            cont ++;
            chamado = 0;
        }

    return;
        
}

/////////////////////////////////////// Exercício de estudo de vírgulas ///////////////////////////////
// Função para rodar o exercício de Vírgula. Recebe o nível de dificuldade, os vetores com as frases //
// dividas em 2, 3 e 4 partes e também recebe o tamanho de cada um desses vetores.                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////

void tela_Ex_Virg(byte nivel, fp2 vet_p2[], fp3 vet_p3[], fp4 vet_p4[], byte tamanho_p2, byte tamanho_p3){
    
    byte opcao_Escolhida, j = 0, cont = 1, total; // Variáveis para receber qual o botão foi pressionado na tela (opcao_Escolhida), 
                                                  // rodar os laços de leitura dos vetores de frases (j), em qual frase estamos
                                                  // no jogo atual (cont) e variável de indicação do total de frases que o nível de
                                                  // de dificuldade tem. O nível 1 possui 28 frases e o nível 2 possui 5 frases.
    bool toque_Opcoes = false; // Variável para indicar que houve toque na tela inicia como FALSO.
    
    if(nivel == 0){ // Caso o nível de dificuldade seja o 1
      total = 28;  // indica que o total de frases será 28.
    }

    else{  // Caso contrário,
      total = 5; // indica que o total de frases será 5.
    }

    nivel = nivel + 3;  // A variável "nivel" é incrementada em 3 para indicar em que estamos no exercício de Vírgula e em qual nível.
                        // Essa variável é usada para indicar para a função que incrementa os pontos de todos os jogos, qual ponto de-
                        // ve ser incrementado.

    while(j < tamanho_p2){ // Laço para percorrer o vetor com as frases divididas em duas partes.

      desenha_botoes_virgula(cont, total); // Chama a função que escreve os botões na tela.
      escreve_frases(vet_p2[j], vet_p3[0], vet_p4[0], vet_p2[j].resp_2, 2, 'V'); // Chama a função que escreve frases na tela.

      while(toque_Opcoes == false){ // Laço para esperar que o jogador selecione um dos botões na tela.
  
        toque_Opcoes = obter_Toque(); // A variável toque_opcoes recebe o retorno da função obter_Toque() para saber se a tela foi pressionada.
        opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 3); // Função para esperar que o jogador deixe de pressionar a tela.

        if(opcao_Escolhida == 7){ // A tela suporta até 6 botões desenhados com frases, caso o toque tenha sido fora de qualquer botão desenhado na tela, indica que o toque deve ser ignorado.

            toque_Opcoes = false;
                
        }
  
      }
  
      botoes[opcao_Escolhida].drawButton(true); // Indica qual dos botões na tela foi pressionado.
      espera_Soltar();  // Função para esperar que o jogador deixe de pressionar a tela.
      botoes[opcao_Escolhida].drawButton(); // Desenha novamente o botão que foi pressionado mas agora com uma animação para mostrar que foi pressionado.

      if(opcao_Escolhida == 2){ // Caso o jogador tenha escolhido sair do nível de dificuldade atual, entra neste caso.

          zerar_Pontos_Nivel(nivel); // Chama a função para zerar os pontos deste nível de dificuldade.

          return;
      }

      if(opcao_Escolhida == vet_p2[j].resp_1){ // Compara a opção escolhida pelo jogador com as respostas corretas.

          correto(nivel); // Caso tenha acertado, chama a função para mostrar que o jogador acertou.
          j++;  // Incrementa a variável de leitura do vetor de frases.
          cont++; // Incrementa o contador que indica em qual das frases do total de frases do nível o jogador está.

      }

      else{

          if(incorreto() == 1){ // Caso tenha errado, chama a função que mostra ao jogador que ele errou e verifica se o jogador escolheu não tentar responder novamente a mesma questão.

              j++; // Incrementa a variável de leitura do vetor de frases.
              cont++; // Incrementa o contador que indica em qual das frases do total de frases do nível o jogador está.
          }
                
      } 

      toque_Opcoes = false;
            
    }
    
    j = 0; // A variável que percorre os vetores é reinicializada.

    while(j < tamanho_p3){ // Laço para percorrer o vetor com as frases divididas em duas partes.

        desenha_botoes_virgula(cont, total);
        escreve_frases(vet_p2[0], vet_p3[j], vet_p4[0], vet_p3[j].resp_2, 3, 'V');

        while(toque_Opcoes == false){
  
          toque_Opcoes = obter_Toque();
          opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 3);

          if(opcao_Escolhida == 7){

            toque_Opcoes = false;

          }
  
        }
  
        botoes[opcao_Escolhida].drawButton(true);
        espera_Soltar();
        botoes[opcao_Escolhida].drawButton();

        if(opcao_Escolhida == 2){

          zerar_Pontos_Nivel(nivel);

          return;
        }

        if( opcao_Escolhida == vet_p3[j].resp_1 ){

            correto(nivel);
            j++;
            cont++;

        }

        else{

            if(incorreto() == 1){

              j++;
              cont++;

            }
                
        } 

        toque_Opcoes = false;

    }


    return;
}

/////////////////////////////////////////// Obtém toque na tela ////////////////////////////////////////
// Função que verifica onde o jogador pressionou a tela.                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////////

bool obter_Toque(){
  
    TSPoint p = ts.getPoint(); // Objeto para receber o toque na tela.
    pinMode(YP, OUTPUT);       // Indica que o pino da variável YP funciona como saída de informação.
    pinMode(XM, OUTPUT);       // Indica que o pino da variável XM funciona como saída de informação.
    digitalWrite(YP, HIGH);    // Passa para o pino da variável YP o valor ALTO.
    digitalWrite(XM, HIGH);    // Passa para o pino da variável XM o valor ALTO.
    bool pressed = (p.z > MINPRESSURE); // A variável "pressed" recebe VERDADEIRO se o nível de pressão na tela for maior do que 
                                        // o valor mínimo indicado no início do programa, caso contrário recebe FALSO.
    
    if (pressed) { // Se a variável "pressed" for verdadeira, indica que o jogador pressionou a tela.
      
          posX = map(p.y, TS_LEFT, TS_RT, 0, 320); // A variável "posX" recebe qual o ponto foi pressionado na tela em relação ao topo da tela.
          posY = map(p.x, TS_TOP, TS_BOT , 0, 240); // A variável "posY" recebe qual o ponto foi pressionado na tela em relação lado direito da tela.
    
          return true; // Caso algum ponto dentro do desenho da tela tenha sido pressionado, indica que houve toque.
        
    }
    
    return false; // Caso contrário, indica que não houve toque na tela.
}

/////////////////////////////// Busca qual parte da tela foi pressionada ///////////////////////////////
// Função que percorre os botões desenhados na tela e indica qual foi pressionado. Retorna o número   //
// botão pressionado.                                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////

byte tecla_Pressionada(bool teclaTocada, byte total_Botoes){
  
  for (byte i = 0; i < total_Botoes ; i++){ // Laço para percorrer os botões desenhados na tela.
    
      botoes[i].press(teclaTocada && botoes[i].contains(posX,posY)); // O botão receberá a indicação de que foi pressonado 
                                                                     // (VERDADEIRO) caso a variável "teclaTocada" seja verdadeira
                                                                     // e o jogador tenha pressionado a tela dentro em um local válido.
    
  }
  
  return checa_Tecla_Pressionada(total_Botoes);
  
}

////////////////////////////// Checa qual tecla foi pressionada na tela ////////////////////////////////
// Função que percorre os botões desenhados na tela e busca qual deles foi pressionado. Retorna qual  //
// o número do botão.                                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////

byte checa_Tecla_Pressionada(byte num_botoes){
  
  for (byte i = 0 ; i < num_botoes ; i++){ // Laço para percorrer todos os botões desenhados na tela.
    
      if (botoes[i].justPressed()){ // Caso o botão esteja sendo pressionado, 
      
          return i;  // retorna o número do botão.
      
      }
    
  }

  return 7; // Caso contrário, retorna o valor que indica que a tela foi pressionada em um local em que não há botão desenhado.
  
}

////////////////////////////// Espera um tempo para a tecla ser solta //////////////////////////////////////
// Função que espera o jogador deixar de pressionar a tela.                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void espera_Soltar(){
  
  while (obter_Toque()){  // Laço que chama a função obter_Toque(), irá terminar quando o jogador soltar a tela.
    
    delayMicroseconds(100); // Espera 100 microsegundos antes de fazder um novo teste para ver se o jogador parou de pressonar a tela.
    
  }
  
}

///////////////////////////////////// Tela de opção correta /////////////////////////////////////////////////
// Função que mostra que a opção marcada pelo jogador estava correta. Recebe uma variável que indica se o  //
// jogador estava no nível 1, 2 ou 3 do exercício de Análise, ou se estava no nível 1 ou 2 do exercício de //
// Vírgula para poder incrementar a pontuação do jogador.                                                  //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void correto(byte op){
            
            tft.fillScreen(PRETO); // A tela é preenchida completamente pela cor indicada em parenteses.
            tft.setCursor(20,100);  // Posiciona o cursor no local da tela onde a frase vai ser escrita.
            tft.setTextColor(VERDE); // Seleciona a cor das letras.
            tft.setTextSize(3); // Seleciona o tamanho das letras.
            tft.print(F("Resposta Correta")); // Escreve na tela a frase "Resposta Correta".
            delay(500);  // Espera 0,5 segundos antes de tirar a frase da tela.

            switch (op){

                case 0: // Caso o jogador esteja no nível 1 do exercício de Análise Sintática
                
                    pontos_N_1++;
                    
                break;


                case 1:
                
                    pontos_N_2++; // Caso o jogador esteja no nível 2 do exercício de Análise Sintática
                    
                break;


                case 2: // Caso o jogador esteja no nível 3 do exercício de Análise Sintática
                
                    pontos_N_3++;
                    
                break;


                case 3: // Caso o jogador esteja no nível 1 do exercício de Vírgula.
                
                    pontos_Virgula_N_1++;
                    
                break;


                case 4: // Caso o jogador esteja no nível 2 do exercício de Vírgula.
                
                    pontos_Virgula_N_2++;
                    
                break;


                default:

                break;
            
            }
            
}

//////////////////////////////////// Tela de opção incorreta ////////////////////////////////////////////////
// Função que mostra que a opção escolhida pelo jogador estava incorreta.                                  //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

byte incorreto(){

            byte opcao_Escolhida;  // Variável para receber a opção que o jogador vai escolher para repetir ou não a questão que ele errou.
            bool toque_Opcoes = false;

            tft.fillScreen(PRETO);
            tft.setCursor(50, 40);
            tft.setTextColor(VERMELHO);
            tft.setTextSize(2);
            tft.print(F("Resposta Incorreta"));

            char bt[8] = {'P', 'r', char(162), 'x', 'i', 'm', 'a', '\0'};
            botoes[0].initButton(&tft, 160, 100, 240, 40, BRANCO, AZUL, PRETO, (char *)"Repetir", 2); // O botão 0 recebe a frase "Repetir"
            botoes[1].initButton(&tft, 160, 158, 240, 40, BRANCO, AMARELO, PRETO, (char *)bt, 2); // O botão 1 recebe a frase "Próxima"
  
            for (byte i = 0 ; i < 2 ; i++){
                
                botoes[i].drawButton(false);
           
            }

            while(toque_Opcoes == false){
  
                toque_Opcoes = obter_Toque();
                opcao_Escolhida = tecla_Pressionada(toque_Opcoes, 2);

                if(opcao_Escolhida == 7){

                    toque_Opcoes = false;
                
                }
  
            }

            botoes[opcao_Escolhida].drawButton(true);
            espera_Soltar();
            botoes[opcao_Escolhida].drawButton();

            return opcao_Escolhida;
            
}

/////////////////////////////////// Tela de encerramentor /////////////////////////////////////////////////////
// Função que encerra o jogo. Escreve na tela a pontuação alcançada em cada um dos níveis de cada exercício. //
// Para sair, o jogador deve pressionar em qualquer lugar dentro da tela.                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void encerrar(){
    
    //byte i;
    bool toque_Opcoes = false;
    
    tft.fillScreen(BRANCO);
    tft.setCursor(25,10);
    tft.setTextColor(AZUL);
    tft.setTextSize(3);
    tft.println(F("TOTAL DE PONTOS"));

    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB12_te);  // extended font
    u8g2_for_adafruit_gfx.setFontMode(1);
    u8g2_for_adafruit_gfx.setCursor(70, 50);
    u8g2_for_adafruit_gfx.setForegroundColor(PRETO);
    u8g2_for_adafruit_gfx.print(F("ANÁLISE SINTÁTICA"));

    u8g2_for_adafruit_gfx.setCursor(100, 80);
    u8g2_for_adafruit_gfx.setForegroundColor(VERMELHO);
    u8g2_for_adafruit_gfx.print(F("Nível 1: "));

    u8g2_for_adafruit_gfx.setForegroundColor(MAGENTA);
    u8g2_for_adafruit_gfx.print(pontos_N_1);
    u8g2_for_adafruit_gfx.setForegroundColor(AZUL);
    u8g2_for_adafruit_gfx.println("/47");

    u8g2_for_adafruit_gfx.setCursor(100, 110);
    u8g2_for_adafruit_gfx.setForegroundColor(VERMELHO);
    u8g2_for_adafruit_gfx.print(F("Nível 2: "));

    u8g2_for_adafruit_gfx.setForegroundColor(MAGENTA);
    u8g2_for_adafruit_gfx.print(pontos_N_2);
    u8g2_for_adafruit_gfx.setForegroundColor(AZUL);
    u8g2_for_adafruit_gfx.println("/53");

    u8g2_for_adafruit_gfx.setCursor(100, 140);
    u8g2_for_adafruit_gfx.setForegroundColor(VERMELHO);
    u8g2_for_adafruit_gfx.print(F("Nível 3: "));

    u8g2_for_adafruit_gfx.setForegroundColor(MAGENTA);
    u8g2_for_adafruit_gfx.print(pontos_N_3);
    u8g2_for_adafruit_gfx.setForegroundColor(AZUL);
    u8g2_for_adafruit_gfx.println("/45");

    u8g2_for_adafruit_gfx.setCursor(70, 170);
    u8g2_for_adafruit_gfx.setForegroundColor(PRETO);
    u8g2_for_adafruit_gfx.print(F("ESTUDO DA VÍRGULA"));
    
    u8g2_for_adafruit_gfx.setCursor(100, 200);
    u8g2_for_adafruit_gfx.setForegroundColor(VERMELHO);
    u8g2_for_adafruit_gfx.print(F("Nível 1: "));

    u8g2_for_adafruit_gfx.setForegroundColor(MAGENTA);
    u8g2_for_adafruit_gfx.print(pontos_Virgula_N_1);
    u8g2_for_adafruit_gfx.setForegroundColor(AZUL);
    u8g2_for_adafruit_gfx.println("/25");
    
    u8g2_for_adafruit_gfx.setCursor(100, 230);
    u8g2_for_adafruit_gfx.setForegroundColor(VERMELHO);
    u8g2_for_adafruit_gfx.print(F("Nível 2: "));

    u8g2_for_adafruit_gfx.setForegroundColor(MAGENTA);
    u8g2_for_adafruit_gfx.print(pontos_Virgula_N_2);
    u8g2_for_adafruit_gfx.setForegroundColor(AZUL);
    u8g2_for_adafruit_gfx.println("/4");


    while(toque_Opcoes == false){
  
              toque_Opcoes = obter_Toque();
    }
  
}

/////////////////////////// Escreve frases dos exercícios na tela ////////////////////////////////////////////
// Função que recebe os vetores com as frases divididas em 2, 3 e 4 partes, recebe quantas vezes já escreve //
// a mesma frase na tela, recebe o tamanho do vetor e se é o exercício de Análise ou de Vírgula.            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void escreve_frases(fp2 p2, fp3 p3, fp4 p4, byte chamada, byte tamanho, char exercicio){

    byte i, j, cont = 37; // Variáveis para percorrer o vetor de frases (i e j) e variável para controlar se devemos começar 
                          // a escrever na próxima linha (cont).
    int cor[4]; // Vetor que guarda as cores das letras de cada parte da frase.
    String frase[4]; //quebra_de_linha; // Vetor para guardar as partes da frase que vai ser escrita na tela (frase[4]).
    int opcao_Cor; // Variável que indica qual cor será utilizada para realçar parte da frase escrita na tela.

    if(exercicio == 'A'){ // Caso o exercício seja o de Análise

        opcao_Cor = AZUL; // a cor de realce é a azul.

    }

    else{ // Caso o exercício seja o de Vírgula

        opcao_Cor = MAGENTA; // a cor de realce é o magenta.

    }
    
    tft.fillRect(0, 0, 320, 70, BRANCO); 
    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB12_te);
    u8g2_for_adafruit_gfx.setFontMode(1);
    u8g2_for_adafruit_gfx.setCursor(1,18);

    if(chamada == 0){ // Caso a frase esteja sendo escrita na tela pela primeira vez, a primeira parte recebe a cor de realce.
                      // As outras partes ficam na cor preta.
        cor[0] = opcao_Cor;
        cor[1] = PRETO;
        cor[2] = PRETO;
        cor[3] = PRETO;
    }

    else if(chamada == 1){ // Caso a frase esteja sendo escrita na tela pela segunda vez, a segunda parte recebe a cor de realce.
                           // As outras partes ficam na cor preta.
        cor[0] = PRETO;
        cor[1] = opcao_Cor;
        cor[2] = PRETO;
        cor[3] = PRETO;
    }

    else if(chamada == 2){ // Caso a frase esteja sendo escrita na tela pela terceira vez, a terceira parte recebe a cor de realce.
                           // As outras partes ficam na cor preta.
        cor[0] = PRETO;
        cor[1] = PRETO;
        cor[2] = opcao_Cor;
        cor[3] = PRETO;
    }

    else{ // Caso a frase esteja sendo escrita na tela pela quarta vez, a quarta parte recebe a cor de realce.
          // As outras partes ficam na cor preta.
        cor[0] = PRETO;
        cor[1] = PRETO;
        cor[2] = PRETO;
        cor[3] = opcao_Cor;
    }

    if(tamanho == 2){ // Caso a frase que vai ser escrita na tela seja dividida em duas partes,
        frase[0] = p2.parte_1; // o vetor que recebe as partes da frase recebe a primeira parte
        frase[1] = p2.parte_2; // e a segunda parte.
    }

    else if(tamanho == 3){ // Caso a frase que vai ser escrita na tela seja dividida em três partes,
        frase[0] = p3.parte_1; // o vetor que recebe as partes da frase recebe a primeira parte,
        frase[1] = p3.parte_2; // a segunda parte
        frase[2] = p3.parte_3; // e a terceira parte.
    }

    else{ // Caso a frase que vai ser escrita na tela seja dividida em quatro partes,
        frase[0] = p4.parte_1; // o vetor que recebe as partes da frase recebe a primeira parte,
        frase[1] = p4.parte_2; // a segunda parte,
        frase[2] = p4.parte_3; // a terceira parte
        frase[3] = p4.parte_4; // e a quarta parte.
    }

    for(j = 0 ; j < tamanho ; j++){ // Laço para percorrer o vetor com as partes da frase.
        u8g2_for_adafruit_gfx.setForegroundColor(cor[j]); // Seleciona a cor indicada para as letras da parte da frase.
        
        for(i = 0 ; i < frase[j].length() ; i++){ // Laço para percorre a parte da frase que está sendo escrita na tela no momento.
            if(u8g2_for_adafruit_gfx.getCursorX() > 310){ // Verifica se a frase vai ultrapassar do limite esquerdo da tela.
                u8g2_for_adafruit_gfx.print('\n');  // Caso vá, vai para a próxima linha.
                if(frase[j][i] != ' '){  // Caso o que vá ser escrito na tela após pular a linha não seja um espaço " ",
                    u8g2_for_adafruit_gfx.print(frase[j][i]); // escreve a letra na tela
                }
            }

            else{
                u8g2_for_adafruit_gfx.print(frase[j][i]); // caso contrário, não é necessário escrever o espaço na tela.
            }
        }
    }
}

////////////////////// Desenhar as opções nos exercícios de Análise Sintática /////////////////////////////////
// Função que desenha os botões dos exercícios de Análise Sintártica. As opções são: Sujeito (Suj.), Comple- //
// mento (Compl.), Verbo (Verb.), Predicativo (Pred.), Adjunto Adverbial (A Adv.) e Sair. A função recebe em //
//  qual frase do total de frases do nível de dificuldade o jogador está atualmente.                         //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void desenha_botoes_frases(byte num){

           tft.fillScreen(PRETO);
           botoes[0].initButton(&tft,  70, 110, 140, 40, BRANCO, VERMELHO, PRETO, (char *)"Suj.", 2);
           botoes[1].initButton(&tft, 240, 110, 150, 40, BRANCO, ROXO, PRETO, (char *)"Verb.", 2);
           botoes[2].initButton(&tft,  70, 160, 140, 40, BRANCO, VERDE, PRETO, (char *)"Compl.", 2);
           botoes[3].initButton(&tft, 240, 160, 150, 40, BRANCO, AMARELO, PRETO, (char *)"Pred.", 2);
           botoes[4].initButton(&tft,  70, 210, 140, 40, BRANCO, CIANO, PRETO, (char *)"A Adv.", 2);
           botoes[5].initButton(&tft, 200, 210, 75, 40, BRANCO, MAGENTA, PRETO, (char *)"Sair", 2);
  
           for (byte i = 0 ; i < 6 ; i++){
                
                botoes[i].drawButton(false);
           
           }

           tft.setCursor(255,210);
           tft.setTextColor(BRANCO);
           tft.setTextSize(2);
           tft.print(num);   // Escreve na tela o número da frase que o jogador está no momento.
           tft.print("/15"); // Escreve o total de frases que o nível de dificuldade tem.
           
}


//////////////////////////// Desenhar as opções nos exercícios de Vírgula /////////////////////////////////////
// Função que desenha os botões dos exercícios de Vírgula. As opções são: Sem vírgula, Com vírgula. A função //
// recebe em qual frase do total de frases do nível de dificuldade o jogador está atualmente e qual o total  //
// de frases o nível possui.                                                                                 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void desenha_botoes_virgula(byte num, byte total){
            
           tft.fillScreen(PRETO);
           char bt[12] = {'S', 'e', 'm', ' ', 'v', char(161), 'r', 'g', 'u', 'l', 'a','\0'};
           char bt2[12] = {'C', 'o', 'm', ' ', 'v', char(161), 'r', 'g', 'u', 'l', 'a','\0'};
           botoes[0].initButton(&tft, 160, 105, 255, 50, BRANCO, AZUL, PRETO, (char *)bt2, 2);
           botoes[1].initButton(&tft, 160, 158, 255, 50, BRANCO, AMARELO, PRETO, (char *)bt, 2);
           botoes[2].initButton(&tft, 160, 211, 150, 50, BRANCO, VERMELHO, PRETO, (char *)"Sair", 2);
  
           for (byte i = 0 ; i < 3 ; i++){
                
                botoes[i].drawButton(false);
           
           }

           tft.setCursor(250,210);
           tft.setTextColor(BRANCO);
           tft.setTextSize(2);
           tft.print(num); // Escreve na tela o número da frase que o jogador está no momento.
           tft.print("/");
           tft.print(total); // Escreve o total de frases que o nível de dificuldade tem.

}

///////////////////////////////////// Tela de apresentação ////////////////////////////////////////////////////////
// Função que roda quando a tela e ligada. Escreve na tela a palavra "GramaticBô" junto com um rosto sorridente. //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tela_Inicial(){


    tft.fillScreen(BRANCO);
    tft.setCursor(50,100);
    tft.setTextColor(PRETO);
    tft.setTextSize(3);
    tft.print(F("GramaticB"));
    tft.write(147);
    tft.print(" ");
    tft.setTextColor(VERMELHO);
    tft.write(2);
    delay(5000);
    
}

/////////////////////////////////// Zera os pontos do nível de um exercício ////////////////////////////////////////
// Função que zera a pontuação de um nível específico dos exercícios caso o jogador escolha a opção "Sair" duran- //
// o exercício.                                                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zerar_Pontos_Nivel(byte nivel){

    switch (nivel){

        case 0:

            pontos_N_1 = 0;

        break;

        case 1:

            pontos_N_2 = 0;

        break;

        case 2:

            pontos_N_3 = 0;

        break;

        case 3:

            pontos_Virgula_N_1 = 0;

        break;

        case 4:

            pontos_Virgula_N_2 = 0;

        break;

        default:

        break;

    }

}

///////////////// Salva os enunciados nos endereços da memória EEPROM de forma sequencial ////////////////////////////
// Função para salvar os enunciados dos exercícios na memória EEPROM da placa Arduino. Esse recurso é utilizado pa- //
// ra que a memória principal da placa seja utilizada somente para rodar o jogo, economizando memória RAM.          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void escreve_String(int enderecoBase, String mensagem){

  if (mensagem.length() > EEPROM.length() || (enderecoBase+mensagem.length()) > EEPROM.length() ){ // verificamos se a string cabe na memória a partir do endereço desejado
      tft.fillScreen(PRETO);
      tft.setCursor(20,100);
      tft.setTextColor(VERDE);
      tft.setTextSize(3);
      tft.print(F("EEPROM cheia!"));
      delay(500);
  }

  else{ // Caso seja possível armazenar 

    for (int i = 0 ; i < mensagem.length() ; i++){ 

       EEPROM.update(enderecoBase, mensagem[i]); // Escrevemos cada byte da string de forma sequencial na memória
       enderecoBase++; // Deslocamos endereço base em uma posição a cada byte salvo

    }

    EEPROM.write(enderecoBase,'\0'); // Salvamos marcador de fim da string 

  }

}

///////////// Se o endereço base for maior que o espaço de endereçamento da EEPROM, retornamos uma string vazia ////

String le_String(int enderecoBase){
  String mensagem="";
  if (enderecoBase>EEPROM.length()){
    return mensagem;
  }
  else { // Caso contrário, lemos byte a byte de cada endereço e montamos uma nova String
    char pos;
    do{
      pos = EEPROM.read(enderecoBase); // Leitura do byte com base na posição atual
      enderecoBase++; // A cada leitura incrementamos a posição a ser lida
      mensagem = mensagem + pos; // Montamos string de saídaa
    }
    while (pos != '\0'); // Fazemos isso até encontrar o marcador de fim de string
  }
  return mensagem; // Retorno da mensagem
}

//////////////////////////////////// Escrever o enunciado da questão ///////////////////////////
// Função que busca na memória EPPROM o enunciado da questão escolhida pelo jogador e escreve //
// na tela.                                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////

void escreve_Enunciado(int endereco_Base){

    bool toque_Opcoes = false;
    String mensagem = le_String(endereco_Base);
    byte cont = 0;

    tft.fillScreen(PRETO);
    tft.fillRect(0, 0, 320, 70, BRANCO);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB12_te);  // extended font
    u8g2_for_adafruit_gfx.setFontMode(1);
    u8g2_for_adafruit_gfx.setCursor(1,18);
    
    for(byte i = 0 ; i < mensagem.length() ; i++){
        if(u8g2_for_adafruit_gfx.getCursorX() > 310){
                u8g2_for_adafruit_gfx.print('\n');
                if(mensagem[i] != ' '){
                    u8g2_for_adafruit_gfx.print(mensagem[i]);
                }
            }

        else{
            u8g2_for_adafruit_gfx.print(mensagem[i]);
        }      
    }

    while(toque_Opcoes == false){
  
              toque_Opcoes = obter_Toque();
    }
}

//////////////////////////////////////////// SETUP ///////////////////////////////////////////////////
// Função principal da Arduino, a inicialização de pinos e objetos ocorrem nesta função. Os vetores //
// com as partes das frases são iniciados aqui.                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  
  Serial.begin(9600);  // Método que inicia a comunicação da plac Arduino.
  uint16_t ID = tft.readID();  // Leitura do identificador da tela TFT.
  tft.begin(ID);               // Inicialização da tela.
  tft.setRotation(ROTATION);   // Configura a orientação da tela para o modo paisagem.
  u8g2_for_adafruit_gfx.begin(tft); // O objeto da biclioteca com os caracteres latinos recebe o objeto da tela TFT.
  tela_Inicial();  // Chama a função da tela de apresentação do jogo.

  escreve_String(0, F("Indique a função sintática do termo destacado em azul."));  // O enunciado do exercício de Análise Sintática é salvo na memória EPPROM.
  escreve_String(60, F("Indique se o termo destacado em rosa deve ser separado por vírgula.")); // O enunciado do exercício de Vírgula é salvo na memória EPPROM.

  //////////////// Nível 1
  // Início dos vetores com as frases divididas em duas partes para o exercício de Análise Sintática.

  vet_N1_p2[0].parte_1 = F("Clarisse "); // Primeira parte da frase. // N1 - 1
  vet_N1_p2[0].parte_2 = F("chorou."); // Segunda parte da frase.
  vet_N1_p2[0].resp_1 = 0;  // Resposta da primeira parte.
  vet_N1_p2[0].resp_2 = 1;  // Resposta da segunda parte.

  vet_N1_p2[1].parte_1 = F("Choveu "); // N1 - 2
  vet_N1_p2[1].parte_2 = F("ontem.");
  vet_N1_p2[1].resp_1 = 1;
  vet_N1_p2[1].resp_2 = 4;

  // Início dos vetores com as frases divididas em três partes para o exercício de Análise Sintática.

  vet_N1_p3[0].parte_1 = F("Cristiane "); // Primeira parte da frase. // N1 - 3
  vet_N1_p3[0].parte_2 = F("gosta ");     // Segunda parte da frase.
  vet_N1_p3[0].parte_3 = F("de sair.");   // Terceira parte da frase.
  vet_N1_p3[0].resp_1 = 0;  // Resposta da primeira parte.
  vet_N1_p3[0].resp_2 = 1;  // Resposta da segunda parte.
  vet_N1_p3[0].resp_3 = 2;  // Resposta da terceira parte.

  vet_N1_p3[1].parte_1 = F("Ele ");      // N1 - 4
  vet_N1_p3[1].parte_2 = F("comprou ");
  vet_N1_p3[1].parte_3 = F("o jeans.");
  vet_N1_p3[1].resp_1 = 0;
  vet_N1_p3[1].resp_2 = 1;
  vet_N1_p3[1].resp_3 = 2;

  vet_N1_p3[2].parte_1 = F("Os gatos ");  // N1 - 5
  vet_N1_p3[2].parte_2 = F("subiam ");
  vet_N1_p3[2].parte_3 = F("nos móveis.");
  vet_N1_p3[2].resp_1 = 0;
  vet_N1_p3[2].resp_2 = 1;
  vet_N1_p3[2].resp_3 = 4;

  vet_N1_p3[3].parte_1 = F("A terra e as plantas "); // N1 - 6
  vet_N1_p3[3].parte_2 = F("estavam ");
  vet_N1_p3[3].parte_3 = F("úmidas.");
  vet_N1_p3[3].resp_1 = 0;
  vet_N1_p3[3].resp_2 = 1;
  vet_N1_p3[3].resp_3 = 3;

  vet_N1_p3[4].parte_1 = F("Poetas ");          // N1 - 7
  vet_N1_p3[4].parte_2 = F("escrevem ");
  vet_N1_p3[4].parte_3 = F("belos textos.");
  vet_N1_p3[4].resp_1 = 0;
  vet_N1_p3[4].resp_2 = 1;
  vet_N1_p3[4].resp_3 = 2;

  vet_N1_p3[5].parte_1 = F("As crianças ");         // N1 - 8
  vet_N1_p3[5].parte_2 = F("comeram ");
  vet_N1_p3[5].parte_3 = F("o bolo de chocolate.");
  vet_N1_p3[5].resp_1 = 0;
  vet_N1_p3[5].resp_2 = 1;
  vet_N1_p3[5].resp_3 = 2;

  vet_N1_p3[6].parte_1 = F("As árvores ");          // N1 - 9
  vet_N1_p3[6].parte_2 = F("balaçam ");
  vet_N1_p3[6].parte_3 = F("fortemente.");
  vet_N1_p3[6].resp_1 = 0;
  vet_N1_p3[6].resp_2 = 1;
  vet_N1_p3[6].resp_3 = 4;

  vet_N1_p3[7].parte_1 = F("Essa música ");       // N1 - 10
  vet_N1_p3[7].parte_2 = F("é ");
  vet_N1_p3[7].parte_3 = F("maravilhosa.");
  vet_N1_p3[7].resp_1 = 0;
  vet_N1_p3[7].resp_2 = 1;
  vet_N1_p3[7].resp_3 = 3;

  vet_N1_p3[8].parte_1 = F("O amor ");              // N1 - 11
  vet_N1_p3[8].parte_2 = F("comeu ");
  vet_N1_p3[8].parte_3 = F("meu nome.");
  vet_N1_p3[8].resp_1 = 0;
  vet_N1_p3[8].resp_2 = 1;
  vet_N1_p3[8].resp_3 = 2;

  // Início dos vetores com as frases divididas em quatro partes para o exercício de Análise Sintática.

  vet_N1_p4[0].parte_1 = F("Mariana e Felipe "); // Primeira parte da frase.     // N1 - 12
  vet_N1_p4[0].parte_2 = F("entregaram "); // Segunda parte da frase.
  vet_N1_p4[0].parte_3 = F("os convites de casamento "); // Terceira parte da frase.
  vet_N1_p4[0].parte_4 = F("aos familiares."); // Quarta parte da frase.
  vet_N1_p4[0].resp_1 = 0; // Resposta da primeira parte.
  vet_N1_p4[0].resp_2 = 1; // Resposta da segunda parte.
  vet_N1_p4[0].resp_3 = 2; // Resposta da terceira parte.
  vet_N1_p4[0].resp_4 = 2; // Resposta da quarta parte.

  vet_N1_p4[1].parte_1 = F("Eu e Helena ");     // N1 - 13
  vet_N1_p4[1].parte_2 = F("vamos ");
  vet_N1_p4[1].parte_3 = F("ao cinema ");
  vet_N1_p4[1].parte_4 = F("mais tarde.");
  vet_N1_p4[1].resp_1 = 0;
  vet_N1_p4[1].resp_2 = 1;
  vet_N1_p4[1].resp_3 = 4;
  vet_N1_p4[1].resp_4 = 4;

  vet_N1_p4[2].parte_1 = F("Igor ");              // N1 - 14
  vet_N1_p4[2].parte_2 = F("assiste ");
  vet_N1_p4[2].parte_3 = F("aos jogos de futebol ");
  vet_N1_p4[2].parte_4 = F("todos os domingos.");
  vet_N1_p4[2].resp_1 = 0;
  vet_N1_p4[2].resp_2 = 1;
  vet_N1_p4[2].resp_3 = 2;
  vet_N1_p4[2].resp_4 = 4;

  vet_N1_p4[3].parte_1 = F("Pedro e Paula ");       // N1 - 15
  vet_N1_p4[3].parte_2 = F("jantaram ");
  vet_N1_p4[3].parte_3 = F("juntos ");
  vet_N1_p4[3].parte_4 = F("ontem.");
  vet_N1_p4[3].resp_1 = 0;
  vet_N1_p4[3].resp_2 = 1;
  vet_N1_p4[3].resp_3 = 4;
  vet_N1_p4[3].resp_4 = 4;
  
  ////////////////////////////// Nível 2
  // Início dos vetores com as frases divididas em duas partes para o exercício de Análise Sintática.
  vet_N2_p2[0].parte_1 = F("Carregamos ");             // N2 - 1
  vet_N2_p2[0].parte_2 = F("os móveis da mudança.");
  vet_N2_p2[0].resp_1 = 1;
  vet_N2_p2[0].resp_2 = 2;

  // Início dos vetores com as frases divididas em três partes para o exercício de Análise Sintática.
  vet_N2_p3[0].parte_1 = F("Sem olhar para trás, "); // N2 - 2
  vet_N2_p3[0].parte_2 = F("Alice ");
  vet_N2_p3[0].parte_3 = F("correu.");
  vet_N2_p3[0].resp_1 = 4;
  vet_N2_p3[0].resp_2 = 0;
  vet_N2_p3[0].resp_3 = 1;
  
  vet_N2_p3[1].parte_1 = F("Durante as manhãs, "); // N2 - 3
  vet_N2_p3[1].parte_2 = F("cantavam ");
  vet_N2_p3[1].parte_3 = F("os passarinhos.");
  vet_N2_p3[1].resp_1 = 4;
  vet_N2_p3[1].resp_2 = 1;
  vet_N2_p3[1].resp_3 = 0;

  vet_N2_p3[2].parte_1 = F("Era ");                 // N2 - 4
  vet_N2_p3[2].parte_2 = F("feliz ");
  vet_N2_p3[2].parte_3 = F("o final do livro.");
  vet_N2_p3[2].resp_1 = 1;
  vet_N2_p3[2].resp_2 = 3;
  vet_N2_p3[2].resp_3 = 0;

  vet_N2_p3[3].parte_1 = F("Compraram, ");            // N2 - 5
  vet_N2_p3[3].parte_2 = F("para a ceia de Natal, ");
  vet_N2_p3[3].parte_3 = F("os ingredientes.");
  vet_N2_p3[3].resp_1 = 1;
  vet_N2_p3[3].resp_2 = 4;
  vet_N2_p3[3].resp_3 = 2;

  vet_N2_p3[4].parte_1 = F("Carlos ");        // N2 - 6
  vet_N2_p3[4].parte_2 = F("dormiu ");
  vet_N2_p3[4].parte_3 = F("na reunião.");
  vet_N2_p3[4].resp_1 = 0;
  vet_N2_p3[4].resp_2 = 1;
  vet_N2_p3[4].resp_3 = 4;
  
  // Início dos vetores com as frases divididas em quatro partes para o exercício de Análise Sintática.
  vet_N2_p4[0].parte_1 = F("Em Brasília, ");                // N2 - 7
  vet_N2_p4[0].parte_2 = F("ela ");
  vet_N2_p4[0].parte_3 = F("encontrou ");
  vet_N2_p4[0].parte_4 = F("seu amigo de infância.");
  vet_N2_p4[0].resp_1 = 4;
  vet_N2_p4[0].resp_2 = 0;
  vet_N2_p4[0].resp_3 = 1;
  vet_N2_p4[0].resp_4 = 2;

  vet_N2_p4[1].parte_1 = F("Assim que leu a pergunta, ");    // N2 - 8
  vet_N2_p4[1].parte_2 = F("Marcos ");
  vet_N2_p4[1].parte_3 = F("soube ");
  vet_N2_p4[1].parte_4 = F("a resposta.");
  vet_N2_p4[1].resp_1 = 4;
  vet_N2_p4[1].resp_2 = 0;
  vet_N2_p4[1].resp_3 = 1;
  vet_N2_p4[1].resp_4 = 2;
 
  vet_N2_p4[2].parte_1 = F("Alunos, ");             // N2 - 9
  vet_N2_p4[2].parte_2 = F("nas escolas, ");
  vet_N2_p4[2].parte_3 = F("testam ");
  vet_N2_p4[2].parte_4 = F("seus conhecimentos.");
  vet_N2_p4[2].resp_1 = 0;
  vet_N2_p4[2].resp_2 = 4;
  vet_N2_p4[2].resp_3 = 1;
  vet_N2_p4[2].resp_4 = 2;

  vet_N2_p4[3].parte_1 = F("Para se sair bem na prova, ");         // N2 - 10
  vet_N2_p4[3].parte_2 = F("Juliana ");
  vet_N2_p4[3].parte_3 = F("estudou ");
  vet_N2_p4[3].parte_4 = F("muito.");
  vet_N2_p4[3].resp_1 = 4;
  vet_N2_p4[3].resp_2 = 0;
  vet_N2_p4[3].resp_3 = 1;
  vet_N2_p4[3].resp_4 = 4;

  vet_N2_p4[4].parte_1 = F("Pela manhã, ");         // N2 - 11
  vet_N2_p4[4].parte_2 = F("os carros ");
  vet_N2_p4[4].parte_3 = F("corriam ");
  vet_N2_p4[4].parte_4 = F("na avenida.");
  vet_N2_p4[4].resp_1 = 4;
  vet_N2_p4[4].resp_2 = 0;
  vet_N2_p4[4].resp_3 = 1;
  vet_N2_p4[4].resp_4 = 4;
 
  vet_N2_p4[5].parte_1 = F("Lívia ");                 // N2 - 12
  vet_N2_p4[5].parte_2 = F("estava ");
  vet_N2_p4[5].parte_3 = F("rouca ");
  vet_N2_p4[5].parte_4 = F("após o show.");
  vet_N2_p4[5].resp_1 = 0;
  vet_N2_p4[5].resp_2 = 1;
  vet_N2_p4[5].resp_3 = 3;
  vet_N2_p4[5].resp_4 = 4;

  vet_N2_p4[6].parte_1 = F("Assim que ouviu a notícia, ");         // N2 - 13
  vet_N2_p4[6].parte_2 = F("Luana ");
  vet_N2_p4[6].parte_3 = F("ficou ");
  vet_N2_p4[6].parte_4 = F("chocada.");
  vet_N2_p4[6].resp_1 = 4;
  vet_N2_p4[6].resp_2 = 0;
  vet_N2_p4[6].resp_3 = 1;
  vet_N2_p4[6].resp_4 = 3;

  vet_N2_p4[7].parte_1 = F("As crianças, ");                  // N2 - 14
  vet_N2_p4[7].parte_2 = F("na semana da Páscoa, ");
  vet_N2_p4[7].parte_3 = F("fizeram ");
  vet_N2_p4[7].parte_4 = F("um coelhinho.");
  vet_N2_p4[7].resp_1 = 0;
  vet_N2_p4[7].resp_2 = 4;
  vet_N2_p4[7].resp_3 = 1;
  vet_N2_p4[7].resp_4 = 2;

  vet_N2_p4[8].parte_1 = F("Daiane, ");             // N2 - 15
  vet_N2_p4[8].parte_2 = F("ontem pela manhã, ");
  vet_N2_p4[8].parte_3 = F("estava ");
  vet_N2_p4[8].parte_4 = F("feliz.");
  vet_N2_p4[8].resp_1 = 0;
  vet_N2_p4[8].resp_2 = 4;
  vet_N2_p4[8].resp_3 = 1;
  vet_N2_p4[8].resp_4 = 3;

 ///////////////////// Nível 3
  // Início dos vetores com as frases divididas em duas partes para o exercício de Análise Sintática.
  vet_N3_p2[0].parte_1 = F("Preciso ");                   // N3 - 1
  vet_N3_p2[0].parte_2 = F("que você venha me ajudar.");
  vet_N3_p2[0].resp_1 = 1;
  vet_N3_p2[0].resp_2 = 2;

  // Início dos vetores com as frases divididas em três partes para o exercício de Análise Sintática.
  vet_N3_p3[0].parte_1 = F("Eles ");                       // N3 - 2
  vet_N3_p3[0].parte_2 = F("disseram ");
  vet_N3_p3[0].parte_3 = F("que o trabalho não estava pronto.");
  vet_N3_p3[0].resp_1 = 0;
  vet_N3_p3[0].resp_2 = 1;
  vet_N3_p3[0].resp_3 = 2;

  vet_N3_p3[1].parte_1 = F("Nossa vontade ");               // N3 - 3
  vet_N3_p3[1].parte_2 = F("era ");
  vet_N3_p3[1].parte_3 = F("que o filme fosse lançado.");
  vet_N3_p3[1].resp_1 = 0;
  vet_N3_p3[1].resp_2 = 1;
  vet_N3_p3[1].resp_3 = 3;

  vet_N3_p3[2].parte_1 = F("Ela ");                           // N3 - 4
  vet_N3_p3[2].parte_2 = F("desejava ");
  vet_N3_p3[2].parte_3 = F("que lhe dessem um carro.");
  vet_N3_p3[2].resp_1 = 0;
  vet_N3_p3[2].resp_2 = 1;
  vet_N3_p3[2].resp_3 = 2;
 
  vet_N3_p3[3].parte_1 = F("É ");                          // N3 - 5
  vet_N3_p3[3].parte_2 = F("possível ");
  vet_N3_p3[3].parte_3 = F("que ela chegue tarde hoje para o jantar.");
  vet_N3_p3[3].resp_1 = 1;
  vet_N3_p3[3].resp_2 = 3;
  vet_N3_p3[3].resp_3 = 0;

  vet_N3_p3[4].parte_1 = F("Meu anseio ");            // N3 - 6
  vet_N3_p3[4].parte_2 = F("é ");
  vet_N3_p3[4].parte_3 = F("que ele chegue logo.");
  vet_N3_p3[4].resp_1 = 0;
  vet_N3_p3[4].resp_2 = 1;
  vet_N3_p3[4].resp_3 = 3;

  vet_N3_p3[5].parte_1 = F("Eu ");                       // N3 - 7
  vet_N3_p3[5].parte_2 = F("revelei ");
  vet_N3_p3[5].parte_3 = F("que sou apaixonada por Silvana.");
  vet_N3_p3[5].resp_1 = 0;
  vet_N3_p3[5].resp_2 = 1;
  vet_N3_p3[5].resp_3 = 2;

  vet_N3_p3[6].parte_1 = F("Fiz ");                        // N3 - 8
  vet_N3_p3[6].parte_2 = F("todos os exercícios ");
  vet_N3_p3[6].parte_3 = F("conforme a professora pediu.");
  vet_N3_p3[6].resp_1 = 1;
  vet_N3_p3[6].resp_2 = 2;
  vet_N3_p3[6].resp_3 = 4;

  vet_N3_p3[7].parte_1 = F("Quando saíram, ");          // N3 - 9
  vet_N3_p3[7].parte_2 = F("uma chuva forte ");
  vet_N3_p3[7].parte_3 = F("começou.");
  vet_N3_p3[7].resp_1 = 4;
  vet_N3_p3[7].resp_2 = 0;
  vet_N3_p3[7].resp_3 = 1;
 
  vet_N3_p3[8].parte_1 = F("Era ");                 // N3 - 10
  vet_N3_p3[8].parte_2 = F("permitido ");
  vet_N3_p3[8].parte_3 = F("que alimentassem os animais.");
  vet_N3_p3[8].resp_1 = 1;
  vet_N3_p3[8].resp_2 = 3;
  vet_N3_p3[8].resp_3 = 0;

  // Início dos vetores com as frases divididas em quatro partes para o exercício de Análise Sintática.
  vet_N3_p4[0].parte_1 = F("Luiza e João ");            // N3 - 11
  vet_N3_p4[0].parte_2 = F("procuraram ");
  vet_N3_p4[0].parte_3 = F("pelos doces, ");
  vet_N3_p4[0].parte_4 = F("quando sentiram fome.");
  vet_N3_p4[0].resp_1 = 0;
  vet_N3_p4[0].resp_2 = 1;
  vet_N3_p4[0].resp_3 = 2;
  vet_N3_p4[0].resp_4 = 4;

  vet_N3_p4[1].parte_1 = F("Embora não estudasse, ");    // N3 - 12
  vet_N3_p4[1].parte_2 = F("Ricardo ");
  vet_N3_p4[1].parte_3 = F("tirava ");
  vet_N3_p4[1].parte_4 = F("notas altas.");
  vet_N3_p4[1].resp_1 = 4;
  vet_N3_p4[1].resp_2 = 0;
  vet_N3_p4[1].resp_3 = 1;
  vet_N3_p4[1].resp_4 = 2;

  vet_N3_p4[2].parte_1 = F("As rosas ");                // N3 - 13
  vet_N3_p4[2].parte_2 = F("estão ");
  vet_N3_p4[2].parte_3 = F("murchas, ");
  vet_N3_p4[2].parte_4 = F("porque não foram molhadas.");
  vet_N3_p4[2].resp_1 = 0;
  vet_N3_p4[2].resp_2 = 1;
  vet_N3_p4[2].resp_3 = 3;
  vet_N3_p4[2].resp_4 = 4;

  vet_N3_p4[3].parte_1 = F("Nós ");                   // N3 - 14
  vet_N3_p4[3].parte_2 = F("usaremos ");
  vet_N3_p4[3].parte_3 = F("casaco ");
  vet_N3_p4[3].parte_4 = F("enquanto estiver frio.");
  vet_N3_p4[3].resp_1 = 0;
  vet_N3_p4[3].resp_2 = 1;
  vet_N3_p4[3].resp_3 = 2;
  vet_N3_p4[3].resp_4 = 4;

  /*vet_N3_p4[4].parte_1 = F("Ele ");                // N3 - 15
  vet_N3_p4[4].parte_2 = F("só me dará ");
  vet_N3_p4[4].parte_3 = F("presente ");
  vet_N3_p4[4].parte_4 = F("se eu sair com ele.");
  vet_N3_p4[4].resp_1 = 0;
  vet_N3_p4[4].resp_2 = 1;
  vet_N3_p4[4].resp_3 = 2;
  vet_N3_p4[4].resp_4 = 4;*/

  //////////////////////////////// Vígulas nível 1
  // Início dos vetores com as frases divididas em duas partes para o exercício de Vírgula.
  vet_V1_p2[0].parte_1 = F("Sem olhar para trás ");    // V1 - 1
  vet_V1_p2[0].parte_2 = F("Alice correu");
  vet_V1_p2[0].resp_1 = 0;
  vet_V1_p2[0].resp_2 = 0;

  vet_V1_p2[1].parte_1 = F("Cansado ");                  // V1 - 2
  vet_V1_p2[1].parte_2 = F("Carlos dormiu na reunião.");
  vet_V1_p2[1].resp_1 = 0;
  vet_V1_p2[1].resp_2 = 0;

  /*vet_V1_p2[2].parte_1 = F("Esther queria ser atriz quando pequena "); // V1 - 3
  vet_V1_p2[2].parte_2 = F("mas se tornou engenheira.");
  vet_V1_p2[2].resp_1 = 0;
  vet_V1_p2[2].resp_2 = 0;*/

  vet_V1_p2[2].parte_1 = F("Ela queria comprar o quadro do artista "); // V1 - 4
  vet_V1_p2[2].parte_2 = F("mas estava sem dinheiro.");
  vet_V1_p2[2].resp_1 = 0;
  vet_V1_p2[2].resp_2 = 0;

  vet_V1_p2[3].parte_1 = F("Quando amanheceu ");           // V1 - 5
  vet_V1_p2[3].parte_2 = F("a família decidiu viajar.");
  vet_V1_p2[3].resp_1 = 0;
  vet_V1_p2[3].resp_2 = 0;

  vet_V1_p2[4].parte_1 = F("O circo, a atração mais esperada, chega à cidade ");   // V1 - 6
  vet_V1_p2[4].parte_2 = F("hoje");
  vet_V1_p2[4].resp_1 = 1;
  vet_V1_p2[4].resp_2 = 1;

  vet_V1_p2[5].parte_1 = F("Maria queria sair ");              // V1 - 7
  vet_V1_p2[5].parte_2 = F("mas estava chovendo muito.");
  vet_V1_p2[5].resp_1 = 0;
  vet_V1_p2[5].resp_2 = 1;

  vet_V1_p2[6].parte_1 = F("Carolina se matriculou na academia ");     // V1 - 8
  vet_V1_p2[6].parte_2 = F("porém não foi nenhuma vez.");
  vet_V1_p2[6].resp_1 = 0;
  vet_V1_p2[6].resp_2 = 1;

  vet_V1_p2[7].parte_1 = F("Em Brasília ");                        // V1 - 9
  vet_V1_p2[7].parte_2 = F("ela encontrou seu amigo de infância.");
  vet_V1_p2[7].resp_1 = 1;
  vet_V1_p2[7].resp_2 = 0;

  vet_V1_p2[8].parte_1 = F("Maria Paula Roberto ");       // V1 - 10
  vet_V1_p2[8].parte_2 = F("e Diego saíram para pescar.");
  vet_V1_p2[8].resp_1 = 0;
  vet_V1_p2[8].resp_2 = 0;

  vet_V1_p2[9].parte_1 = F("Assim que leu a pergunta ");                   // V1 - 21
  vet_V1_p2[9].parte_2 = F("Marcos já sabia a resposta.");
  vet_V1_p2[9].resp_1 = 0;
  vet_V1_p2[9].resp_2 = 0;

  // Início dos vetores com as frases divididas em três partes para o exercício de Vírgula.
  vet_V1_p3[0].parte_1 = F("Daiane ");                           // V1 - 11
  vet_V1_p3[0].parte_2 = F("professora de Literatura ");
  vet_V1_p3[0].parte_3 = F("está de férias.");
  vet_V1_p3[0].resp_1 = 0;
  vet_V1_p3[0].resp_2 = 1;

  vet_V1_p3[1].parte_1 = F("Os adolescentes sabiam que a prova seria fácil e ");       // V1 - 12
  vet_V1_p3[1].parte_2 = F("mesmo assim ");
  vet_V1_p3[1].parte_3 = F("estudaram.");
  vet_V1_p3[1].resp_1 = 0;
  vet_V1_p3[1].resp_2 = 1;

  /*vet_V1_p3[2].parte_1 = F("Sem saber onde estava ");                              // V1 - 13
  vet_V1_p3[2].parte_2 = F("a família ");
  vet_V1_p3[2].parte_3 = F("decidia se seguia o mapa ou as placas da estrada.");
  vet_V1_p3[2].resp_1 = 1;
  vet_V1_p3[2].resp_2 = 1;*/

  vet_V1_p3[2].parte_1 = F("Nessa semana, o filme ");                       // V1 - 14
  vet_V1_p3[2].parte_2 = F(",que todos esperavam ");
  vet_V1_p3[2].parte_3 = F("será lançado.");
  vet_V1_p3[2].resp_1 = 1;
  vet_V1_p3[2].resp_2 = 1;

  vet_V1_p3[3].parte_1 = F("Rogério, o pai de Adalberto, ");            // V1 - 15
  vet_V1_p3[3].parte_2 = F("comprou uma casa ");
  vet_V1_p3[3].parte_3 = F("próxima ao centro da cidade.");
  vet_V1_p3[3].resp_1 = 1;
  vet_V1_p3[3].resp_2 = 1;

  /*vet_V1_p3[4].parte_1 = F("Já que ");                               // V1 - 16
  vet_V1_p3[4].parte_2 = F("estavam precisando de dinheiro ");
  vet_V1_p3[4].parte_3 = F("eles venderam a casa.");
  vet_V1_p3[4].resp_1 = 1;
  vet_V1_p3[4].resp_2 = 1;*/

  vet_V1_p3[4].parte_1 = F("Quando chegou o Natal ");      // V1 - 17
  vet_V1_p3[4].parte_2 = F("perceberam que ");
  vet_V1_p3[4].parte_3 = F("o ano havia passado rápido.");
  vet_V1_p3[4].resp_1 = 0;
  vet_V1_p3[4].resp_2 = 0;

  vet_V1_p3[5].parte_1 = F("Alex ");                         // V1 - 18
  vet_V1_p3[5].parte_2 = F("o gerente do banco ");
  vet_V1_p3[5].parte_3 = F("me ligou hoje pela manhã.");
  vet_V1_p3[5].resp_1 = 0;
  vet_V1_p3[5].resp_2 = 1;

  vet_V1_p3[6].parte_1 = F("Ele ");                  // V1 - 19
  vet_V1_p3[6].parte_2 = F("de fato ");
  vet_V1_p3[6].parte_3 = F("sempre foi amado.");
  vet_V1_p3[6].resp_1 = 0;
  vet_V1_p3[6].resp_2 = 1;

  vet_V1_p3[7].parte_1 = F("Durante as manhãs ");          // V1 - 20
  vet_V1_p3[7].parte_2 = F("cantavam ");
  vet_V1_p3[7].parte_3 = F("os passarinhos.");
  vet_V1_p3[7].resp_1 = 0;
  vet_V1_p3[7].resp_2 = 0;

  vet_V1_p3[8].parte_1 = F("Maria e Adalberto saíram para comprar ");    // V1 - 22
  vet_V1_p3[8].parte_2 = F("alface tomate arroz ");
  vet_V1_p3[8].parte_3 = F("e manjericão.");
  vet_V1_p3[8].resp_1 = 0;
  vet_V1_p3[8].resp_2 = 1;

  vet_V1_p3[9].parte_1 = F("O livro tinha aventura, romance ");               // V1 - 23
  vet_V1_p3[9].parte_2 = F("ficção ");
  vet_V1_p3[9].parte_3 = F("e mistério.");
  vet_V1_p3[9].resp_1 = 0;
  vet_V1_p3[9].resp_2 = 1;

  vet_V1_p3[10].parte_1 = F("Antônio ");                                 // V1 - 24
  vet_V1_p3[10].parte_2 = F("filho de José ");
  vet_V1_p3[10].parte_3 = F("amou o presente que ganhou do pai.");
  vet_V1_p3[10].resp_1 = 0;
  vet_V1_p3[10].resp_2 = 1;

  vet_V1_p3[11].parte_1 = F("Ricardo viajou para a capital e trouxe ");     // V1 - 25
  vet_V1_p3[11].parte_2 = F("roupas celulares bolsas ");
  vet_V1_p3[11].parte_3 = F("e chaveiros para suas filhas.");
  vet_V1_p3[11].resp_1 = 0;
  vet_V1_p3[11].resp_2 = 1;

  vet_V1_p3[12].parte_1 = F("Após o almoço, como era de costume, ");    // V1 - 26
  vet_V1_p3[12].parte_2 = F("Augusto ");
  vet_V1_p3[12].parte_3 = F("tomou café.");
  vet_V1_p3[12].resp_1 = 1;
  vet_V1_p3[12].resp_2 = 1;

  vet_V1_p3[13].parte_1 = F("Milhares de alunos ");            // V1 - 27
  vet_V1_p3[13].parte_2 = F("nas escolas públicas ");
  vet_V1_p3[13].parte_3 = F("testam seus conhecimentos.");
  vet_V1_p3[13].resp_1 = 0;
  vet_V1_p3[13].resp_2 = 1;

  vet_V1_p3[14].parte_1 = F("Não obstante o recesso, convém que ");      // V1 - 28
  vet_V1_p3[14].parte_2 = F("Antônio ");
  vet_V1_p3[14].parte_3 = F("estude para as provas.");
  vet_V1_p3[14].resp_1 = 1;
  vet_V1_p3[14].resp_2 = 1;

 /////////////////////// Virgula nível 2

  // Início dos vetores com as frases divididas em duas partes para o exercício de Vírgula.
  vet_V2_p2[0].parte_1 = F("Porque estava cansada ");    // V2 - 1
  vet_V2_p2[0].parte_2 = F("Marlene deixou o dinheiro com a assistente para que ela comprasse os materiais e fosse ao banco.");
  vet_V2_p2[0].resp_1 = 0;
  vet_V2_p2[0].resp_2 = 0;

  vet_V2_p2[1].parte_1 = F("Quando o ônibus parou, os passageiros desceram e ");     // V2 - 2
  vet_V2_p2[1].parte_2 = F("caminharam em direção a faculdade.");
  vet_V2_p2[1].resp_1 = 1;
  vet_V2_p2[1].resp_2 = 1;

  vet_V2_p2[2].parte_1 = F("Embora não soubesse onde estavam ");                       // V2 - 3
  vet_V2_p2[2].parte_2 = F("a família decidia se seguia o mapa ou as placas da estrada.");
  vet_V2_p2[2].resp_1 = 0;
  vet_V2_p2[2].resp_2 = 0;

  /*vet_V2_p2[3].parte_1 = F("Ao sair de casa ");                        // V2 - 4
  vet_V2_p2[3].parte_2 = F("Anna se deu conta que havia esquecido as chaves.");
  vet_V2_p2[3].resp_1 = 0;
  vet_V2_p2[3].resp_2 = 0;*/

  // Início dos vetores com as frases divididas em três partes para o exercício de Vírgula.
  vet_V2_p3[0].parte_1 = F("Ao sair o sol, os meninos ");      // V2 - 5
  vet_V2_p3[0].parte_2 = F("que jogavam dentro de casa ");
  vet_V2_p3[0].parte_3 = F("foram nadar na piscina.");
  vet_V2_p3[0].resp_1 = 0;
  vet_V2_p3[0].resp_2 = 1;

}

///////////////////////////////////////////// LOOP //////////////////////////////////////////////////
// Segunda função principal da Arduino. Roda repetidamente enquanto a placa estiver ligada.        //
/////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  byte opcao, nivel; // Variáveis para receber o exercício escolhido pelo jogador (opcao) e qual o nível de dificuldade (nivel)

  switch (estado){ // A variável global "estado" é usada para saber se o jogo está sendo inicidado/reiniciado, se irá para o 
                   // exercício de Análise, exercício de Vírgula ou se vai mostrar a pontuação final antes de reiniciar.

      case 0:  // Neste estado o jogo irá para a tela de opções de exercícios.

            opcao = tela_Opcoes();  // A opção escolhida pelo jogador na tela é recebida como retorno da função tela_Opcoes().
            opcao++;  // Incrementa a opção para mudar a variável global "estado".
            estado = opcao; // 
            
      break;


      case 1:  // Neste estado o jogo irá para o exercício de Análise Sintática.

             if(enunciado_Analise == true){ // Caso o jogo entre no exercício de Análise pela primeira vez ou o jogo tenha sido 
                                            // reiniciado, o enunciado será mostrado na tela.

                escreve_Enunciado(0);
                enunciado_Analise = false;  // Indica que o enunciado não precisa ser escrito na tela caso o jogador volte para
                                            // a tela de escolha de exercício. 

             }
             
             nivel = tela_Escolha_Nivel(0); // A variável "nivel" recebe da função tela_Escolha_Nivel() o nível de dificuldade 
                                            // escolhido pelo jogador.

             if(nivel == 0){ // Caso o jogador tenha escolhido o nível 1
                  tela_Ex_Func_Sint(nivel, vet_N1_p2, vet_N1_p3, vet_N1_p4, 2, 9, 4);
             }

             else if(nivel == 1){ // Caso o jogador tenha escolhido o nível 2
                  tela_Ex_Func_Sint(nivel, vet_N2_p2, vet_N2_p3, vet_N2_p4, 1, 5, 9);
             }

             else if(nivel == 2){ // Caso o jogador tenha escolhido o nível 3
                  tela_Ex_Func_Sint(nivel, vet_N3_p2, vet_N3_p3, vet_N3_p4, 1, 9, 4);
             }

             else if(nivel == 3){ // Caso o jogador tenha escolhido sair do exercício

                  estado = 0;
             }

             else{ // Caso o jogador tenha escolhido encerrar o jogo
                  encerrar();
             }
             
      break;


      case 2: // Neste estado o jogo irá para o exercício de Vírgula

            if(enunciado_Virgula == true){ // Caso o jogo entre no exercício de Vírgula pela primeira vez ou o jogo tenha sido 
                                           // reiniciado, o enunciado será mostrado na tela.

                escreve_Enunciado(60);
                enunciado_Virgula = false; // Indica que o enunciado não precisa ser escrito na tela caso o jogador volte para
                                            // a tela de escolha de exercício. 

             }
             
            nivel = tela_Escolha_Nivel(1);

            if(nivel == 0){ // Caso o jogador tenha escolhido o nível 1
      
                  tela_Ex_Virg(nivel, vet_V1_p2, vet_V1_p3, vet_N3_p4,  10, 15);
            }

            else if(nivel == 1){ // Caso o jogador tenha escolhido o nível 2
                  
                  tela_Ex_Virg(nivel, vet_V2_p2, vet_V2_p3, vet_N3_p4,  3, 1);
            }

            else if(nivel == 3){ // Caso o jogador tenha escolhido sair do exercício
                  estado = 0;
            }

            else{ // Caso o jogador tenha escolhido encerrar o jogo
                encerrar();
            }
            
      break;


      case 3: // Neste estado o jogo será encerrado

            encerrar();  // Chama a função que encerra o jogo
            pontos_N_1 = 0;  // Zera a pontuação do exercício de Análise Sintática de nível 1
            pontos_N_2 = 0;  // Zera a pontuação do exercício de Análise Sintática de nível 2
            pontos_N_3 = 0;  // Zera a pontuação do exercício de Análise Sintática de nível 3
            pontos_Virgula_N_1 = 0; // Zera a pontuação do exercício de Vírgula de nível 1
            pontos_Virgula_N_2 = 0; // Zera a pontuação do exercício de Vírgula de nível 2
            estado = 0;  // Indica que o próximo estado deve ser o início do jogo
            enunciado_Analise = true;  // Indica que o enunciado do exercício de Análise Sintática deve ser mostrado novamente quando entrar no exercício
            enunciado_Virgula = true;  // Indica que o enunciado do exercício de Vírgula deve ser mostrado novamente quando entrar no exercício
            
      break;

      default:  // O estado padrão para o jogo será o estado inicial

           estado = 0;
            
      break;
  }
        
  
}

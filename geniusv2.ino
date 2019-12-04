#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

int sequencia[32] = {};
int botoes[4] = {3, 5, 7, 9};
int botaoconfig = 11 ;
int leds[4] = {2, 4, 6, 8};
int tons[4] = {262, 294, 330, 349};
int buzzer = 10;
int rodada = 0;
int passo = 0;
bool game_over = true;
int numdelay = 500;
int melhorrodada;
int endereco = 0;
float mult = 1;
bool confi = false;

void setup() {
  // Leds
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);
  // Buzzer
  pinMode(10, OUTPUT);
  // Botões
  pinMode(3, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);  
  pinMode(11, INPUT_PULLUP);
  randomSeed(analogRead(A0));
  melhorrodada = EEPROM.read(endereco);
  //Serial.begin(230400);
  lcd.begin (16,2);
}

void loop() {
   // estrutura usada para reiniciar todas as variáveis (novo jogo)
  if (game_over == true) {
    int medida = analogRead(A1);
    float delaynum = 25 + 0.9531*(medida+0.001);
    numdelay = delaynum;
    lcd.setBacklight(HIGH);
    sequencia[32] = {};  
    rodada = 0;
    passo = 0;
    //numdelay = 500;
    game_over = false;
    confi = false;
  }
  delay(1000);

  proximaRodada();
  reproduzirSequencia();
  aguardarJogador();
}

/********* Funções **********/
//void potenciometro(){
//  int medida = analogRead(A1);
//  //medida será um inteiro de 0 a 1023
//  //convertar para um valor de 50 a 1000
//  // 50 + converte de 0 a 950
//  int delaynum = 50 + 0.92864125122189638318670576735093*(medida+0.1);
//  //Serial.println(delaynum);
//  numdelay = delaynum;
//}

void proximaRodada() {
  int sorteio = random(4);
  sequencia[rodada] = sorteio;
  atualizarPlacar();
  rodada = rodada + 1;
  //Serial.print(sorteio);
}

void reproduzirSequencia() {
  for (int i = 0; i < rodada; i++) {
    tone(buzzer, mult*tons[sequencia[i]]); //toca tom
    digitalWrite(leds[sequencia[i]], HIGH); //acende lede
    delay(numdelay);    //delay
    noTone(buzzer);  //para tom
    digitalWrite(leds[sequencia[i]], LOW); //apaga led
    delay(150);
  }
}

void atualizarPlacar(){
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Placar atual: ");
  lcd.print(rodada);
  lcd.setCursor(0,1);
  lcd.print("Best placar: ");
  
  if(rodada > melhorrodada){
    melhorrodada = rodada;
    EEPROM.write(endereco, melhorrodada);
  }
  
  lcd.print(melhorrodada);
}

void easter(){
  int botao_pressionado = 0;
  for (int i = 0; i <= 3; i++) {
    if (!digitalRead(botoes[i]) == HIGH) {
        int medida = analogRead(A1);
        mult = 1 + medida*0.009775;      
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   FREQUENCIA    ");
        lcd.setCursor(0,1);
        lcd.print("    ");
        lcd.print(mult*tons[i]);  
        lcd.print(" Hz");                        
      botao_pressionado = i;              
      tone(buzzer, mult*tons[i]);
      digitalWrite(leds[i], HIGH);
      delay(100);
      digitalWrite(leds[i], LOW);
      noTone(buzzer);
      delay(100);                
    }
  }
}

void configuracao(){
  if (!digitalRead(botaoconfig) == HIGH){ 
  bool saiu = false;
  tone (buzzer, mult*1270);
  delay(100);
  noTone(buzzer);
  tone (buzzer, mult*600);
  delay(100);
  noTone(buzzer);
    
    while(!saiu){   
      if (!digitalRead(botaoconfig) == HIGH){ 
        delay(300);
        if(!digitalRead(botaoconfig) == HIGH){
          delay(1700);        
          if(!digitalRead(botaoconfig) == 1){
            bool jogo = false;
            tone (buzzer, mult*2270);
            delay(100);
            noTone(buzzer);
            tone (buzzer, mult*3380);
            delay(100);
            noTone(buzzer);
            delay(1000);
            while(!jogo){
              easter();
              if (!digitalRead(botaoconfig) == 1){
                jogo = true;
              }
            }
          } 
        }
      }
      int medida = analogRead(A1);
      float delaynum = 25 + 0.9531*(medida+0.001);
      numdelay = delaynum;
      delay(100);   
               
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Velocidade: ");
      lcd.print(numdelay,1);      
      lcd.setCursor(0,1);
      if(delaynum<150){
        lcd.print("Dificil");
      }
      if(delaynum>150 && delaynum<300){
        lcd.print("Medio");
      }
      if(delaynum>300 && delaynum<500){
        lcd.print("Facil");
      }
      if(delaynum>500){
        lcd.print("Muito facil");
      }
     
      if (!digitalRead(botaoconfig) == 1){
          tone (buzzer, mult*600);
          delay(100);
          noTone(buzzer);
          tone (buzzer, mult*1270);
          delay(100);
          noTone(buzzer);
          atualizarPlacar();
          confi = true;
          saiu = true;
      }
    }
  }
}
  

void aguardarJogador() {
  int botao_pressionado = 0;
  for (int i = 0; i < rodada; i++) {
    bool jogada_efetuada = false;    
    while (!jogada_efetuada) {      
      if(!confi){
        configuracao();   
      }      
      for (int i = 0; i <= 3; i++) {
        if (!digitalRead(botoes[i]) == HIGH) {
          botao_pressionado = i;
          tone(buzzer, mult*tons[i]);
          digitalWrite(leds[i], HIGH);
          delay(numdelay);
          digitalWrite(leds[i], LOW);
          noTone(buzzer);
          jogada_efetuada = true;
          delay(175);   
        }
      }
    }
    
    //verificar a jogada
    if (sequencia[passo] != botao_pressionado) {
      //efeito luminoso/sonoro indicando que o jogador perdeu
      lcd.clear();
      for (int i = 0; i <= 3; i++) {
        lcd.setBacklight(LOW);
        lcd.setCursor(0,0);
        lcd.print("    PERDEU!");
        lcd.setCursor(0,1);
        lcd.print("TENTE NOVAMENTE");
        tone (buzzer, mult*70);
        digitalWrite(leds[i], HIGH);
        delay(100);
        digitalWrite(leds[i], LOW);
        noTone(buzzer);
        lcd.setBacklight(HIGH);
        
      }
      game_over = true;
      break;
    }
    passo = passo + 1;
  }
  passo = 0;
}

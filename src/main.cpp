#include <Arduino.h> // bliblioteca do arduino
#include <LiquidCrystal.h> // controla os display LCD 

// put function declarations here:
int myFunction(int, int);
void limpaTabuleiro();
void desenhaTabuleiro();
void atualizaPeca(int novoX);

// Inicializa o LCD com os pinos
LiquidCrystal lcd(12, 11, 5, 4, 2, 3);

// Pinos dos botões
const int botaoEsq = 6;
const int botaoDir = 7;
const int botaoRot = 8;

//  altera isso pq nao sei qual o tananho do lcd 
char tabuleiro[0][0];

// Posição inicial da peça
int pecaX = 7;
int pecaY = 0;

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}



// aqui começa o codigo pro joguinho 
void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

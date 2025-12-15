/* 
   /////////////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////////////
   /        __________   _________   ___________   __________    _________                             /
   /       /___   ___/  /   _____/  /  _____   /  /  _____   \  /  ______/       \\          //        /
   /          /  /     /  /        /  /    /  /  /  /    /  /  /  /_____        \\            //       /
   /         /  /     /  /        /  /____/  /  /  /___/  /   /  ______/   \\\   \\    []    //   ///  /
   /     ___/  /__   /  /_____   /   ____   /  /   ___   \   /  /_____        \\\\--]]]/\[[[--////     /
   /    /________/  /________/  /__/    /__/  /__/    \__/  /________/         /////--][][--\\\\\      /
   /                                                                         //   //--][][--\\   \\    /
   /   ______________________________________________________________           //    ][][    \\       /
   /                                                                                                   /
   /                                                                                                   /
   /  This program is free software for arduino; you can redistribute it and/or modify.                /
   /  Ce programme pour Arduino est libre et gratuit; vous pouvez le distribuez et/ou le modifier.     /
   /                                                                                                   /
   / Auteurs:                                                                                          /
   / -Ulysse Lardier                                                                                   /
   /                                                                                                   /
   / Another codes / Autres codes:                                                                     /
   / https://www.icaremaker.com                                                                        /
   / Mail: ionocraft@hotmail.fr                                                                        /
   /////////////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#include <MD_MAX72xx.h>
#include <SPI.h>

#define MAX_DEVICES 1
#define DATA_PIN 11
#define CLK_PIN 13
#define CS_PIN 10

MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Joystick
#define VRX A0
#define VRY A1
#define SW 2

// Jeu Snake
int snakeX[64];
int snakeY[64];
int snakeLength = 3;
int dir = 1; // 0=Gauche, 1=Droite, 2=Haut, 3=Bas
int foodX, foodY;

unsigned long lastMove = 0;
int speedDelay = 750; // Vitesse du jeu en ms

void setup() {
  mx.begin();
  pinMode(SW, INPUT_PULLUP);
  randomSeed(analogRead(A2)); // Initialisation aléatoire

  resetSnake();
  spawnFood();
}

void loop() {
  readJoystick();
//J'integre le jeu dans une boucle temporelle que je personnalise
  if (millis() - lastMove > speedDelay) {
    moveSnake();
    checkCollisions();
    drawSnake();
    lastMove = millis();
  }
}






// ======================
// Gestion du jeu
// ======================



//A mettre dans le SETUP!!!!!
//===========================

void resetSnake() {
  snakeLength = 3; //Le serpent mesure 3 pixels au démarage
  snakeX[0] = 3;  //Sa position dans l'écran de jeu au démarage
  snakeY[0] = 4;
  snakeX[1] = 2;
  snakeY[1] = 4;
  snakeX[2] = 1;
  snakeY[2] = 4;
  dir = 1; // droite
}
//Je positionne au hasard un sucre sur l'écran
void spawnFood() {
  foodX = random(0, 8);
  foodY = random(0, 8);
}

void moveSnake() {
  int prevX = snakeX[0];
  int prevY = snakeY[0];
  int prev2X, prev2Y;

  // Déplacer la tête
  if (dir == 0) snakeX[0]--;
  else if (dir == 1) snakeX[0]++;
  else if (dir == 2) snakeY[0]--;
  else if (dir == 3) snakeY[0]++;

  // Faire suivre le corps
  for (int i = 1; i < snakeLength; i++) {
    prev2X = snakeX[i];
    prev2Y = snakeY[i];
    snakeX[i] = prevX;
    snakeY[i] = prevY;
    prevX = prev2X;
    prevY = prev2Y;
  }

  // Vérifie si le serpent mange
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++;
    if (speedDelay > 80) {speedDelay -= 10; // accélère légèrement
    spawnFood();
  }
}

void checkCollisions() {
  // Collision bord
  if (snakeX[0] < 0 || snakeX[0] > 7 || snakeY[0] < 0 || snakeY[0] > 7) {
    resetSnake();
  }

  // Collision corps
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      resetSnake();
    }
  }
}

void drawSnake() {
  mx.clear();

  // Dessiner serpent
  for (int i = 0; i < snakeLength; i++) {
    mx.setPoint(snakeY[i], snakeX[i], true);
  }

  // Nourriture
  mx.setPoint(foodY, foodX, true);
}









// ======================
// Lecture du joystick
// ======================

void readJoystick() {
  int xValue = analogRead(VRX);
  int yValue = analogRead(VRY);

  if (xValue < 400 && dir != 1) dir = 0; // gauche
  else if (xValue > 600 && dir != 0) dir = 1; // droite
  else if (yValue < 400 && dir != 3) dir = 3; // haut
  else if (yValue > 600 && dir != 2) dir = 2; // bas
}

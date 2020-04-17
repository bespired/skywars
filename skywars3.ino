#include <MD_MAX72xx.h>
#include <SPI.h>
#include "logo.h"
#include "skywars.h"

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES); // Arbitrary pins

int xMap, yMap, xValue, yValue;

unsigned char button;

unsigned char ledBuffer[32];
unsigned char fieldBuffer[36];
unsigned char gamemode = 1; // 1: in game ---

uint32_t prevTimeAnim = 0; // remember the millis() value in animations
unsigned int gameframe = 0; // when to add stuff...

int explode = 0;
int invincible = 30;
int planeY = 10;
int planeX = 50;

unsigned char leftCloudTime = 18;
unsigned char rightCloudTime = 32;
unsigned char midCloudTime = 40;

unsigned char leftCloudType = 1;
unsigned char rightCloudType = 0;

unsigned char leftCloudOffset = 18;
unsigned char rightCloudOffset = 19;
unsigned char midCloudOffset = 10;


void resetMatrix(void) {
  mx.control(MD_MAX72XX::INTENSITY, 5);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
}

void ledDisplay(void) {
  mx.clear();
  for (int t = 0; t < 32; t++) {
    mx.setColumn(t, ledBuffer[t]);
  }
}

void scroll(void) {
  for (int t = 31; t > 0; t--) {
    fieldBuffer[t] = fieldBuffer[t - 1];
  }
}

void copyFlyField(void) {
  for (int t = 31; t > 5; t--) {
    ledBuffer[t] = fieldBuffer[t];
  }
}

void addCloud(char side, char next, char type, char row) {
  // add the side cloud ...

  fieldBuffer[1] |= setPixel(side);

  switch (type) {
    case 0:
      if (row == 3) fieldBuffer[1] |= setPixel(next);
      if (row == 4) fieldBuffer[1] |= setPixel(next);
      break;
    case 1:
      if (row == 2) fieldBuffer[1] |= setPixel(next);
      if (row == 4) fieldBuffer[1] |= setPixel(next);
      break;
    case 2:
      if (row == 1) fieldBuffer[1] |= setPixel(next);
      if (row == 4) fieldBuffer[1] |= setPixel(next);
      if (row == 3) fieldBuffer[1] |= setPixel(next);
      break;
  }

}

void addMidCloud(void) {
  // add the middle cloud ...

  char type   = random(1, 6);
  char center = random(0, 7);

  switch(type){

    case 2:
      fieldBuffer[1] |= setPixel(center + 1);
    case 1:
      fieldBuffer[2] |= setPixel(center);
      fieldBuffer[2] |= setPixel(center + 1);
    break;

    case 4:
      fieldBuffer[2] |= setPixel(center - 1);
    case 3:
      fieldBuffer[1] |= setPixel(center);
      fieldBuffer[2] |= setPixel(center);
      fieldBuffer[2] |= setPixel(center + 1);
    break;
    
    case 5:
      fieldBuffer[1] |= setPixel(center-1);
      fieldBuffer[1] |= setPixel(center+1);
      fieldBuffer[1] |= setPixel(center);
      fieldBuffer[2] |= setPixel(center);
  }

}

void newclouds(void) {

  leftCloudTime--;
  rightCloudTime--;
  midCloudTime--;

  if (leftCloudTime < 6) {
    addCloud(0, 1, leftCloudType, leftCloudTime);
    if (leftCloudTime == 0) {
      leftCloudTime = random(leftCloudOffset, leftCloudOffset + 4);
      leftCloudType = random(0, 2);
    }
  }


  if (rightCloudTime < 6) {
    addCloud(7, 6, rightCloudType, rightCloudTime);
    if (rightCloudTime == 0) {
      rightCloudTime = random(rightCloudOffset, rightCloudOffset + 4);
      rightCloudType = random(0, 2);
    }
  }

  if (midCloudTime == 0) {
    addMidCloud();
    midCloudTime = random(midCloudOffset, midCloudOffset + 4);
  }

}

void getButtons(void) {

  button = digitalRead(btn2);

  if (button) {
    explode = 0;
    invincible = 30;
  }

}


void getJoystick(void) {

  xValue = analogRead(joyX);
  yValue = analogRead(joyY);

  xMap = map(xValue, 0, 1024, -7, 7);
  yMap = map(yValue, 0, 1024, 15, -15);
  if ( xMap == -1 ) xMap = 0;

  planeX += xMap;
  if (planeX < 0) planeX = 0;
  if (planeX > 70) planeX = 70;

  planeY += yMap;
  if (planeY < 0) planeY = 0;
  if (planeY > 150) planeY = 150;

}

void checkCollision(void) {

  char xPos = planeX / 10;
  char yPos = planeY / 10;

  char mask = setPixel(xPos);

  if ((fieldBuffer[29 - yPos] & mask) > 0) explode = 1;

}


char setPixel(char x) {
  if (x < 0) return 0;
  if (x > 7) return 0;
  return 1 << (7 - x);
}

void drawPlane(void) {
  char xPos = planeX / 10;
  char yPos = planeY / 10;

  if (explode == 0) {
    if (invincible % 2 == 0) {
      ledBuffer[29 - yPos] |= voor[xPos];
      ledBuffer[30 - yPos] |= midden[xPos];
      ledBuffer[31 - yPos] |= achter[xPos];
    }
  } else {


    if (explode < 8) {

      if (random(100) < 50) ledBuffer[29 - yPos] |= setPixel(xPos);
      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos);
      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos - 1);

    } else if (explode < 16) {

      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos - 1);
      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos - 2);
      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos + 1);
      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos + 2);
      if (random(100) < 50) ledBuffer[29 - yPos] |= setPixel(xPos);
      if (random(100) < 50) ledBuffer[29 - yPos] |= setPixel(xPos - 1);
      if (random(100) < 50) ledBuffer[29 - yPos] |= setPixel(xPos + 1);
      if (random(100) < 50) ledBuffer[28 - yPos] |= setPixel(xPos);
      if (random(100) < 50) ledBuffer[31 - yPos] |= setPixel(xPos);
      if (random(100) < 50) ledBuffer[31 - yPos] |= setPixel(xPos + 1);
      if (random(100) < 50) ledBuffer[32 - yPos] |= setPixel(xPos);

    } else if (explode < 24) {

      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos - 2);
      if (random(100) < 50) ledBuffer[30 - yPos] |= setPixel(xPos + 2);
      if (random(100) < 50) ledBuffer[29 - yPos] |= setPixel(xPos + 2);
      if (random(100) < 50) ledBuffer[29 - yPos] |= setPixel(xPos - 1);
      if (random(100) < 50) ledBuffer[29 - yPos] |= setPixel(xPos - 2);
      if (random(100) < 50) ledBuffer[28 - yPos] |= setPixel(xPos - 1);
      if (random(100) < 50) ledBuffer[28 - yPos] |= setPixel(xPos);
      if (random(100) < 50) ledBuffer[28 - yPos] |= setPixel(xPos + 1);
      if (random(100) < 50) ledBuffer[31 - yPos] |= setPixel(xPos + 2);
      if (random(100) < 50) ledBuffer[31 - yPos] |= setPixel(xPos - 2);
      if (random(100) < 50) ledBuffer[32 - yPos] |= setPixel(xPos - 1);
      if (random(100) < 50) ledBuffer[32 - yPos] |= setPixel(xPos);
      if (random(100) < 50) ledBuffer[32 - yPos] |= setPixel(xPos + 1);
    }
    explode++;
    if (explode == 50) {
      explode = 0;
      invincible = 30;
    }

  }

}


void game(void) {

  // flyfield
  scroll();

  if (explode < 60) newclouds();

  // interaction
  if (explode == 0) getJoystick();
  getButtons();

  // buffer
  copyFlyField();

  if (invincible > 0) {
    invincible--;
  } else if (explode == 0) {
    checkCollision();
  }

  drawPlane();
  // drawCount();

  gameframe++;
}

void setup() {
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT);

  mx.begin();
  resetMatrix();

}

void loop() {

  // don't do stuff until it's time to do stuff

  // getJoystick();

  char front = planeY / 3;

  if (millis() - prevTimeAnim < ANIMATION_DELAY - front) return;
  prevTimeAnim = millis();

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // put stuff to display
  switch (gamemode) {

    // game play buffer to screen
    case 1:
      game();
      ledDisplay();
      break;
  }

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

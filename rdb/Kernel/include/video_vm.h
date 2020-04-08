#ifndef _VDVM_H
#define _VDVM_H

#include <stdint.h>
#include <font.h>

void init_VM_Driver(void);
unsigned int getScreenWidth(void);
unsigned int getScreenHeight(void);
void drawPixel(int x, int y, int color);
void drawRectangle(int x, int y, unsigned int b, unsigned int h, int color);
void drawEllipse(int x, int y, unsigned int a, unsigned int b, int color);
void drawCircle(int x, int y, unsigned int r, int color);
void drawChar(int x, int y, char character, int fontColor, int backgroundColor);
void paintScreen(void);
void clearDisplay(int backgroundColor);
void scrollUp(unsigned int n);
void invertPixelColor(int x, int y);
void invertRectangleColor(int x, int y, unsigned int b, unsigned int h);

int sys_video(void * option, void * arg1, void * arg2, void * arg3, void * arg4);

typedef struct point {
    int x;
    int y;
} point;

#endif
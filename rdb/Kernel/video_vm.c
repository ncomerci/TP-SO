#include <stdint.h>
#include <video_vm.h>
#include <screen.h>

unsigned int SCREEN_WIDTH = 1024;
unsigned int SCREEN_HEIGHT = 768;
unsigned int SCREEN_bPP = 3;

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

struct vbe_mode_info_structure * screenData = (void*)0x5C00;

unsigned int getScreenHeight() {
    return SCREEN_HEIGHT;
}

unsigned int getScreenWidth() {
    return SCREEN_WIDTH;
}

void init_VM_Driver() {
	SCREEN_bPP = screenData->bpp / 8;  //bits por pixel
	SCREEN_HEIGHT = screenData->height;
	SCREEN_WIDTH = screenData->width;
}

void paintScreen() {
    int white = 0xFFFFFF;
    drawRectangle(0,0,SCREEN_WIDTH,SCREEN_WIDTH, white);
}

void drawPixel(int x, int y, int color) {
    char* screen = (char *)(uint64_t)screenData->framebuffer; // Casts purpose was to remove warnings
    unsigned where = (x + y*SCREEN_WIDTH) * SCREEN_bPP;
    screen[where] = color & 255;              // BLUE
    screen[where + 1] = (color >> 8) & 255;   // GREEN
    screen[where + 2] = (color >> 16) & 255;  // RED  byte mas significativo
}

void drawRectangle(int x, int y, unsigned int b, unsigned int h, int color) {
    for(int i = 0; i < b; i++)
        for (int j = 0; j < h; j++)
            drawPixel(x+i,y+j,color);
}

void drawEllipse(int x, int y, unsigned int a, unsigned int b, int color) { // Formula: ((i-x)*(i-x))*((b)*(b)) + ((j-y)*(j-y))*((a)*(a)) <= ((a)*(a))*((b)*(b))
	for(int i = x - a; i < x + a; i++) {
		for(int j = y - b; j < y + b; j++) {
			if ((i >= 0) && (i < SCREEN_WIDTH) && (j >= 0) && (j < SCREEN_HEIGHT) && ((i-x)*(i-x))*((b)*(b)) + ((j-y)*(j-y))*((a)*(a)) <= ((a)*(a))*((b)*(b))) {
				drawPixel(i,j,color);
			}
		}
	}	
}

void drawChar(int x, int y, char character, int fontColor, int backgroundColor) {
    int aux_x = x;
    int aux_y = y;

    char bitsIsPresent;

    unsigned char * toDraw = charBitmap(character);

    for (int i = 0; i < CHAR_HEIGHT; i++) {
        for(int j = 0; j < CHAR_WIDTH; j++) {
            bitsIsPresent = (1 << (CHAR_WIDTH - j)) & toDraw[i];

            if(bitsIsPresent)
                drawPixel(aux_x, aux_y, fontColor);
            else
                drawPixel(aux_x, aux_y, backgroundColor);

            aux_x += 1;
        }
        aux_x = x;
        aux_y += 1;
    }
}

void clearDisplay(int backgroundColor) {
    char * pos = (char *)(uint64_t)screenData->framebuffer;
    for(int i = 0 ; i < SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_bPP ; i+=SCREEN_bPP) {
        pos[i] = backgroundColor & 255;
        pos[i + 1] = (backgroundColor >> 8) & 255;
        pos[i + 2] = (backgroundColor >> 16) & 255;
    }
}

void scrollUp(unsigned int n) {
	char* screen = (char *)(uint64_t)screenData->framebuffer; 
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
			unsigned where = (i + j*SCREEN_WIDTH) * SCREEN_bPP;
			int r, g, b;
            if (j + n >= SCREEN_HEIGHT) {
				r = g = b = 0;
			} 
			else {
				unsigned aux_where = (i + (j + n) * SCREEN_WIDTH) * SCREEN_bPP;
				r = screen[aux_where];
				g = screen[aux_where+1];
				b = screen[aux_where+2];
			}
			screen[where] = r;
			screen[where + 1] = g;
			screen[where + 2] = b;
        }
    }
}

void invertPixelColor(int x, int y) {
	char* screen = (char *)(uint64_t)screenData->framebuffer; // Casts purpose was to remove warnings
    unsigned where = (x + y*SCREEN_WIDTH) * SCREEN_bPP;
    screen[where] = 255 - screen[where];            // BLUE
    screen[where + 1] = 255 - screen[where + 1];   // GREEN
    screen[where + 2] = 255 - screen[where + 2];  // RED 
}

void invertRectangleColor(int x, int y, unsigned int b, unsigned int h) {
	for(int i = 0; i < b; i++)
        for (int j = 0; j < h; j++)
            invertPixelColor(x+i,y+j);
}

int sys_video(void * option, void * arg1, void * arg2, void * arg3, void * arg4) {
	switch ((uint64_t) option) {
		case 0:
			clearDisplay((uint64_t) arg1);
			return 0;
		case 1:
			return getScreenWidth();
		case 2:
			return getScreenHeight();
		case 3:
			drawEllipse(((point *)arg1)->x, ((point *)arg1)->y, (uint64_t) arg2, (uint64_t) arg3, (uint64_t) arg4);
			break;
		case 4:
			drawRectangle(((point *)arg1)->x, ((point *)arg1)->y, (uint64_t) arg2, (uint64_t) arg3, (uint64_t) arg4);
			break;
	}
	return 0;
}
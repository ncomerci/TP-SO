#include <naiveConsole.h>

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;

void ncPrint(const char * string) {
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void _scrollScreen(void) {
	for (int i = 0; i < width * (height - 1); i++) {
		video[2 * i] = video[2 * (i + width)];
		video[2 * i + 1] = video[2 * (i + width) + 1]; 
	}
}

void ncPrintChar(char character) {
	if (currentVideo == video + 2 * (width * height)) {
		_scrollScreen();
		currentVideo -= width * 2;
	}
	*currentVideo = character;
	currentVideo += 2;
}

void ncNewline() {
	do
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value) {
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value) {
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value) {
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear() {
	for (int i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

void ncDeleteNChars(int n){
	for(int i = 0; i < n; i++ ){   
		currentVideo -= 2;
		*currentVideo = ' ';  
	}
}

void ncPrintError(const char * string) {
	for (int i = 0; string[i] != 0; i++)
		ncPrintErrorChar(string[i]);
}

void ncPrintErrorChar(char character) {
	if (currentVideo == video + 2 * (width * height)) {
		_scrollScreen();
		currentVideo -= width * 2;
	}
	*currentVideo = character;
	*(currentVideo + 1) = 0x04; // Codigo de fondo rojo y letra blanca;
	currentVideo += 2;
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

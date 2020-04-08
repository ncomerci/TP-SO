#include <stdint.h>
#include <lib_user.h>

static char buffer[64] = { '0' };

static void _64Hexfill(int n, char * buffer);

// ----------- System ------------

int getMem(void *pos, uint64_t *mem_buffer, unsigned int dim) {
	_sys_system(0, pos, (void *) mem_buffer, (void *)(uint64_t) dim);
	return 0;
}

// ----------- Timet ------------

unsigned long getTicks() {
	return _sys_timet(0, 0, 0);
}

unsigned long getSecondsElapsed() {
	return _sys_timet((void *) 1, 0, 0);
}

int addTimeFunction(function f, unsigned int ticks) {
	return _sys_timet((void *) 2, (void *) f, (void *)(uint64_t) ticks);
}

int updateTimeFunction(function f, unsigned int new_ticks) {
	return _sys_timet((void *) 3, (void *) f, (void *)(uint64_t) new_ticks);
}

void removeTimeFunction(function f) {
	_sys_timet((void *) 3, (void *) f, 0);
}

void wait(unsigned int millis) {
	int last_ticks = getTicks();
	while ((getTicks() - last_ticks) < millis * 1000);
}

// ----------- RTC ------------

time_struct getTime() {
	time_struct aux;
	aux.hours = _sys_rtc((void *) 2);
	aux.mins = _sys_rtc((void *) 1);
	aux.secs = _sys_rtc((void *) 0);
	return aux;
}

// ----------- Read ------------

int read(char *buffer, unsigned int buff_size) {
	int finished = 0;
	int i = 0;
	while (i < buff_size && !finished) { // Mientras no se llene el buffer
		char c;
		while (_sys_read((void *) &c) != 0); // Mientras no consiga el caracter sigo pidiendolo.	
		buffer[i++] = c;
		if (c == '\n')
			finished = 1;
	} 
	return i;
}

int scan(char *buffer, unsigned int buff_size) { // Make sure u have at least buff_size + 1 to allocate the result string.
	int size_read = read(buffer, buff_size);
	buffer[size_read] = 0;
	return 0;
}

char scanChar() {
	char c;
	read(&c, 1);
	return c;
}

// ----------- Screen ------------

void clearScreen() {
	_sys_screen(0, 0, 0, 0);
}

void setBackgroundColor(uint32_t color) {
	_sys_screen((void *) 5, (void *)(uint64_t) color, 0, 0);
}

void setCursor(unsigned int x, unsigned int y) {
	_sys_screen((void *) 6, (void *)(uint64_t) x, (void *)(uint64_t) y, 0);
}

void shiftCursor(int offset) {
	if (offset > 0)
		for(int i = 0; i < offset; i++)
			_sys_screen((void *) 4, (void *)(uint64_t) 1, 0, 0);
	else if (offset < 0)
		for(int i = 0; i < -offset; i++)
			_sys_screen((void *) 4, (void *)(uint64_t) -1, 0, 0);
}

void showCursor(int status) {
	_sys_screen((void *) 3, (void *)(uint64_t) status, 0, 0);
}

int write(const char *str, unsigned int str_size, int color) {
	return _sys_screen((void *) 1, (void *) str, (void *)(uint64_t) str_size, (void *)(uint64_t) color);
}

int printColored(const char *str, int color) {
	return write(str, strlen(str), color);
}

int putColoredChar(char c, int color) {
	return write(&c, 1, color);
}

int print(const char *str) {
	return printColored(str, WHITE_COLOR);
}

int printf(const char *format, ...) {
    va_list pa;  // Lista de parámetros
    va_start(pa, format);

    char *tmp;
	int num;

    while (*format != '\0') {
        if (*format != '%') {
            putChar(*format);
            format++;
            continue;
        }

        format++;

		switch(*format) {
			case 'd': // Si es un decimal
				num = va_arg(pa, int);
				if (num < 0) {
					putChar('-'); // Agrego signo
					num = -num; // Convierto en positivo
				}
				printDec(num);
				break;
			case 'x': // Si es hexadecimal
				num = va_arg(pa, unsigned int);
				printHex(num);
				break;
			case 'X':
				num = va_arg(pa, unsigned int);
				print64Hex(num);
				break;
			case 'o': // Si es octal
				num = va_arg(pa, unsigned int);
				printOct(num);
				break;
			case 'c':
				num = va_arg(pa, int);
				putChar(num);
				break;
			case 's':
				tmp = va_arg(pa, char *);
				print(tmp);
				break;
		}
        format++;
    }

    va_end(pa);
    return 0;
}

int putChar(char c) {
	return write(&c, 1, WHITE_COLOR);
}

int println(const char *str) {
	print(str);
	print("\n");
	return 0;
}

int printError(const char *str) {
	return printColored(str, ERROR_COLOR);
}

int printDec(uint64_t value) {
	return printBase(value, 10);
}

int printOct(uint64_t value) {
	return printBase(value, 8);
}

int printHex(uint64_t value) {
	return printBase(value, 16);
}

int print64Hex(uint64_t value) {
	int digits = uintToBase(value, buffer, 16);
	_64Hexfill(16 - digits, buffer);
	return print(buffer);
}

static void _64Hexfill(int n, char * buffer) {
	for (int i = 15; i >= 0; i--) {
		if (i >= n)
			buffer[i] = buffer[i - n];
		else
			buffer[i] = '0';
	}
	buffer[16] = 0;
}
 
int printBase(uint64_t value, uint32_t base) { // Had to change definition (now it returns the amount of digits, is it okay?)
    int digits = uintToBase(value, buffer, base);
    print(buffer);
	return digits;
}

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
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

int deleteChar(void){
	return _sys_screen((void *)(uint64_t) 2,0,0,0);
}

int deleteNChars(int n){
	for (int i = 0; i < n; i++)
		deleteChar();
	return 0;
}

// ----------- Video ------------

void paintScreen(uint32_t color) {
	_sys_video( 0, (void *)(uint64_t) color, 0, 0, 0);
}

unsigned int getScreenWidth() {
	return _sys_video((void *)(uint64_t) 1, 0, 0, 0, 0);
}

unsigned int getScreenHeight() {
	return _sys_video((void *)(uint64_t) 2, 0, 0, 0, 0);
}

void drawEllipse(int x, int y, unsigned int a, unsigned int b, uint32_t color) {
	point p = {x, y};
	_sys_video((void *)(uint64_t) 3, (void *)(uint64_t) &p, (void *)(uint64_t) a, (void *)(uint64_t) b, (void *)(uint64_t) color);
}

void drawCircle(int x, int y, unsigned int r, uint32_t color) {
	drawEllipse(x,y,r,r,color);
}

void drawRectangle(int x, int y, unsigned int b, unsigned int h, uint32_t color) {
	point p = {x, y};
	_sys_video((void *)(uint64_t) 4, (void *)(uint64_t) &p, (void *)(uint64_t) b, (void *)(uint64_t) h, (void *)(uint64_t) color);
}

void drawSquare(int x, int y, unsigned int l, uint32_t color) {
	drawRectangle(x,y,l,l,color);
}

// ----------- Sound ------------

#define BEEP_FREQ 350

void play_timed_sound(uint32_t freq, long duration) {
	_sys_sound((void *)(uint64_t) 2, (void *)(uint64_t) freq, (void *)(uint64_t) duration);
}

void play_sound(uint32_t freq) {
	_sys_sound((void *)(uint64_t) 1, (void *)(uint64_t) freq, 0);
}

void shut_sounds() {
	_sys_sound(0, 0, 0);
}

void beeps(uint32_t freq) {
	play_timed_sound(freq, 1);
}

// ----------- Strings ------------

unsigned int strlen(const char *str) {
	int i = 0; 
	while (str[i++] != 0);
	return i - 1;
}

int strcmp(const char *s1, const char *s2) {
    unsigned char c1, c2;
    while ((c1 = *s1++) == (c2 = *s2++)) {
        if (c1 == '\0')
            return 0;
    }
    return c1 - c2;
}

int strncmp(const char *s1, const char *s2, unsigned int n) {
    unsigned char c1, c2;
	int i = 1;
    while ((c1 = *s1++) == (c2 = *s2++)) {
        if (c1 == '\0' || i == n)
            return 0;
		i++;
    }
    return c1 - c2;	
}

int strcpy(char *dst, const char *src) {
	int i = 0;
	do {
		dst[i] = src[i];
	} while(src[i++] != 0);
	return 0;
}

long int strtoint(char* s){
	long int num = 0;
	int neg = 0; 
	int i = 0;

	if (s[i] == '-') {
		neg = 1;
		i++;
	}
    while (s[i]) { 
        num = num * 10 + s[i] - '0'; 
		i++;
	}
	
	if (neg)
		num = -num;

    return num; 
}
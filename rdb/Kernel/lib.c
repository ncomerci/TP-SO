#include <stdint.h> 

static int wrapSprintf(char * buff, const char *format, va_list pa);

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

unsigned int strlen(const char *str) {
	int i = 0; 
	while (str[i++] != 0);
	return i - 1;
}

int strcpy(char *dst, const char *src) {
	int i = 0;
	do {
		dst[i] = src[i];
	} while(src[i++] != 0);
	return 0;
}

int strcat(char *dst, const char *src) {
	return strcpy(dst + strlen(dst), src);
}

int strcmp(const char *s1, const char *s2) {
    unsigned char c1, c2;
    while ((c1 = *s1++) == (c2 = *s2++)) {
        if (c1 == '\0')
            return 0;
    }
    return c1 - c2;
}

int sprintf(char * buff, const char *format, ...) {
    va_list pa;  // Lista de parámetros
    va_start(pa, format);
	int ret = wrapSprintf(buff, format, pa);
    va_end(pa);
    return ret;
}

static int wrapSprintf(char * buff, const char *format, va_list pa) {
	unsigned int i = 0;
    char *tmp;
	int num;
	uint64_t unum;
	int size;

    while (*format != '\0') {
        if (*format != '%') {
            buff[i++] = *format;
            format++;
            continue;
        }

        format++;

		switch(*format) {
			case 'd': // Si es un decimal
				num = va_arg(pa, int);
				if (num < 0) {
					buff[i++] = '-'; // Agrego signo
					num = -num; // Convierto en positivo
				}
				size = uintToBase(num, buff + i, 10);
				i += size;
				break;
			case 'u':
				unum = va_arg(pa, int);
				size = uintToBase(unum, buff + i, 10);
				i += size;
				break;				
			case 'p': // Si es pointer
				buff[i++] = '0';
				buff[i++] = 'x';
			case 'x': // Si es hexadecimal
				num = va_arg(pa, uint64_t);
				size = uintToBase(num, buff + i, 16);
				i += size;
				break;
			case 'X':
				num = va_arg(pa, unsigned int);
				size = uintToBase(num, buff + i, 16);
				_64Hexfill(16 - size, buff + i);
				i += 16;
				break;
			case 'o': // Si es octal
				num = va_arg(pa, unsigned int);
				size = uintToBase(num, buff + i, 8);
				i += size;
				break;
			case 'c':
				num = va_arg(pa, int);
				buff[i++] = num;
				break;
			case 's':
				tmp = va_arg(pa, char *);
				size = strcpy(buff + i, tmp) - 1;
				i += size;
				break;
		}
        format++;
    }
	buff[i++] = '\0';
	return i;
}
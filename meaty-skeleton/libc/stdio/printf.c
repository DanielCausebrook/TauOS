#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


static bool print(const char *data, size_t length) {
    const unsigned char *bytes = (const unsigned char *) data;
    for (size_t i = 0; i < length; i++)
        if (putchar(bytes[i]) == EOF)
            return false;
    return true;
}

int printf(const char *restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;
    int flags = 0;
    int lFlag = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (!flags && (format[0] != '%' || format[1] == '%')) {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, amount))
                return -1;
            format += amount;
            written += amount;
            continue;
        }

        const char *format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(&c, sizeof(c)))
                return -1;
            written++;
        } else if (*format == 's') {
            format++;
            const char *str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len))
                return -1;
            written += len;
        } else if (*format == 'X') {
            format++;
            char c = (char) va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
            }
            // Get each half of the byte and convert it to a character.
            for (int i = 1; i >= 0; i--) {
                int t = (c >> (i * 4)) & 0xF;
                char o = t + ((t<0xA)?'0':'7');
                if (!print(&o, sizeof(o)))
                    return -1;
                written++;
            }
        } else if (*format == 'd') {
            format++;
            long c;
            int nDigits;
            if(lFlag) {
                if(!flags) return -1;
                c = va_arg(parameters, long);
                nDigits = 20;
                flags = 0;
                lFlag = 0;
            } else {
                c = va_arg(parameters, int);
                nDigits = 10;
            }
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            char num[nDigits];
            for (int i = nDigits - 1; i >= 0; i--) {
                int r = (int) (c % 10);
                c = c / 10; // Rounds down
                num[i] = (char) ('0' + r);
            }
            int leadingZero = 1;
            for(int i = 0; i < nDigits; i++) {
                if(leadingZero && num[i] == '0' && i != nDigits - 1) continue;
                leadingZero = 0;
                if (!print(&num[i], sizeof(char)))
                    return -1;
                written++;
            }
        } else if (*format == 'u') {
            format++;
            unsigned long c;
            int nDigits;
            if(lFlag) {
                if(!flags) return -1;
                c = va_arg(parameters, unsigned long);
                nDigits = 20;
                flags = 0;
                lFlag = 0;
            } else {
                c = va_arg(parameters, unsigned int);
                nDigits = 10;
            }
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            char num[nDigits];
            for (int i = nDigits - 1; i >= 0; i--) {
                int r = (int) (c % 10);
                c = c / 10; // Rounds down
                num[i] = (char) ('0' + r);
            }
            int leadingZero = 1;
            for(int i = 0; i < nDigits; i++) {
                if(leadingZero && num[i] == '0' && i != nDigits - 1) continue;
                leadingZero = 0;
                if (!print(&num[i], sizeof(char)))
                    return -1;
                written++;
            }
        } else if (*format == 'l') {
            flags = 1;
            lFlag = 1;
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, len))
                return -1;
            written += len;
            format += len;
        }
        if(flags == 1) flags = 2;
        else if(flags == 2) return -1;
    }

    va_end(parameters);
    return written;
}

#include "stdio_simple.h"
#include "../flyx5_hw.h"
#include "../common.h"

void _puts(const char* str)
{
	UARTStringPut(BASE_PERIPH(UART_DEBUG), str);
}

/**
 * Send a string to the UART, blocks execution until done.
 *
 * String must be null terminated.
 */
void UARTStringPut(uint32_t ui32Base, const char *s)
{
    char c;

    while((c = *s) != '\0') {
        R_(UARTCharPut)(ui32Base, c);
        s++;
    }
}

/**
 * Convert int to string and send to UART, blocks execution until done.
 */
void UARTIntPut(uint32_t ui32Base, int x)
{
#define INT_MAX_S_LEN 32
    char s[INT_MAX_S_LEN];
    int i = INT_MAX_S_LEN-2;

    s[INT_MAX_S_LEN-1] = 0;

    while (x) {
        s[i--] = x%10 + '0';
        x = x/10;
    }

    UARTStringPut(ui32Base, s + i + 1);
}

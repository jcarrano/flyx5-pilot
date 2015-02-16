/**
 * Entry point for the flyx5 firmware.
 *
 * @author Juan I. Carrano
 *
 * @copyright Copyright 2014 Juan I Carrano, Andrés Calcabrini, Juan I Ubeira,
 *              Nicolás venturo.
 */

#include "common.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "flyx5_hw.h"
#include "hw_init.h"

#include "peripheral/buzzer.h"

/**
 * This error routine that is called if the driver library encounters an error.
 */
#ifdef DEBUG
void __error__(char *filename, uint32_t line)
{
    // FIXME: unimplemented
}
#endif

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
 * Send a string to the UART, blocks execution until done.
 *
 * String must be null terminated.
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

int main(void)
{
    init_failsafe();

    init_clock();

    //init_pins();
    /* Initialise all ports */
    init_all_gpio();

    if (!running_under_debugger()) {
            SysCtlDelay(10000000);
            init_jtag_muxed_pins();
    }

    /* Initialize port */
    ENABLE_AND_RESET(UART_DEBUG);
    CFG_PIN(DEBUG_RX);
    CFG_PIN(DEBUG_TX);

    R_(UARTConfigSetExpClk)(BASE_PERIPH(UART_DEBUG) , R_(SysCtlClockGet)(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

#define HELLO_TXT "Hello, nigga!\r\n"
    UARTStringPut(BASE_PERIPH(UART_DEBUG), HELLO_TXT);
#define CLK_TXT "Clock speed is: "
#define ENDL "\r\n"
    UARTStringPut(BASE_PERIPH(UART_DEBUG), CLK_TXT);
    UARTIntPut(BASE_PERIPH(UART_DEBUG), R_(SysCtlClockGet)());
    UARTStringPut(BASE_PERIPH(UART_DEBUG), ENDL);

    buzzer_init();

    buzzer_play_note(16667, 16667/2);

    //
    // Loop forever.
    //
    while(1)
    {
        int c = R_(UARTCharGet)(BASE_PERIPH(UART_DEBUG));
        c = (c <= 'z' && c >= 'a')? c - 'a' + 'A' : c;
        R_(UARTCharPut)(BASE_PERIPH(UART_DEBUG), c);

        buzzer_play_note(16667, (16667/4)*3);

        c = R_(UARTCharGet)(BASE_PERIPH(UART_DEBUG));
        R_(UARTCharPut)(BASE_PERIPH(UART_DEBUG), c);

        buzzer_play_note(16667, 16667/2);
    }
}

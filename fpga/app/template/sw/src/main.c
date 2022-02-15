#include <stdint.h>

#include "finka.h"

void print(const char*str){
    while(*str){
        uart_write(UART,*str);
        str++;
    }
}
void println(const char*str){
    print(str);
    uart_write(UART, '\r');
    uart_write(UART, '\n');
}

void delay(uint32_t loops){
    for(int i=0;i<loops;i++){
        int tmp = GPIO_A->OUTPUT;
    }
}

void main() {
    Uart_Config uartConfig;
    uartConfig.dataLength = 8;
    uartConfig.parity = NONE;
    uartConfig.stop = ONE;
    // 250 MHz core clock, 115200 baud
    uartConfig.clockDivider = (250000000ULL/8/115200)-1;
    uart_applyConfig(UART, &uartConfig);

    GPIO_A->OUTPUT_ENABLE = 0x0000000F;
    GPIO_A->OUTPUT = 0x00000001;

    println("Hello world!");
    const int nleds = 4;
    const int nloops = 2000000;
    while (1) {
        uint32_t timer_value = TIMER_A->VALUE;
        println("Hello world!\n");
        for(unsigned int i=0;i<nleds-1;i++){
            GPIO_A->OUTPUT = 1<<i;
            delay(nloops);
        }
        for(unsigned int i=0;i<nleds-1;i++){
            GPIO_A->OUTPUT = (1<<(nleds-1))>>i;
            *AXI_A = 0xABCDEF12;
            *AXI_A = 0xABCDEF34;
            *AXI_A = 0xABCDEF56;
            delay(nloops);
        }
    }
}

void irqCallback(){
}

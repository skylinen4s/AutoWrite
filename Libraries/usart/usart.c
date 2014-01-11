#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void init_USART3(void) {                                                                                                                                                                       
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	USART_InitStruct.USART_BaudRate = 9600;

	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(USART3, &USART_InitStruct);
	USART_Cmd(USART3, ENABLE);
}

void USART3_Interrupt(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable transmit and receive interrupts for the USART3. */
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	/* Enable the USART3 IRQ in the NVIC module (so that the USART3 interrupt
	* handler is enabled). */                                                                                                                                                                                       
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
#if 0
char USART3_Printf(char *string){
    while(*string){
        // Send message to USART3 //
        USART_SendData(USART3, *string++);
        // Wait for sending message //
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}


char rec_char = 0;
volatile char rec_str[29+3];

void USART3_IRQHandler()
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) )
	{
        static uint8_t cnt = 0;

		rec_char = USART_ReceiveData(USART3);

        if(rec_char != '\r')
        {
            rec_str[cnt++] = rec_char;
        }
        else if((rec_char == '\r') || (rec_char == '\n'))
        {
            rec_str[cnt] = '\0';
            cnt = 0;
            USART3_Printf(rec_str);
            USART3_Printf("\n\r");
            check();
        }
    }
}
void check()
{
    if(!strncmp(rec_str, "abc", 3))
    {       
        GPIO_SetBits(GPIOD,GPIO_Pin_12);
        GPIO_ResetBits(GPIOD,GPIO_Pin_15);
        USART3_Printf("ABC \n\r");
    } 
    else if(!strncmp(rec_str, "123", 3))
    {       
        USART3_Printf("321 \n\r");
        GPIO_SetBits(GPIOD,GPIO_Pin_13);
        GPIO_ResetBits(GPIOD,GPIO_Pin_15);
    }   
    else if(!strncmp(rec_str, "sky", 3))
    {       
        USART3_Printf("yks \n\r");
        GPIO_SetBits(GPIOD,GPIO_Pin_14);
        GPIO_ResetBits(GPIOD,GPIO_Pin_15);
    }
    else{

        GPIO_ResetBits(GPIOD,GPIO_Pin_12);
        GPIO_ResetBits(GPIOD,GPIO_Pin_13);
        GPIO_ResetBits(GPIOD,GPIO_Pin_14);
        GPIO_ResetBits(GPIOD,GPIO_Pin_15);
    }
}
#endif

void USART_puts(volatile char *s){

    while(*s){
        // wait until data register is empty
        while( !(USART3->SR & 0x00000040) ); 

        USART_SendData(USART3, *s);
        *s++;
    }
}

void USART3_IRQHandler(void){
    GPIO_SetBits(GPIOD,GPIO_Pin_15);
    // check if the USART2 receive interrupt flag was set
    if( USART_GetITStatus(USART3, USART_IT_RXNE) ){

    // send back 
    USART_SendData( USART3, USART3->DR );

    }
}
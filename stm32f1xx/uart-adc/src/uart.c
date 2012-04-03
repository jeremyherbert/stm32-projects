#include "stm32f10x.h"
#include "uart.h"

#define CPU_SPEED 24000000

void uart1_init(uint32_t baud) {
  // turn on usart1, gpioa, afio clocks
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // set the baud rate
  USART1->BRR = CPU_SPEED/baud;
  
  // set PA9 to AFIO, push-pull, 50mhz (0xB)
  // set PA10 to floating input (0x4) 
  GPIOA->CRH = 0x00004BB;
  
  // enable interrupt in the core
  NVIC->ISER[1] = (1<<(USART1_IRQn & 0x1F));

  // enable usart, tx, rx, rx interrupt
  USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void uart1_tx(char c) {
  while (!(USART1->SR & USART_SR_TXE));
  USART1->DR = c;
}

void uart1_tx_string(char *c) {
  uint32_t i;
  for(i=0; c[i] != '\0'; i++) uart1_tx(c[i]);
}


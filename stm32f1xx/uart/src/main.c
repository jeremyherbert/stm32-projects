#include "stm32f10x.h"

#define CPU_SPEED 24000000

void uart1_init(uint32_t baud) {
  // turn on usart1, gpioa, afio clocks
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // set the baud rate
  USART1->BRR = CPU_SPEED/baud;
  
  // set PA9 to AFIO, push-pull, 50mhz (0xB)
  // set PA10 to floating input (0x4) 
  GPIOA->CRH = 0x00004B0;
  
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

void USART1_IRQHandler(void) {
  if (USART1->SR & USART_SR_RXNE) {
    USART1->SR &= ~USART_SR_RXNE; // turn off status flag
    uart1_tx(USART1->DR);
  }
    GPIOC->ODR = 0xFFFFFFFF;
}

void TIM2_IRQHandler(void) {
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~TIM_SR_UIF;
    GPIOC->ODR ^= 0xFFFFFFFF;
  }
}

int main(void)
{
  //RCC->CR |= RCC_CR_PLLON; // PLL on!
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

  // usart config
  uart1_init(9600);
  uart1_tx_string("Just booted up!\n");

  // configure portc lower to be output
  GPIOC->CRL = 0x11111111; // all general purpose push-pull output
  GPIOC->CRH = 0x11111111;
  GPIOC->ODR = 0x00000000;

  // enable tim2 (general purpose timer)
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  //TIM2->CCR1 = 0x1000; // count to 1
  NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F)); // enable position 28 interrupt (TIM2)
  TIM2->PSC = 0x20;
  TIM2->DIER |= TIM_DIER_UIE; // compare 1 interrupt enable
  TIM2->ARR = 0xFFFF;
  TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // enable the clock
  TIM2->EGR = 1;	
  
  while(1);
}

#include "stm32f10x.h"

void TIM2_IRQHandler(void) {
  GPIOC->ODR ^= 0xFFFFFFFF;
}

int main(void)
{
//  RCC->CR |= RCC_CR_PLLON; // PLL on!
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN; // turn on port a, b, c clocks

  // configure portc lower to be output
  GPIOC->CRL = 0x11111111; // all general purpose push-pull output
  GPIOC->CRH = 0x11111111;
  GPIOC->ODR = 0xFFFFFFFF;

  // enable tim2 (general purpose timer)
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  TIM2->CCR1 = 0x1; // count to 1
  TIM2->DIER |= TIM_DIER_CC1IE | TIM_DIER_TIE | TIM_DIER_UIE;	
  TIM2->CR1 |= TIM_CR1_CEN; // enable the clock
  NVIC->ISER[0] |= (1 << (28 & 0x1F));
  
  while(1);
}

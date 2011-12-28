#include "stm32f4xx_conf.h"

void TIM2_IRQHandler(void) {
  // flash on update event
  if (TIM2->SR & TIM_SR_UIF) GPIOC->ODR ^= 0xFFFFFFFF;
  TIM2->SR = 0x0;
}

int main(void)
{
  // turn the PLL on
  RCC->CR |= RCC_CR_PLLON; // turn the PLL on for high speed
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // turn on PORTC clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // enable tim2 clock

  GPIOC->MODER = 0x55555555; // all outputs
  GPIOC->OTYPER = 0x0; // all push-pull
  
  GPIOC->ODR = 0xFFFFFFFF; // set all pins to high 

  NVIC->ISER[0] |= (1<< (TIM2_IRQn & 0x1F)); // enable the tim2 irq

  TIM2->PSC = 0x0; // no prescaler
  TIM2->DIER |= TIM_DIER_UIE; // enable update interrupt
  TIM2->ARR = 0x01; // count to 1 (autoreload value 1)
  TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // autoreload on, counter enabled
  TIM2->EGR = 1; // trigger update event to reload timer registers

  while(1);
}


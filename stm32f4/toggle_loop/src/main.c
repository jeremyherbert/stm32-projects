#include "stm32f4xx_conf.h"

int main(void)
{
  // turn the PLL on
  RCC->CR |= RCC_CR_PLLON; // turn the PLL on for high speed
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // turn on PORTC clock

  GPIOC->MODER = 0x55555555; // all outputs
  GPIOC->OTYPER = 0x0; // all push-pull
  GPIOC->OSPEEDR = 0xFFFFFFFF; // 100mhz output speed 

  GPIOC->ODR = 0xFFFFFFFF; // set all pins to high 

  while(1) GPIOC->ODR ^= 0xFFFFFFFF;
}


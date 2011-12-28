#include "stm32f10x.h"

volatile char sine_counter;
volatile int sine_table[] = {0x800, 0x880, 0x900, 0x97f, 0x9fd, 0xa78, 0xaf1, 0xb67, 0xbda, 0xc49, 0xcb3, 0xd19, 0xd79, 0xdd4, 0xe2a, 0xe78, 0xec1, 0xf02, 0xf3d, 0xf70, 0xf9b, 0xfbf, 0xfdb, 0xfef, 0xffb, 0xfff, 0xffb, 0xfef, 0xfdb, 0xfbf, 0xf9b, 0xf70, 0xf3d, 0xf02, 0xec1, 0xe78, 0xe2a, 0xdd4, 0xd79, 0xd19, 0xcb3, 0xc49, 0xbda, 0xb67, 0xaf1, 0xa78, 0x9fd, 0x97f, 0x900, 0x880, 0x7ff, 0x77f, 0x6ff, 0x680, 0x602, 0x587, 0x50e, 0x498, 0x425, 0x3b6, 0x34c, 0x2e6, 0x286, 0x22b, 0x1d5, 0x187, 0x13e, 0xfd, 0xc2, 0x8f, 0x64, 0x40, 0x24, 0x10, 0x4, 0x0, 0x4, 0x10, 0x24, 0x40, 0x64, 0x8f, 0xc2, 0xfd, 0x13e, 0x187, 0x1d5, 0x22b, 0x286, 0x2e6, 0x34c, 0x3b6, 0x425, 0x498, 0x50e, 0x587, 0x602, 0x680, 0x6ff, 0x77f};

void TIM2_IRQHandler(void) {
  if (TIM2->SR & TIM_SR_UIF) {
    GPIOC->ODR ^= 0xFFFFFFFF;
    DAC->DHR12R1 = sine_table[sine_counter++]; // load the next piece of data into the dac data register
    if (sine_counter > 99) sine_counter = 0; // reset the counter
  }
  TIM2->SR = 0; // clear the status reg
}

int main(void)
{
  sine_counter = 0;

  RCC->CR |= RCC_CR_PLLON; // PLL on!
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // turn on portc clock

  // configure portc lower to be output
  GPIOC->CRL = 0x11111111; // all general purpose push-pull output
  GPIOC->CRH = 0x11111111;
  GPIOC->ODR = 0xFFFFFFFF; // set all pins high

  // enable tim2 (general purpose timer)
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F)); // enable position 28 interrupt (TIM2)
  TIM2->PSC = 0x0; // no prescaler
  TIM2->DIER |= TIM_DIER_UIE; // compare 1 interrupt enable
  TIM2->ARR = 0x30; // count to 50
  TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // enable the clock
  TIM2->EGR = 1;	
  
  // do the dac
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // enable port a clock for dac output
  GPIOA->CRL = 0x00000000; // set to analog

  RCC->APB1ENR |= RCC_APB1ENR_DACEN; // enable dac clock
  DAC->CR |= DAC_CR_EN1; // enable dac 1
  while(1);
}

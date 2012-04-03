#define CPU_SPEED 24000000

#include "stm32f10x.h"
#include "uart.h"
void USART1_IRQHandler(void) {
  if (USART1->SR & USART_SR_RXNE) {
    USART1->SR &= ~USART_SR_RXNE; // turn off status flag
    uart1_tx(USART1->DR);
    GPIOC->ODR = 0;
  }
}

void TIM2_IRQHandler(void) {
  int i;
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~TIM_SR_UIF;
  }
  if (TIM2->SR & TIM_SR_CC2IF) {
    TIM2->SR &= ~TIM_SR_CC2IF;
    GPIOC->ODR |= (1<<9);
  }
}

void adc1_init() {
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // clock on
  ADC1->CR1 = ADC_CR1_EOCIE; // eoc interrupt, channel 0
  ADC1->CR2 = ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0; // external trigger for injected, tim2 TRGO trigger
  ADC1->CR2 = ADC_CR2_ADON; // wake up ADC (needs to be done with its own write)
  NVIC->ISER[0] |= (1 << (ADC1_IRQn & 0x1F)); // IRQ enable
}

void ADC1_IRQHandler(void) {
  if (ADC1->SR & ADC_SR_EOC) {
    ADC1->SR &= ~ADC_SR_EOC;
    uart1_tx(48 + ADC1->DR/410);
  }
}

int main(void)
{
  int j = 0;
  //RCC->CR |= RCC_CR_PLLON; // PLL on!
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

  // usart config
  uart1_init(115200);
  uart1_tx_string("Just booted up!\n");
  adc1_init();
  // configure portc lower to be output
  GPIOC->CRL = 0x11111111; // all general purpose push-pull output
  GPIOC->CRH = 0x11111111;
  GPIOC->ODR = 0x00000000;
  ADC1->CR2 = ADC_CR2_ADON;
  // enable tim2 (general purpose timer)
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F)); // enable position 28 interrupt (TIM2)
  TIM2->PSC = 0xFFFF;
  TIM2->DIER = TIM_DIER_UIE | TIM_DIER_CC2IE; // compare 1 interrupt enable
  TIM2->ARR = 0xF;
  TIM2->CCR2 = 0x6;
  TIM2->CR1 = TIM_CR1_CEN; // enable the clock

  TIM2->EGR = 1;
  
  // init dac
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRL = 0;
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  DAC->CR |= DAC_CR_EN1;
  
  while(1) {
    if (TIM2->CNT != 0x6) GPIOC->ODR &= ~(1<<9);
    DAC->DHR12R1 = TIM2->CNT*256;
  }
}

#include "stm32f10x.h"
#include "stm32f10x_it.h"

static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime);
void TimingDelay_Decrement(void);

int main(void)
{
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }

  // PLL on, change to 24MHz
  RCC->CR |= (1<<24);

  RCC->APB2ENR |= (1<<4); // turn on GPIOC clock
  GPIOC->CRL = 0x11111111; // set all pins to push/pull
  GPIOC->CRH = 0x11111111;
  
  int16_t i = 1;
  int16_t direction = 1;
  while(1)
  {
	Delay(i);
	GPIOC->ODR ^= (1<<9); // toggle the green LED
  	i+=5*direction;
	if (i > 150) direction = -1;
	if (i < 1) {
		direction = 1;
		i=1;
	}
	
	// if the PLL is locked, light up the blue LED
	if (RCC->CR & (1<<25)) GPIOC->BSRR = (1<<8);
	else GPIOC->BRR = (1<<8);
  }
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

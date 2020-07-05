#include "stm32l1xx.h"
volatile uint32_t DEBUG_VAR = 0;
volatile uint32_t BACKGROUND = 0;
volatile uint32_t ADC_VAR = 0;
volatile uint32_t SYSTICK = 0;

volatile uint32_t msTicks = 0;        /* Variable to store millisecond ticks */

void init_ADC(void);

void SysTick_Handler(void)
{   /* SysTick interrupt Handler. */
	  SYSTICK = 1;
    msTicks++;                       
}

void init_ADC(void)
{
		/* We want to check if 01(0x1): HSI oscillator is used as system clock by checking RCC->CFGR SWS Flag. 
		   Bits 3:2 SWS[1:0]: System clock switch status is checked by masking(bitwise AND)	first RCC->CFGR with 0b1100(0xC)
   		 and then checking for equality with 0b0100(0x4) */
	  while((RCC->CFGR & 0xC) == RCC_CFGR_SWS_HSI){
			  DEBUG_VAR = 0xFF88EEDD;         /* Hardware latency until it finally indicates SWS status as HSI oscillator used. */
		}
		
		RCC->CR |= (1UL << 0);              /* Set Bit 0 HSION: Internal high-speed clock enable */
		while((RCC->CR & RCC_CR_HSIRDY) == RCC_CR_HSIRDY){  /* Check RCC_CR_HSIRDY flag Bit 1 */
		}
    RCC->CFGR |= (0UL << 11);          /* Clock configuration register (RCC_CFGR). Clear Bits 13:11 0xx: HCLK not divided PPRE2[2:0]: APB high-speed prescaler (APB2) */
		
		/* Enable ADC1 clock */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	  RCC->APB2ENR |= (1UL << 9);        // Set bit 9 to enable ADC1 peripheral clock 
		
    ADC1->CR2 &= ~ADC_CR2_ADON;        /* Clear Bit 0 ADON: A/D Converter ON / OFF. Or ADC1->CR2 |= (0UL << 0); */
		ADC1->CR2 |= (0UL << 2);           /* Clear Bit 2 ADC_CFG: ADC configuration. 0: Bank A selected for channels ADC_IN0..31. line 1198 ADC_CR2_CFG stm32l1xx.h */
		
		ADC1->CR2 |= ADC_CR2_CONT;         /* Enable continuous conversiont */
		ADC1->CR2 &= ~ADC_CR2_EXTSEL;      /* Clear Bits 27:24 EXTSEL[3:0]: External event select for regular group */
		ADC1->CR2 |= (4UL << 24);          /* Select 0100(0x4): TIM3_TRGO event ADC trigger.  */
		
		
		ADC1->CR2 |= (1UL << 0);           /* Enable ADC. Set Bit 0. Or ADC1->CR2 &= ~ADC_CR2_ADON */
		while((ADC1->SR & 0x40) == ADC_CR2_ADON){  /* ADC1->SR Bit 6 ADONS: ADC ON status. mask 0b1000000(0x40) */
		    DEBUG_VAR = 0xBEEF012D3;       /* ADC not enabled yet so not ready to convert via ADONS ADC1->SR Bit 6 flag */
		}
		ADC1->SMPR3 |= (7UL << 0);         /* Configure sample time for channel 1 ADC_IN1 , slowest or max cycles to be safe */
		ADC1->CR2 |= ADC_CR2_SWSTART;      /* Start conversion of regular channel */		
}

void init_TIM3(){
	  RCC->APB1ENR	|= RCC_APB1ENR_TIM3EN;  /* Enable TIM3 clock */
		TIM3->PSC		= 16000000/10000 - 1;		// 16MHz is default internal clock. Prescaler value
		TIM3->ARR		= 1000;								// Auto-reload value
		
		TIM3->CCMR1	&= ~TIM_CCMR1_CC1S;			// ~0b11(~0x3) = 0x00: Clear Bits [1:0] CC1S: Capture/Compare 1 selection
		TIM3->CCMR1	|= (0UL << 0);          /* Clear Bits 1:0 CC1S: Capture/Compare 1 selection. 00: CC1 channel is configured as output. */
		TIM3->CCER	|= (1<<0);
		TIM3->CCR1	= 500;								// Compare and output register channel 1
	TIM3->CR1 |= (1UL << 2);
	TIM3->EGR |= (1UL << 0);
		TIM3->CR1		= TIM_CR1_CEN;				// Enable timer 3
		NVIC_SetPriority(TIM3_IRQn, 0x03);	// Set EXTI0 priority 3(low priority)
		NVIC_EnableIRQ(TIM3_IRQn);					// Enable EXTI0 interrupt
}

void ADC1_IRQHandler(void)
{
    int16_t i;

    BACKGROUND = 0;                             /* Clear all the toggle bits */
    SYSTICK = 0;
  
	for (i = 0;i < 0x1000;i++)
	{
		ADC_VAR = 1;                            /* Set the ADC  toggle bit */
	}
    ADC1->SR &= ~(1UL << 1);                    /* ADC status register (ADC_SR), Bit 1 EOC: Regular channel end of conversion. Clear bit 1 EOC set by hardware */
    ADC_VAR = 0;                                 /* Clear the ADC toggle bit */
}

int main(void){
    uint32_t returnCode;

    returnCode = SysTick_Config(SystemCoreClock / 1000);   

    if(returnCode != 0){                               
    // Error Handling
    }
		init_TIM3();
		init_ADC();

		
    while(1){
			BACKGROUND = 0;
		}
}

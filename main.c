#ifndef _NSC_MAIN_H
#define _NSC_MAIN_H
#define GPIO_AF2            0x2

#include "stm32l1xx.h"
#include "nc_stm32l1_rcc.h"
#include "nc_stm32l1_adc.h"
#include "nc_defines.h"

volatile uint32_t DEBUG_VAR = 0;      /* Declare global variables(outside main) "volatile" to force compiler to generate
                                         code that rereads variables stored in memory & not just in registers. */
volatile uint8_t BACKGROUND = 0;
volatile uint32_t SYSTICK = 0;
volatile uint32_t msTicks = 0;        /* Variable to store millisecond ticks */
volatile uint32_t ADC_VAR = 0;
volatile uint32_t TIM3_VAR = 0;
volatile uint32_t Result = 0;
volatile uint32_t Result_1 = 0;
volatile uint32_t Result_2 = 0;
volatile uint32_t Result_3 = 0;
volatile uint32_t Result_4 = 0;

#endif
void init_ADC(void);

void SysTick_Handler(void){           /* SysTick interrupt Handler. */
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

//    ADC_DeInit(ADC1);                           /* Put everything back to power-on defaults */

//    ADC_InitTypeDef  ADC_InitStructure;         /* Declare typedef pointer ADC_InitTypeDef. line 83 typedef in nc_stm32l1_adc.h */

//    ADC_StructInit(&ADC_InitStructure);         /* Fills each ADC_InitStruct member with its default value. line 283 stm32l1xx_adc.c */
    //ADC1->CR2 |= ADC_CR2_CONT;                  /* Set bit 1 to enable continuous conversion. Manual configure but auto default in ADC_StructInit */
 //   ADC_Init(ADC1, &ADC_InitStructure);         /* Initializes the ADCx peripheral according to the specified parameters in the ADC_InitStruct. */


    ADC1->CR2 &= ~ADC_CR2_ADON;        /* Clear Bit 0 ADON: A/D Converter ON / OFF. Or ADC1->CR2 |= (0UL << 0); */
		ADC1->CR2 |= (0UL << 2);           /* Clear Bit 2 ADC_CFG: ADC configuration. 0: Bank A selected for channels ADC_IN0..31. line 1198 ADC_CR2_CFG stm32l1xx.h */

		ADC1->CR2 |= ADC_CR2_CONT;         /* Enable continuous conversiont */
		ADC1->CR2 &= ~ADC_CR2_EXTSEL;      /* Clear Bits 27:24 EXTSEL[3:0]: External event select for regular group */
		ADC1->CR2 |= (4UL << 24);          /* Select 0b0100(0x4): TIM3_TRGO event ADC trigger. Select 0b0111(0x7): TIM3_CC1 event. */

		/* Configure ADC regular sequence. 1) ADC regular sequence register 1 (ADC_SQR1). Bits 24:20 L[4:0]: Regular channel sequence length. These
       bits are written by software to define the total number of conversions in the regular channel conversion sequence.
       00000: 1 conversion
       00001: 2 conversions
       ...
       11010: 27 conversions
       11011: 28 conversions.
       For this case, single channel & single conversion so we select 0x0 for bits 24:20. ADC1->SQR1 |= (0UL << 20);. */
    ADC1->SQR1 &= ~(ADC_SQR1_L);       /* Select single channel & single conversion. ADC1->SQR1 |= (0UL << 20); */
		/* The channel is selected in bank A or bank B depending on the ADC_CFG bit in the ADC_CR2 register. These bits are written by software with
       the channel number (0..31) assigned as the 6th in the sequence to be converted. Bits 4:0 SQ1[4:0]: 1st conversion in regular sequence. */
    ADC1->SQR5 = 0;                    /* Clear register. */
    //ADC1->SQR5 |= 10;
    ADC1->SQR5 |= (1UL << 4);          /* Place channel 1 in regular sequence. 0x10 = 0b10000 */
//		NVIC_SetPriority(ADC1_IRQn, 0x03); /* Set ADC1 priority 3(low priority) */
//		NVIC_EnableIRQ(ADC1_IRQn);         /* Enable ADC1 interrupt */
		ADC1->CR2 |= (1UL << 0);           /* Enable ADC. Set Bit 0. Or ADC1->CR2 &= ~ADC_CR2_ADON */
		while((ADC1->SR & 0x40) == ADC_CR2_ADON){  /* ADC1->SR Bit 6 ADONS: ADC ON status. mask 0b1000000(0x40) */
		    DEBUG_VAR = 0xBEEF012D3;       /* ADC not enabled yet so not ready to convert via ADONS ADC1->SR Bit 6 flag */
		}
		ADC1->SMPR3 |= (7UL << 0);         /* Configure sample time for channel 1 ADC_IN1 , slowest or max cycles to be safe */
		ADC1->CR2 |= ADC_CR2_SWSTART;      /* Start conversion of regular channel */
}

void init_TIM3(){
	  /* Configure channel 1 output of timer 3 used as trigger signal of ADC. Square wave output with frequency 1Hz and duty cycle 50% */
	  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; /* Enable TIM3 clock */
	  /* Output Compare Mode. Timer 3 channel 1 output 1Hz. page 395, 402 rm0038.
	     1) Select counter clock(internal, external, and prescaler. Default: SMS=000 TIMx_SMCR register.  */
		TIM3->PSC = 2097000/1000 - 1;     /* MSI 2.097MHz is default internal clock. Timer 3 counter clock 1KHz, Prescaler value */
		/* 2) Write desired data in TIMx_ARR and TIMx_CCRx registers. */
	  TIM3->ARR = 1000-1;               /* Auto-reload value */
    TIM3->CCR1 = 499;                 /* Output compare register for channel 1 */
	  /* 3) Set the CCxIE and/or CCxDE bits if an interrupt and/or a DMA request is to be generated. */
	  /* 4) Select output mode */
	  TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0;   /* OC1M = 110 for PWM output mode 1 on channel 1 */
		TIM3->CCMR1	|= TIM_CCMR1_OC1PE;   /* Enable preload for channel 1 */
		TIM3->CR1 |= TIM_CR1_ARPE;        /* Auto-reload preload enable */
	  /* 5) Enable Output Enable Circuit, TIMx->CCER CC1E */
  	TIM3->CCER |= TIM_CCER_CC1E;      /* Capture/Compare 1 output enable */
	  TIM3->EGR |= TIM_EGR_UG;          /* Update Generation, re-initialize timer counter */
		/* 6) Enable the counter by setting the CEN bit in the TIMx_CR1 register. */
	  TIM3->CR1 |= TIM_CR1_CEN;         /* Enable timer 3/counter */
}

void ADC1_IRQHandler(void)
{
    /* For regular channel, check End of Conversion (EOC) flag. Reading ADC data register (DR) clears the EOC flag.
       Regular channels share the single ADC->DR data register.	*/
    if(ADC1->SR & ADC_SR_EOC){
        Result = ADC1->DR;
    } else if(ADC1->SR & ADC_SR_JEOC){
        /* For injected channels, check the JEOC flag. Reading injected data registers doesn't clear JEOC flag.
           Eacg injected channel has its own dedicated data register. */
        Result_1 = ADC1->JDR1;        /* Injected channel 1 */
        Result_2 = ADC1->JDR2;        /* Injected channel 2 */
        Result_3 = ADC1->JDR3;        /* Injected channel 3 */
        Result_4 = ADC1->JDR4;        /* Injected channel 4 */
        ADC1->SR &= ~(ADC_SR_JEOC);   /* Clear JEOC flag */
    }
    int16_t i;

    BACKGROUND = 0;                   /* Clear all the toggle bits */
    SYSTICK = 0;

	  for (i = 0;i < 0x1000;i++)
	  {
		    ADC_VAR = 1;                  /* Set the ADC  toggle bit */
	  }
    ADC1->SR &= ~(1UL << 1);          /* ADC status register (ADC_SR), Bit 1 EOC: Regular channel end of conversion. Clear bit 1 EOC set by hardware */
    ADC_VAR = 0;                      /* Clear the ADC toggle bit */
		TIM3_VAR = 1;
}

void GPIO_Pin_Init(){
		/* MODER6[1:0] involving bits 12 & 13. 0b0010(0x02): Alternate function mode */
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(0x03 << 12);			// Clear mode bits
		GPIOB->MODER |= 0x02 << 12;					// Set pin PB.6 as alternate function
		GPIOB->AFR[0] &= ~0xF << (24);			// Clear bits 24-27
		GPIOB->AFR[0] |= GPIO_AF2 << (24);	// Set 0x02 mask alternate function
		
		GPIOB->OTYPER &= ~(0x1<<6);					// Set PB.6 pin as push-pull output type		
		
		// Set IO output speed
		GPIOB->OSPEEDR &= ~(0x03<<(2*6));
		GPIOB->OSPEEDR |= 0x03<<(2*6);
	
		// Set IO no pull-up pull-down
		GPIOB->PUPDR |= ~(0x00<<(2*6));	
}

int main(void){
    uint32_t returnCode;

    returnCode = 1; //SysTick_Config(SystemCoreClock / 1000);

    if(returnCode != 0){
    // Error Handling
    }
		GPIO_Pin_Init();
		init_TIM3();
	//	init_ADC();

    while(1){
			BACKGROUND = 1;
		}
}

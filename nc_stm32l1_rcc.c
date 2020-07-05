/**
 * @file		nc_stm32l1_rcc.c
 * @author  Noel Cruz
 * @email   noel_s_cruz@yahoo.com
 * @github  https://github.com/noey2020
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   RCC Library for STM32L1xx devices
 *
@verbatim
----------------------------------------------------------------------
Copyright (C) 2020, Noel Cruz

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@endverbatim
@verbatim        stm32l1xx_hal_rcc.c
==============================================================================
										##### RCC specific features #####
==============================================================================
	[..]
		After reset the device is running from multispeed internal oscillator clock
		(MSI 2.097MHz) with Flash 0 wait state and Flash prefetch buffer is disabled,
		and all peripherals are off except internal SRAM, Flash and JTAG.
		(+) There is no prescaler on High speed (AHB) and Low speed (APB) buses;
				all peripherals mapped on these buses are running at MSI speed.
		(+) The clock for all peripherals is switched off, except the SRAM and FLASH.
		(+) All GPIOs are in input floating state, except the JTAG pins which
				are assigned to be used for debug purpose.
	[..] Once the device started from reset, the user application has to:
		(+) Configure the clock source to be used to drive the System clock
				(if the application needs higher frequency/performance)
		(+) Configure the System clock frequency and Flash settings
		(+) Configure the AHB and APB buses prescalers
		(+) Enable the clock for the peripheral(s) to be used
		(+) Configure the clock source(s) for peripherals whose clocks are not
				derived from the System clock (I2S, RTC, ADC, USB OTG FS/SDIO/RNG)
				(*) SDIO only for STM32L1xxxD devices

										##### RCC Limitations #####
==============================================================================
	[..]
		A delay between an RCC peripheral clock enable and the effective peripheral
		enabling should be taken into account in order to manage the peripheral read/write
		from/to registers.
		(+) This delay depends on the peripheral mapping.
			(++) AHB & APB peripherals, 1 dummy read is necessary

	[..]
		Workarounds:
		(#) For AHB & APB peripherals, a dummy read to the peripheral register has been
				inserted in each __HAL_RCC_PPP_CLK_ENABLE() macro.

@endverbatim
 */

#include "nc_stm32l1_rcc.h"

//#ifdef NC_STM32L1_RCC_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup RCC_Private_Variables RCC Private Variables
  * @{
  */
extern const uint8_t PLLMulTable[];          /* Defined in CMSIS (system_stm32l0xx.c)
																							  stm32l1xx_hal_rcc.c line 96 */

//static __I uint8_t PLLMulTable[9] = {3, 4, 6, 8, 12, 16, 24, 32, 48};
static __I uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

/** @defgroup RCC_Group3 Peripheral clocks configuration functions
 *  @brief   Peripheral clocks configuration functions
 *
@verbatim
 ===============================================================================
             ##### Peripheral clocks configuration functions #####
 ===============================================================================
    [..] This section provide functions allowing to configure the Peripheral clocks.
         (#) The RTC/LCD clock which is derived from the LSE, LSI or 1 MHz HSE_RTC
         (HSE divided by a programmable prescaler).
         (#) After restart from Reset or wakeup from STANDBY, all peripherals are
             off except internal SRAM, Flash and JTAG. Before to start using a
             peripheral you have to enable its interface clock. You can do this
             using RCC_AHBPeriphClockCmd(), RCC_APB2PeriphClockCmd() and
             RCC_APB1PeriphClockCmd() functions.

         (#) To reset the peripherals configuration (to the default state after
             device reset) you can use RCC_AHBPeriphResetCmd(),
             RCC_APB2PeriphResetCmd() and RCC_APB1PeriphResetCmd() functions.
         (#) To further reduce power consumption in SLEEP mode the peripheral
             clocks can be disabled prior to executing the WFI or WFE instructions.
             You can do this using RCC_AHBPeriphClockLPModeCmd(),
             RCC_APB2PeriphClockLPModeCmd() and RCC_APB1PeriphClockLPModeCmd()
             functions.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the AHB peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @param  RCC_AHBPeriph: specifies the AHB peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     @arg RCC_AHBPeriph_GPIOA:         GPIOA clock
  *     @arg RCC_AHBPeriph_GPIOB:         GPIOB clock
  *     @arg RCC_AHBPeriph_GPIOC:         GPIOC clock
  *     @arg RCC_AHBPeriph_GPIOD:         GPIOD clock
  *     @arg RCC_AHBPeriph_GPIOE:         GPIOE clock
  *     @arg RCC_AHBPeriph_GPIOH:         GPIOH clock
  *     @arg RCC_AHBPeriph_GPIOF:         GPIOF clock
  *     @arg RCC_AHBPeriph_GPIOG:         GPIOG clock
  *     @arg RCC_AHBPeriph_CRC:           CRC clock
  *     @arg RCC_AHBPeriph_FLITF: (has effect only when the Flash memory is in power down mode)
  *     @arg RCC_AHBPeriph_DMA1:          DMA1 clock
  *     @arg RCC_AHBPeriph_DMA2:          DMA2 clock
  *     @arg RCC_AHBPeriph_AES:           AES clock
  *     @arg RCC_AHBPeriph_FSMC:          FSMC clock
  * @param  NewState: new state of the specified peripheral clock.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RCC_AHB_PERIPH(RCC_AHBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState != DISABLE){
        RCC->AHBENR |= RCC_AHBPeriph;
    }
    else{
        RCC->AHBENR &= ~RCC_AHBPeriph;
    }
}

/**
  * @brief  Enables or disables the High Speed APB (APB2) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     @arg RCC_APB2Periph_SYSCFG: SYSCFG APB2 Clock.
  *     @arg RCC_APB2Periph_TIM9: TIM9 APB2 Clock.
  *     @arg RCC_APB2Periph_TIM10: TIM10 APB2 Clock.
  *     @arg RCC_APB2Periph_TIM11: TIM11 APB2 Clock.
  *     @arg RCC_APB2Periph_ADC1: ADC1 APB2 Clock.
  *     @arg RCC_APB2Periph_SDIO: SDIO APB2 Clock.
  *     @arg RCC_APB2Periph_SPI1: SPI1 APB2 Clock.
  *     @arg RCC_APB2Periph_USART1: USART1 APB2 Clock.
  * @param  NewState: new state of the specified peripheral clock.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */

void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APB2ENR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2ENR &= ~RCC_APB2Periph;
  }
}

/**
  * @brief  Forces or releases High Speed APB (APB2) peripheral reset.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to reset.
  *   This parameter can be any combination of the following values:
  *     @arg RCC_APB2Periph_SYSCFG:       SYSCFG clock
  *     @arg RCC_APB2Periph_TIM9:         TIM9 clock
  *     @arg RCC_APB2Periph_TIM10:        TIM10 clock
  *     @arg RCC_APB2Periph_TIM11:        TIM11 clock
  *     @arg RCC_APB2Periph_ADC1:         ADC1 clock
  *     @arg RCC_APB2Periph_SDIO:         SDIO clock
  *     @arg RCC_APB2Periph_SPI1:         SPI1 clock
  *     @arg RCC_APB2Periph_USART1:       USART1 clock
  * @param  NewState: new state of the specified peripheral reset.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APB2RSTR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2RSTR &= ~RCC_APB2Periph;
  }
}

//#endif /* NC_STM32L1_RCC_ENABLED */
/**
  * @}
  */
/************************ Copyright (C) 2020, Noel Cruz *****END OF FILE****/

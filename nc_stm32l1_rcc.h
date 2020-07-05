/**
 * @file    nc_stm32l1_rcc.h
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
 */
 /* Define to prevent recursive inclusion -- */
#ifndef NC_STM32L1_RCC_H
#define NC_STM32L1_RCC_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif /* NC_STM32L1_RCC_H */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "nc_stm32l1_conf.h"

/* Exported types ------------------------------------------------------------*/

typedef struct
{
  uint32_t SYSCLK_Frequency;
  uint32_t HCLK_Frequency;
  uint32_t PCLK1_Frequency;
  uint32_t PCLK2_Frequency;
}RCC_ClocksTypeDef;

/* Exported constants --------------------------------------------------------*/
/** @defgroup RCC_AHB_Peripherals
  * @{
  */

#define RCC_AHBPeriph_GPIOA               RCC_AHBENR_GPIOAEN
#define RCC_AHBPeriph_GPIOB               RCC_AHBENR_GPIOBEN
#define RCC_AHBPeriph_GPIOC               RCC_AHBENR_GPIOCEN
#define RCC_AHBPeriph_GPIOD               RCC_AHBENR_GPIODEN
#define RCC_AHBPeriph_GPIOE               RCC_AHBENR_GPIOEEN
#define RCC_AHBPeriph_GPIOH               RCC_AHBENR_GPIOHEN
#define RCC_AHBPeriph_GPIOF               RCC_AHBENR_GPIOFEN
#define RCC_AHBPeriph_GPIOG               RCC_AHBENR_GPIOGEN
#define RCC_AHBPeriph_CRC                 RCC_AHBENR_CRCEN
#define RCC_AHBPeriph_FLITF               RCC_AHBENR_FLITFEN
#define RCC_AHBPeriph_SRAM                RCC_AHBLPENR_SRAMLPEN
#define RCC_AHBPeriph_DMA1                RCC_AHBENR_DMA1EN
#define RCC_AHBPeriph_DMA2                RCC_AHBENR_DMA2EN
#define RCC_AHBPeriph_AES                 RCC_AHBENR_AESEN
#define RCC_AHBPeriph_FSMC                RCC_AHBENR_FSMCEN

#define IS_RCC_AHB_PERIPH(PERIPH) ((((PERIPH) & 0xB4FF6F00) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_AHB_LPMODE_PERIPH(PERIPH) ((((PERIPH) & 0xB4FF6F00) == 0x00) && ((PERIPH) != 0x00))

/** @defgroup RCC_APB2_Peripherals
  * @{
  */

#define RCC_APB2Periph_SYSCFG            RCC_APB2ENR_SYSCFGEN
#define RCC_APB2Periph_TIM9              RCC_APB2ENR_TIM9EN
#define RCC_APB2Periph_TIM10             RCC_APB2ENR_TIM10EN
#define RCC_APB2Periph_TIM11             RCC_APB2ENR_TIM11EN
#define RCC_APB2Periph_ADC1              RCC_APB2ENR_ADC1EN
#define RCC_APB2Periph_SDIO              RCC_APB2ENR_SDIOEN
#define RCC_APB2Periph_SPI1              RCC_APB2ENR_SPI1EN
#define RCC_APB2Periph_USART1            RCC_APB2ENR_USART1EN

#define IS_RCC_APB2_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFA5E2) == 0x00) && ((PERIPH) != 0x00))
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/* Peripheral clocks configuration functions **********************************/
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* NC_STM32L1_RCC_H */

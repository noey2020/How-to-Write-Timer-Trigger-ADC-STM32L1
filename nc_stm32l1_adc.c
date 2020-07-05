/**
 * @file    nc_stm32l1_adc.c
 * @author  Noel Cruz
 * @email   noel_s_cruz@yahoo.com
 * @github  https://github.com/noey2020
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   ADC Library for STM32L1xx devices
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

/* Includes ------------------------------------------------------------------*/
#include "nc_stm32l1_adc.h"
#include "nc_stm32l1_rcc.h"

//#ifdef NC_STM32L1_ADC_ENABLED

/** @addtogroup STM32L1xx_StdPeriph_Driver
  * @{
  */

/** @defgroup ADC
  * @brief ADC driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* ADC DISCNUM mask */
#define CR1_DISCNUM_RESET         ((uint32_t)0xFFFF1FFF)

/* ADC AWDCH mask */
#define CR1_AWDCH_RESET           ((uint32_t)0xFFFFFFE0)

/* ADC Analog watchdog enable mode mask */
#define CR1_AWDMODE_RESET         ((uint32_t)0xFF3FFDFF)

/* CR1 register Mask */
#define CR1_CLEAR_MASK            ((uint32_t)0xFCFFFEFF)

/* ADC DELAY mask */
#define CR2_DELS_RESET            ((uint32_t)0xFFFFFF0F)

/* ADC JEXTEN mask */
#define CR2_JEXTEN_RESET          ((uint32_t)0xFFCFFFFF)

/* ADC JEXTSEL mask */
#define CR2_JEXTSEL_RESET         ((uint32_t)0xFFF0FFFF)

/* CR2 register Mask */
#define CR2_CLEAR_MASK            ((uint32_t)0xC0FFF7FD)

/* ADC SQx mask */
#define SQR5_SQ_SET               ((uint32_t)0x0000001F)
#define SQR4_SQ_SET               ((uint32_t)0x0000001F)
#define SQR3_SQ_SET               ((uint32_t)0x0000001F)
#define SQR2_SQ_SET               ((uint32_t)0x0000001F)
#define SQR1_SQ_SET               ((uint32_t)0x0000001F)

/* ADC L Mask */
#define SQR1_L_RESET              ((uint32_t)0xFE0FFFFF)

/* ADC JSQx mask */
#define JSQR_JSQ_SET              ((uint32_t)0x0000001F)

/* ADC JL mask */
#define JSQR_JL_SET               ((uint32_t)0x00300000)
#define JSQR_JL_RESET             ((uint32_t)0xFFCFFFFF)

/* ADC SMPx mask */
#define SMPR1_SMP_SET             ((uint32_t)0x00000007)
#define SMPR2_SMP_SET             ((uint32_t)0x00000007)
#define SMPR3_SMP_SET             ((uint32_t)0x00000007)
#define SMPR0_SMP_SET             ((uint32_t)0x00000007)

/* ADC JDRx registers offset */
#define JDR_OFFSET                ((uint8_t)0x30)

/* ADC CCR register Mask */
#define CR_CLEAR_MASK             ((uint32_t)0xFFFCFFFF)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup ADC_Private_Functions
  * @{
  */

/** @defgroup ADC_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions.
 *
@verbatim
 ===============================================================================
          ##### Initialization and Configuration functions #####
 ===============================================================================
    [..] This section provides functions allowing to:
        (+) Initialize and configure the ADC Prescaler.
        (+) ADC Conversion Resolution (12bit..6bit).
        (+) Scan Conversion Mode (multichannel or one channel) for regular group.
        (+) ADC Continuous Conversion Mode (Continuous or Single conversion) for
            regular group.
        (+) External trigger Edge and source of regular group.
        (+) Converted data alignment (left or right).
        (+) The number of ADC conversions that will be done using the sequencer
            for regular channel group.
        (+) Enable or disable the ADC peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes ADC1 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void ADC_DeInit(ADC_TypeDef* ADCx)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));

  if(ADCx == ADC1)
  {
    /* Enable ADC1 reset state */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
    /* Release ADC1 from reset state */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
  }
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct.
  * @note   This function is used to configure the global features of the ADC (
  *         Resolution and Data Alignment), however, the rest of the configuration
  *         parameters are specific to the regular channels group (scan mode
  *         activation, continuous mode activation, External trigger source and
  *         edge, number of conversion in the regular channels group sequencer).
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure that contains
  *         the configuration information for the specified ADC peripheral.
  * @retval None
  */
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct)
{
  uint32_t tmpreg1 = 0;
  uint8_t tmpreg2 = 0;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_RESOLUTION(ADC_InitStruct->ADC_Resolution));
  assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ScanConvMode));
  assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(ADC_InitStruct->ADC_ExternalTrigConvEdge));
  assert_param(IS_ADC_EXT_TRIG(ADC_InitStruct->ADC_ExternalTrigConv));
  assert_param(IS_ADC_DATA_ALIGN(ADC_InitStruct->ADC_DataAlign));
  assert_param(IS_ADC_REGULAR_LENGTH(ADC_InitStruct->ADC_NbrOfConversion));

  /*---------------------------- ADCx CR1 Configuration -----------------*/
  /* Get the ADCx CR1 value */
  tmpreg1 = ADCx->CR1;
  /* Clear RES and SCAN bits */
  tmpreg1 &= CR1_CLEAR_MASK;
  /* Configure ADCx: scan conversion mode and resolution */
  /* Set SCAN bit according to ADC_ScanConvMode value */
  /* Set RES bit according to ADC_Resolution value */
  tmpreg1 |= (uint32_t)(((uint32_t)ADC_InitStruct->ADC_ScanConvMode << 8) | ADC_InitStruct->ADC_Resolution);
  /* Write to ADCx CR1 */
  ADCx->CR1 = tmpreg1;

  /*---------------------------- ADCx CR2 Configuration -----------------*/
  /* Get the ADCx CR2 value */
  tmpreg1 = ADCx->CR2;
  /* Clear CONT, ALIGN, EXTEN and EXTSEL bits */
  tmpreg1 &= CR2_CLEAR_MASK;
  /* Configure ADCx: external trigger event and edge, data alignment and continuous conversion mode */
  /* Set ALIGN bit according to ADC_DataAlign value */
  /* Set EXTEN bits according to ADC_ExternalTrigConvEdge value */
  /* Set EXTSEL bits according to ADC_ExternalTrigConv value */
  /* Set CONT bit according to ADC_ContinuousConvMode value */
  tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_DataAlign | ADC_InitStruct->ADC_ExternalTrigConv |
              ADC_InitStruct->ADC_ExternalTrigConvEdge | ((uint32_t)ADC_InitStruct->ADC_ContinuousConvMode << 1));
  /* Write to ADCx CR2 */
  ADCx->CR2 = tmpreg1;

  /*---------------------------- ADCx SQR1 Configuration -----------------*/
  /* Get the ADCx SQR1 value */
  tmpreg1 = ADCx->SQR1;
  /* Clear L bits */
  tmpreg1 &= SQR1_L_RESET;
  /* Configure ADCx: regular channel sequence length */
  /* Set L bits according to ADC_NbrOfConversion value */
  tmpreg2 |= (uint8_t)(ADC_InitStruct->ADC_NbrOfConversion - (uint8_t)1);
  tmpreg1 |= ((uint32_t)tmpreg2 << 20);
  /* Write to ADCx SQR1 */
  ADCx->SQR1 = tmpreg1;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @note   This function is used to initialize the global features of the ADC (
  *         Resolution and Data Alignment), however, the rest of the configuration
  *         parameters are specific to the regular channels group (scan mode
  *         activation, continuous mode activation, External trigger source and
  *         edge, number of conversion in the regular channels group sequencer).
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
{
  /* Reset ADC init structure parameters values */
  /* Initialize the ADC_Resolution member */
  ADC_InitStruct->ADC_Resolution = ADC_Resolution_12b;

  /* Initialize the ADC_ScanConvMode member */
  ADC_InitStruct->ADC_ScanConvMode = DISABLE;

  /* Initialize the ADC_ContinuousConvMode member */
  ADC_InitStruct->ADC_ContinuousConvMode = DISABLE;

  /* Initialize the ADC_ExternalTrigConvEdge member */
  ADC_InitStruct->ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;

  /* Initialize the ADC_ExternalTrigConv member */
  ADC_InitStruct->ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;

  /* Initialize the ADC_DataAlign member */
  ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;

  /* Initialize the ADC_NbrOfConversion member */
  ADC_InitStruct->ADC_NbrOfConversion = 1;
}

/**
  * @brief  Initializes the ADCs peripherals according to the specified parameters
  *          in the ADC_CommonInitStruct.
  * @param  ADC_CommonInitStruct: pointer to an ADC_CommonInitTypeDef structure
  *         that contains the configuration information (Prescaler) for ADC1 peripheral.
  * @retval None
  */
void ADC_CommonInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_ADC_PRESCALER(ADC_CommonInitStruct->ADC_Prescaler));

  /*---------------------------- ADC CCR Configuration -----------------*/
  /* Get the ADC CCR value */
  tmpreg = ADC->CCR;

  /* Clear ADCPRE bit */
  tmpreg &= CR_CLEAR_MASK;

  /* Configure ADCx: ADC prescaler according to ADC_Prescaler */
  tmpreg |= (uint32_t)(ADC_CommonInitStruct->ADC_Prescaler);

  /* Write to ADC CCR */
  ADC->CCR = tmpreg;
}

/**
  * @brief  Fills each ADC_CommonInitStruct member with its default value.
  * @param  ADC_CommonInitStruct: pointer to an ADC_CommonInitTypeDef structure
  *         which will be initialized.
  * @retval None
  */
void ADC_CommonStructInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct)
{
  /* Reset ADC init structure parameters values */
  /* Initialize the ADC_Prescaler member */
  ADC_CommonInitStruct->ADC_Prescaler = ADC_Prescaler_Div1;
}

/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param  ADCx: where x can be 1 to select the ADC1 peripheral.
  * @param  NewState: new state of the ADCx peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set the ADON bit to wake up the ADC from power down mode */
    ADCx->CR2 |= (uint32_t)ADC_CR2_ADON;
  }
  else
  {
    /* Disable the selected ADC peripheral */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_ADON);
  }
}




//#endif /* NC_STM32L1_ADC_ENABLED */
/**
  * @}
  */
/************************ Copyright (C) 2020, Noel Cruz *****END OF FILE****/

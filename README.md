# How-to-Write-Timer-Trigger-ADC-STM32L1
July 5, 2020

I appreciate comments. Shoot me an email at noel_s_cruz@yahoo.com!

Hire me!

We will study our typedefs to determine how we can "fill in the blanks" into what is need by our adc registers to work properly. The nc_stm32l1_adc.c and nc_stm32l1_adc.h have pre-written defines and function prototypes/implementation to save us time and effort. You always have to refer to the technical reference manua, datasheets, block diagram, and schematics. Unless you don't understand how to deal with pointers and typedef structs, you will be at the mercy of code generators. Code generators are easy but you have a lot of work to do. Example code generators are Cube MX and the like. 

Some helpful posts to keep review: https://github.com/noey2020/How-to-Understand-Interrupts-Timers-Stack-and-Register-File.

I have listed them below.

typedef struct
{
  uint32_t ADC_Resolution;                /*!< Selects the resolution of the conversion.
                                               This parameter can be a value of @ref ADC_Resolution */

  FunctionalState ADC_ScanConvMode;       /*!< Specifies whether the conversion is performed in
                                               Scan (multichannel) or Single (one channel) mode.
                                               This parameter can be set to ENABLE or DISABLE */

  FunctionalState ADC_ContinuousConvMode; /*!< Specifies whether the conversion is performed in
                                               Continuous or Single mode.
                                               This parameter can be set to ENABLE or DISABLE. */

  uint32_t ADC_ExternalTrigConvEdge;      /*!< Selects the external trigger Edge and enables the
                                               trigger of a regular group. This parameter can be a value
                                               of @ref ADC_external_trigger_edge_for_regular_channels_conversion */

  uint32_t ADC_ExternalTrigConv;          /*!< Defines the external trigger used to start the analog
                                               to digital conversion of regular channels. This parameter
                                               can be a value of @ref ADC_external_trigger_sources_for_regular_channels_conversion */

  uint32_t ADC_DataAlign;                 /*!< Specifies whether the ADC data alignment is left or right.
                                               This parameter can be a value of @ref ADC_data_align */

  uint8_t  ADC_NbrOfConversion;           /*!< Specifies the number of ADC conversions that will be done
                                               using the sequencer for regular channel group.
                                               This parameter must range from 1 to 27. */
}ADC_InitTypeDef;

typedef struct
{
  uint32_t ADC_Prescaler;                 /*!< Selects the ADC prescaler.
                                               This parameter can be a value
                                               of @ref ADC_Prescaler */
}ADC_CommonInitTypeDef;

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
{...}


Check this out again, https://github.com/noey2020/How-to-Understand-Interrupts-Timers-Stack-and-Register-File to review.

I appreciate comments. Shoot me an email at noel_s_cruz@yahoo.com!

Hire me!

Hope this helps and happy coding!

tbc

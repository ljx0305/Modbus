/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�systemprocess.c                                                **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��STM32Ӳ��ƽ̨�����ù���                                **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stm32f4xx_hal.h"
#include "noos_config.h"

/* ϵͳʱ�ӳ�ʼ������ */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* �������ڲ������������ѹ */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* ��ʼ��CPU, AHB �� APB ���ߵ�ʱ�� */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /* ��ʼ��CPU, AHB �� APB ���ߵ�ʱ�� */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /* ����Systick�жϵ�ʱ�䣬ÿ1ms�ж�һ�� */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  /* ����Systick��ʱ��Դ��ΪϵͳHCLKʱ�� */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn�ж��������ȼ� */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* GPIO��ʼ������ */
 void GPIO_Init_Configuration(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO�˿�ʱ��ʹ�� */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*����GPIO����: ETH_MII_INT_Pin */
  GPIO_InitStruct.Pin = ETH_MII_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ETH_MII_INT_GPIO_Port, &GPIO_InitStruct);

}

/* �������� */
void _Error_Handler(char *file, int line)
{
  
  while(1)
  {
  }
  
}






/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�canprocess.c                                                   **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��CAN����ͨѶ                                            **/
/**             PB8     ------> CAN1_RX                                      **/
/**             PB9     ------> CAN1_TX                                      **/
/**             PB5     ------> CAN2_RX                                      **/
/**             PB6     ------> CAN2_TX                                      **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stm32f4xx_hal.h"
#include "canprocess.h"

/* ����ȫ�ֱ��� */
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

static void CAN1_Init_Configuration(void);
static void CAN2_Init_Configuration(void);


/* CAN���߳�ʼ������ */
void CAN_Init_Configuration(void)
{
  CAN1_Init_Configuration();
  CAN2_Init_Configuration();
}

/* CAN1��ʼ������ */
static void CAN1_Init_Configuration(void)
{

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 12;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_7TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* CAN2 ��ʼ������ */
static void CAN2_Init_Configuration(void)
{

  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 12;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_7TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
/******************************************************************************/
/** ģ�����ƣ�Modbus��������ʵ��ģ��                                         **/
/** �ļ����ƣ�mcutsprocess.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺Modbus��������ʵ��ģ����Զ˿ڴ��нӿ������ʵ�ֶ���λ����ͨ   **/
/**           Ѷ������USART3�˿ڣ�����RS485��ʵ��Modbus RTU��վ��            **/
/**           PB10      USART3_TX       USART3���з���                       **/
/**           PB11      USART3_RX       USART3���н���                       **/
/**           PB7       RS485_CTL3      RS485�շ�����                        **/
/**           ����STM32F407ZGT6Ӳ��ƽ̨����������HAL FW_F4 V1.26.0��       **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2021-03-29          ���Ҿ�            �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MCUTS_PROCESS_H
#define __MCUTS_PROCESS_H

#include "noos_config.h"
#include "mbconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/*�豸��������ͨѶ����*/
void LocalMasterProcess(void);

/*�Ա��������˿�����*/
void LocalMasterConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif  /* __MCUTS_PROCESS_H */

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/

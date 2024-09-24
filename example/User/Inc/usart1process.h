/******************************************************************************/
/** ģ�����ƣ�Modbus���ش�վʵ��ģ��                                         **/
/** �ļ����ƣ�mcudsprocess.h                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺Modbus���ش�վʵ��ģ����λ��ʾ���нӿ������ʵ�ֶ���λ����ͨ   **/
/**           Ѷ������USART1�˿ڣ�����RS232��ʵ��Modbus RTU��վ��            **/
/**           PA9       USART1_TX       USART1���з���                       **/
/**           PA10      USART1_RX       USART1���н���                       **/
/**           PD8       RS232_INT       RS232�ж��ź�                        **/
/**           ����STM32F407VGT6Ӳ��ƽ̨����������HAL FW_F4 V1.26.0��       **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2021-03-29          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MCUDS_PROCESS_H
#define __MCUDS_PROCESS_H

#include "noos_config.h"
#include "mbconfig.h"

#ifdef __cplusplus
extern "C" {
#endif


/*�豸����ͨѶ���ݴ���*/
void LocalSlaveProcess(void);

/*�豸����ͨѶ���ݴ���*/
void LocalSlaveConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif  /* __MCUDS_PROCESS_H */

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/

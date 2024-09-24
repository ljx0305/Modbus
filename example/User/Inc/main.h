/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�main.h                                                         **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��LwIP��̫��ͨѶ���س���                                 **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx_hal.h"
#include "lwipprocess.h"
#include "noos_config.h"

#if (UDP_SERVER_ENABLE>(0)||UDP_CLIENT_ENABLE>(0))
#include "udpprocess.h"
#endif

#if TFTP_SERVER_ENABLE>(0)
#include "tftpprocess.h"
#endif

#if (TCP_SERVER_ENABLE>(0)||TCP_CLIENT_ENABLE>(0))
#include "tcpprocess.h"
#endif

#if HTTP_SERVER_ENABLE>(0)
#include "httpprocess.h"
#endif

#if TELNET_SERVER_ENABLE>(0)
#include "telnetprocess.h"
#endif

#include "usart1process.h"

#include "usart3process.h"

/* ϵͳʱ�ӳ�ʼ������*/
void SystemClock_Config(void);

/* GPIO��ʼ������ */
 void GPIO_Init_Configuration(void);



#endif /* __MAIN_H__ */

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/

/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�lwipprocess.h                                                  **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��LwIP��̫��ͨѶ�������ü�����                           **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LWIP_PROCESS_H
#define __LWIP_PROCESS_H

/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "ethernetif.h"

/* Includes for RTOS ---------------------------------------------------------*/
#if WITH_RTOS
#include "lwip/tcpip.h"
#endif /* WITH_RTOS */


/* Global Variables ----------------------------------------------------------*/
extern ETH_HandleTypeDef heth;

/* LWIP��ʼ������ */	
void LWIP_Init_Configuration(void);

#if !WITH_RTOS

/* Function defined in lwip.c to:
 *   - Read a received packet from the Ethernet buffers 
 *   - Send it to the lwIP stack for handling
 *   - Handle timeouts if NO_SYS_NO_TIMERS not set
 */ 
void EthernetProcess(void);

#endif /* WITH_RTOS */

#endif /* __LWIP_PROCESS_H */



/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/

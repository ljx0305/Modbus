/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�noos_config.h                                                  **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�ֻ���LwIP��������ܵ�����                               **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __NOOS_CONFIG_H
#define __NOOS_CONFIG_H

#include "stdbool.h"
#include "stm32f4xx_hal.h"
#include "mbconfig.h"

/* GPIO�궨�� */
#define ETH_MII_INT_Pin GPIO_PIN_0
#define ETH_MII_INT_GPIO_Port GPIOB

#ifndef UDP_SERVER_ENABLE
#define UDP_SERVER_ENABLE       (0)
#endif

#ifndef UDP_CLIENT_ENABLE
#define UDP_CLIENT_ENABLE       (0)
#endif

#ifndef TFTP_SERVER_ENABLE
#define TFTP_SERVER_ENABLE      (0)
#endif

#ifndef TCP_SERVER_ENABLE
#define TCP_SERVER_ENABLE       (1)
#endif

#ifndef TCP_CLIENT_ENABLE
#define TCP_CLIENT_ENABLE       (0)
#endif

#ifndef HTTP_SERVER_ENABLE
#define HTTP_SERVER_ENABLE       (0)
#endif

#ifndef HTTP_CLIENT_ENABLE
#define HTTP_CLIENT_ENABLE       (0)
#endif

#ifndef TELNET_SERVER_ENABLE
#define TELNET_SERVER_ENABLE       (0)
#endif

/*�������ڱ���������ͨѶ������*/
typedef union {
    struct {
        uint32_t	beatTime;	//�������
        float	mbAI1;	//ģ��������
        float	mbAO1;	//ģ��������
        uint16_t	mbAI2;	//ģ��������
        uint16_t	mbAO2;	//ģ��������
		
        float	mbSalve1AI1;	//Ŀ���վ1��ģ�����������1
        uint32_t	mbSalve1AI2;	//Ŀ���վ1��ģ�����������2
        uint16_t	mbSalve1AI3;	//Ŀ���վ1��ģ�����������3
        uint16_t	mbSalve1AO1;	//Ŀ���վ1��ģ�����������1
        uint16_t	mbSalve1AO2;	//Ŀ���վ1��ģ�����������2
        uint16_t	mbSalve1AO3;	//Ŀ���վ1��ģ�����������3
		
        float	mbSalve2AI1;	//Ŀ���վ2��ģ�����������1
        uint32_t	mbSalve2AI2;	//Ŀ���վ2��ģ�����������2
        uint16_t	mbSalve2AI3;	//Ŀ���վ2��ģ�����������3
        uint16_t	mbSalve2AO1;	//Ŀ���վ2��ģ�����������1
        uint16_t	mbSalve2AO2;	//Ŀ���վ2��ģ�����������2
        uint16_t	mbSalve2AO3;	//Ŀ���վ2��ģ�����������3
		
        float	mbSalve3AI1;	//Ŀ���վ3��ģ�����������1
        uint32_t	mbSalve3AI2;	//Ŀ���վ3��ģ�����������2
        uint16_t	mbSalve3AI3;	//Ŀ���վ3��ģ�����������3
        uint16_t	mbSalve3AO1;	//Ŀ���վ3��ģ�����������1
        uint16_t	mbSalve3AO2;	//Ŀ���վ3��ģ�����������2
        uint16_t	mbSalve3AO3;	//Ŀ���վ3��ģ�����������3
		
        float	mbSalve4AI1;	//Ŀ���վ4��ģ�����������1
        uint32_t	mbSalve4AI2;	//Ŀ���վ4��ģ�����������2
        uint16_t	mbSalve4AI3;	//Ŀ���վ4��ģ�����������3
        uint16_t	mbSalve4AO1;	//Ŀ���վ4��ģ�����������1
        uint16_t	mbSalve4AO2;	//Ŀ���վ4��ģ�����������2
        uint16_t	mbSalve4AO3;	//Ŀ���վ4��ģ�����������3
        
    }phyPara;
    uint16_t holdingRegister[HoldingRegisterEndAddress+1]; /*���ּĴ���*/
}AnalogParaTypeDef;

typedef union {
    struct {
        bool	mbDI1;	//�������������1
        bool	mbDI2;	//�������������2
        bool	mbDI3;	//�������������3
        bool	mbDI4;	//�������������4
        bool	mbDO1;	//�������������1
        bool	mbDO2;	//�������������2
        bool	mbDO3;	//�������������3
        bool	mbDO4;	//�������������4
		
        bool	mbSalve1DI1;	//Ŀ���վ1���������������1
        bool	mbSalve1DI2;	//Ŀ���վ1���������������2
        bool	mbSalve1DI3;	//Ŀ���վ1���������������3
        bool	mbSalve1DI4;	//Ŀ���վ1���������������4
        bool	mbSalve1DO1;	//Ŀ���վ1���������������1
        bool	mbSalve1DO2;	//Ŀ���վ1���������������2
        bool	mbSalve1DO3;	//Ŀ���վ1���������������3
        bool	mbSalve1DO4;	//Ŀ���վ1���������������4
		
        bool	mbSalve2DI1;	//Ŀ���վ2���������������1
        bool	mbSalve2DI2;	//Ŀ���վ2���������������2
        bool	mbSalve2DI3;	//Ŀ���վ2���������������3
        bool	mbSalve2DI4;	//Ŀ���վ2���������������4
        bool	mbSalve2DO1;	//Ŀ���վ2���������������1
        bool	mbSalve2DO2;	//Ŀ���վ2���������������2
        bool	mbSalve2DO3;	//Ŀ���վ2���������������3
        bool	mbSalve2DO4;	//Ŀ���վ2���������������4
		
        bool	mbSalve3DI1;	//Ŀ���վ3���������������1
        bool	mbSalve3DI2;	//Ŀ���վ3���������������2
        bool	mbSalve3DI3;	//Ŀ���վ3���������������3
        bool	mbSalve3DI4;	//Ŀ���վ3���������������4
        bool	mbSalve3DO1;	//Ŀ���վ3���������������1
        bool	mbSalve3DO2;	//Ŀ���վ3���������������2
        bool	mbSalve3DO3;	//Ŀ���վ3���������������3
        bool	mbSalve3DO4;	//Ŀ���վ3���������������4
		
        bool	mbSalve4DI1;	//Ŀ���վ4���������������1
        bool	mbSalve4DI2;	//Ŀ���վ4���������������2
        bool	mbSalve4DI3;	//Ŀ���վ4���������������3
        bool	mbSalve4DI4;	//Ŀ���վ4���������������4
        bool	mbSalve4DO1;	//Ŀ���վ4���������������1
        bool	mbSalve4DO2;	//Ŀ���վ4���������������2
        bool	mbSalve4DO3;	//Ŀ���վ4���������������3
        bool	mbSalve4DO4;	//Ŀ���վ4���������������4
        
    }phyPara;
    bool coil[CoilEndAddress+1];         /*��Ȧ��*/
}DigitalParaTypeDef;




void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#endif

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
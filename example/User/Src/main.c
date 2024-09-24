/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�main.c                                                         **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��LwIP��̫��ͨѶ���س���                                 **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "main.h"

/* ��������ϵͳLwIP��̫��ͨѶ���س��� */
int main(void)
{
  /* ��λȫ�����裬��ʼ��Flash�ӿڼ���શ�ʱ�� */
  HAL_Init();

  /* ����ϵͳʱ�� */
  SystemClock_Config();

  /* ��ʼ���������� */
  GPIO_Init_Configuration();

  /* ��ʼ���������� */
  LWIP_Init_Configuration();

#if UDP_SERVER_ENABLE > (0)
  /* UDP��������ʼ������ */
  UDP_Server_Initialization();
#endif

#if UDP_CLIENT_ENABLE > (0)
  /* UDP�ͻ��˳�ʼ������ */
  UDP_Client_Initialization();
#endif
  
#if TFTP_SERVER_ENABLE > (0)
  /* ��ʼ��TFTP������ */
  Tftp_Server_Initialization();
#endif
  
#if TCP_SERVER_ENABLE > (0)
  /* TCP��������ʼ������ */
  Tcp_Server_Initialization();
#endif

#if TCP_CLIENT_ENABLE > (0)
  /* TCP�ͻ��˳�ʼ������ */
  Tcp_Client_Initialization();
#endif
  
#if HTTP_SERVER_ENABLE > (0)
  /* ��ʼ��HTTP������ */
  Http_Server_Initialization();
#endif
  
#if HTTP_CLIENT_ENABLE > (0)
  /* ��ʼ��HTTP�ͻ��� */
  Http_Client_Initialization();
#endif
  
#if TELNET_SERVER_ENABLE > (0)
  /* ��ʼ��TELNET������ */
  Telnet_Server_Initialization();
#endif
  
  
  /*�豸����ͨѶ���ݴ���*/
    LocalSlaveConfiguration();
    
    /*�Ա��������˿�����*/
    LocalMasterConfiguration();
  
  while (1)
  {
    /*�豸����ͨѶ���ݴ���*/
    LocalSlaveProcess();
    
    /*�豸��������ͨѶ����*/
    LocalMasterProcess();
    
  /* ���紦�� */
    EthernetProcess();
  }

}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/

/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�udpprocess.c                                                   **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�ֻ���LwIP��UDP�������Ϳͻ��˹���                        **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "string.h"
#include "udpprocess.h"

#include "netportdefine.h"

uint8_t udpServerIP[4]={192,168,0,1};

/* ����UDP���������ݴ���ص����� */
static void UDPServerCallback(void *arg,struct udp_pcb *upcb,struct pbuf *p,const ip_addr_t *addr,u16_t port);

/* ����UDP�ͻ������ݴ���ص����� */
static void UDPClientCallback(void *arg,struct udp_pcb *upcb,struct pbuf *p,const ip_addr_t *addr,u16_t port);

/* �ͻ������ݷ��ͺ��� */
void UdpClientSendPacket(struct udp_pcb *upcb,char* data);


/* ����UDP���������ݴ���ص����� */
static void UDPServerCallback(void *arg,struct udp_pcb *upcb,struct pbuf *revBuf,const ip_addr_t *addr,u16_t port)
{
  struct pbuf *sendBuf = NULL;
  const char* reply = "This is reply!\n";

  pbuf_free(revBuf);
   
  sendBuf = pbuf_alloc(PBUF_TRANSPORT, strlen(reply)+1, PBUF_RAM);
  if(!sendBuf)
  {
    return;
  }

  memset(sendBuf->payload,0,sendBuf->len);
  memcpy(sendBuf->payload, reply, strlen(reply));
  udp_sendto(upcb, sendBuf, addr, port);
  pbuf_free(sendBuf);
}

/* UDP��ʼ������ */
void UDP_Server_Initialization(void)
{
  static char * recv_arg="We recieved a UDP data\n";
  struct udp_pcb *upcb;

  /* ����һ���µ�UDP���ƿ� */
  upcb = udp_new();
   
  /* ��upcb�鵽����IP��ַ��ָ���˿�*/
  udp_bind(upcb, IP_ADDR_ANY, UDP_ECHO_SERVER_PORT);
 
  /* Ϊupcbָ�����ݴ���ص����� */
  //udp_recv(upcb, UDPServerCallback, NULL);
  udp_recv(upcb,UDPServerCallback,(void *)recv_arg);
}

/* UDP�ͻ��˳�ʼ������ */
void UDP_Client_Initialization(void)
{
  ip_addr_t DestIPaddr;
  err_t err;
  struct udp_pcb *upcb;
  char data[]="This is a Client.";

  /* ���÷������˵�IP��ַ */
  IP4_ADDR( &DestIPaddr,udpServerIP[0],udpServerIP[1],udpServerIP[2],udpServerIP[3]);

  /* ����һ���µ�UDP���ƿ� */
  upcb = udp_new();

  if (upcb!=NULL)
  {
    /* �������˵�ַ���˿����� */
    err= udp_connect(upcb, &DestIPaddr, UDP_ECHO_SERVER_PORT);

    if (err == ERR_OK)
    {
      /* ע��ص����� */
      udp_recv(upcb, UDPClientCallback,(void *)data);
      /**���ݷ��ͣ���һ������ʱ�ͻ��˷����������������ˣ����ͺ����л��������ԴIP��ַ�����ã����ԴIP��ַδ���ã������ݷ���ʧ�ܡ��ô����ֵ������ں����ܽ����ᵽ��**/
      UdpClientSendPacket(upcb,data);   
    }
  }
}

/* ����UDP�ͻ������ݴ���ص����� */
static void UDPClientCallback(void *arg,struct udp_pcb *upcb,struct pbuf *p,const ip_addr_t *addr,u16_t port)
{
  udp_send(upcb, p);     //���ݻ��� 

  pbuf_free(p);
}

/* �ͻ������ݷ��ͺ��� */
void UdpClientSendPacket(struct udp_pcb *upcb,char* data)
{
  struct pbuf *p;

  /* �����ڴ�ռ� */
  p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)data), PBUF_POOL);

  if (p != NULL)
  {

    /* �������ݵ�pbuf */
    pbuf_take(p, (char*)data, strlen((char*)data));

    /* �������� */
    udp_send(upcb, p);     //��������

    /* �ͷ�pbuf */
    pbuf_free(p);
  }

}


/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
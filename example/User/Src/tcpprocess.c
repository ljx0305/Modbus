/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�tcpprocess.c                                                   **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�ֻ���LwIP��TCP�������Ϳͻ��˹���                        **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "tcpprocess.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lwip/ip_addr.h"
#include "netportdefine.h"

#include "mbconfig.h"

#define TCP_SERVER_PORT TCP_ECHO_SERVER_PORT
#define TCP_CLIENT_PORT TCP_ECHO_SERVER_PORT

uint8_t serverIP[4]={192,168,1,31};

/* TCP���������ջص����� */
static err_t TCPServerAccept(void *arg, struct tcp_pcb *pcb, err_t err);

/* TCP���������ݴ���������ص����� */
static err_t TCPServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err);

/* TCP�ͻ������ӵ��������ص����� */
static err_t TCPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err);

/* TCP�ͻ��˽��յ����ݺ�����ݴ���ص����� */
static err_t TCPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err);

/* TCP�ͻ������ӷ���������ص����� */
static void TCPClientConnectError(void *arg, err_t err);

/* TCP��������ʼ�� */
void Tcp_Server_Initialization(void)
{
  struct tcp_pcb *tcp_server_pcb;

  /* Ϊtcp����������һ��tcp_pcb�ṹ�� */
  tcp_server_pcb = tcp_new();

  /* �󶨱��ض˺ź�IP��ַ */
  tcp_bind(tcp_server_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);

  /* ����֮ǰ�����Ľṹ��tcp_server_pcb */
  tcp_server_pcb = tcp_listen(tcp_server_pcb);

  /* ��ʼ���ṹ����ջص����� */
  tcp_accept(tcp_server_pcb, TCPServerAccept);
}

/* TCP���������ջص����������ͻ��˽������Ӻ󱾺��������� */
static err_t TCPServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  /* ע����ջص����� */
  tcp_recv(pcb, TCPServerCallback);

  return ERR_OK;
}

/* TCP���������ݴ���������ص����� */
static err_t TCPServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
  //struct pbuf *tcp_send_pbuf;
  //char echoString[]="This is the client content echo:\r\n";
    
    struct pbuf tcp_send_pbuf;
    uint8_t respondBuffer[100];
    //tcp_send_pbuf.next=NULL;
    tcp_send_pbuf.payload=respondBuffer;

  if (tcp_recv_pbuf != NULL)
  {
    /* ���½��մ��� */
    tcp_recved(pcb, tcp_recv_pbuf->tot_len);

    /* �����յ����ݿ��������ͽṹ�� */
//    tcp_send_pbuf = tcp_recv_pbuf;
//    tcp_write(pcb,echoString, strlen(echoString), 1);
    tcp_send_pbuf.len=ParsingClientAccessCommand(tcp_recv_pbuf->payload,tcp_send_pbuf.payload);
    /* �����յ���������ת����ȥ */
    tcp_write(pcb,&tcp_send_pbuf.payload, tcp_send_pbuf.len, 1);

    pbuf_free(tcp_recv_pbuf);
    tcp_close(pcb);
  }
  else if (err == ERR_OK)
  {
    return tcp_close(pcb);
  }

  return ERR_OK;
}

/* TCP�ͻ��˳�ʼ�� */
void Tcp_Client_Initialization(void)
{
  struct tcp_pcb *tcp_client_pcb;
  ip_addr_t ipaddr;

  /* ��Ŀ���������IPд��һ���ṹ�壬Ϊpc����������IP��ַ */
  IP4_ADDR(&ipaddr,serverIP[0],serverIP[1],serverIP[2],serverIP[3]);

  /* Ϊtcp�ͻ��˷���һ��tcp_pcb�ṹ��    */
  tcp_client_pcb = tcp_new();

  /* �󶨱��ض˺ź�IP��ַ */
  tcp_bind(tcp_client_pcb, IP_ADDR_ANY, TCP_CLIENT_PORT);

  if (tcp_client_pcb != NULL)
  {
    /* ��Ŀ��������������ӣ�����������Ŀ��˿ں�Ŀ��IP */
    tcp_connect(tcp_client_pcb, &ipaddr, TCP_SERVER_PORT, TCPClientConnected);
    
    tcp_err(tcp_client_pcb, TCPClientConnectError);
  }
}

/* TCP�ͻ������ӵ��������ص����� */
static err_t TCPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
  char clientString[]="This is a new client connection.\r\n";

  /* ���ý��ջص����� */
  tcp_recv(pcb, TCPClientCallback);

  /* ����һ���������ӵ��ʺ��ַ���*/
  tcp_write(pcb,clientString, strlen(clientString),0);

  return ERR_OK;
}

/* TCP�ͻ��˽��յ����ݺ�����ݴ���ص����� */
static err_t TCPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
  struct pbuf *tcp_send_pbuf;
  char echoString[]="This is the server content echo:\r\n";

  if (tcp_recv_pbuf != NULL)
  {
    /* ���½��մ��� */
    tcp_recved(pcb, tcp_recv_pbuf->tot_len);

    /* �����յ��ķ��������ݻ���*/
    tcp_write(pcb,echoString, strlen(echoString), 1);
    tcp_send_pbuf = tcp_recv_pbuf;
    tcp_write(pcb, tcp_send_pbuf->payload, tcp_send_pbuf->len, 1);

    pbuf_free(tcp_recv_pbuf);
  }
  else if (err == ERR_OK)
  {
    tcp_close(pcb);
    Tcp_Client_Initialization();

    return ERR_OK;
  }

  return ERR_OK;
}

/* TCP�ͻ������ӷ���������ص����� */
static void TCPClientConnectError(void *arg, err_t err)
{
  /* ������������ */
  Tcp_Client_Initialization();
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/

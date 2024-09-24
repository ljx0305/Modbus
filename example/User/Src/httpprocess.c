/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�httpprocess.c                                                  **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�ֻ���LwIP��HTTP�������Ϳͻ��˹���                       **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "httpprocess.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include <stdio.h>	
#include <string.h>
#include "netportdefine.h"

#define TCP_HTTP_CLIENT_PORT TCP_HTTP_SERVER_PORT

uint8_t httpServerIP[4]={192,168,0,1};

const unsigned char htmlMessage[] = "	\
        <html>	\
        <head><title> A LwIP WebServer !!</title></head> \
            <center><p>This is a WebServer for testing!</center>\
	    <center><p>The WebServer based on LwIP v2.0.3!</center>\
	    </html>";

const unsigned char strHttpGet[]="GET https://www.cnblogs.com/foxclever/ HTTP/1.1\r\n"
                                    "Host:www.cnblogs.com:80\r\n\r\n";
              
              
/* HTTP���ջص��������ͻ��˽������Ӻ󣬱����������� */
static err_t HttpServerAccept(void *arg, struct tcp_pcb *pcb, err_t err);
/* HTTP��������Ϣ����ص����� */
static err_t HttpServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

/* HTTP�ͻ������ӵ��������ص����� */
static err_t HTTPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err);

/* HTTP�ͻ��˽��յ����ݺ�����ݴ���ص����� */
static err_t HTTPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err);

/* HTTP�ͻ������ӷ���������ص����� */
static void HTTPClientConnectError(void *arg, err_t err);


/* HTTP��������Ϣ����ص����� */
static err_t HttpServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  char *data = NULL;

  if (p != NULL) 
  {        
    /* ���½��մ��� */
    tcp_recved(pcb, p->tot_len);
    data =  p->payload;
    
    /* �����http���󣬷���html��Ϣ����������Ӧ */
    if(p->len >=3 && data[0] == 'G'&& data[1] == 'E'&& data[2] == 'T')
    {
      tcp_write(pcb, htmlMessage, sizeof(htmlMessage), 1);
    }
    else
    {

    }
    pbuf_free(p);
    tcp_close(pcb);
  } 
  else if (err == ERR_OK) 
  {
    return tcp_close(pcb);
  }
  return ERR_OK;
}

/* HTTP���ջص��������ͻ��˽������Ӻ󣬱����������� */
static err_t HttpServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  /*ע��HTTP�������ص�����*/
  tcp_recv(pcb, HttpServerCallback);
  
  return ERR_OK;
}

/* HTTP��������ʼ������*/
 void Http_Server_Initialization(void)
{
  struct tcp_pcb *pcb = NULL;	            		
  
  /* ����һ���µ�TCP���ƿ� */
  pcb = tcp_new();	                		 	

  /* ���ƿ�������IP�Ͷ�Ӧ�˿� */
  tcp_bind(pcb, IP_ADDR_ANY, TCP_HTTP_SERVER_PORT);       

  /* ��������������״̬ */
  pcb = tcp_listen(pcb);				

  /* ע�������accept�ص����� */	
  tcp_accept(pcb, HttpServerAccept);   
										
}

/* HTTP�ͻ��˳�ʼ������*/
 void Http_Client_Initialization(void)
{
  struct tcp_pcb *tcp_client_pcb;
  ip_addr_t ipaddr;

  /* ��Ŀ���������IPд��һ���ṹ�壬Ϊpc����������IP��ַ */
  IP4_ADDR(&ipaddr,httpServerIP[0],httpServerIP[1],httpServerIP[2],httpServerIP[3]);

  /* Ϊtcp�ͻ��˷���һ��tcp_pcb�ṹ��    */
  tcp_client_pcb = tcp_new();

  /* �󶨱��ض˺ź�IP��ַ */
  tcp_bind(tcp_client_pcb, IP_ADDR_ANY, TCP_HTTP_CLIENT_PORT);

  if (tcp_client_pcb != NULL)
  {
    /* ��Ŀ��������������ӣ�����������Ŀ��˿ں�Ŀ��IP */
    tcp_connect(tcp_client_pcb, &ipaddr, TCP_HTTP_SERVER_PORT, HTTPClientConnected);
    
    tcp_err(tcp_client_pcb, HTTPClientConnectError);
  }
}


/* HTTP�ͻ������ӵ��������ص����� */
static err_t HTTPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
  char clientString[]="GET https://www.cnblogs.com/foxclever/ HTTP/1.1\r\n"
                                    "Host:www.cnblogs.com:80\r\n\r\n";

  /* ���ý��ջص����� */
  tcp_recv(pcb, HTTPClientCallback);

  /* ����һ���������ӵ��ʺ��ַ���*/
  tcp_write(pcb,clientString, strlen(clientString),0);

  return ERR_OK;
}

/* HTTP�ͻ��˽��յ����ݺ�����ݴ���ص����� */
static err_t HTTPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
  struct pbuf *tcp_send_pbuf;
  char echoString[]="GET https://www.cnblogs.com/foxclever/ HTTP/1.1\r\n"
                                    "Host:www.cnblogs.com:80\r\n\r\n";

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
    Http_Client_Initialization();

    return ERR_OK;
  }

  return ERR_OK;
}

/* HTTP�ͻ������ӷ���������ص����� */
static void HTTPClientConnectError(void *arg, err_t err)
{
  /* ������������ */
  Http_Client_Initialization();
}


/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
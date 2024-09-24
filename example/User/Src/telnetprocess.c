/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�telnetprocess.c                                                **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�ֻ���LwIP��Telnet����������                             **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "telnetprocess.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#include "netportdefine.h"

#define MAX_MSG_SIZE 30         //�������󳤶�
#define LOGIN_INFO "Please input Password to login:"    //�ͻ��˵�¼��ʾ��Ϣ
#define PASSWORD "111111"       //Ĭ������

extern unsigned int sys_now(void);
char *command[] = {"hello","date","time","version","quit","help"};
enum TELNET_STATE 
{
  TELNET_SETUP,
  TELNET_CONNECTED,
};

typedef struct 
{
  int state;
  u16_t client_port;
  u16_t bytes_len;
  char bytes[MAX_MSG_SIZE];
}telnet_conn_arg;

/* TELNET���ջص��������ͻ��˽������Ӻ󣬱����������� */
static err_t TelnetServerAccept(void *arg, struct tcp_pcb *pcb, err_t err);
/* TELNET���Ӵ���ص����������ӹ���ʱ���ñ����� */
static void TelnetServeConnectError(void *arg, err_t err);
/* TELNET��������Ϣ����ص�������������Ϣ��Ҫ����ʱ�����ô˺��� */
static err_t TelnetServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
/* TELNET�������봦��*/
static int TelnetCommandInput(struct tcp_pcb *pcb, telnet_conn_arg * conn_args, struct pbuf *p);
/* TELNET�������봦��*/
static int TelnetCommandInput(struct tcp_pcb *pcb, telnet_conn_arg * conn_args, struct pbuf *p);

/* �����������ر�TELNET����*/
static void ServerCloseTelnetConnection(struct tcp_pcb *pcb)
{
  if(NULL != (void *)pcb->callback_arg)
  {
    mem_free((void *)pcb->callback_arg);
    tcp_arg(pcb, NULL);
  }

  tcp_close(pcb);
}

/* TELNET�������*/
static int TelnetCommandParse(struct tcp_pcb *pcb, char *req)
{
  char res_buffer[100] = {0,};
  int strlen = 0;
  int close_flag = 0;
	
  if(strcmp(req, command[0]) == 0)              //hello����
  {
    strlen = sprintf(res_buffer, "Hello! This is an LwIP-based Telnet Server..\r\n");
  }
  else if(strcmp(req, command[1]) == 0)         //date����
  {
    strlen = sprintf(res_buffer, "The current date is August 1, 2018..\r\n");
  }
  else if(strcmp(req, command[2]) == 0)         //time����
  {
    strlen = sprintf(res_buffer, "The current system time is [%u]..\r\n", sys_now());
  }
  else if(strcmp(req, command[3]) == 0)         //version����
  {
    strlen = sprintf(res_buffer, "The current version is V1.0.1..\r\n");
  }
  else if(strcmp(req, command[4]) == 0)         //quit����
  {
    strlen = sprintf(res_buffer, "The Connection will shutdown..\r\n");
    close_flag = 1;
  }
  else if(strcmp(req, command[5]) == 0)         //help����
  {
    strlen = sprintf(res_buffer,"Suppprted Command:date  hello  systick  version  help  quit..\r\n");
  }
  else          //δ��������
  {
    strlen = sprintf(res_buffer, "This command is not supported by the system..\r\n");
  }

  /* ��������лظ����������ʾ��*/
  tcp_write(pcb, res_buffer, strlen, TCP_WRITE_FLAG_COPY);
  strlen = sprintf(res_buffer, "LwIP Telnet>");
  tcp_write(pcb, res_buffer, strlen, TCP_WRITE_FLAG_COPY);

  return close_flag;
}

/* TELNET�������봦��*/
static int TelnetCommandInput(struct tcp_pcb *pcb, telnet_conn_arg * conn_args, struct pbuf *p)
{
  int strlen = 0;
  char buf[20];
  u16_t len=p->len;
  u8_t * datab = (unsigned char *)p->payload;

  if((len == 2) && (*datab == 0x0d) && (*(datab+1) == 0x0a))    //���յ��س�����Ϊ��������
  {	
    conn_args->bytes[conn_args->bytes_len] = 0x00;
    return 1;
  }
  else if((len == 1) && (*datab >= 0x20) && (*datab <= 0x7e))   //�յ���ͨ�ַ�
  {
    conn_args->bytes[conn_args->bytes_len] = *datab;
    
    if(conn_args->bytes_len < (MAX_MSG_SIZE-1))
    {
      conn_args->bytes_len++;
    }
		
  }
  else if((len == 1) && (*datab == 0x08) && (conn_args->bytes_len > 0)) //�յ��˸������ǰ����������ַ�
  {
    conn_args->bytes_len--;
    strlen = sprintf(buf," \b \b");
    tcp_write(pcb, buf, strlen, TCP_WRITE_FLAG_COPY);
  }
  else if((len == 1) && (*datab == 0x08))       //�յ��˸������ǰ����������ַ�
  {
    conn_args->bytes_len = 0;
    strlen = sprintf(buf,">");
    tcp_write(pcb, buf, strlen, TCP_WRITE_FLAG_COPY);
  }
  return 0;
}

/* TELNET��������Ϣ����ص�������������Ϣ��Ҫ����ʱ�����ô˺��� */
static err_t TelnetServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  telnet_conn_arg *conn_args = (telnet_conn_arg *)arg;
  char sndbuf[50];
  int strlen = 0;
  int ret = 0;
  
  if(NULL == conn_args || pcb->remote_port != conn_args->client_port)
  {
    if(p!= NULL)
    {
      pbuf_free(p);
    }
    return ERR_ARG;
  }
  
  if (p != NULL) 
  {        
    /* ���½��մ��� */
    tcp_recved(pcb, p->tot_len);

    ret = TelnetCommandInput(pcb, conn_args, p);

    if(ret == 1)//����������
    {
      switch(conn_args->state)
      {
      case TELNET_SETUP:
        {
          if(strcmp(conn_args->bytes,PASSWORD) == 0)//������ȷ
          {
            strlen = sprintf(sndbuf,"##Hello! This is an LwIP-based Telnet Server##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY); 
            strlen = sprintf(sndbuf,"##Created by Moonan...                      ##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY);
            strlen = sprintf(sndbuf,"##Enter help for help.  Enter quit for quit.##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY);
            strlen = sprintf(sndbuf,"LwIP Telnet>");
            tcp_write(pcb,sndbuf,strlen, 1);

            conn_args->state = TELNET_CONNECTED;//ת��״̬
          }
          else//���������ʾ���µ�¼
          {
            strlen = sprintf(sndbuf,"##PASSWORD ERROR! Try again:##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY);
          }
          memset(conn_args->bytes, 0, MAX_MSG_SIZE);
          conn_args->bytes_len = 0;
          break;
        }
      case TELNET_CONNECTED:
        {
          if(TelnetCommandParse(pcb, conn_args->bytes) == 0)
          {
            memset(conn_args->bytes, 0, MAX_MSG_SIZE);
            conn_args->bytes_len = 0;
          }
          else
          {
            /* �������ر����� */
            ServerCloseTelnetConnection(pcb);
          }
          break;
        }
      default:
        {
          break;
        }
      }
    }
    pbuf_free(p);
  }  
  else if (err == ERR_OK) 
  {
    /* �������ر����� */
    ServerCloseTelnetConnection(pcb);
  }
  
  return ERR_OK;

}

/* TELNET���Ӵ���ص����������ӹ���ʱ���ñ����� */
static void TelnetServeConnectError(void *arg, err_t err)
{
  Telnet_Server_Initialization();
}

/* TELNET���ջص��������ͻ��˽������Ӻ󣬱����������� */
static err_t TelnetServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{     
  u32_t remote_ip;
  char linkInfo [100];
  u8_t iptab[4];
  telnet_conn_arg *conn_arg = NULL;
  remote_ip = pcb->remote_ip.addr;

  iptab[0] = (u8_t)(remote_ip >> 24);
  iptab[1] = (u8_t)(remote_ip >> 16);
  iptab[2] = (u8_t)(remote_ip >> 8);
  iptab[3] = (u8_t)(remote_ip);

  //���ɵ�¼��ʾ��Ϣ
  sprintf(linkInfo, "Welcome to Telnet! your IP:Port --> [%d.%d.%d.%d:%d]\r\n", \
  	              iptab[3], iptab[2], iptab[1], iptab[0], pcb->remote_port);	

  conn_arg = mem_calloc(sizeof(telnet_conn_arg), 1);
  if(!conn_arg)
  {
    return ERR_MEM;
  }

  conn_arg->state = TELNET_SETUP;
  conn_arg->client_port = pcb->remote_port;
  conn_arg->bytes_len = 0;
  memset(conn_arg->bytes, 0, MAX_MSG_SIZE);
  
  tcp_arg(pcb, conn_arg);
  
  /* ע��Telnet���������Ӵ���ص����� */
  tcp_err(pcb, TelnetServeConnectError);
  /* ע��Telnet��������Ϣ����ص�����*/
  tcp_recv(pcb, TelnetServerCallback);
  
  /* ���ӳɹ������͵�¼��ʾ��Ϣ */  
  tcp_write(pcb, linkInfo, strlen(linkInfo), 1);
  tcp_write(pcb, LOGIN_INFO, strlen(LOGIN_INFO), 1); 
  
  return ERR_OK;
}

/* TELNET��������ʼ������*/
void Telnet_Server_Initialization(void)
{
  struct tcp_pcb *pcb;	            		
  
  /* ����һ���µ�TCP���ƿ� */
  pcb = tcp_new();	                		 	

  /* ���ƿ�������IP�Ͷ�Ӧ�˿� */
  tcp_bind(pcb, IP_ADDR_ANY, TCP_TELNET_SERVER_PORT);       


  /* ��������������״̬ */
  pcb = tcp_listen(pcb);				

  /* ע�������accept�ص����� */	
  tcp_accept(pcb, TelnetServerAccept); 						
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
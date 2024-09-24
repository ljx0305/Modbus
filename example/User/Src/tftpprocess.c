/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�tftpprocess.c                                                  **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�ֻ���LwIP��TFTP����������                               **/

/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "tftpprocess.h"
#include "netportdefine.h"

/* TFTP�������ַ��� */
char *tftp_errorcode_string[] = {
                                  "not defined",
                                  "file not found",
                                  "access violation",
                                  "disk full",
                                  "illegal operation",
                                  "unknown transfer id",
                                  "file already exists",
                                  "no such user",
                                };

/* ����TFTP��Ϣ */
static err_t SendTftpMessage(struct udp_pcb *upcb,const ip_addr_t *to_ip, int to_port, char *buf, int buflen);
/* ʹ�û�õĴ�����Ϣ��Ϊ��������buf�й���һ��������Ϣ */
static int ConstructTftpErrorMessage(char *buf, tftp_errorcode err);
/* ���첢�Ҵ������ݰ� */
static int SendTftpDataPacket(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, int block,char *buf, int buflen);
/* ���첢��ͻ��˷���һ��������Ϣ */
static int SendTftpErrorMessage(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, tftp_errorcode err);
/* ��ÿһ������addr:port�������󴴽�һ���µĶ˿���������Ӧ����������Ӧ���� */
static void ProcessTftpRequest(struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port);
/* TFTP�������ص����� */
static void TftpServerCallback(void *arg, struct udp_pcb *upcb, struct pbuf *p,const ip_addr_t *addr, u16_t port);


/* ����TFTP��Ϣ */
static err_t SendTftpMessage(struct udp_pcb *upcb,const ip_addr_t *to_ip, int to_port, char *buf, int buflen)
{
  err_t err;
  struct pbuf *pkt_buf = NULL; /* �������͵�pbuf�� */

  /* ָ������� */
  pkt_buf = pbuf_alloc(PBUF_TRANSPORT, buflen, PBUF_RAM);

  if (!pkt_buf)
    return ERR_MEM;

  memcpy(pkt_buf->payload, buf, buflen);

  /* Sending packet by UDP protocol */
  err = udp_send(upcb, pkt_buf);

  /* free the buffer pbuf */
  pbuf_free(pkt_buf);

  return err;
}


/* ʹ�û�õĴ�����Ϣ��Ϊ��������buf�й���һ��������Ϣ */
static int ConstructTftpErrorMessage(char *buf, tftp_errorcode err)
{
  int errorlen;
  /* �ڿ�ͷ��2���ֽ����ò����� */
  SetTftpOpCode(buf, TFTP_ERROR);
  /* �ڽ��ӵ�2���ֽ����ô����� */
  SetTftpErrorCode(buf, err);
  /* ����󼸸��ֽ����ô�����Ϣ */
  SetTftpErrorMessage(buf, tftp_errorcode_string[err]);
  /* ���ô�����Ϣ�ĳ��� */
  errorlen = strlen(tftp_errorcode_string[err]);

  /* ������Ϣ���� */
  return (4 + errorlen + 1);
}

/* ���첢��ͻ��˷���һ��������Ϣ */
static int SendTftpErrorMessage(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, tftp_errorcode err)
{
  char buf[512];
  int error_len;

  error_len = ConstructTftpErrorMessage(buf, err);

  return SendTftpMessage(upcb, to, to_port, buf, error_len);
}

/* ���첢�Ҵ������ݰ� */
static int SendTftpDataPacket(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, int block,char *buf, int buflen)
{
  /* ����ʼ��2���ֽ�����Ϊ������ */
  SetTftpOpCode(buf, TFTP_DATA);
  
  /* ������2���ֽ�����Ϊ��� */
  SetTftpBlockNumber(buf, block);
  
  /* �ں�������n���ֽڵ����� */

  /* �������ݰ� */
  return SendTftpMessage(upcb, to, to_port, buf, buflen + 4);
}

/*���첢����ȷ�ϰ�*/
int SendTftpAckPacket(struct udp_pcb *upcb,const ip_addr_t *to, int to_port, int block)
{
  /* ����һ��TFTP ACK�� */
  char packet[TFTP_ACK_PKT_LEN];

  /* ����ʼ��2���ֽ�����Ϊ������ */
  SetTftpOpCode(packet, TFTP_ACK);

  /* �ƶ�ACK�Ŀ�� */
  SetTftpBlockNumber(packet, block);

  return SendTftpMessage(upcb, to, to_port, packet, TFTP_ACK_PKT_LEN);
}

/* �ر��ļ����͡��Ͽ����Ӳ�ɾ�����ƿ� */
void CleanTftpConnection(struct udp_pcb *upcb, tftp_connection_args *args)
{
  /* �ͷ�args�ṹ�� */
  mem_free(args);

  /* �Ͽ����ƿ����� */
  udp_disconnect(upcb);

  /* ɾ�����ƿ� */
  udp_remove(upcb);

}

/* ������һ���� */
void SendNextBlock(struct udp_pcb *upcb, tftp_connection_args *args,const ip_addr_t *to_ip, u16_t to_port)
{
  /* ���ļ��ж�ȡ512�ֽڣ���������뵽"args->data" */
  int total_block = args->tot_bytes/TFTP_DATA_LEN_MAX;
  total_block +=1;

  /* ��Ŵ��󣬷��� */
  if(total_block < 1 || args->block > total_block )
  {
    return;
  }

  args->data_len = TFTP_DATA_LEN_MAX;
  //�ж��Ƿ�Ϊ���һ��
  if(total_block == args->block)
  {
    if(args->tot_bytes%TFTP_DATA_LEN_MAX == 0)
    {
      args->data_len = 0;
    }
    else
    {
      args->data_len = args->tot_bytes - (total_block - 1)*TFTP_DATA_LEN_MAX;
    }
  }
  
  memset(args->data + TFTP_DATA_PKT_HDR_LEN, ('a'-1) + args->block%26 , args->data_len);

  /* �������ݰ� */
  SendTftpDataPacket(upcb, to_ip, to_port, args->block, args->data, args->data_len);

}

/* ��������ص����������յ����������������ACK�����ñ��������� */
void RrqReceiveCallback(void *_args, struct udp_pcb *upcb, struct pbuf *p,const ip_addr_t *addr, u16_t port)
{
  /* ��ȡ����״̬ */
  tftp_connection_args *args = (tftp_connection_args *)_args;
  if(port != args->remote_port)
  {
    /* ������� */
    CleanTftpConnection(upcb, args);

    pbuf_free(p);
    
    return;
  }

  if (CheckTftpIsCorrectAck(p->payload, args->block))
  {
    /* ���ӿ�� */
    args->block++;
  }
  else
  {
    /* ��֤������Ų��䣬���·��� */
  }

  /* ������һ�ζ�ȡ���ص��ֽ�������������ֽ��������ļ�������� */
  if (args->data_len < TFTP_DATA_LEN_MAX)
  {
    /* ������� */
    CleanTftpConnection(upcb, args);

    pbuf_free(p);

    return;
  }

  /* ��������ļ���û�з����꣬����� */
  SendNextBlock(upcb, args, addr, port);

  pbuf_free(p);

}

/* TFTP��������*/
int TftpReadProcess(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, char* FileName)
{
  tftp_connection_args *args = NULL;

  /* ��������ڻص������б����ã�����жϱ����ã�������ǿ���ʹ�ó����malloc */
  args = mem_malloc(sizeof(tftp_connection_args));
  
  if (!args)
  {
    /* �ڴ����ʧ�� */
    SendTftpErrorMessage(upcb, to, to_port, TFTP_ERR_NOTDEFINED);

    CleanTftpConnection(upcb, args);

    return 0;
  }

  /* i��ʼ�����ӽṹ��  */
  args->op = TFTP_RRQ;
  args->remote_port = to_port;
  args->block = 1; /* ��Ŵ�1��ʼ */
  args->tot_bytes = 10*1024*1024;


  /* ע��ص����� */
  udp_recv(upcb, RrqReceiveCallback, args);

  /* ͨ�����͵�һ�������������ӣ����������յ�ACK����*/
   SendNextBlock(upcb, args, to, to_port);

  return 1;
}

/*д������ص��������ͻ����յ�ACK�󣬷������ݰ������������ñ��������� */
void WrqReceiveCallback(void *_args, struct udp_pcb *upcb, struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port)
{
  tftp_connection_args *args = (tftp_connection_args *)_args;

  u16_t next_block = 0;
  
  if (port != args->remote_port || pkt_buf->len != pkt_buf->tot_len)
  {
    CleanTftpConnection(upcb, args);
    pbuf_free(pkt_buf);
    return;
  }

  next_block = args->block + 1;
  
  /* �ж����ݰ��Ƿ�����Ч������ */
  if ((pkt_buf->len > TFTP_DATA_PKT_HDR_LEN)&&(ExtractTftpBlock(pkt_buf->payload) == next_block))
  {
    /* �������ǵĿ�ţ�ʹ֮���յ��Ŀ��ƥ�� */
    args->block++;
    /* �������ֽ��� */
    (args->tot_bytes) += (pkt_buf->len - TFTP_DATA_PKT_HDR_LEN);
  }
  else if (ExtractTftpBlock(pkt_buf->payload) == next_block)
  {
    /* �������ǵĿ�ţ�ʹ֮���յ��Ŀ��ƥ�� */
    args->block++;
  }
  else
  {

  }

  /* ��ͻ��˷���ACK */
  SendTftpAckPacket(upcb, addr, port, args->block);

  /* �ж��Ƿ�����ϣ�������ݳ���С��512������� */
  if (pkt_buf->len < TFTP_DATA_PKT_LEN_MAX)
  {
    CleanTftpConnection(upcb, args);
    pbuf_free(pkt_buf);
  }
  else
  {
    pbuf_free(pkt_buf);
    return;
  }

}

/* TFTPд������ */
int TftpWriteProcess(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, char *FileName)
{
  tftp_connection_args *args = NULL;

  /* ��������ڻص������б����ã�����жϱ����ã�������ǿ���ʹ�ó����malloc */
  args = mem_malloc(sizeof(tftp_connection_args));
  
  if (!args)
  {
    SendTftpErrorMessage(upcb, to, to_port, TFTP_ERR_NOTDEFINED);

    CleanTftpConnection(upcb, args);

    return 0;
  }

  args->op = TFTP_WRQ;
  args->remote_port = to_port;
  args->block = 0;      //WRQ��Ӧ�Ŀ��Ϊ0
  args->tot_bytes = 0;

  /* Ϊ���ƿ�ע��ص����� */
  udp_recv(upcb, WrqReceiveCallback, args);

  /* ͨ�����͵�һ��ack������д���� */
  SendTftpAckPacket(upcb, to, to_port, args->block);

  return 0;
}

/* ��ÿһ������addr:port�������󴴽�һ���µĶ˿���������Ӧ����������Ӧ���� */
static void ProcessTftpRequest(struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port)
{
  tftp_opcode op = ExtractTftpOpcode(pkt_buf->payload);
  char FileName[50] = {0};
  struct udp_pcb *upcb = NULL;
  err_t err;
  
  /* �����µ�UDP PCB���ƿ� */
  upcb = udp_new();
  if (!upcb)
  {
    return;
  }
  
  /* ���� */
  err = udp_connect(upcb, addr, port);
  if (err != ERR_OK)
  {  
    return;
  }
  
  ExtractTftpFilename(FileName, pkt_buf->payload);

  switch (op)
  {
  case TFTP_RRQ:
    {

      TftpReadProcess(upcb, addr, port, FileName);
      break;
    }
  case TFTP_WRQ:
    {
      /* ����TFTPдģʽ */
      TftpWriteProcess(upcb, addr, port, FileName);
      break;
    }
  default:
    {
      /* �쳣�����ʹ�����Ϣ */
      SendTftpErrorMessage(upcb, addr, port, TFTP_ERR_ACCESS_VIOLATION);

      udp_remove(upcb);

      break;
    }
  }
}

/* TFTP�������ص����� */
static void TftpServerCallback(void *arg, struct udp_pcb *upcb, struct pbuf *p,const ip_addr_t *addr, u16_t port)
{
  /* �����µ��������� */
  ProcessTftpRequest(p, addr, port);

  pbuf_free(p);
}

/* ��ʼ��TFTP������ */
void Tftp_Server_Initialization(void)
{
  err_t err;
  struct udp_pcb *tftp_server_pcb = NULL;

  /* �����µ� UDP PCB���ƿ� */
  tftp_server_pcb = udp_new();
  
  /* �ж�UDP���ƿ��Ƿ���ȷ���� */
  if (NULL == tftp_server_pcb)
  {  
    return;
  }

  /* ��PCB���ƿ鵽ָ���˿� */
  err = udp_bind(tftp_server_pcb, IP_ADDR_ANY, UDP_TFTP_SERVER_PORT);
  
  if (err != ERR_OK)
  {
    udp_remove(tftp_server_pcb);
    return;
  }

  /* ע��TFTP������������ */
  udp_recv(tftp_server_pcb, TftpServerCallback, NULL);
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
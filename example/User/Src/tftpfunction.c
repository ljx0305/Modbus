/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�tftpfunction.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�ֻ���LwIP��TFTP��������������������TFTP��׼�ļ�         **/
/**           RFC1350ʵ�֡�TFTP����5�ֹ������4�ֱ��ĸ�ʽ��                  **/
/**           ���й��������£�                                               **/
/**           +-------+--------------------+                                 **/
/**           | Opcode|     operation      |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   1   |Read request (RRQ)  |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   2   |Write request (WRQ) |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   3   |Data (DATA)         |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   4   |Acknowledgment (ACK)|                                 **/
/**           +-------+--------------------+                                 **/
/**           |   5   |Error (ERROR)       |                                 **/
/**           +-------+--------------------+                                 **/
/**           ������/д�������ʽ��                                          **/
/**           +--------+----------+------+-------+------+                    **/
/**           |2 bytes |  string  |1 byte|string |1 byte|                    **/
/**           +--------+----------+------+-------+------+                    **/
/**           | Opcode | Filename |   0  | Mode  |   0  |                    **/
/**           +--------+----------+------+-------+------+                    **/
/**           ���ݰ���ʽ��                                                   **/
/**           +--------+----------+---------+                                **/
/**           |2 bytes | 2 bytes  | n bytes |                                **/
/**           +--------+----------+---------+                                **/
/**           | Opcode |  Block # |   Data  |                                **/
/**           +--------+----------+---------+                                **/
/**           ȷ�ϰ���ʽ��                                                   **/
/**           +--------+---------+                                           **/
/**           | 2 bytes| 2 bytes |                                           **/
/**           +------------------+                                           **/
/**           | Opcode | Block # |                                           **/
/**           +--------+---------+                                           **/
/**           �������ʽ��                                                   **/
/**           +--------+----------+--------+------+                          **/
/**           |2 bytes |2 bytes   | string |1 byte|                          **/
/**           +--------+----------+--------+------+                          **/
/**           | Opcode | ErrorCode| ErrMsg |   0  |                          **/
/**           +--------+----------+--------+------+                          **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include <string.h>
#include "lwip/inet.h"
#include "tftpfunction.h"


/* ��TFTP��Ϣ�н��������� */
tftp_opcode ExtractTftpOpcode(char *buf)
{
  return (tftp_opcode)(buf[1]);
}

/* ��TFTP��Ϣ����ȡ��� */
uint16_t ExtractTftpBlock(char *buf)
{
  u16_t *b = (u16_t*)buf;
  return ntohs(b[1]);
}

/* ��TFTP��Ϣ�н����ļ��� */
void ExtractTftpFilename(char *fname, char *buf)
{
  strncpy(fname, buf + 2, 30);
}

/* ���ò����룺 RRQ / WRQ / DATA / ACK / ERROR */
void SetTftpOpCode(char *buffer, tftp_opcode opcode)
{

  buffer[0] = 0;
  buffer[1] = (u8_t)opcode;
}

/* ���ô����� */
void SetTftpErrorCode(char *buffer, tftp_errorcode errCode)
{

  buffer[2] = 0;
  buffer[3] = (u8_t)errCode;
}

/* ���ô�����Ϣ */
void SetTftpErrorMessage(char * buffer, char* errormsg)
{
  strcpy(buffer + 4, errormsg);
}

/* ��ACK/DATA�ڶ����ֽ����ÿ�� */
void SetTftpBlockNumber(char* packet, u16_t block)
{
  u16_t *p = (u16_t *)packet;
  p[1] = htons(block);
}

/* ΪDATA������Ϣ������ֽ� */
void SetTftpDataMessage(char* packet, char* buf, int buflen)
{
  memcpy(packet + 4, buf, buflen);
}

/* ����յ���ȷ���Ƿ���ȷ */
uint32_t CheckTftpIsCorrectAck(char *buf, int block)
{
  /* �ж��Ƿ�Ϊ����ȷ�ϰ� */
  if (ExtractTftpOpcode(buf) != TFTP_ACK)
    return 0;

  /* �ȽϿ����� */
  if (block != ExtractTftpBlock(buf))
    return 0;

  return 1;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
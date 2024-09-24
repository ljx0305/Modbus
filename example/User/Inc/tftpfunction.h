/******************************************************************************/
/** ģ�����ƣ���������ϵͳLwIP��̫��ͨѶ                                     **/
/** �ļ����ƣ�tftpfunction.h                                                 **/
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

#ifndef __TFTP_FUNCTION_H
#define __TFTP_FUNCTION_H

/* TFTP�����붨�� */
typedef enum {
  TFTP_RRQ = 1,
  TFTP_WRQ = 2,
  TFTP_DATA = 3,
  TFTP_ACK = 4,
  TFTP_ERROR = 5
} tftp_opcode;


/* TFTP�����붨�� */
typedef enum {
  TFTP_ERR_NOTDEFINED,
  TFTP_ERR_FILE_NOT_FOUND,
  TFTP_ERR_ACCESS_VIOLATION,
  TFTP_ERR_DISKFULL,
  TFTP_ERR_ILLEGALOP,
  TFTP_ERR_UKNOWN_TRANSFER_ID,
  TFTP_ERR_FILE_ALREADY_EXISTS,
  TFTP_ERR_NO_SUCH_USER,
} tftp_errorcode;

/* ��TFTP��Ϣ�н��������� */
tftp_opcode ExtractTftpOpcode(char *buf);

/* ��TFTP��Ϣ����ȡ��� */
uint16_t ExtractTftpBlock(char *buf);

/* ��TFTP��Ϣ�н����ļ��� */
void ExtractTftpFilename(char *fname, char *buf);

/* ���ò����룺 RRQ / WRQ / DATA / ACK / ERROR */
void SetTftpOpCode(char *buffer, tftp_opcode opcode);

/* ���ô����� */
void SetTftpErrorCode(char *buffer, tftp_errorcode errCode);

/* ���ô�����Ϣ */
void SetTftpErrorMessage(char * buffer, char* errormsg);

/* ��ACK/DATA�ڶ����ֽ����ÿ�� */
void SetTftpBlockNumber(char* packet, u16_t block);

/* ΪDATA������Ϣ������ֽ� */
void SetTftpDataMessage(char* packet, char* buf, int buflen);

/* ����յ���ȷ���Ƿ���ȷ */
uint32_t CheckTftpIsCorrectAck(char *buf, int block);

#endif

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
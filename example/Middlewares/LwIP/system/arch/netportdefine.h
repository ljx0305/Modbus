/******************************************************************************/
/** ģ�����ƣ���̫��ͨѶ����ģ��                                             **/
/** �ļ����ƣ�netportdefine.h                                                **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����TCP/IPЭ�����У���������˿ڵĺ�                           **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-08-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __NET_PORT_DEFINE_H
#define __NET_PORT_DEFINE_H

#define TCP_ECHO_SERVER_PORT            7       //TCP ECHO�����ԣ�Э��˿�
#define UDP_ECHO_SERVER_PORT            7       //UDP ECHO�����ԣ�Э��˿�

#define TCP_FTP_SERVER_DATA_PORT        20      //�ļ�����Э�� - Ĭ�����ݶ˿�
#define UDP_FTP_SERVER_DATA_PORT        20      //�ļ�����Э�� - Ĭ�����ݶ˿�
#define TCP_FTP_SERVER_CTRL_PORT	21	//�ļ�����Э�� - ���ƶ˿�
#define TCP_FTP_SERVER_CTRL_PORT	21	//�ļ�����Э�� - ���ƶ˿�

#define	TCP_TELNET_SERVER_PORT          23      //Telnet �ն˷���Э�� - δ�����ı�ͨ��
#define	UDP_TELNET_SERVER_PORT          23	//Telnet �ն˷���Э�� - δ�����ı�ͨ��

#define	TCP_DNS_SERVER_PORT             53	//DNS����������ϵͳ��
#define	UDP_DNS_SERVER_PORT             53	//DNS����������ϵͳ��

#define	UDP_TFTP_SERVER_PORT            69	//TFTP��С���ļ�����Э�飩

#define	TCP_HTTP_SERVER_PORT            80	//HTTP�����ı�����Э�飩- ���ڴ�����ҳ

#define	TCP_REMOTE_TELNET_SERVER_PORT   107	//Զ��TelnetЭ��

#define	UDP_NTP_SERVER_PORT             123	//NTP (����ʱ��Э��) - ����ʱ��ͬ��

#define	TCP_SNMP_SERVER_PORT            161	//SNMP (���������Э��)
#define	UDP_SNMP_SERVER_PORT            161	//SNMP (���������Э��)

#define	TCP_MODBUS_SERVER_PORT          502	//ModbusЭ��
#define	UDP_MODBUS_SERVER_PORT          502	//ModbusЭ��

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�mbtcpclient.c                                                  **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺��������Modbus TCP�ͻ��˵�������Լ�����                       **/
/**           1��Modbus TCP�ͻ����û�Ӧ�õĽӿڲ�                            **/
/**           2��ʵ�ַ�����������������ɲ����䴫��Ӧ�ò�                    **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2016-04-17          ľ��              �����ļ�               **/
/**     V1.1.0  2020-09-01          ľ��              ����һЩ��֪����       **/
/**                                                                          **/
/******************************************************************************/ 

#include "mbtcpclient.h"

/*����MBAP����ͷ*/
static uint16_t CreateMbapHeadPart(TCPLocalClientType *client,uint8_t * mbapHead);
/* ��TCP�ͻ������TCP�������б�ڵ� */
static void AddTCPServerNode(TCPLocalClientType *client,TCPAccessedServerType *server);
/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ��*/
static int FindCommandForRecievedMessage(TCPLocalClientType *client,uint8_t *recievedMessage);
/*�������վ״̬��������Ϣ������Ȧ״̬λ0x01������*/
static void HandleReadCoilStatusRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*�������վ״̬��������Ϣ��������״̬λ0x02������*/
static void HandleReadInputStatusRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*�������վ�Ĵ���ֵ�ķ�����Ϣ�������ּĴ���0x03�����룩*/
static void HandleReadHoldingRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*�������վ�Ĵ���ֵ�ķ�����Ϣ��������Ĵ���0x04������*/
static void HandleReadInputRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);

void (*HandleServerRespond[])(TCPLocalClientType *client,uint8_t *,uint16_t,uint16_t)={HandleReadCoilStatusRespond,
HandleReadInputStatusRespond,
HandleReadHoldingRegisterRespond,
HandleReadInputRegisterRespond};

/*���ɷ��ʷ�����������*/
uint16_t CreateAccessServerCommand(TCPLocalClientType *client,ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes)
{
  uint16_t commandLength=0;
  
  CreateMbapHeadPart(client,commandBytes);
  
  /*���ɶ���������������������0x01��0x02��0x03��0x04,�����12���ֽ�*/
  if((objInfo.functionCode>=ReadCoilStatus)&&(objInfo.functionCode <= ReadInputRegister))
  {
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,NULL,NULL,commandBytes);
  }
  
  /*����Ԥ�÷�������������������0x05,0x0F,������淢�����ݶ���*/
  if((objInfo.functionCode==WriteSingleCoil)||(objInfo.functionCode==WriteMultipleCoil))
  {
    bool *statusList=(bool*)dataList;
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,statusList,NULL,commandBytes);
  }
  
  /*����Ԥ�÷�������������������0x06,0x10,������淢�����ݶ���*/
  if((objInfo.functionCode==WriteSingleRegister)||(objInfo.functionCode==WriteMultipleRegister))
  {
    uint16_t *registerList=(uint16_t*)dataList;
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,NULL,registerList,commandBytes);
  }
  
  return commandLength;
}

/*�����յ��ķ�������Ӧ��Ϣ*/
void ParsingServerRespondMessage(TCPLocalClientType *client,uint8_t *recievedMessage)
{
  TCPAccessedServerType *currentServer=client->pCurrentServer;
  
  /*�жϽ��յ�����Ϣ�Ƿ�����Ӧ������*/
  int cmdIndex=FindCommandForRecievedMessage(client,recievedMessage);
  
  if((cmdIndex<0))      //û�ж�Ӧ�������������Ų����
  {
    return;
  }
  
  if((recievedMessage[2]!=0x00)||(recievedMessage[3]!=0x00)) //����Modbus TCPЭ��
  {
    return;
  }
  
  if(recievedMessage[7]>0x04)   //���������0x04���Ƕ������
  {
    return;
  }
  
  uint16_t mLength=(recievedMessage[4]<<8)+recievedMessage[5];
  uint16_t dLength=(uint16_t)recievedMessage[8];
  if(mLength!=dLength+3)        //���ݳ��Ȳ�һ��
  {
    return;
  }
  
  FunctionCode fuctionCode=(FunctionCode)recievedMessage[7];
  
  if(fuctionCode!=currentServer->pReadCommand[cmdIndex][7])
  {
    return;
  }
  
  uint16_t startAddress=(uint16_t)currentServer->pReadCommand[cmdIndex][8];
  startAddress=(startAddress<<8)+(uint16_t)currentServer->pReadCommand[cmdIndex][9];
  uint16_t quantity=(uint16_t)currentServer->pReadCommand[cmdIndex][10];
  quantity=(quantity<<8)+(uint16_t)currentServer->pReadCommand[cmdIndex][11];
  
  if(quantity*2!=dLength)       //��������ݳ����뷵�ص����ݳ��Ȳ�һ��
  {
    return;
  }
  
  if((fuctionCode>=ReadCoilStatus)&&(fuctionCode<=ReadInputRegister))
  {
    HandleServerRespond[fuctionCode-1](client,recievedMessage,startAddress,quantity);
  }
}

/*������������ѷ��������б����������*/
void AddCommandBytesToList(TCPLocalClientType *client,uint8_t *commandBytes)
{
  TCPAccessedServerType *currentServer=client->pCurrentServer;

  if(currentServer->cmdOrder>=currentServer->cmdNumber)
  {
    currentServer->cmdOrder=0;
  }
  
  for(int i=0;i<12;i++)
  {
    currentServer->pReadCommand[currentServer->cmdOrder][i]=commandBytes[i];
  }
  
  currentServer->cmdOrder++;
}

/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ��*/
/*��������ѷ��������б���ɾ�������������������������Ϣ*/
static int FindCommandForRecievedMessage(TCPLocalClientType *client,uint8_t *recievedMessage)
{
  int cmdIndex=-1;
  TCPAccessedServerType *currentServer=client->pCurrentServer;
  
  for(int i=0;i<currentServer->cmdNumber;i++)
  {
    if((recievedMessage[0]==currentServer->pReadCommand[i][0])&&(recievedMessage[1]==currentServer->pReadCommand[i][1]))
    {
      cmdIndex=i;
      break;
    }
  }
  
  return cmdIndex;
}

/*�����������״̬��������Ϣ������Ȧ״̬λ0x01������*/
static void HandleReadCoilStatusRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool coilStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,coilStatus,NULL);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateCoilStatus(serverAddress,startAddress,quantity,coilStatus);
}

/*�����������״̬��������Ϣ��������״̬λ0x02������*/
static void HandleReadInputStatusRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool inputStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,inputStatus,NULL);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateInputStatus(serverAddress,startAddress,quantity,inputStatus);
}

/*������������Ĵ���ֵ�ķ�����Ϣ�������ּĴ���0x03�����룩*/
static void HandleReadHoldingRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t holdingRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,holdingRegister);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateHoldingRegister(serverAddress,startAddress,quantity,holdingRegister);
}

/*������������Ĵ���ֵ�ķ�����Ϣ��������Ĵ���0x04������*/
static void HandleReadInputRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t inputRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,inputRegister);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateInputResgister(serverAddress,startAddress,quantity,inputRegister);
}

/*����MBAPͷ����,Ϊ�˷�����RTU��ͬ��������Ԫ��ʶ����Զ�̴�վ��ַ��������������ʱ����*/
/*�˴�MBAPͷֻ��7���ֽڣ����Ԫ��ʶ��ΪĬ��ֵ0x01*/
static uint16_t CreateMbapHeadPart(TCPLocalClientType *client,uint8_t * mbapHead)
{
  uint16_t index=0;
  //���������־
  if(client->transaction==65535)
  {
    client->transaction=1;
  }
  else
  {
    client->transaction=client->transaction+1;
  }
  mbapHead[index++]=client->transaction>>8;//�����ʶ��
  mbapHead[index++]=client->transaction;//�����ʶ��
  mbapHead[index++]=0x00;//Э���ʶ����modbus��ʶ��Ϊ0
  mbapHead[index++]=0x00;//Э���ʶ����modbus��ʶ��Ϊ0
  mbapHead[index++]=0x00;//�ֽڳ��ȣ��˴��в���ȷ�����ں�������
  mbapHead[index++]=0x01;//�ֽڳ��ȣ��˴��в���ȷ�����ں�������
  mbapHead[index++]=0x01;//��Ԫ��ʶ��Ĭ��ֵ
  return index;
}

/*��ʼ��TCP�ͻ��˶���*/
void InitializeTCPClientObject(TCPLocalClientType *client,
                               UpdateCoilStatusType pUpdateCoilStatus,
                               UpdateInputStatusType pUpdateInputStatus,
                               UpdateHoldingRegisterType pUpdateHoldingRegister,
                               UpdateInputResgisterType pUpdateInputResgister
                                 )
{
  client->transaction=0;

  client->pServerList=NULL;
  client->pCurrentServer=NULL;
  
  client->pUpdateCoilStatus=pUpdateCoilStatus!=NULL?pUpdateCoilStatus:UpdateCoilStatus;
  
  client->pUpdateInputStatus=pUpdateInputStatus!=NULL?pUpdateInputStatus:UpdateInputStatus;
  
  client->pUpdateHoldingRegister=(pUpdateHoldingRegister!=NULL)?pUpdateHoldingRegister:UpdateHoldingRegister;
  
  client->pUpdateInputResgister=(pUpdateInputResgister!=NULL)?pUpdateInputResgister:UpdateInputResgister;
}

/* ʵ����TCP���������� */
void InstantiateTCPServerObject(TCPAccessedServerType *server,          //Ҫʵ�����ķ���������
                                TCPLocalClientType *client,             //�������������ؿͻ��˶���
                                uint8_t ipSegment1,                     //IP��ַ��1��
                                uint8_t ipSegment2,                     //IP��ַ��2��
                                uint8_t ipSegment3,                     //IP��ַ��3��
                                uint8_t ipSegment4,                     //IP��ַ��4��
                                uint16_t port,                          //�˿ڣ�Ĭ��Ϊ502
                                uint16_t cmdNumber,                     //����������������127
                                uint8_t(*pReadCommand)[12],             //�������б�
                                uint16_t writedCoilNumber,              //��д��Ȧ���ڵ������
                                WritedCoilListNode *pCoilList,          //д��Ȧ�б�
                                uint16_t writedRegisterNumber,          //��д�Ĵ������ڵ������
                                WritedRegisterListNode *pRegisterList)  //д�Ĵ����б�
{
    if ((server == NULL) || (client == NULL))
    {
        return;
    }

  server->ipAddress.ipSegment[0]=ipSegment1;
  server->ipAddress.ipSegment[1]=ipSegment2;
  server->ipAddress.ipSegment[2]=ipSegment3;
  server->ipAddress.ipSegment[3]=ipSegment4;
  
  server->port = port > 0 ? port : 502;

  server->flagPresetServer=0;
  server->cmdNumber = cmdNumber;
  server->cmdOrder = 0;
  server->pReadCommand = pReadCommand;
  
  server->writedCoilNumber=writedCoilNumber;
  server->pWritedCoilList=pCoilList;
  
  server->writedRegisterNumber=writedRegisterNumber;
  server->pWritedRegisterList=pRegisterList;
  
  server->pNextNode=NULL;
  
  /* ��TCP�ͻ������TCP�������б�ڵ� */
  AddTCPServerNode(client,server);
}

/* ��TCP�ͻ������TCP�������б�ڵ� */
static void AddTCPServerNode(TCPLocalClientType *client,TCPAccessedServerType *server)
{
  TCPAccessedServerType *currentNode=NULL;
  
  if((client==NULL)||(server==NULL))
  {
    return;
  }

  currentNode=client->pServerList;
  
  if(currentNode==NULL)
  {
    client->pServerList=server;
  }
  else if(server->ipAddress.ipSegment[3]<currentNode->ipAddress.ipSegment[3])
  {
    client->pServerList=server;
    server->pNextNode=currentNode;
  }
  else
  {
    while(currentNode->pNextNode!=NULL)
    {
      if((currentNode->ipAddress.ipSegment[3]<=server->ipAddress.ipSegment[3])&&(server->ipAddress.ipSegment[3]<currentNode->pNextNode->ipAddress.ipSegment[3]))
      {
        server->pNextNode=currentNode->pNextNode;
        currentNode->pNextNode=server;
        break;
      }
      else
      {
        currentNode=currentNode->pNextNode;
      }
    }
    
    if(currentNode->pNextNode==NULL)
    {
      currentNode->pNextNode=server;
    }
  }
}

/* ʹ�ܻ���ʧ��д������������־λ���޸ķ�������дʹ�ܱ�־λ�� */
void ModifyWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress,bool en)
{
  TCPAccessedServerType *currentNode;
  currentNode=client->pServerList;
  
  while(currentNode!=NULL)
  {
    if(currentNode->ipAddress.ipSegment[3]==ipAddress)
    {
      if(en)
      {
        currentNode->flagPresetServer=1;
      }
      else
      {
        currentNode->flagPresetServer=0;
      }
    }
    currentNode=currentNode->pNextNode;
  }
}

/* ��÷�������дʹ�ܱ�־λ��״̬ */
bool GetWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress)
{
  bool status=false;
  
  TCPAccessedServerType *currentNode;
  currentNode=client->pServerList;
  
  while(currentNode!=NULL)
  {
    if((currentNode->ipAddress.ipSegment[3]==ipAddress)&&(currentNode->flagPresetServer>0))
    {
      status=true;
      break;
    }
    currentNode=currentNode->pNextNode;
  }
  
  return status;
}

/* �жϵ�ǰ�Ƿ���д����ʹ�� */
bool CheckWriteTCPServerNone(TCPLocalClientType *client)
{
  bool status=true;
  
  TCPAccessedServerType *currentNode;
  currentNode=client->pServerList;
  
  while(currentNode!=NULL)
  {
    if(currentNode->flagPresetServer>0)
    {
      status=false;
      break;
    }
    currentNode=currentNode->pNextNode;
  }
  
  return status;
}

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/
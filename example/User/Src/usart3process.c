/******************************************************************************/
/** ģ�����ƣ�Modbus��������ʵ��ģ��                                         **/
/** �ļ����ƣ�mcutsprocess.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺Modbus��������ʵ��ģ����Զ˿ڴ��нӿ������ʵ�ֶ���λ����ͨ   **/
/**           Ѷ������USART3�˿ڣ�����RS485��ʵ��Modbus RTU��վ��            **/
/**           PB10      USART3_TX       USART3���з���                       **/
/**           PB11      USART3_RX       USART3���н���                       **/
/**           PB7       RS485_CTL3      RS485�շ�����                        **/
/**           ����STM32F407ZGT6Ӳ��ƽ̨����������HAL FW_F4 V1.26.0��       **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2021-03-29          ���Ҿ�            �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "usart3process.h"

#define RS485_CTL3_Pin GPIO_PIN_7
#define RS485_CTL3_GPIO_Port GPIOB

#define MCUT_RECIEVE_ALLOW()   HAL_GPIO_WritePin(RS485_CTL3_GPIO_Port,RS485_CTL3_Pin,GPIO_PIN_RESET)    /*����Ϊ����*/
#define MCUT_TRANSMIT_ALLOW()  HAL_GPIO_WritePin(RS485_CTL3_GPIO_Port,RS485_CTL3_Pin,GPIO_PIN_SET)      /*����Ϊ����*/

/*����Զ�̴�վ��վ��ַ*/
#define RemoteSlave1    1
#define RemoteSlave2    2
#define RemoteSlave3    3
#define RemoteSlave4    4

#define MCUTRECEIVELENGTH 256    //�������ݵ���󳤶�

UART_HandleTypeDef huart3;

extern AnalogParaTypeDef aPara;
extern DigitalParaTypeDef dPara;

uint16_t mcutRxLength=0;
uint8_t mcutRxBuffer[MCUTRECEIVELENGTH];

RTULocalMasterType deMaster;
uint8_t deReadRegisterCommand[8][8];
RTUAccessedSlaveType deSlave[4]={{RemoteSlave1,0,2,&deReadRegisterCommand[0],NULL,0x00,0x00},
{RemoteSlave2,0,2,&deReadRegisterCommand[2],NULL,0x00,0x00},
{RemoteSlave3,0,2,&deReadRegisterCommand[4],NULL,0x00,0x00},
{RemoteSlave4,0,2,&deReadRegisterCommand[6],NULL,0x00,0x00}};

/* RS485�������ų�ʼ�� */
static void GPIO_Init_Configuration(void);
/* ���������˿�USART3��ʼ�� */
static void USART3_Init_Configuration(void);
/*��������*/
static void McutSendData(uint8_t *sData,uint16_t sSize);
/*���¶������ı��ּĴ���*/
static void DeUpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);
/*���¶���������Ȧ״̬*/
static void DeUpdateCoilStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue);
/* д�Ĵ������� */
static void WriteSlaveOperation(void);

/*�豸��������ͨѶ����*/
void LocalMasterProcess(void)
{
    WriteSlaveOperation();
    
    if(deMaster.readOrder>=deMaster.slaveNumber)
    {
        deMaster.readOrder=0;
    }
    
    if(CheckWriteRTUSlaveNone(&deMaster))      //û����������
    {
        /*���Ͷ��������������*/
        if(deSlave[deMaster.readOrder].cmdOrder>=deSlave[deMaster.readOrder].commandNumber)
        {
            deSlave[deMaster.readOrder].cmdOrder=0;
        }
        
        McutSendData(deSlave[deMaster.readOrder].pReadCommand[deSlave[deMaster.readOrder].cmdOrder],8);
        
        deSlave[deMaster.readOrder].pLastCommand=deSlave[deMaster.readOrder].pReadCommand[deSlave[deMaster.readOrder].cmdOrder];
        
        deSlave[deMaster.readOrder].cmdOrder++;
        
        HAL_Delay(30);
        
        ParsingSlaveRespondMessage(&deMaster,mcutRxBuffer,NULL);
        
        deMaster.readOrder++;
    }

}

/* д�Ĵ������� */
static void WriteSlaveOperation(void)
{
     /* д��վ1���� */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[0].stationAddress))
    {
        
        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[0].stationAddress,false);
    }
    
    /* д��վ2���� */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[1].stationAddress))
    {
        

        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[1].stationAddress,false);
    }
    
    /* д��վ3���� */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[2].stationAddress))
    {
        
        
        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[2].stationAddress,false);
    }
    
    /* д��վ4���� */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[3].stationAddress))
    {
        
        
        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[3].stationAddress,false);
    }
}

/*��������*/
static void McutSendData(uint8_t *sData,uint16_t sSize)
{
    /*�ر��ж�*/
    __HAL_UART_DISABLE_IT(&huart3,UART_IT_RXNE);
    /*RS485����Ϊ����ģʽ��׼������*/
    MCUT_TRANSMIT_ALLOW();
    mcutRxLength=0;
    
    HAL_UART_Transmit(&huart3,sData,sSize,1000);
    
    /*������ϣ���RS485��Ϊ����ģʽ׼������*/
    MCUT_RECIEVE_ALLOW();
    
    /*���ô��ڽ����ж�*/
    __HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
    
}

/*���ݽ����жϴ�����,��ӵ�USART3�ж���Ӧ������*/
void USART3_ReceiveDataHandle(void)
{
    if(mcutRxLength>=MCUTRECEIVELENGTH)
    {
        mcutRxLength=0;
    }
    
    if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_ORE)!=RESET)
    {
        uint8_t rData;
        /*��ȡ���յ����ֽ�*/
        HAL_UART_Receive(&huart3,&rData,1,100);
    }
    
    /*���ռĴ���Ϊ��,�ȴ��ֽڱ���Ӧ�Ĵ�����ȫ����*/
    if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_RXNE)!=RESET)
    {
        uint8_t rData;
        /*��ȡ���յ����ֽ�*/
        HAL_UART_Receive(&huart3,&rData,1,100);
        if((mcutRxLength !=0x00 )||(rData != 0x00))
        {
            mcutRxBuffer[mcutRxLength++] = rData;
        }
        __HAL_UART_CLEAR_FLAG(&huart3,UART_FLAG_RXNE);
    }
}

/*�Ա��������˿�����*/
void LocalMasterConfiguration(void)
{
    ObjAccessInfo objInfo;
    
    /* RS485�������ų�ʼ�� */
    GPIO_Init_Configuration();
    /* ���������˿�USART3��ʼ�� */
    USART3_Init_Configuration();

    objInfo.unitID=RemoteSlave1;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[0]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[1]);
    
    objInfo.unitID=RemoteSlave2;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[2]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[3]);
    
    objInfo.unitID=RemoteSlave3;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[4]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[5]);
    
    objInfo.unitID=RemoteSlave4;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[6]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[7]);
    
    /*��ʼ��RTU��վ����*/
    InitializeRTUMasterObject(&deMaster,        //��վ����
                              4,                  //��վ��Ӧ�Ĵ�վ����
                              deSlave,         //��վ����Ĵ�վ�б�
                              DeUpdateCoilStatus,               //��վ������Ȧ������������Ȧ��NULL
                              NULL,               //��վ����״̬������������״̬NULL
                              DeUpdateHoldingRegister,  //���±��ּĴ�������
                              NULL    //��������Ĵ�������
                                  );
    
    MCUT_RECIEVE_ALLOW();
}

/* ���������˿�USART3��ʼ�� */
static void USART3_Init_Configuration(void)
{
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 9600;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        Error_Handler();
    }
    
    /*���ô��ڽ����ж�*/
    __HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
}

/* RS485�������ų�ʼ�� */
static void GPIO_Init_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* ʹ��GPIO�˿�ʱ�� */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    /* ����GPIO���ų�ʼ��ƽ */
    HAL_GPIO_WritePin(RS485_CTL3_GPIO_Port, RS485_CTL3_Pin, GPIO_PIN_RESET);
    
    /* ����GPIO����RS485_CTL3_Pin */
    GPIO_InitStruct.Pin = RS485_CTL3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RS485_CTL3_GPIO_Port, &GPIO_InitStruct);
    
}

/*���¶���������Ȧ״̬*/
static void DeUpdateCoilStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue)
{
    uint16_t startCoil=CoilEndAddress+1;
    
  switch(salveAddress)
    {
    case RemoteSlave1:       //���¶�ȡ��վ1����
        {
            startCoil=8;
            break;
        }
    case RemoteSlave2:      //���¶�ȡ��վ2����
        {
            startCoil=16;
            break;
        }
    case RemoteSlave3:      //���¶�ȡ��վ3����
        {
            startCoil=24;
            break;
        }
    case RemoteSlave4:      //���¶�ȡ��վ4����
        {
            startCoil=32;
            break;
        }
    default:                      //����̬
        {
            startCoil=CoilEndAddress+1;
            break;
        }
    }
  
  if(startCoil<=CoilEndAddress)
    {
        for(int i=0;i<quantity;i++)
        {
            dPara.coil[startCoil+i]=stateValue[i];
        }
    }
}

/*���¶������ı��ּĴ���*/
static void DeUpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
    uint16_t startRegister=HoldingRegisterEndAddress+1;
    
    switch(salveAddress)
    {
    case RemoteSlave1:       //���¶�ȡ��վ1����
        {
            startRegister=8;
            break;
        }
    case RemoteSlave2:      //���¶�ȡ��վ2����
        {
            startRegister=16;
            break;
        }
    case RemoteSlave3:      //���¶�ȡ��վ3����
        {
            startRegister=24;
            break;
        }
    case RemoteSlave4:      //���¶�ȡ��վ4����
        {
            startRegister=32;
            break;
        }
    default:                      //����̬
        {
            startRegister=HoldingRegisterEndAddress+1;
            break;
        }
    }
    
    if(startRegister<=HoldingRegisterEndAddress)
    {
        for(int i=0;i<quantity;i++)
        {
            aPara.holdingRegister[startRegister+i]=registerValue[i];
        }
    }
}
/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/
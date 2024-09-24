/******************************************************************************/
/** ģ�����ƣ�Modbus���ش�վʵ��ģ��                                         **/
/** �ļ����ƣ�mcudsprocess.h                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺Modbus���ش�վʵ��ģ����λ��ʾ���нӿ������ʵ�ֶ���λ����ͨ   **/
/**           Ѷ������USART1�˿ڣ�����RS232��ʵ��Modbus RTU��վ��            **/
/**           PA9       USART1_TX       USART1���з���                       **/
/**           PA10      USART1_RX       USART1���н���                       **/
/**           PD8       RS232_INT       RS232�ж��ź�                        **/
/**           ����STM32F407ZGT6Ӳ��ƽ̨����������HAL FW_F4 V1.26.0��       **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2021-03-29          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "usart1process.h"

#define RS232_INT_Pin GPIO_PIN_8
#define RS232_INT_GPIO_Port GPIOD

#define MCUDRECEIVELENGTH 256    //�������ݵ���󳤶�

UART_HandleTypeDef huart1;

extern AnalogParaTypeDef aPara;
extern DigitalParaTypeDef dPara;

uint16_t localStation=1;
uint16_t mcudRxLength=0;
uint8_t mcudRxBuffer[MCUDRECEIVELENGTH];

/* USART1��ʼ�� */
static void USART1_Init_Configuration(void);
/* RS232ͨѶGPIO��ʼ�� */
static void GPIO_Init_Configuration(void);
/* USART1���ݷ��ͺ��� */
static void McudSendData(uint8_t *sData,uint16_t sSize);

/*�豸����ͨѶ���ݴ���*/
void LocalSlaveProcess(void)
{
    uint16_t respondLength=0;
    if(mcudRxLength>=8)
    {
        uint8_t respondBytes[MCUDRECEIVELENGTH];
        
        respondLength=ParsingMasterAccessCommand(mcudRxBuffer,respondBytes,mcudRxLength,localStation);
        if(respondLength!=65535)
        {
            /* USART1���ݷ��ͺ��� */
            McudSendData(respondBytes,respondLength);
        }
    }
}

/* USART1���ݷ��ͺ��� */
static void McudSendData(uint8_t *sData,uint16_t sSize)
{
    /*�ر��ж�*/
    __HAL_UART_DISABLE_IT(&huart1,UART_IT_RXNE);
    mcudRxLength=0;
    
    HAL_UART_Transmit(&huart1,sData,sSize,1000);
    
    /*���ô��ڽ����ж�*/
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
}

/* ���ݽ����жϴ�����,��ӵ�USART1�ж���Ӧ������ */
void USART1_ReceiveDataHandle(void)
{
    if(mcudRxLength>=MCUDRECEIVELENGTH)
    {
        mcudRxLength=0;
    }
    
    if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_ORE)!=RESET)
    {
        uint8_t rData;
        /*��ȡ���յ����ֽ�*/
        HAL_UART_Receive(&huart1,&rData,1,100);
    }
    
    /*���ռĴ���Ϊ��,�ȴ��ֽڱ���Ӧ�Ĵ�����ȫ����*/
    if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET)
    {
        uint8_t rData;
        /*��ȡ���յ����ֽ�*/
        HAL_UART_Receive(&huart1,&rData,1,100);
        if((0!=mcudRxLength)||(localStation==rData))
        {
            mcudRxBuffer[mcudRxLength++] = rData;
        }
        __HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
    }
}

/*�豸����ͨѶ���ݴ���*/
void LocalSlaveConfiguration(void)
{
    /* USART1��ʼ�� */
    USART1_Init_Configuration();
    
    /* RS232ͨѶGPIO��ʼ�� */
    GPIO_Init_Configuration();
    
    
}

/* RS232ͨѶGPIO��ʼ�� */
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
    
    /* ����GPIO���ţ�RS232_INT_Pin */
    GPIO_InitStruct.Pin = RS232_INT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* USART1��ʼ�� */
static void USART1_Init_Configuration(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    
    /*���ô��ڽ����ж�*/
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
}

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/

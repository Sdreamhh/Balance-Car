#include "UART1.h"
#include "stm32f10x_conf.h"
#include "Queue.h"

//���ڷ���״̬
typedef enum
{
  UART_STATE_OFF, //����δ��������
  UART_STATE_ON,  //�������ڷ�������
  UART_STATE_MAX
}EnumUARTState;             

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/    
static  StructCirQue s_structUARTSendCirQue;  //���ʹ���ѭ������
static  StructCirQue s_structUARTRecCirQue;   //���մ���ѭ������
static  u8  s_arrSendBuf[UART1_BUF_SIZE];     //���ʹ���ѭ�����еĻ�����
static  u8  s_arrRecBuf[UART1_BUF_SIZE];      //���մ���ѭ�����еĻ�����

static  u8  s_iUARTTxSts;                     //���ڷ�������״̬
          
/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  void  InitUARTBuf(void);      //��ʼ�����ڻ��������������ͻ������ͽ��ջ����� 
static  u8    WriteReceiveBuf(u8 d);  //�����յ�������д����ջ�����
static  u8    ReadSendBuf(u8 *p);     //��ȡ���ͻ������е�����
                                            
static  void  ConfigUART(u32 bound);  //���ô�����صĲ���������GPIO��RCC��USART��NVIC 
static  void  EnableUARTTx(void);     //ʹ�ܴ��ڷ��ͣ�WriteUARTx�е��ã�ÿ�η�������֮����Ҫ����                                      
                                            
static  void  SendCharUsedByFputc(u16 ch);  //�����ַ�������ר��fputc��������
  
/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitUARTBuf
* �������ܣ���ʼ�����ڻ��������������ͻ������ͽ��ջ�����  
* ���������void
* ���������void
* �� �� ֵ��void 
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void  InitUARTBuf(void)
{
  i16 i;

  for(i = 0; i < UART1_BUF_SIZE; i++)
  {
    s_arrSendBuf[i] = 0;
    s_arrRecBuf[i]  = 0;  
  }

  InitQueue(&s_structUARTSendCirQue, s_arrSendBuf, UART1_BUF_SIZE);
  InitQueue(&s_structUARTRecCirQue,  s_arrRecBuf,  UART1_BUF_SIZE);
}

/*********************************************************************************************************
* �������ƣ�WriteReceiveBuf
* �������ܣ�д���ݵ����ڽ��ջ����� 
* ���������d����д�봮�ڽ��ջ�����������
* ���������void
* �� �� ֵ��д�����ݳɹ���־��0-���ɹ���1-�ɹ� 
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  u8  WriteReceiveBuf(u8 d)
{
  u8 ok = 0;  //д�����ݳɹ���־��0-���ɹ���1-�ɹ�
                                                                    
  ok = EnQueue(&s_structUARTRecCirQue, &d, 1);   
                                                                    
  return ok;  //����д�����ݳɹ���־��0-���ɹ���1-�ɹ� 
}

/*********************************************************************************************************
* �������ƣ�ReadSendBuf
* �������ܣ���ȡ���ڷ��ͻ������е����� 
* ���������p�������������ݴ�ŵ��׵�ַ
* ���������p�������������ݴ�ŵ��׵�ַ
* �� �� ֵ����ȡ���ݳɹ���־��0-���ɹ���1-�ɹ� 
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  u8  ReadSendBuf(u8 *p)
{
  u8 ok = 0;  //��ȡ���ݳɹ���־��0-���ɹ���1-�ɹ�
                                                                   
  ok = DeQueue(&s_structUARTSendCirQue, p, 1);  
                                                                   
  return ok;  //���ض�ȡ���ݳɹ���־��0-���ɹ���1-�ɹ� 
}

/*********************************************************************************************************
* �������ƣ�ConfigUART
* �������ܣ����ô�����صĲ���������GPIO��RCC��USART��NVIC  
* ���������bound��������
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void  ConfigUART(u32 bound)
{
  GPIO_InitTypeDef  GPIO_InitStructure;   //GPIO_InitStructure���ڴ��GPIO�Ĳ���
  USART_InitTypeDef USART_InitStructure;  //USART_InitStructure���ڴ��USART�Ĳ���
  NVIC_InitTypeDef  NVIC_InitStructure;   //NVIC_InitStructure���ڴ��NVIC�Ĳ���
  
  //ʹ��RCC���ʱ��                                                            
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //ʹ��USART1��ʱ��    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��GPIOA��ʱ��
  
  //����TX��GPIO 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;               //����TX������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;          //����TX��ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //����TX��I/O������ٶ�
  GPIO_Init(GPIOA, &GPIO_InitStructure);                    //���ݲ�����ʼ��TX��GPIO
  
  //����RX��GPIO
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;              //����RX������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;    //����RX��ģʽ
  GPIO_Init(GPIOA, &GPIO_InitStructure);                    //���ݲ�����ʼ��RX��GPIO
  
  //����USART�Ĳ���
  USART_StructInit(&USART_InitStructure);                   //��ʼ��USART_InitStructure
  USART_InitStructure.USART_BaudRate   = bound;             //���ò�����
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //���������ֳ���
  USART_InitStructure.USART_StopBits   = USART_StopBits_1;  //����ֹͣλ
  USART_InitStructure.USART_Parity     = USART_Parity_No;   //������żУ��λ
  USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;           //����ģʽ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //����Ӳ��������ģʽ
  USART_Init(USART1, &USART_InitStructure);                 //���ݲ�����ʼ��USART1

  //����NVIC
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;         //�ж�ͨ����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //������ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; //���������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);                           //���ݲ�����ʼ��NVIC

  //ʹ��USART1�����ж�
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //ʹ�ܽ��ջ������ǿ��ж�
  USART_ITConfig(USART1, USART_IT_TXE,  ENABLE);  //ʹ�ܷ��ͻ��������ж�
  USART_Cmd(USART1, ENABLE);                      //ʹ��USART1
                                                                     
  s_iUARTTxSts = UART_STATE_OFF;  //���ڷ�������״̬����Ϊδ��������
}

/*********************************************************************************************************
* �������ƣ�EnableUARTTx
* �������ܣ�ʹ�ܴ��ڷ��ͣ���WriteUARTx�е��ã���ÿ�η�������֮����Ҫ�������������ʹ�ܷ����ж� 
* ���������void
* ���������void
* �� �� ֵ��void 
* �������ڣ�2018��01��01��
* ע    �⣺s_iUARTTxSts = UART_STATE_ON;��仰�������USART_ITConfig֮ǰ������ᵼ���жϴ��޷�ִ��
*********************************************************************************************************/
static  void  EnableUARTTx(void)
{
  s_iUARTTxSts = UART_STATE_ON;                     //���ڷ�������״̬����Ϊ���ڷ�������

  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);     //ʹ�ܷ����ж�
}

/*********************************************************************************************************
* �������ƣ�SendCharUsedByFputc
* �������ܣ������ַ�������ר��fputc��������  
* ���������ch�������͵��ַ�
* ���������void
* �� �� ֵ��void 
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void  SendCharUsedByFputc(u16 ch)
{  
  USART_SendData(USART1, (u8)ch);

  //�ȴ��������
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
    
  }
}


u8 flag_move, flag_back, flag_stop, flag_left, flag_right;
extern float Kd;
void USART1_IRQHandler(void)                
{
  u8  uData = 0;

  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //���ջ������ǿ��ж�
  {                                                         
    NVIC_ClearPendingIRQ(USART1_IRQn);  //���USART1�жϹ���
    uData = USART_ReceiveData(USART1);  //��USART1���յ������ݱ��浽uData
		
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));

		
		switch(uData)
		{
			case 0x01://ǰ��
				flag_move = 1;
				flag_back = 0;
				flag_stop = 0;
				flag_left = 0;
				flag_right= 0;
				break;
			case 0x02://����
				flag_move = 0;
				flag_back = 1;
				flag_stop = 0;
				flag_left = 0;
				flag_right= 0;
				break;
			case 0x03://��ת
				flag_move = 0;
				flag_back = 0;
				flag_stop = 0;
				flag_left = 1;
				flag_right= 0;

				break;
			case 0x04://��ת
				flag_move = 0;
				flag_back = 0;
				flag_stop = 0;
				flag_left = 0;
				flag_right= 1;
				break;
			case 0x05://ֹͣ
				flag_move = 0;
				flag_back = 0;
				flag_stop = 1;
				flag_left = 0;
				flag_right= 0;
				break;
			case 0x06://����
				Kd += 0.01;

				break;
			case 0x07:
				Kd -= 0.01;
			  break;
		}
		
                                                          
    WriteReceiveBuf(uData);  //�����յ�������д����ջ�����                                 
  }                                                         
                                                            
                                                           
  if(USART_GetITStatus(USART1, USART_IT_TXE)!= RESET)    //���ͻ��������ж�
  {                                                        
    USART_ClearITPendingBit(USART1, USART_IT_TXE);       //��������жϱ�־
    NVIC_ClearPendingIRQ(USART1_IRQn);                   //���USART1�жϹ���
                                                           
    ReadSendBuf(&uData);                                 //��ȡ���ͻ����������ݵ�uData
                                                                    
    USART_SendData(USART1, uData);                       //��uDataд��USART_DR
                                                                                           
    if(QueueEmpty(&s_structUARTSendCirQue))              //�����ͻ�����Ϊ��ʱ
    {                                                               
      s_iUARTTxSts = UART_STATE_OFF;                     //���ڷ�������״̬����Ϊδ��������       
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);     //�رմ��ڷ��ͻ��������ж�
    }
  }
} 

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitUART1
* �������ܣ���ʼ��UARTģ�� 
* ���������bound,������
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void InitUART1(u32 bound)
{
  InitUARTBuf();        //��ʼ�����ڻ��������������ͻ������ͽ��ջ�����  
                  
  ConfigUART(bound);    //���ô�����صĲ���������GPIO��RCC��USART��NVIC  
}

/*********************************************************************************************************
* �������ƣ�WriteUART1
* �������ܣ�д���ڣ���д���ݵ��Ĵ��ڷ��ͻ�����  
* ���������pBuf��Ҫд�����ݵ��׵�ַ��len������д�����ݵĸ���
* ���������void
* �� �� ֵ���ɹ�д�����ݵĸ�������һ�����β�len���
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u8  WriteUART1(u8 *pBuf, u8 len)
{
  u8 wLen = 0;  //ʵ��д�����ݵĸ���
                                                                  
  wLen = EnQueue(&s_structUARTSendCirQue, pBuf, len);

  if(wLen < UART1_BUF_SIZE)
  {
    if(s_iUARTTxSts == UART_STATE_OFF)
    {
      EnableUARTTx();
    }    
  }
                                                                  
  return wLen;  //����ʵ��д�����ݵĸ���
}

/*********************************************************************************************************
* �������ƣ�ReadUART1
* �������ܣ������ڣ�����ȡ���ڽ��ջ������е�����  
* ���������pBuf����ȡ�����ݴ�ŵ��׵�ַ��len��������ȡ���ݵĸ���
* ���������pBuf����ȡ�����ݴ�ŵ��׵�ַ
* �� �� ֵ���ɹ���ȡ���ݵĸ�������һ�����β�len���
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u8  ReadUART1(u8 *pBuf, u8 len)
{
  u8 rLen = 0;  //ʵ�ʶ�ȡ���ݳ���
                                                    
  rLen = DeQueue(&s_structUARTRecCirQue, pBuf, len);

  return rLen;  //����ʵ�ʶ�ȡ���ݵĳ���
}
    
/*********************************************************************************************************
* �������ƣ�fputc
* �������ܣ��ض�����  
* ���������ch��f
* ���������void
* �� �� ֵ��int 
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
int fputc(int ch, FILE* f)
{
  SendCharUsedByFputc((u8) ch);  //�����ַ�������ר��fputc��������
                               
  return ch;                     //����ch
}

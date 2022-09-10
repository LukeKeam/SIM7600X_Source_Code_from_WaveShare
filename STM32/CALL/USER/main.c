#include "stm32f10x.h"
#include "usart.h"
#include "Led.h"
#include "SysTick.h"
#include "timer.h"
#include "string.h"



#define Buf2_Max 200 					  //����2���泤��

char Uart2_Buf[Buf2_Max];
u8 Times=0,First_Int = 0,shijian=0;
u16 Heartbeat=0;
vu8 Timer0_start;	//��ʱ��0��ʱ����������
vu8 Heart_beat;		//��������֡��־λ

int ATCMD_number=4;//��Ҫ���͵�ָ������
char *AT_Command[]={
  "AT+CPIN?",
	"AT+CSQ",
  "AT+CREG?",
	"ATD10086;",
	//"AT+CMGF=1",
  //"AT+CMGS=\"10086\"",
};
void CLR_Buf2(void);
void UART2_Send(float *a)
{
	while(*a)//����ַ���������
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
		USART_SendData(USART2 ,*a++);//���͵�ǰ�ַ�
	}
}


/*******************************************************************************
* ������ : main 
* ����   : ������
* ע��   : ����2������CP2102ģ��ͨ�ţ�����1���ڴ��ڵ��ԣ����Ա��������س���ʱ����
					 �����͵�ģ��
*******************************************************************************/
u16 data=0x1a;
int main(void)
{
  int cmd_count;
	SysTick_Init_Config();
  GPIO_Config();
  Timer2_Init_Config();
  USART1_Init_Config(115200);
  USART2_Init_Config(115200);
  UART1_SendString("SIM7600X call 10086 Demo\r\n");
  for(cmd_count=0;cmd_count<ATCMD_number;cmd_count++){
  UART2_SendString(AT_Command[cmd_count]);
    UART2_SendLR();
		Delay_nMs(5000); 
    UART1_SendString(Uart2_Buf);
    UART1_SendLR();
    CLR_Buf2();
  }
		Delay_nMs(8000); 

}

/*******************************************************************************
* ������  : USART2_IRQHandler
* ����    : ����1�жϷ������
* ����    : ��
* ����    : �� 
* ˵��    : 
*******************************************************************************/
void USART2_IRQHandler(void)                	
{
			u8 Res=0;
			Res =USART_ReceiveData(USART2);
			Uart2_Buf[First_Int] = Res;  	  //�����յ����ַ����浽������
      
			First_Int++;                			//����ָ������ƶ�
			if(First_Int > Buf2_Max)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				First_Int = 0;
			}    
} 	

/*******************************************************************************
* ������  : TIM2_IRQHandler
* ����    : ��ʱ��2�ж϶Ϸ�����
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void TIM2_IRQHandler(void)   //TIM3�ж�
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if(Timer0_start)
		Times++;
		if(Times > shijian)
		{
			Timer0_start = 0;
			Times = 0;
		}
		
		Heartbeat++;
		if(Heartbeat>9)//ÿ10�뷢������֡
		{
			Heartbeat=0;
			Heart_beat=1;
		}
		if(flag)
		{
			LED4_ON(); 
			flag=0;
		}
		else
		{
			LED4_OFF(); 
			flag=1;
		}
	}	
}

void CLR_Buf2(void)
{
	u16 k;
	for(k=0;k<Buf2_Max;k++)      //��������������
	{
		Uart2_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}

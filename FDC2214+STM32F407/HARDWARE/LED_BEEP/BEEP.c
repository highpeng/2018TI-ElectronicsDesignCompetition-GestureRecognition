#include "BEEP.h" 
#include "delay.h"
#include "sys.h"
/*-------------------------------------------------------------------------
	���ܣ�����GPIO��
	���ߣ������	
	ʱ�䣺2018/6/7(version1.0)
	�޸�˵����
--------------------------------------------------------------------------*/

/***************************************************************************
	*�������ƣ�GPIO��ʼ������
	*�������ܣ���ʼ��GPIO��
	*��ڲ�������
	*���ز�������
	*��ע��
		GPIO_InitStructure����˵����
		GPIO_Pin��  GPIO_Pin_x ��x can be 0~15
		GPIO_Mode�� GPIO_Mode_IN ���� GPIO_Mode_OUT ��� GPIO_Mode_AF �˿ڸ��� GPIO_Mode_AN ģ��
		GPIO_OType��GPIO_OType_PP ���������GPIO_OType_OD ��©���
		GPIO_Speed��GPIO_Speed_2MHz��GPIO_Speed_25MHz��GPIO_Speed_50MHz��GPIO_Speed_100MHz
		GPIO_PuPd�� GPIO_PuPd_NOPULL �������� GPIO_PuPd_UP ���� GPIO_PuPd_DOWN ����
                                  
****************************************************************************/
void BEEP_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;                //����GPIO���ýṹ��

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; //����GPIOF4-5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;       //��©���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //�ٶ�100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;    //��������
  GPIO_Init(GPIOF, &GPIO_InitStructure);               //��ʼ����������GPIOF�ڽ��г�ʼ��
	
  GPIO_SetBits(GPIOF,GPIO_Pin_4);        							 //GPIOF4���ø�
  GPIO_SetBits(GPIOF,GPIO_Pin_5);        							 //GPIOF5���ø�
}


//������1����
void BEEP1_long_EN(void )
{
 GPIO_ResetBits(GPIOF,GPIO_Pin_4);        							 //GPIOF4���õ�
 delay_ms(10);
}


void BEEP1_DISEN(void)
{
 GPIO_SetBits(GPIOF,GPIO_Pin_4);                         //GPIOF4���ø�
}


//������2����
void BEEP2_long_EN(void )
{
 GPIO_ResetBits(GPIOF,GPIO_Pin_5);        							 //GPIOF5���õ�
 delay_ms(10);
}

void BEEP2_DISEN(void)
{
 GPIO_SetBits(GPIOF,GPIO_Pin_5);                         //GPIOF5���ø�
}

void Music_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;                //����GPIO���ýṹ��

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOFʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 					 //����GPIOF6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //�ٶ�100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;        //��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);               //��ʼ����������GPIOF�ڽ��г�ʼ��

  GPIO_ResetBits(GPIOC,GPIO_Pin_0);        					
}
void music_do(int i)//441hz,2.267ms=2268/2=1134us
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(1134);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(1134);}
}
void music_re(int i)//495hz,2.020ms=2020/2=1010us
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(1010);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(1010);}
}
void music_me(int i)
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(899);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(899);}
}

void music_fa(int i)
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(848);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(848);}
}
void music_so(int i)
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(756);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(756);}
}
void music_la(int i)
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(673);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(673);}
}
void music_xi(int i)
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(600);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(600);}
}

void music_doo(int i)
{
while(i--){
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_us(550);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_us(550);}
}


#include "sys.h"
#include "delay.h"
#include "silencht_usart.h"	
#include "fdc2214.h"
#include "fdc22142.h"
#include "DataScope_DP.h"
#include "lcd.h"
#include <stdio.h>
#include "exti.h"
#include "BEEP.h" 

//��������   ��1��  SCL: PC4  SDA:PC5  SD/ADDR:GND VCC:3.3V
//��������   ��2��  SCL��PF8  SDA:PF9  SD/ADDR:GND VCC:3.3V  
//�ĸ�������ť ��      �������� PF0-PF3
//����������+LED���У� �������� PF4-PF5
//��Դ������ PF7    

//��λ�������о�ʱ��ǰֵ8ͨ���Ĵ�
float res[8]={0};
float gesture[8]={0}; //����ʶ��Ĵ�
float num4_Sum1=0,num4_Sum2=0; //��¼�о�ʱѵ����1��2��number4�ĵ��ݺͣ��ж��Ƿ�����������


//ֱ��+ѵ���о�ʱ��ȭ����ͨ����¼�Ĵ�
u8 hand_record=0; 
const u8 rock_record_direc=0x77,rock_record_direc2=0xEE,
				 scissors_record_direc=0x66,scissors_record_direc2=0xCC,scissors_record_direc3=0x33,
				 paper_record_direc=0xff;

u8 gesture_record=0;  //����ʶ��ͨ����¼�Ĵ� ,����˳�������

//ֱ��+ѵ���о�ʱ��ȭ����ͨ����¼�Ĵ�
u8 number_hand=0;

const u8 num1_record_direc=0x11,num1_record_direc2=0x22,num1_record_direc3=0x44,num1_record_direc4=0x88,
				 num2_record_direc=0x33,num2_record_direc2=0x66,num2_record_direc3=0xCC,
				 num3_record_direc=0x77,num3_record_direc2=0xEE,
				 num4_record_direc=0xff;

//�о�ʱ������1/2�����Ƽ�¼
char test1_hand,test2_hand;


/*
	modeģʽָ��[����ť2����ģʽѡ��]��
							��ģʽ0����ʼ��������
							��ģʽ1��ֱ���о���ȭģʽ�������ɲ�����Աָ��
										  ʯͷռ1-3ͨ��������ռ1-2ͨ������ռ0-3ͨ��
							��ģʽ2��ֱ���о���ȭģʽ�������ɲ�����Աָ��
											
							��ģʽ3��ѵ����ȭģʽ��ʯͷ���������������������Ա�Լ�ָ����ѵ��
							��ģʽ4���о���ȭģʽ
							��ģʽ5��ѵ����ȭģʽ��ʯͷ���������������������Ա�Լ�ָ����ѵ��
							��ģʽ6���о���ȭģʽ
							��ģʽ7�����ټ�ģʽ
							��ģʽ8����������ʶ��
	modeģʽ����[[����ť3����ģʽ����]]��
							ÿ�Ρ���ʼ������ĳ��ģʽʱ��start_end��1
							ÿ�Ρ�����������ĳ��ģʽʱ��start_end��0
*/


int main(void)
{ 
//  unsigned char i;          //��������
//  unsigned char Send_Count; //������Ҫ���͵����ݸ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);      //��ʼ����ʱ����
	EXTIX_Init();         //�ⲿ�жϰ�����ʼ��
	usart1_init(115200);  //���Դ��� 
	BEEP_Init();					//������+LED���г�ʼ��
	Music_Init();         //�ټ���ʼ��
	LCD_Init();						//��ʼ��LCD
	LCD_Clear(WHITE);	 	  //����
	screen_init_background();
	delay_ms(10);         

	while(FDC2214_Init()) delay_ms(100);//FDC2214��ʼ��
	while(FDC2214_2Init())delay_ms(100);//FDC22142��ʼ��
	
  Get_CapInit_Value(); //ɨ��õ���ʼ����ֵ


/* 
����1����ó�ʼ����ֵ�жϰ�ť
����2���л�ģʽ	ģʽ1:ѵ����ȭ��ģʽ2:�о���ȭ��ģʽ3:ѵ����ȭ��12345����ģʽ4:�о���ȭ��12345��
					 u8 mode;               u8 start_end;  
����3��ģʽ����          u8 start_end;   //��ǰģʽ����Ϊ0����ʼΪ1
����4��ѵ��ʱ�䵹��ʱ			u8 roshambo_wait=0; //0��δ��ɣ�1�����

��ע��ֻ����ģʽ����start_end�رյ�ʱ�򣬲����л�ģʽ
 hand_record  ����ͨ����¼����4λΪѵ����1��0-3ͨ��������λΪѵ����2��0-3ͨ�� ��ӦλֵΪ0Ϊ����ָ��ֵΪ1Ϊ����ָ
*/
   while(1)
   	{	
			test1_hand='9';test2_hand='8';//Ϲд��������һ������
		  BEEP1_DISEN();
		  BEEP2_DISEN();
		if(start_end==1)    //�����ģʽ������
		{
			if(mode==8)       //��������ʶ��
			{
				gesture_record=Get_gesture_Value(gesture);
				if(gesture_record==0x80)LCD_ShowString(12,270,80,24,24," RIGHT ");
				if(gesture_record==0x40)LCD_ShowString(12,270,80,24,24,"  LEFT ");
				if(gesture_record==0x20)LCD_ShowString(135,270,80,24,24,"  RIGHT  ");
				if(gesture_record==0x10)LCD_ShowString(135,270,80,24,24,"  LEFT  ");			
			
			}
//------------------------------------------ֱ���о���ȭģʽ---------------------------------------------------------------	

			else if(mode==1)   
		  {
			      Get_CapCurrent_Value(res);//��ȡ��ǰ����ֵ
						hand_record=0;												//�����ȭ���Ƽ�¼
				    if((res[0]*100)>Threshold)	hand_record|=0x80;
						if((res[1]*100)>Threshold)	hand_record|=0x40;	
						if((res[2]*100)>Threshold)	hand_record|=0x20;
						if((res[3]*100)>Threshold)	hand_record|=0x10;	
						if((res[4]*100)>Threshold)	hand_record|=0x08;
					  if((res[5]*100)>Threshold)	hand_record|=0x04;	
					  if((res[6]*100)>Threshold)	hand_record|=0x02; 
					  if((res[7]*100)>Threshold)	hand_record|=0x01;
										
						if(	((hand_record&0xf0)==(rock_record_direc&0xf0))	|| ((hand_record&0xf0)==(rock_record_direc2&0xf0))	){ test1_hand='R';LCD_ShowString(15,140,128,16,16,"    Rock  ");} //u1_printf ("һ��ʯͷ!");
						else if((hand_record&0xf0)==(scissors_record_direc&0xf0)||(hand_record&0xf0)==(scissors_record_direc2&0xf0)||(hand_record&0xf0)==(scissors_record_direc3&0xf0)){ test1_hand='S';LCD_ShowString(20,140,128,16,16,"Scissors");}
						else if((hand_record&0xf0)==(paper_record_direc&0xf0)){test1_hand='P';LCD_ShowString(15,140,128,16,16,"   Paper ");}
						else {test1_hand='2';LCD_ShowString(15,140,71,16,16,"   Wait  ");}				
						
						if(	((hand_record&0x0f)==(rock_record_direc&0x0f)) || ((hand_record&0x0f)==(rock_record_direc2&0x0f)) ){	test2_hand='R';LCD_ShowString(120,140,128,16,16,"    Rock  ");	}				
						else if((hand_record&0x0f)==(scissors_record_direc&0x0f)||(hand_record&0x0f)==(scissors_record_direc2&0x0f)||(hand_record&0x0f)==(scissors_record_direc3&0x0f)){ test2_hand='S';LCD_ShowString(140,140,128,16,16,"Scissors");}			
						else if((hand_record&0x0f)==(paper_record_direc&0x0f)){test2_hand='P';LCD_ShowString(135,140,128,16,16,"   Paper ");}				
						else {test2_hand='3';LCD_ShowString(120,140,80,16,16,"     Wait    "); }
						
						if ((test1_hand == 'R' && test2_hand == 'S')||(test1_hand == 'S' && test2_hand == 'P')||(test1_hand == 'P' && test2_hand == 'R'))
							{LCD_ShowString(12,270,80,24,24,"  WIN!  ");LCD_ShowString(135,270,80,24,24,"  LOSE  ");BEEP1_long_EN();}
						else if((test1_hand == 'R' && test2_hand == 'P')||(test1_hand == 'S' && test2_hand == 'R')||(test1_hand == 'P' && test2_hand == 'S'))
							{LCD_ShowString(12,270,80,24,24,"  LOSE  ");LCD_ShowString(135,270,80,24,24,"  WIN!  ");BEEP2_long_EN();}
						else {LCD_ShowString(12,270,80,24,24,"  TIE! ");LCD_ShowString(135,270,80,24,24,"  TIE!  ");}
			
			
			}
//------------------------------------------ֱ���о���ȭģʽ---------------------------------------------------------------			
			else if(mode==2)  
			{
						Get_CapCurrent_Value(res);//��ȡ��ǰ8ͨ������ֵ
					  number_hand=0;num4_Sum1=0;num4_Sum2=0;//���Ƽ�¼�����ݺ�ֵ����
					 
				    if((res[0]*100)>Threshold)	number_hand|=0x80;num4_Sum1+=res[0]*100;
						if((res[1]*100)>Threshold)	number_hand|=0x40;num4_Sum1+=res[1]*100;	
						if((res[2]*100)>Threshold)	number_hand|=0x20;num4_Sum1+=res[2]*100;
						if((res[3]*100)>Threshold)	number_hand|=0x10;num4_Sum1+=res[3]*100;
						if((res[4]*100)>Threshold)	number_hand|=0x08;num4_Sum2+=res[4]*100;
					  if((res[5]*100)>Threshold)	number_hand|=0x04;num4_Sum2+=res[5]*100;
					  if((res[6]*100)>Threshold)	number_hand|=0x02;num4_Sum2+=res[6]*100;
					  if((res[7]*100)>Threshold)	number_hand|=0x01;num4_Sum2+=res[7]*100;

						if(num4_Sum1>NumberThr)LCD_ShowNum(48,205,5,1,24);  
						else if((number_hand&0xf0)==(num1_record_direc&0xf0)||(number_hand&0xf0)==(num1_record_direc2&0xf0)||(number_hand&0xf0)==(num1_record_direc3&0xf0)||(number_hand&0xf0)==(num1_record_direc4&0xf0))LCD_ShowNum(48,205,1,1,24);
						else if((number_hand&0xf0)==(num2_record_direc&0xf0)||(number_hand&0xf0)==(num2_record_direc2&0xf0)||(number_hand&0xf0)==(num2_record_direc3&0xf0))LCD_ShowNum(48,205,2,1,24);
						else if((number_hand&0xf0)==(num3_record_direc&0xf0)||(number_hand&0xf0)==(num3_record_direc2&0xf0))LCD_ShowNum(48,205,3,1,24);
						else if((number_hand&0xf0)==(num4_record_direc&0xf0))LCD_ShowNum(48,205,4,1,24);		
						else LCD_ShowNum(48,205,0,1,24);  
													
						if(num4_Sum2>NumberThr)LCD_ShowNum(168,205,5,1,24);
						else if((number_hand&0x0f)==(num1_record_direc&0x0f)||(number_hand&0x0f)==(num1_record_direc2&0x0f)||(number_hand&0x0f)==(num1_record_direc3&0x0f)||(number_hand&0x0f)==(num1_record_direc4&0x0f))LCD_ShowNum(168,205,1,1,24);
						else if((number_hand&0x0f)==(num2_record_direc&0x0f)||(number_hand&0x0f)==(num2_record_direc2&0x0f)||(number_hand&0x0f)==(num2_record_direc3&0x0f))LCD_ShowNum(168,205,2,1,24);
						else if((number_hand&0x0f)==(num3_record_direc&0x0f)||(number_hand&0x0f)==(num3_record_direc2&0x0f))LCD_ShowNum(168,205,3,1,24);
						else if((number_hand&0x0f)==(num4_record_direc&0x0f))LCD_ShowNum(168,205,4,1,24);		
						else LCD_ShowNum(168,205,0,1,24);  
			
			}
//------------------------------------------ѵ����ȭģʽ��������ͬʱ---------------------------------------------------------------			
			else if(mode==3)  
			{
			
				 if(roshambo_wait==0)
					{					
					 LCD_ShowString(82,57,152,16,16,"Put your Rock,and");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"press button 4!             "); //28 ���37�ַ�  
					 
					}  //�밴��ť4ѵ��ʯͷ
				 else if(roshambo_wait==1)
				 {
				   LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
				 }
				 else if(roshambo_wait==2)
				 {
				   LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
				 }
				 
	
				 else if(roshambo_wait==3)
					{
					 LCD_ShowString(82,57,152,16,16,"Put your Scissor,");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
					 
					}  //�밴��ť4����ѵ������
				 else if(roshambo_wait==4)
				 {
				 	 LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
				 
				 }
				 else if(roshambo_wait==5)
				 {
				 	 LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
				 
				 }
				 	 
				 else if(roshambo_wait==6)
				 {
				 	 LCD_ShowString(82,57,152,16,16,"Put your Paper,  ");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
				 
				 } //�밴��ť4����ѵ����
				 else if(roshambo_wait==7)
					{
				 	 LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
					} 
				 else if(roshambo_wait==8)
				 {
				 	 LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
				 
				 }
				 else if(roshambo_wait==9) 
					{	
						LCD_ShowString(82,57,152,16,16,"Train Successful!");//19 ���19�ַ�
			      LCD_ShowString(8,74,297,16,16,"off Current mode,open mode 4"); //28 ���37�ַ�
					}//ѵ�����
			}
			
//------------------------------------------�о���ȭģʽ---------------------------------------------------------------				
			else if(mode==4)
			{
				    Get_CapCurrent_Value(res);
						hand_record=0;
				    if((res[0]*100)>Threshold)	hand_record|=0x80;
						if((res[1]*100)>Threshold)	hand_record|=0x40;	
						if((res[2]*100)>Threshold)	hand_record|=0x20;
						if((res[3]*100)>Threshold)	hand_record|=0x10;	
						if((res[4]*100)>Threshold)	hand_record|=0x08;
					  if((res[5]*100)>Threshold)	hand_record|=0x04;	
					  if((res[6]*100)>Threshold)	hand_record|=0x02; 
					  if((res[7]*100)>Threshold)	hand_record|=0x01;
										
						if((hand_record&0xf0)==(rock_hand_record&0xf0)){LCD_ShowString(15,140,128,16,16,"   Rock!   ");test1_hand='R';}//һ����ʯͷ��						
						else if((hand_record&0xf0)==(scissors_hand_record&0xf0)){LCD_ShowString(20,140,128,16,16,"Scissors!");test1_hand='S';} //һ���Ǽ�����						
						else if((hand_record&0xf0)==(paper_hand_record&0xf0)){LCD_ShowString(15,140,128,16,16,"   Paper! ");test1_hand='P';}//һ���ǲ���						
						else LCD_ShowString(15,140,71,16,16,"   Wait     ");
														
						if((hand_record&0x0f)==(rock_hand_record&0x0f)){LCD_ShowString(120,140,128,16,16,"   Rock!   ");test2_hand='R';}//������ʯͷ��						
						else if((hand_record&0x0f)==(scissors_hand_record&0x0f)){LCD_ShowString(140,140,128,16,16,"Scissors!");test2_hand='S';} //�����Ǽ�����						
						else if((hand_record&0x0f)==(paper_hand_record&0x0f)){LCD_ShowString(135,140,128,16,16,"   Paper! ");test2_hand='P';}//�����ǲ���						
						else LCD_ShowString(135,140,80,16,16,"   Wait     ");
						
					//������������������������������������������������������������������������������������������������������������
						if(	((hand_record&0xf0)==(rock_record_direc&0xf0))	|| ((hand_record&0xf0)==(rock_record_direc2&0xf0))	){ test1_hand='R';LCD_ShowString(15,140,128,16,16,"    Rock  ");} //u1_printf ("һ��ʯͷ!");
						else if((hand_record&0xf0)==(scissors_record_direc&0xf0)||(hand_record&0xf0)==(scissors_record_direc2&0xf0)||(hand_record&0xf0)==(scissors_record_direc3&0xf0)){ test1_hand='S';LCD_ShowString(20,140,128,16,16,"Scissors");}
						else if((hand_record&0xf0)==(paper_record_direc&0xf0)){test1_hand='P';LCD_ShowString(15,140,128,16,16,"   Paper ");}			
						
						if(	((hand_record&0x0f)==(rock_record_direc&0x0f)) || ((hand_record&0x0f)==(rock_record_direc2&0x0f)) ){	test2_hand='R';LCD_ShowString(120,140,128,16,16,"    Rock  ");	}				
						else if((hand_record&0x0f)==(scissors_record_direc&0x0f)||(hand_record&0x0f)==(scissors_record_direc2&0x0f)||(hand_record&0x0f)==(scissors_record_direc3&0x0f)){ test2_hand='S';LCD_ShowString(140,140,128,16,16,"Scissors");}			
						else if((hand_record&0x0f)==(paper_record_direc&0x0f)){test2_hand='P';LCD_ShowString(135,140,128,16,16,"   Paper ");}				
					//������������������������������������������������������������������������������������������������������������	
					
						if ((test1_hand == 'R' && test2_hand == 'S')||(test1_hand == 'S' && test2_hand == 'P')||(test1_hand == 'P' && test2_hand == 'R'))
							{LCD_ShowString(12,270,80,24,24,"  WIN!  ");LCD_ShowString(135,270,80,24,24,"  LOSE  ");BEEP1_long_EN();}
						else if((test1_hand == 'R' && test2_hand == 'P')||(test1_hand == 'S' && test2_hand == 'R')||(test1_hand == 'P' && test2_hand == 'S'))
							{LCD_ShowString(12,270,80,24,24,"  LOSE  ");LCD_ShowString(135,270,80,24,24,"  WIN!  ");BEEP2_long_EN();}
						else {LCD_ShowString(12,270,80,24,24,"  TIE! ");LCD_ShowString(135,270,80,24,24,"  TIE!  ");}			
			}
//------------------------------------------ѵ����ȭģʽ��������ͬʱ---------------------------------------------------------------
			else if(mode==5)
			{
						if(number_wait==0)
							 { 
								LCD_ShowString(82,57,152,16,16,"Put your One    ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"Finger,and press button 4!  "); //28 ���37�ַ�  
							 }  //�밴��ť4ѵ��1
						 else if(number_wait==1)
						 {
						    LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ� 
						 }
						 else if(number_wait==2)
						 {
						    LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
						 }
						 else if(number_wait==3)
						 {
						   	LCD_ShowString(82,57,152,16,16,"Put your Two    ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"Finger,and press button 4!  "); //28 ���37�ַ� 					
						 }
						 else if(number_wait==4)
						 {
						    LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ� 
						 }
						 else if(number_wait==5)
						 {
						    LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�					 
						 }
						 else if(number_wait==6)
						 {
						    LCD_ShowString(82,57,152,16,16,"Put your Three  ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"Finger,and press button 4!  "); //28 ���37�ַ�						  
						 }
						 else if(number_wait==7)
						 {
						    LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ� 
						 }
						 else if(number_wait==8)
						 { 
						    LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�					 
						 }
						 else if(number_wait==9)
						 {
						    LCD_ShowString(82,57,152,16,16,"Put your Four   ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"Finger,and press button 4!  "); //28 ���37�ַ�
						 }
						 else if(number_wait==10)
						 {
						    LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ� 
						 }
						 else if(number_wait==11)
						 {
						    LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
						 }
						 else if(number_wait==12)
							{ 
							  LCD_ShowString(82,57,152,16,16,"Put your Five   ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"Finger,and press button 4!  "); //28 ���37�ַ�				 
								 
							}  
						 else if(number_wait==13)
							 {
							   LCD_ShowString(82,57,152,16,16,"     Again!        ");//19 ���19�ַ�
			           LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ� 
							 
							 }  //�밴��ť4����
						 else if(number_wait==14) 
							 { 
							   LCD_ShowString(82,57,152,16,16,"   Again!Again!   ");//19 ���19�ַ�
			           LCD_ShowString(8,74,297,16,16,"and press button 4!         "); //28 ���37�ַ�
							 }  //�밴��ť4����
						 else if(number_wait==15) 
						 {
						    LCD_ShowString(82,57,152,16,16,"Train Successful ");//19 ���19�ַ�
			          LCD_ShowString(8,74,297,16,16,"off Current mode,open mode 4"); //28 ���37�ַ�						 
						 }//ѵ�����
			}
//------------------------------------------�о���ȭģʽ��������ͬʱ---------------------------------------------------------------			
			else if(mode==6)	
			{
			      Get_CapCurrent_Value(res);
					  number_hand=0;num4_Sum1=0;num4_Sum2=0;//���Ƽ�¼�����ݺ�ֵ����
					 
				    if((res[0]*100)>Threshold)	number_hand|=0x80;num4_Sum1+=res[0]*100;
						if((res[1]*100)>Threshold)	number_hand|=0x40;num4_Sum1+=res[1]*100;	
						if((res[2]*100)>Threshold)	number_hand|=0x20;num4_Sum1+=res[2]*100;
						if((res[3]*100)>Threshold)	number_hand|=0x10;num4_Sum1+=res[3]*100;
						if((res[4]*100)>Threshold)	number_hand|=0x08;num4_Sum2+=res[4]*100;
					  if((res[5]*100)>Threshold)	number_hand|=0x04;num4_Sum2+=res[5]*100;
					  if((res[6]*100)>Threshold)	number_hand|=0x02;num4_Sum2+=res[6]*100;
					  if((res[7]*100)>Threshold)	number_hand|=0x01;num4_Sum2+=res[7]*100;
													
						if(num4_Sum1>NumberThr)LCD_ShowNum(48,205,5,1,24);  
						else if((number_hand&0xf0)==(number1_record&0xf0))LCD_ShowNum(48,205,1,1,24);  
						else if((number_hand&0xf0)==(number2_record&0xf0))LCD_ShowNum(48,205,2,1,24);  
						else if((number_hand&0xf0)==(number3_record&0xf0))LCD_ShowNum(48,205,3,1,24);  
						else if((number_hand&0xf0)==(number4_record&0xf0))LCD_ShowNum(48,205,4,1,24);  			
						else LCD_ShowNum(48,205,0,1,24);  
													
						if(num4_Sum2>NumberThr)LCD_ShowNum(168,205,5,1,24);
						else if((number_hand&0x0f)==(number1_record&0x0f))LCD_ShowNum(168,205,1,1,24);
						else if((number_hand&0x0f)==(number2_record&0x0f))LCD_ShowNum(168,205,2,1,24);
						else if((number_hand&0x0f)==(number3_record&0x0f))LCD_ShowNum(168,205,3,1,24);
						else if((number_hand&0x0f)==(number4_record&0x0f))LCD_ShowNum(168,205,4,1,24);				
						else LCD_ShowNum(168,205,0,1,24);
		//����������������������������������������������������������������������������������������������������������������������������������������������
						if(num4_Sum1>NumberThr)LCD_ShowNum(48,205,5,1,24);  
						else if((number_hand&0xf0)==(num1_record_direc&0xf0)||(number_hand&0xf0)==(num1_record_direc2&0xf0)||(number_hand&0xf0)==(num1_record_direc3&0xf0)||(number_hand&0xf0)==(num1_record_direc4&0xf0))LCD_ShowNum(48,205,1,1,24);
						else if((number_hand&0xf0)==(num2_record_direc&0xf0)||(number_hand&0xf0)==(num2_record_direc2&0xf0)||(number_hand&0xf0)==(num2_record_direc3&0xf0))LCD_ShowNum(48,205,2,1,24);
						else if((number_hand&0xf0)==(num3_record_direc&0xf0)||(number_hand&0xf0)==(num3_record_direc2&0xf0))LCD_ShowNum(48,205,3,1,24);
						else if((number_hand&0xf0)==(num4_record_direc&0xf0))LCD_ShowNum(48,205,4,1,24);		 
								
						if(num4_Sum2>NumberThr)LCD_ShowNum(168,205,5,1,24);
						else if((number_hand&0x0f)==(num1_record_direc&0x0f)||(number_hand&0x0f)==(num1_record_direc2&0x0f)||(number_hand&0x0f)==(num1_record_direc3&0x0f)||(number_hand&0x0f)==(num1_record_direc4&0x0f))LCD_ShowNum(168,205,1,1,24);
						else if((number_hand&0x0f)==(num2_record_direc&0x0f)||(number_hand&0x0f)==(num2_record_direc2&0x0f)||(number_hand&0x0f)==(num2_record_direc3&0x0f))LCD_ShowNum(168,205,2,1,24);
						else if((number_hand&0x0f)==(num3_record_direc&0x0f)||(number_hand&0x0f)==(num3_record_direc2&0x0f))LCD_ShowNum(168,205,3,1,24);
						else if((number_hand&0x0f)==(num4_record_direc&0x0f))LCD_ShowNum(168,205,4,1,24);		 
		//������������	����������������������������������������������������������������������������������������������������������������������������������		
			
			}
			else if(mode==7)//����ģʽ
			{
					  Get_CapCurrent_Value(res);

				    if((res[0]*100)>MusicThr-50)	music_do(175);   //�����ǰͨ�������ټ���ֵ����ô��Ϊ���ټ�������
						if((res[1]*100)>MusicThr-50)	music_re(175);
						if((res[2]*100)>MusicThr-50)	music_me(175);
						if((res[3]*100)>MusicThr-50)	music_fa(175);
						if((res[4]*100)>MusicThr)	    music_so(175);
					  if((res[5]*100)>MusicThr)	    music_la(175);
					  if((res[6]*100)>MusicThr)	    music_xi(175);
					  if((res[7]*100)>MusicThr)	    music_doo(175);
			
			}
		}
		else ;
				
//					DataScope_Get_Channel_Data( res[0]*100 , 1 ); //������ 1.0  д��ͨ�� 1                                                                                                                      
//					DataScope_Get_Channel_Data( res[1]*100 , 2 ); //������ 2.0  д��ͨ�� 2
//					DataScope_Get_Channel_Data( res[2]*100 , 3 ); //������ 3.0  д��ͨ�� 3
//					DataScope_Get_Channel_Data( res[3]*100 , 4 ); //������ 4.0  д��ͨ�� 4
//					DataScope_Get_Channel_Data( res[4]*100 , 5 ); //������ 5.0  д��ͨ�� 5
//					DataScope_Get_Channel_Data( res[5]*100 , 6 ); //������ 6.0  д��ͨ�� 6
//					DataScope_Get_Channel_Data( res[6]*100 , 7 ); //������ 7.0  д��ͨ�� 7
//					DataScope_Get_Channel_Data( res[7]*100 , 8 ); //������ 8.0  д��ͨ�� 8
//					
//					Send_Count = DataScope_Data_Generate(8); //����8��ͨ���� ��ʽ��֡���ݣ�����֡���ݳ���
//					
//					for( i = 0 ; i < Send_Count; i++)  //ѭ������,ֱ���������
//					{
//						while((USART1->SR&0X40)==0);  
//						USART1->DR = DataScope_OutPut_Buffer[i]; //�Ӵ��ڶ�һ���ֽ����ݳ�ȥ      
//					}
//					delay_ms(10); //20fps, ֡���ʱ�䡣 ��ͬ�������ü� USB-TTL �豸�����Ӿ���Ӱ���ʱ��ĳ��̣�����ʵ��Ϊ׼�� 
			
  	}
}



#include "exti.h"
#include "delay.h" 
#include "stm32f4xx_exti.h"
#include "silencht_usart.h"
#include "fdc2214.h"
#include "fdc22142.h"
#include "lcd.h"
#include "math.h"
#include "DataScope_DP.h"

/*
						1��ѵ����1�� ����ƽ���˲��������뺯������   ѵ����2�� �˲��������뺯������
						2�����ͨ����������ֵ��������
						3�����[ѵ��/�о�ʱ]ͨ������ֵ��������	
						4���ⲿ�������ݳ�ֵ����
						5��ѵ����ѵ����ȭ��������,��0-3��Ϊ��ѵ����1�����ĸ�ͨ��,��4-7��Ϊ��ѵ����2��
						6��ѵ����ѵ����ȭ��������,��0-3��Ϊ��ѵ����1�����ĸ�ͨ��,��4-7��Ϊ��ѵ����2��
*/

//float filter0_buf[FILTER_N+1],filter1_buf[FILTER_N+1],filter2_buf[FILTER_N+1],filter3_buf[FILTER_N+1];
//float filter4_buf[FILTER_N+1],filter5_buf[FILTER_N+1],filter6_buf[FILTER_N+1],filter7_buf[FILTER_N+1];
float init_Cap_buffer[InitCap_N+1]; 
float crrent_Cap_buffer[CrrentCap_N+1]; 


		  
u8 mode=0;      
u8 start_end=0;

u8 roshambo_wait=0; //��ȭѵ���ȴ���־����0����ȫ��δ��ɡ�1������������δ��ɡ�2������δ��ɡ�3����ȫ�����
u8 number_wait=0;   //��ȭѵ���ȴ���־����0����ȫ��δ��ɡ�1����234δ��ɡ�2����34δ��ɡ�3����4δ��ɡ�4��:ȫ�����


//ѵ��ʱ��ȭ����ͨ����¼�Ĵ棬��4λΪѵ����1��0-3ͨ��������λΪѵ����2��0-3ͨ�� ��ӦλֵΪ0Ϊ����ָ��ֵΪ1Ϊ����ָ�����μ�⡿
u8 rock_hand_record=0,paper_hand_record=0,scissors_hand_record=0;
u8 rock_hand_record2=0,paper_hand_record2=0,scissors_hand_record2=0;
u8 rock_hand_record3=0,paper_hand_record3=0,scissors_hand_record3=0;


//ѵ��ʱ��ȭ��ʰͨ����¼�Ĵ棬number1-5��¼ѵ���߻�ȭ����1-5,����number����λΪѵ����1��ͨ����¼������λΪѵ����2��ͨ����¼
//��¼number4���ܵ���ֵ��һ������֮����number5��[��]��λȫ��1����ʱΪ�����ָ
u8 number1_record=0,number2_record=0,number3_record=0,number4_record=0,number5_record=0;
u8 number1_record2=0,number2_record2=0,number3_record2=0,number4_record2=0,number5_record2=0;
u8 number1_record3=0,number2_record3=0,number3_record3=0,number4_record3=0,number5_record3=0;
float num4_SumCap1=0,num4_SumCap2=0; //��¼ѵ����1��2������4����number4�ĵ��ݺ���Ϊ��ֵ����main���ж��Ƿ��������ơ���5����ָ���ϡ�

//��������������
u8 usart_num=0;




float temp[8],test_temp[8]; 				//���ݳ�ֵ����,��ʼ�����Ե��ݳ�ֵ����

float rock[8],paper[8],scissors[8]; //ѵ���߲�ȭ����,��0-3��Ϊ��ѵ����1��,��4-7��Ϊ��ѵ����2��

float number[8];				            //ѵ���߻�ȭ����,��0-3��Ϊ��ѵ����1��,��4-7��Ϊ��ѵ����2��


/*�ⲿ�ж�0������� PF0  	

------------------------------------1��ʼ�жϰ�ť------------------------------------------------														
															
*/
void EXTI0_IRQHandler(void)
{
  delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0)==1) //������
	{
		Get_CapInit_Value(); //ɨ��õ���ʼ����ֵ
		roshambo_wait=0;
		number_wait=0; 
//	u1_printf("%f,%f,%f,%f,%f,%f,%f,%f \r\n",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7]); 
	}
	 EXTI_ClearITPendingBit(EXTI_Line0); //���LINE10�ϵ��жϱ�־λ 
}	


/*�ⲿ�ж�1������� PF1	

-------------------------------------2ģʽ�л�,����ѭ����ť-----------------------------------------------													
		          ��ģʽ0����ʼ��������
							��ģʽ1��ֱ���о���ȭģʽ�������ɲ�����Աָ��
							��ģʽ2��ֱ���о���ȭģʽ�������ɲ�����Աָ��
							��ģʽ3��ѵ����ȭģʽ��ʯͷ���������������������Ա�Լ�ָ����ѵ��
							��ģʽ4���о���ȭģʽ
							��ģʽ5��ѵ����ȭģʽ��ʯͷ���������������������Ա�Լ�ָ����ѵ��
							��ģʽ6���о���ȭģʽ															
*/
void EXTI1_IRQHandler(void)
{
  delay_ms(20);
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)==1) 
		{
			 if(start_end==0){ mode++;if(mode>8)mode=1; } //ֻ����ģʽ����start_end�رյ�ʱ�򣬲����л�ģʽ
			 else 
			 {
					 LCD_ShowString(82,57,152,16,16,"Current mode is not");//19 ���19�ַ�
			     LCD_ShowString(8,74,297,16,16,"closed,switch cannot be done"); //28 ���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
				}
			 if(mode==1)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);     //u1_printf ("��ǰģʽ��%d ,ֱ���о���ȭģʽ\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"  Direct Judgment  ");
					 LCD_ShowString(8,74,297,16,16," Guessing Mode!             "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
			 else if(mode==2)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);//u1_printf ("��ǰģʽ��%d ,ֱ���о���ȭģʽ\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"  Direct Judgment  ");
					 LCD_ShowString(8,74,297,16,16," Punch Mode!                "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
			 else if(mode==3)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);//u1_printf ("��ǰģʽ��%d ,ѵ����ȭģʽ\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"  Train Guessing  ");
					 LCD_ShowString(8,74,297,16,16,"  Mode!                     "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
			 else if(mode==4)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);//u1_printf ("��ǰģʽ��%d ,�о���ȭģʽ\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"  Judgment Guessing  ");
					 LCD_ShowString(8,74,297,16,16,"  Mode!                     "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
			 else if(mode==5)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);//u1_printf ("��ǰģʽ��%d ,ѵ����ȭģʽ\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"  Train Punch         ");
					 LCD_ShowString(8,74,297,16,16,"  Mode!                     "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
			 else if(mode==6)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);//u1_printf ("��ǰģʽ��%d ,�о���ȭģʽ	\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"  Judgment Punch      ");
					 LCD_ShowString(8,74,297,16,16,"  Mode!                     "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
			 else if(mode==7)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);//u1_printf ("��ǰģʽ��%d ,�о���ȭģʽ	\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"  Music Piano         ");
					 LCD_ShowString(8,74,297,16,16,"  Mode!                     "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
			 else if(mode==8)
			 {
					 LCD_ShowNum(25+120,31,mode,1,24);//u1_printf ("��ǰģʽ��%d ,�о���ȭģʽ	\r\n",mode);
					 LCD_ShowString(82,57,152,16,16,"       Gesture         ");
					 LCD_ShowString(8,74,297,16,16,"  Mode!                     "); //���37�ַ�  u1_printf ("��ǰģʽδ�رգ����ܽ���ģʽ�л���");
			 }
		}
	 EXTI_ClearITPendingBit(EXTI_Line1);//���LINE1�ϵ��жϱ�־λ 
}



/*�ⲿ�ж�1������� PF2	

---------------------------------------3ģʽ���ذ�ť-------------------------------------------------------
															
*/
void EXTI2_IRQHandler(void)
{
	delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_2)==1)
		{
			start_end=!start_end;  //����ת��
			if(start_end>0)LCD_ShowString(185,33,32,16,16,"open");
			else {LCD_ShowString(185,33,32,16,16,"off!");MinGet_CapInit_Value();}//u1_printf ("ģʽ�أ�\r\n");
		}
	 EXTI_ClearITPendingBit(EXTI_Line2);//���LINE1�ϵ��жϱ�־λ 
}


/*�ⲿ�ж�1������� PF3	

---------------------------------------4ѵ��ģʽ��ѵ����1��2ͬʱѵ��-----------------------------------------

*/
void EXTI3_IRQHandler(void)
{
	delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_3)==1)
		{

/*-------------------------------------------------------ѵ����ȭģʽ----------------------------------------------------------*/
			if(mode==3) //�����ǰΪѵ����ȭģʽ
			{
				if(roshambo_wait==0)	    //��ǰȫ��δ��ɣ����ڼ��ȭͷ
					{
					
							Get_CapCurrent_Value(rock);
							
							if((rock[0]*100)>Threshold)	rock_hand_record|=0x80;
							if((rock[1]*100)>Threshold)	rock_hand_record|=0x40;
							if((rock[2]*100)>Threshold)	rock_hand_record|=0x20;
							if((rock[3]*100)>Threshold)	rock_hand_record|=0x10;
							if((rock[4]*100)>Threshold)	rock_hand_record|=0x08;
							if((rock[5]*100)>Threshold)	rock_hand_record|=0x04;
							if((rock[6]*100)>Threshold)	rock_hand_record|=0x02;
							if((rock[7]*100)>Threshold)	rock_hand_record|=0x01;
				
							
					}
				else if(roshambo_wait==1)		//��ǰȫ��δ��ɣ����ڵڶ��μ��ȭͷ
				 {
					    Get_CapCurrent_Value(rock);
							
							if((rock[0]*100)>Threshold)	rock_hand_record2|=0x80;
							if((rock[1]*100)>Threshold)	rock_hand_record2|=0x40;
							if((rock[2]*100)>Threshold)	rock_hand_record2|=0x20;
							if((rock[3]*100)>Threshold)	rock_hand_record2|=0x10;
							if((rock[4]*100)>Threshold)	rock_hand_record2|=0x08;
							if((rock[5]*100)>Threshold)	rock_hand_record2|=0x04;
							if((rock[6]*100)>Threshold)	rock_hand_record2|=0x02;
							if((rock[7]*100)>Threshold)	rock_hand_record2|=0x01;
							
							if(rock_hand_record==rock_hand_record2)roshambo_wait=2;//�����һ�μ��͵ڶ��μ����ͬ����ôֱ�����������β��ԣ�������һ�׶�
				
				 }
				else if(roshambo_wait==2)  //ǰ���μ�ⲻһ�£����е����β���
				 {
				 			Get_CapCurrent_Value(rock);
							
							if((rock[0]*100)>Threshold)	rock_hand_record3|=0x80;
							if((rock[1]*100)>Threshold)	rock_hand_record3|=0x40;
							if((rock[2]*100)>Threshold)	rock_hand_record3|=0x20;
							if((rock[3]*100)>Threshold)	rock_hand_record3|=0x10;
							if((rock[4]*100)>Threshold)	rock_hand_record3|=0x08;
							if((rock[5]*100)>Threshold)	rock_hand_record3|=0x04;
							if((rock[6]*100)>Threshold)	rock_hand_record3|=0x02;
							if((rock[7]*100)>Threshold)	rock_hand_record3|=0x01;
							
							if(rock_hand_record2==rock_hand_record3)rock_hand_record=rock_hand_record2;//����ڶ��κ͵�����һ������ô�����Ƽ�¼Ϊ�ڶ��Σ������һ�κ͵�����һ������ô���Ƽ�¼����						
				 
				 }
				else if(roshambo_wait==3)  //���м�����һ��ѵ��
				 {
				      Get_CapCurrent_Value(scissors);
							
							if((scissors[0]*100)>Threshold)	scissors_hand_record|=0x80;
							if((scissors[1]*100)>Threshold)	scissors_hand_record|=0x40;	
							if((scissors[2]*100)>Threshold)	scissors_hand_record|=0x20;
							if((scissors[3]*100)>Threshold)	scissors_hand_record|=0x10;
							if((scissors[4]*100)>Threshold)	scissors_hand_record|=0x08;
							if((scissors[5]*100)>Threshold)	scissors_hand_record|=0x04;
							if((scissors[6]*100)>Threshold)	scissors_hand_record|=0x02;
							if((scissors[7]*100)>Threshold)	scissors_hand_record|=0x01;
				 
				 }
				else if(roshambo_wait==4) //�����ڶ���ѵ��
				{
						  Get_CapCurrent_Value(scissors);
							
							if((scissors[0]*100)>Threshold)	scissors_hand_record2|=0x80;
							if((scissors[1]*100)>Threshold)	scissors_hand_record2|=0x40;	
							if((scissors[2]*100)>Threshold)	scissors_hand_record2|=0x20;
							if((scissors[3]*100)>Threshold)	scissors_hand_record2|=0x10;
							if((scissors[4]*100)>Threshold)	scissors_hand_record2|=0x08;
							if((scissors[5]*100)>Threshold)	scissors_hand_record2|=0x04;
							if((scissors[6]*100)>Threshold)	scissors_hand_record2|=0x02;
							if((scissors[7]*100)>Threshold)	scissors_hand_record2|=0x01;
							
							if(scissors_hand_record==scissors_hand_record2)roshambo_wait=5;//�����һ�μ��͵ڶ��μ����ͬ����ôֱ�����������β��ԣ�������һ�׶�
				
				}
				else if(roshambo_wait==5) //
					{
							Get_CapCurrent_Value(scissors);
							
							if((scissors[0]*100)>Threshold)	scissors_hand_record3|=0x80;
							if((scissors[1]*100)>Threshold)	scissors_hand_record3|=0x40;	
							if((scissors[2]*100)>Threshold)	scissors_hand_record3|=0x20;
							if((scissors[3]*100)>Threshold)	scissors_hand_record3|=0x10;
							if((scissors[4]*100)>Threshold)	scissors_hand_record3|=0x08;
							if((scissors[5]*100)>Threshold)	scissors_hand_record3|=0x04;
							if((scissors[6]*100)>Threshold)	scissors_hand_record3|=0x02;
							if((scissors[7]*100)>Threshold)	scissors_hand_record3|=0x01;
							
							if(scissors_hand_record2==scissors_hand_record3)rock_hand_record=rock_hand_record2;//ͬ��
					}
				else if(roshambo_wait==6)//����һ�μ��
				 {
				      Get_CapCurrent_Value(paper);
							
							if((paper[0]*100)>Threshold)	paper_hand_record|=0x80;
							if((paper[1]*100)>Threshold)	paper_hand_record|=0x40;
							if((paper[2]*100)>Threshold)	paper_hand_record|=0x20;
							if((paper[3]*100)>Threshold)	paper_hand_record|=0x10;
							if((paper[4]*100)>Threshold)	paper_hand_record|=0x08;
							if((paper[5]*100)>Threshold)	paper_hand_record|=0x04;
						  if((paper[6]*100)>Threshold)	paper_hand_record|=0x02;
						  if((paper[7]*100)>Threshold)	paper_hand_record|=0x01;
				 
				 
				 }
				else if(roshambo_wait==7)//���ڶ��μ��
					{
							Get_CapCurrent_Value(paper);
							
							if((paper[0]*100)>Threshold)	paper_hand_record2|=0x80;
							if((paper[1]*100)>Threshold)	paper_hand_record2|=0x40;
							if((paper[2]*100)>Threshold)	paper_hand_record2|=0x20;
							if((paper[3]*100)>Threshold)	paper_hand_record2|=0x10;
							if((paper[4]*100)>Threshold)	paper_hand_record2|=0x08;
							if((paper[5]*100)>Threshold)	paper_hand_record2|=0x04;
						  if((paper[6]*100)>Threshold)	paper_hand_record2|=0x02;
						  if((paper[7]*100)>Threshold)	paper_hand_record2|=0x01;	
							
							if(paper_hand_record==paper_hand_record2)roshambo_wait=8;
							
					}
				else if(roshambo_wait==8)//��ǰ���μ�ⲻһ�£����е����μ��
				 {
							Get_CapCurrent_Value(paper);
							
							if((paper[0]*100)>Threshold)	paper_hand_record3|=0x80;
							if((paper[1]*100)>Threshold)	paper_hand_record3|=0x40;
							if((paper[2]*100)>Threshold)	paper_hand_record3|=0x20;
							if((paper[3]*100)>Threshold)	paper_hand_record3|=0x10;
							if((paper[4]*100)>Threshold)	paper_hand_record3|=0x08;
							if((paper[5]*100)>Threshold)	paper_hand_record3|=0x04;
						  if((paper[6]*100)>Threshold)	paper_hand_record3|=0x02;
						  if((paper[7]*100)>Threshold)	paper_hand_record3|=0x01;	
							
							if(paper_hand_record2==paper_hand_record3)paper_hand_record=paper_hand_record2;//ͬ��			
				 }
				 else if(roshambo_wait==9)//��ǰ���μ�ⲻһ�£����е����μ��
				 {
						LCD_ShowString(82,57,152,16,16,"Train Successful!");//19 ���19�ַ�
			      LCD_ShowString(8,74,297,16,16,"off Current mode,open mode 4"); //28 ���37�ַ�				 
				 }
			   
				if(roshambo_wait<9) roshambo_wait++;
			}
			
/*-------------------------------------------------------ѵ����ȭģʽ----------------------------------------------------------*/
			if(mode==5)//�����ǰΪѵ����ȭģʽ��1234
			{
				if(number_wait==0)//��һ�μ�� 1	
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number1_record|=0x80;
							if((number[1]*100)>Threshold)	number1_record|=0x40;
							if((number[2]*100)>Threshold)	number1_record|=0x20;
							if((number[3]*100)>Threshold)	number1_record|=0x10;
							if((number[4]*100)>Threshold)	number1_record|=0x08;
							if((number[5]*100)>Threshold)	number1_record|=0x04;
						  if((number[6]*100)>Threshold)	number1_record|=0x02;
						  if((number[7]*100)>Threshold)	number1_record|=0x01;								
					}
				else if(number_wait==1)//�ڶ��μ��1
				{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number1_record2|=0x80;
							if((number[1]*100)>Threshold)	number1_record2|=0x40;
							if((number[2]*100)>Threshold)	number1_record2|=0x20;
							if((number[3]*100)>Threshold)	number1_record2|=0x10;
							if((number[4]*100)>Threshold)	number1_record2|=0x08;
							if((number[5]*100)>Threshold)	number1_record2|=0x04;
						  if((number[6]*100)>Threshold)	number1_record2|=0x02;
						  if((number[7]*100)>Threshold)	number1_record2|=0x01;
							
							if(number1_record==number1_record2)number_wait=2;		
				}
				else if(number_wait==2)//�����μ��1
				{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number1_record3|=0x80;
							if((number[1]*100)>Threshold)	number1_record3|=0x40;
							if((number[2]*100)>Threshold)	number1_record3|=0x20;
							if((number[3]*100)>Threshold)	number1_record3|=0x10;
							if((number[4]*100)>Threshold)	number1_record3|=0x08;
							if((number[5]*100)>Threshold)	number1_record3|=0x04;
						  if((number[6]*100)>Threshold)	number1_record3|=0x02;
						  if((number[7]*100)>Threshold)	number1_record3|=0x01;
							
							if(number1_record2==number1_record3)number1_record=number1_record2;
				}
				else if(number_wait==3)//��һ�μ��2
				{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number2_record|=0x80;
							if((number[1]*100)>Threshold)	number2_record|=0x40;
							if((number[2]*100)>Threshold)	number2_record|=0x20;
							if((number[3]*100)>Threshold)	number2_record|=0x10;
							if((number[4]*100)>Threshold)	number2_record|=0x08;
							if((number[5]*100)>Threshold)	number2_record|=0x04;
						  if((number[6]*100)>Threshold)	number2_record|=0x02;
						  if((number[7]*100)>Threshold)	number2_record|=0x01;	
				}
					
				else if(number_wait==4)//�ڶ��μ��2
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number2_record2|=0x80;
							if((number[1]*100)>Threshold)	number2_record2|=0x40;
							if((number[2]*100)>Threshold)	number2_record2|=0x20;
							if((number[3]*100)>Threshold)	number2_record2|=0x10;
							if((number[4]*100)>Threshold)	number2_record2|=0x08;
							if((number[5]*100)>Threshold)	number2_record2|=0x04;
						  if((number[6]*100)>Threshold)	number2_record2|=0x02;
						  if((number[7]*100)>Threshold)	number2_record2|=0x01;
							
							if(number2_record==number2_record2)number_wait=5;											        			
					}
				else if(number_wait==5)//�����μ��2
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number2_record3|=0x80;
							if((number[1]*100)>Threshold)	number2_record3|=0x40;
							if((number[2]*100)>Threshold)	number2_record3|=0x20;
							if((number[3]*100)>Threshold)	number2_record3|=0x10;
							if((number[4]*100)>Threshold)	number2_record3|=0x08;
							if((number[5]*100)>Threshold)	number2_record3|=0x04;
						  if((number[6]*100)>Threshold)	number2_record3|=0x02;
						  if((number[7]*100)>Threshold)	number2_record3|=0x01;
							
							if(number2_record2==number2_record3)number2_record=number2_record2;
							 
					}	
				else if(number_wait==6)//��һ�μ��3
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number3_record|=0x80;
							if((number[1]*100)>Threshold)	number3_record|=0x40;
							if((number[2]*100)>Threshold)	number3_record|=0x20;
							if((number[3]*100)>Threshold)	number3_record|=0x10;
							if((number[4]*100)>Threshold)	number3_record|=0x08;
							if((number[5]*100)>Threshold)	number3_record|=0x04;
						  if((number[6]*100)>Threshold)	number3_record|=0x02;
						  if((number[7]*100)>Threshold)	number3_record|=0x01;					 
					}
				else if(number_wait==7)//�ڶ��μ��3
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number3_record2|=0x80;
							if((number[1]*100)>Threshold)	number3_record2|=0x40;
							if((number[2]*100)>Threshold)	number3_record2|=0x20;
							if((number[3]*100)>Threshold)	number3_record2|=0x10;
							if((number[4]*100)>Threshold)	number3_record2|=0x08;
							if((number[5]*100)>Threshold)	number3_record2|=0x04;
						  if((number[6]*100)>Threshold)	number3_record2|=0x02;
						  if((number[7]*100)>Threshold)	number3_record2|=0x01;
							if(number3_record==number3_record2)number_wait=8;					 
					}
				else if(number_wait==8)//�����μ��3
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number3_record3|=0x80;
							if((number[1]*100)>Threshold)	number3_record3|=0x40;
							if((number[2]*100)>Threshold)	number3_record3|=0x20;
							if((number[3]*100)>Threshold)	number3_record3|=0x10;
							if((number[4]*100)>Threshold)	number3_record3|=0x08;
							if((number[5]*100)>Threshold)	number3_record3|=0x04;
						  if((number[6]*100)>Threshold)	number3_record3|=0x02;
						  if((number[7]*100)>Threshold)	number3_record3|=0x01;
							
							if(number3_record2==number3_record3)number3_record=number3_record2;	 
					}
				else if(number_wait==9)//��һ�μ��4
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number4_record|=0x80;
							if((number[1]*100)>Threshold)	number4_record|=0x40;
							if((number[2]*100)>Threshold)	number4_record|=0x20;
							if((number[3]*100)>Threshold)	number4_record|=0x10;
							if((number[4]*100)>Threshold)	number4_record|=0x08;
							if((number[5]*100)>Threshold)	number4_record|=0x04;
						  if((number[6]*100)>Threshold)	number4_record|=0x02;
						  if((number[7]*100)>Threshold)	number4_record|=0x01;
							 
					}
				else if(number_wait==10)//�ڶ��μ��4
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number4_record2|=0x80;
							if((number[1]*100)>Threshold)	number4_record2|=0x40;
							if((number[2]*100)>Threshold)	number4_record2|=0x20;
							if((number[3]*100)>Threshold)	number4_record2|=0x10;
							if((number[4]*100)>Threshold)	number4_record2|=0x08;
							if((number[5]*100)>Threshold)	number4_record2|=0x04;
						  if((number[6]*100)>Threshold)	number4_record2|=0x02;
						  if((number[7]*100)>Threshold)	number4_record2|=0x01;
							
							if(number4_record==number4_record2)number_wait=11;			 
					}
				else if(number_wait==11)//�����μ��4
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number4_record3|=0x80;
							if((number[1]*100)>Threshold)	number4_record3|=0x40;
							if((number[2]*100)>Threshold)	number4_record3|=0x20;
							if((number[3]*100)>Threshold)	number4_record3|=0x10;
							if((number[4]*100)>Threshold)	number4_record3|=0x08;
							if((number[5]*100)>Threshold)	number4_record3|=0x04;
						  if((number[6]*100)>Threshold)	number4_record3|=0x02;
						  if((number[7]*100)>Threshold)	number4_record3|=0x01;
							
							if(number4_record2==number4_record3)number4_record=number4_record2;
							 
					}
				else if(number_wait==12)//��һ�μ��5
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number5_record|=0x80;
							if((number[1]*100)>Threshold)	number5_record|=0x40;
							if((number[2]*100)>Threshold)	number5_record|=0x20;
							if((number[3]*100)>Threshold)	number5_record|=0x10;
							if((number[4]*100)>Threshold)	number5_record|=0x08;
							if((number[5]*100)>Threshold)	number5_record|=0x04;
						  if((number[6]*100)>Threshold)	number5_record|=0x02;
						  if((number[7]*100)>Threshold)	number5_record|=0x01;
					}
				else if(number_wait==13)//�ڶ��μ��5
					{
							Get_CapCurrent_Value(number); 
							if((number[0]*100)>Threshold)	number5_record2|=0x80;
							if((number[1]*100)>Threshold)	number5_record2|=0x40;
							if((number[2]*100)>Threshold)	number5_record2|=0x20;
							if((number[3]*100)>Threshold)	number5_record2|=0x10;
							if((number[4]*100)>Threshold)	number5_record2|=0x08;
							if((number[5]*100)>Threshold)	number5_record2|=0x04;
						  if((number[6]*100)>Threshold)	number5_record2|=0x02;
						  if((number[7]*100)>Threshold)	number5_record2|=0x01;
							
							if(number5_record==number5_record2)number_wait=14;
							 
					}
				else if(number_wait==14)//�����μ��5
					{
							Get_CapCurrent_Value(number); //�õ���ǰ��ȭ������ֵ
							if((number[0]*100)>Threshold)	number5_record3|=0x80;
							if((number[1]*100)>Threshold)	number5_record3|=0x40;
							if((number[2]*100)>Threshold)	number5_record3|=0x20;
							if((number[3]*100)>Threshold)	number5_record3|=0x10;
							if((number[4]*100)>Threshold)	number5_record3|=0x08;
							if((number[5]*100)>Threshold)	number5_record3|=0x04;
						  if((number[6]*100)>Threshold)	number5_record3|=0x02;
						  if((number[7]*100)>Threshold)	number5_record3|=0x01;
							
							if(number5_record2==number5_record3)number5_record=number5_record2;
							 
					}
				else if(number_wait==15)
				 {
						LCD_ShowString(82,57,152,16,16,"Train Successful!");//19 ���19�ַ�
			      LCD_ShowString(8,74,297,16,16,"off Current mode,open mode 4"); //28 ���37�ַ�				 
				 }
					if(number_wait<15) number_wait++;
			}
		}
		
	 EXTI_ClearITPendingBit(EXTI_Line3);//���LINE1�ϵ��жϱ�־λ 
}
	   
//�ⲿ�жϳ�ʼ������
//�ж�����.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_Init(); //������Ӧ��IO�ڳ�ʼ��
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource0);//PF0 ���ӵ��ж���0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource1);//PF1 ���ӵ��ж���1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource2);//PF2 ���ӵ��ж���2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource3);//PF2 ���ӵ��ж���2
	
  /* ����EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //            
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE
  EXTI_Init(&EXTI_InitStructure);//����
	
	/* ����EXTI_Line1 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
	/* ����EXTI_Line2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
	/* ����EXTI_Line3 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//�ⲿ�ж�1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//�ⲿ�ж�3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	   
}

//������ʼ������ GPIOF0/F1/F2
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIOF0123
	
} 



//��ð�ͨ�����ݳ�ʼֵ������main�����ʼ����ֵ
void Get_CapInit_Value(void)
{
 	int i,j,k;//iΪ8ͨ��ѭ����������jΪ��������ָ���,kΪ��ͼ�����
	float sum,total;//sum���Լ�¼ÿ��ͨ����ֵ�����Բɼ�����[InitCap_N]���õ�ͨ����ʼ����ֵ��total��¼����ͨ����ʼ����ֵ��
start:
	for(i=0;i<8;i++) //����ͨ��
	{	
		sum=0;//��ֵ����
		
		if(i<4) //��1   0-3ͨ��
			{	
				for(j=0;j<InitCap_N;j++) {	init_Cap_buffer[j] = Cap_Calculate(i);sum+=init_Cap_buffer[j];delay_ms(10);	}//ÿ��ͨ���ɼ�InitCap_N������,���ۼӺ�
			}
		else 	  //��2   4-7ͨ��
			{	
				for(j=0;j<InitCap_N;j++) {	init_Cap_buffer[j] = Cap_2Calculate(i%4);sum+=init_Cap_buffer[j];delay_ms(10);	}//ÿ��ͨ���ɼ�InitCap_N�����ݣ����ۼӺ�
			}
					 
		temp[i] = sum/InitCap_N;//��8ͨ����ʼ��ƽ��ֵ
	}
		total=0;
  	delay_ms(750); //�ȴ�һ���ٴβɼ�
	
  	Get_CapCurrent_Value(test_temp);//ȡһ�ε�ǰ����ֵ
  	for(k=0;k<8;k++) total+=test_temp[k]; //����ͨ������ֵ��
		
	  if(	0<total && total<Init_Test){ LCD_ShowString(30,10,16,16,16,"*");return ;}
	  else { LCD_ShowString(30,10,16,16,16,"!");goto start;}
	
}

//��ȡ�������ֵ,

//�õ���ǰѵ��ʱ����ֵ��a[]Ϊװ8ͨ������ֵ����ָ���ַ
void Get_CapCurrent_Value(float cap[])
{	
	int i,j;//iΪ8ͨ��ѭ����������jΪ��������ָ���
	float sum=0;
	for(i=0;i<8;i++) //����ͨ��
	{	
		sum=0;//��ֵ����
		
		if(i<4) //ѵ����1  0-3ͨ��
			{	
				for(j=0;j<CrrentCap_N;j++) {	crrent_Cap_buffer[j] = Cap_Calculate(i);sum+=crrent_Cap_buffer[j];delay_us(100);}//ÿ��ͨ���ɼ�InitCap_N������,���ۼӺ�
			}
		else 	  //ѵ����2  4-7ͨ��
			{	
				for(j=0;j<CrrentCap_N;j++) {	crrent_Cap_buffer[j] = Cap_2Calculate(i%4);sum+=crrent_Cap_buffer[j];delay_us(100);	}//ÿ��ͨ���ɼ�InitCap_N�����ݣ����ۼӺ�
			}
			
		cap[i] = sum/CrrentCap_N;//��8ͨ����ǰѵ��ƽ��ֵ
		cap[i] -= temp[i];       //��8ͨ�����ʼ���Ĳ�ֵ
	}
}

//�õ���ǰ����ֵ������0Ϊ�����ƣ�����0x80Ϊ1�ң�0x40Ϊ1��0x20Ϊ2�ң�0x10Ϊ2��
u8 Get_gesture_Value(float cap[])
{	
	int i,j;//iΪ8ͨ��ѭ����������jΪ��������ָ���
	float sum=0;
	static u8 gesture1_zuo=0,gesture1_you=0,gesture2_zuo=0,gesture2_you=0; 
	
	for(i=0;i<8;i++) //����ͨ��
	{	
		sum=0;
		
		if(i<4) //ѵ����1  0-3ͨ��
			{	
				for(j=0;j<CrrentCap_N;j++) {	crrent_Cap_buffer[j] = Cap_Calculate(i);sum+=crrent_Cap_buffer[j];delay_us(100);}//ÿ��ͨ���ɼ�InitCap_N������,���ۼӺ�
			}
		else 	  //ѵ����2  4-7ͨ��
			{	
				for(j=0;j<CrrentCap_N;j++) {	crrent_Cap_buffer[j] = Cap_2Calculate(i%4);sum+=crrent_Cap_buffer[j];delay_us(100);	}//ÿ��ͨ���ɼ�InitCap_N�����ݣ����ۼӺ�
			}
			
		cap[i] = sum/CrrentCap_N;//��8ͨ����ǰѵ��ƽ��ֵ
		cap[i] -= temp[i];       //��8ͨ�����ʼ���Ĳ�ֵ
		
		if((cap[i]*100)>MusicThr) //�����ǰͨ����ͨ
			{
					if(i==0)//�ǲ���0ͨ�����ǵĻ������Ƽ�¼��1
					{
						gesture1_you=1;
						if(gesture1_zuo==1)return 0x40;
						else gesture1_zuo=0;
					}
					if(i==3)//�ǲ���3ͨ��������ǣ�֮ǰ0ͨ���ǲ����Ѿ���ͨ��������ǣ���ô���������ƣ�����������Ƽ�¼
					{
					  gesture1_zuo=1;
						if(gesture1_you==1)return 0x80;
						else gesture1_you=0;
					}
					if(i==4)//�ǲ���4ͨ�����ǵĻ������Ƽ�¼��1
					{
						gesture2_you=1;
						if(gesture2_zuo==1)return 0x10;
						else gesture2_zuo=0;
					}
					if(i==7)//�ǲ���3ͨ��������ǣ�֮ǰ0ͨ���ǲ����Ѿ���ͨ��������ǣ���ô���������ƣ�����������Ƽ�¼
					{
					  gesture2_zuo=1;
						if(gesture2_you==1)return 0x20;
						else gesture2_you=0;
					}
			}
	}
	return 0;
}

//��ʱ���㻷����ֵ������������С��
void MinGet_CapInit_Value(void)
{
 	int i,j,k;//iΪ8ͨ��ѭ����������jΪ��������ָ���,kΪ��ͼ�����
	float sum,total;//sum���Լ�¼ÿ��ͨ����ֵ�����Բɼ�����[MinInitCap_N]���õ�ͨ����ʼ����ֵ��total��¼����ͨ����ʼ����ֵ��
start:
  LCD_ShowString(30,10,16,16,16,"!");
	for(i=0;i<8;i++) //����ͨ��
	{	
		sum=0;//��ֵ����
		
		if(i<4) //��1   0-3ͨ��
			{	
				for(j=0;j<MinInitCap_N;j++) {	init_Cap_buffer[j] = Cap_Calculate(i);sum+=init_Cap_buffer[j];delay_ms(10);	}//ÿ��ͨ���ɼ�InitCap_N������,���ۼӺ�
			}
		else 	  //��2   4-7ͨ��
			{	
				for(j=0;j<MinInitCap_N;j++) {	init_Cap_buffer[j] = Cap_2Calculate(i%4);sum+=init_Cap_buffer[j];delay_ms(10);	}//ÿ��ͨ���ɼ�InitCap_N�����ݣ����ۼӺ�
			}
					 
		temp[i] = sum/MinInitCap_N;//��8ͨ����ʼ��ƽ��ֵ
	}
		total=0;
  	delay_ms(25); //�ȴ�10ms�ٴβɼ�
	
  	Get_CapCurrent_Value(test_temp);//ȡһ�ε�ǰ����ֵ
  	for(k=0;k<8;k++) total+=test_temp[k]; //����ͨ������ֵ��
		
	  if(	0<total && total<MinInit_Test){ LCD_ShowString(30,10,16,16,16,"*");return ;}
	  else { LCD_ShowString(30,10,16,16,16,"!");goto start;}
	
}

////��1�˲�
//float Filter(u8 t) {
//    int i;
//    float filter0_sum = 0,filter1_sum = 0,filter2_sum = 0,filter3_sum = 0;
//		switch(t)
//		{
//			case 0:
//							filter0_buf[FILTER_N] = Cap_Calculate(0);
//							for(i = 0; i < FILTER_N; i++) {filter0_buf[i] = filter0_buf[i + 1]; filter0_sum += filter0_buf[i];}
//							return (float)(filter0_sum / FILTER_N);
//							
//			case 1:
//							filter1_buf[FILTER_N] = Cap_Calculate(1);
//							for(i = 0; i < FILTER_N; i++) {filter1_buf[i] = filter1_buf[i + 1]; filter1_sum += filter1_buf[i];}
//							return (float)(filter1_sum / FILTER_N);
//							
//			case 2:
//							filter2_buf[FILTER_N] = Cap_Calculate(2);
//							for(i = 0; i < FILTER_N; i++) {filter2_buf[i] = filter2_buf[i + 1]; filter2_sum += filter2_buf[i];}
//							return (float)(filter2_sum / FILTER_N);
//							
//			case 3: 
//							filter3_buf[FILTER_N] = Cap_Calculate(3);
//							for(i = 0; i < FILTER_N; i++) {filter3_buf[i] = filter3_buf[i + 1]; filter3_sum += filter3_buf[i];}
//							return (float)(filter3_sum / FILTER_N);
//								
//			default:return (float)(0);
//		 
//		}
//}

////��2�˲�
//float Filter2(u8 t) {
//    int i;
//    float filter0_sum = 0,filter1_sum = 0,filter2_sum = 0,filter3_sum = 0;
//		switch(t)
//		{
//			case 0:
//							filter4_buf[FILTER_N] = Cap_2Calculate(0);
//							for(i = 0; i < FILTER_N; i++) {filter4_buf[i] = filter4_buf[i + 1]; filter0_sum += filter4_buf[i];}
//							return (float)(filter0_sum / FILTER_N);
//							
//			case 1:
//							filter5_buf[FILTER_N] = Cap_2Calculate(1);
//							for(i = 0; i < FILTER_N; i++) {filter5_buf[i] = filter5_buf[i + 1]; filter1_sum += filter5_buf[i];}
//							return (float)(filter1_sum / FILTER_N);
//							
//			case 2:
//							filter6_buf[FILTER_N] = Cap_2Calculate(2);
//							for(i = 0; i < FILTER_N; i++) {filter6_buf[i] = filter6_buf[i + 1]; filter2_sum += filter6_buf[i];}
//							return (float)(filter2_sum / FILTER_N);
//							
//			case 3: 
//							filter7_buf[FILTER_N] = Cap_2Calculate(3);
//							for(i = 0; i < FILTER_N; i++) {filter7_buf[i] = filter7_buf[i + 1]; filter3_sum += filter7_buf[i];}
//							return (float)(filter3_sum / FILTER_N);
//								
//			default:return (float)(0);
//		 
//		}
//}






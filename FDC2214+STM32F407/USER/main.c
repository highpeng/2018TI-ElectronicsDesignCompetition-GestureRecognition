#include "sys.h"
#include "delay.h"
#include "silencht_usart.h"	
#include "led.h"
#include "fdc2214.h"
#include "fdc22142.h"
#include "DataScope_DP.h"
#include "lcd.h"
#include <stdio.h>
#include "exti.h"

//��������   ��1��  SCL: PC4  SDA:PC5  SD/ADDR:GND VCC:3.3V
//��������   ��2��  SCL��PF8  SDA:PF9  SD/ADDR:GND VCC:3.3V       

//��λ�������о�ʱ��ǰֵ8ͨ���Ĵ�
float res[8];
float num4_Sum1=0,num4_Sum2=0; //��¼�о�ʱѵ����1��2��number4�ĵ��ݺͣ��ж��Ƿ�����������


//ֱ��+ѵ���о�ʱ��ȭ����ͨ����¼�Ĵ�
u8 hand_record=0;
const u8 rock_record_direc=0x77,scissors_record_direc=0x66,paper_record_direc=0xff;

//ֱ��+ѵ���о�ʱ��ȭ����ͨ����¼�Ĵ�
u8 number_hand=0;
const u8 num1_record_direc=0x11,num2_record_direc=0x33,num3_record_direc=0x77,num4_record_direc=0xff;


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
							
	modeģʽ����[[����ť3����ģʽ����]]��
							ÿ�Ρ���ʼ������ĳ��ģʽʱ��start_end��1
							ÿ�Ρ�����������ĳ��ģʽʱ��start_end��0
*/


int main(void)
{ 
//  unsigned char i;          //��������
//	unsigned char Send_Count; //������Ҫ���͵����ݸ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);      //��ʼ����ʱ����
	EXTIX_Init();         //�ⲿ�жϰ�����ʼ��
	usart1_init(115200);  //���Դ��� 
	LCD_Init();						//��ʼ��LCD
	LCD_Clear(WHITE);	 	  //����
	delay_ms(10);         

	while(FDC2214_Init()) delay_ms(100);//FDC2214��ʼ��
	while(FDC2214_2Init())delay_ms(100);//FDC22142��ʼ��
	
	delay_ms(1000);//�ȴ��ȶ�
	
	u1_printf("welcome!\r\n");
	
  Get_CapInit_Value(); //ɨ��õ���ʼ����ֵ
	u1_printf("%f,%f,%f,%f,%f,%f,%f,%f \r\n",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7]); 
	
//	u1_printf("CH0:%3.3f CH1:%3.3f CH2:%3.3f CH3:%3.3f��CH4:%3.3f CH5:%3.3f CH6:%3.3f CH7:%3.3f\r\n",res0,res1,res2,res3,res4,res5,res6,res7); 
	u1_printf ("[����ť2����ģʽѡ��]��\r\n��ģʽ0����ʼ�������� \r\n��ģʽ1��ֱ���о���ȭģʽ\r\n��ģʽ2��ֱ���о���ȭģʽ\r\n��ģʽ3��ѵ����ȭģʽ\r\n��ģʽ4���о���ȭģʽ\r\n��ģʽ5��ѵ����ȭģʽ\r\n��ģʽ6���о���ȭģʽ\r\n[��ť3����ģʽ����]\r\n");
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
		if(start_end==1)    //�����ģʽ������
		{
			if(mode==0) ;
//------------------------------------------ֱ���о���ȭģʽ---------------------------------------------------------------	

			else if(mode==1)   
		  {
			      Get_CapCurrent_Value(res,CrrentCap_N);//��ȡ��ǰ����ֵ
						hand_record=0;												//�����ȭ���Ƽ�¼
				    if((res[0]*100)>Threshold)	hand_record|=0x80;
						if((res[1]*100)>Threshold)	hand_record|=0x40;	
						if((res[2]*100)>Threshold)	hand_record|=0x20;
						if((res[3]*100)>Threshold)	hand_record|=0x10;	
						if((res[4]*100)>Threshold)	hand_record|=0x08;
					  if((res[5]*100)>Threshold)	hand_record|=0x04;	
					  if((res[6]*100)>Threshold)	hand_record|=0x02; 
					  if((res[7]*100)>Threshold)	hand_record|=0x01;
										
						if((hand_record&0xf0)==(rock_record_direc&0xf0))u1_printf ("һ��ʯͷ!");//һ����ʯͷ��
						else if((hand_record&0xf0)==(scissors_record_direc&0xf0))u1_printf ("һ�ż���!"); //һ���Ǽ�����
						else if((hand_record&0xf0)==(paper_record_direc&0xf0))u1_printf ("һ�Ų�!");//һ���ǲ���	
						else u1_printf ("һ���������������!");					
						
						if((hand_record&0x0f)==(rock_record_direc&0x0f))u1_printf ("����ʯͷ!");//������ʯͷ��					
						else if((hand_record&0x0f)==(scissors_record_direc&0x0f))u1_printf ("���ż���!"); //�����Ǽ�����					
						else if((hand_record&0x0f)==(paper_record_direc&0x0f))u1_printf ("���Ų�!");//�����ǲ���					
						else u1_printf ("�����������������!");
			
			}
//------------------------------------------ֱ���о���ȭģʽ---------------------------------------------------------------			
			else if(mode==2)  
			{
						Get_CapCurrent_Value(res,CrrentCap_N);//��ȡ��ǰ8ͨ������ֵ
					  number_hand=0;num4_Sum1=0;num4_Sum2=0;//���Ƽ�¼�����ݺ�ֵ����
					 
				    if((res[0]*100)>Threshold)	number_hand|=0x80;num4_Sum1+=res[0]*100;
						if((res[1]*100)>Threshold)	number_hand|=0x40;num4_Sum1+=res[1]*100;	
						if((res[2]*100)>Threshold)	number_hand|=0x20;num4_Sum1+=res[2]*100;
						if((res[3]*100)>Threshold)	number_hand|=0x10;num4_Sum1+=res[3]*100;
						if((res[4]*100)>Threshold)	number_hand|=0x08;num4_Sum2+=res[4]*100;
					  if((res[5]*100)>Threshold)	number_hand|=0x04;num4_Sum2+=res[5]*100;
					  if((res[6]*100)>Threshold)	number_hand|=0x02;num4_Sum2+=res[6]*100;
					  if((res[7]*100)>Threshold)	number_hand|=0x01;num4_Sum2+=res[7]*100;

						if(num4_Sum1>1200)u1_printf ("һ��5!\r\n");
						else if((number_hand&0xf0)==(num1_record_direc&0xf0))u1_printf ("һ��1!\r\n");
						else if((number_hand&0xf0)==(num2_record_direc&0xf0))u1_printf ("һ��2!\r\n"); 
						else if((number_hand&0xf0)==(num3_record_direc&0xf0))u1_printf ("һ��3!\r\n");
						else if((number_hand&0xf0)==(num4_record_direc&0xf0))u1_printf ("һ��4!\r\n");						
						else u1_printf ("һ����������\r\n");
													
						if(num4_Sum2>1200)u1_printf ("����5!\r\n");
						else if((number_hand&0x0f)==(num1_record_direc&0x0f))u1_printf ("����1!\r\n");
						else if((number_hand&0x0f)==(num2_record_direc&0x0f))u1_printf ("����2!\r\n"); 
						else if((number_hand&0x0f)==(num3_record_direc&0x0f))u1_printf ("����3!\r\n");
						else if((number_hand&0x0f)==(num4_record_direc&0x0f))u1_printf ("����4!\r\n");						
						else u1_printf ("������������\r\n");
			
			}
//--------------------------------------ѵ����ȭģʽ��������ͬʱ---------------------------------------------------------------			
			else if(mode==3)  
			{
				 if(roshambo_wait==0&&usart_num==1)
						{u1_printf ("��������ʯͷ�����Ұ��°�ť4\r\n");usart_num=0;}  //�밴��ť4ѵ��ʯͷ
				 else if(roshambo_wait==1&&usart_num==1)
						{u1_printf ("�������ļ��������Ұ��°�ť4\r\n");usart_num=0;}  //�밴��ť4����ѵ������
				 else if(roshambo_wait==2&&usart_num==1)
						{u1_printf ("�������Ĳ��� ���Ұ��°�ť4\r\n");usart_num=0;}   //�밴��ť4����ѵ����
				 else if(roshambo_wait==3&&usart_num==1) 
						{	roshambo_wait=0;u1_printf ("ѵ����ɣ���رյ�ǰģʽ��ʼģʽ��4�����ԣ�\r\n");usart_num=0;}//ѵ�����
			}
			
//------------------------------------------�о���ȭģʽ---------------------------------------------------------------				
			else if(mode==4)
			{
				    Get_CapCurrent_Value(res,CrrentCap_N);
						hand_record=0;
				    if((res[0]*100)>Threshold)	hand_record|=0x80;
						if((res[1]*100)>Threshold)	hand_record|=0x40;	
						if((res[2]*100)>Threshold)	hand_record|=0x20;
						if((res[3]*100)>Threshold)	hand_record|=0x10;	
						if((res[4]*100)>Threshold)	hand_record|=0x08;
					  if((res[5]*100)>Threshold)	hand_record|=0x04;	
					  if((res[6]*100)>Threshold)	hand_record|=0x02; 
					  if((res[7]*100)>Threshold)	hand_record|=0x01;
										
						if((hand_record&0xf0)==(rock_hand_record&0xf0))u1_printf ("һ��ʯͷ!");//һ����ʯͷ��
						
						else if((hand_record&0xf0)==(scissors_hand_record&0xf0))u1_printf ("һ�ż���!"); //һ���Ǽ�����
						
						else if((hand_record&0xf0)==(paper_hand_record&0xf0))u1_printf ("һ�Ų�!");//һ���ǲ���
						
						else u1_printf ("һ�����ʶ������!");
														
						if((hand_record&0x0f)==(rock_hand_record&0x0f))u1_printf ("����ʯͷ!");//������ʯͷ��
						
						else if((hand_record&0x0f)==(scissors_hand_record&0x0f))u1_printf ("���ż���!"); //�����Ǽ�����
						
						else if((hand_record&0x0f)==(paper_hand_record&0x0f))u1_printf ("���Ų�!");//�����ǲ���
						
						else u1_printf ("�������ʶ������!");																				
			}
//------------------------------------------ѵ����ȭģʽ��������ͬʱ---------------------------------------------------------------
			else if(mode==5)
			{
						if(number_wait==0&&usart_num==1)
							 { u1_printf ("�������һ����ָ�����Ұ��°�ť4\r\n");usart_num=0; }  //�밴��ť4ѵ��1
						 else if(number_wait==1&&usart_num==1)
							 { u1_printf ("�������������ָ�����Ұ��°�ť4\r\n");usart_num=0; }  //�밴��ť4����2
						 else if(number_wait==2&&usart_num==1)
							 { u1_printf ("�������������ָ�����Ұ��°�ť4\r\n");usart_num=0; }  //�밴��ť4����3
						 else if(number_wait==3&&usart_num==1) 
							 { u1_printf ("��������ĸ���ָ�����Ұ��°�ť4\r\n");usart_num=0; }  //�밴��ť4����4
						 else if(number_wait==0&&usart_num==1){ number_wait=4;u1_printf ("ѵ����ɣ���رյ�ǰģʽ��ʼģʽ��6�����ԣ�\r\n");usart_num=0;	}//ѵ�����
			}
//------------------------------------------�о���ȭģʽ��������ͬʱ---------------------------------------------------------------			
			else if(mode==6)	
			{
			      Get_CapCurrent_Value(res,CrrentCap_N);
					  number_hand=0;num4_Sum1=0;num4_Sum2=0;//���Ƽ�¼�����ݺ�ֵ����
					 
				    if((res[0]*100)>Threshold)	number_hand|=0x80;num4_Sum1+=res[0]*100;
						if((res[1]*100)>Threshold)	number_hand|=0x40;num4_Sum1+=res[1]*100;	
						if((res[2]*100)>Threshold)	number_hand|=0x20;num4_Sum1+=res[2]*100;
						if((res[3]*100)>Threshold)	number_hand|=0x10;num4_Sum1+=res[3]*100;
						if((res[4]*100)>Threshold)	number_hand|=0x08;num4_Sum2+=res[4]*100;
					  if((res[5]*100)>Threshold)	number_hand|=0x04;num4_Sum2+=res[5]*100;
					  if((res[6]*100)>Threshold)	number_hand|=0x02;num4_Sum2+=res[6]*100;
					  if((res[7]*100)>Threshold)	number_hand|=0x01;num4_Sum2+=res[7]*100;
													
						num4_Sum1=num4_Sum1-num4_SumCap1;
						if(num4_Sum1>150)u1_printf ("һ��5!\r\n");
						else if((number_hand&0xf0)==(number1_record&0xf0))u1_printf ("һ��1!\r\n");
						else if((number_hand&0xf0)==(number2_record&0xf0))u1_printf ("һ��2!\r\n"); 
						else if((number_hand&0xf0)==(number3_record&0xf0))u1_printf ("һ��3!\r\n");
						else if((number_hand&0xf0)==(number4_record&0xf0))u1_printf ("һ��4!\r\n");						
						else u1_printf ("һ����������\r\n");
													
						num4_Sum2=num4_Sum2-num4_SumCap2;
						if(num4_Sum2>150)u1_printf ("����5!");
						else if((number_hand&0x0f)==(number1_record&0x0f))u1_printf ("����1!");
						else if((number_hand&0x0f)==(number2_record&0x0f))u1_printf ("����2!"); 
						else if((number_hand&0x0f)==(number3_record&0x0f))u1_printf ("����3!");
						else if((number_hand&0x0f)==(number4_record&0x0f))u1_printf ("����4!");						
						else u1_printf ("������������");
			
			}
		}
		else ;

			 
				
//					
//					DataScope_Get_Channel_Data( res[0]*100 , 1 ); //������ 1.0  д��ͨ�� 1
//					DataScope_Get_Channel_Data( res[1]*100 , 2 ); //������ 2.0  д��ͨ�� 2
//					DataScope_Get_Channel_Data( res[2]*100 , 3 ); //������ 3.0  д��ͨ�� 3
//					DataScope_Get_Channel_Data( res[3]*100 , 4 ); //������ 4.0  д��ͨ�� 4
//					DataScope_Get_Channel_Data( res[4]*100 , 5 ); //������ 5.0  д��ͨ�� 5
//					DataScope_Get_Channel_Data( res[5]*100 , 6 ); //������ 6.0  д��ͨ�� 6
//					DataScope_Get_Channel_Data( res[6]*100 , 7 ); //������ 7.0  д��ͨ�� 7
//					DataScope_Get_Channel_Data( res[7]*100 , 8 ); //������ 8.0  д��ͨ�� 8
//					DataScope_Get_Channel_Data( hand_record ,9 ); //������ 8.0  д��ͨ�� 8
//					
//					Send_Count = DataScope_Data_Generate(9); //����8��ͨ���� ��ʽ��֡���ݣ�����֡���ݳ���
//					
//					for( i = 0 ; i < Send_Count; i++)  //ѭ������,ֱ���������
//					{
//						while((USART1->SR&0X40)==0);  
//						USART1->DR = DataScope_OutPut_Buffer[i]; //�Ӵ��ڶ�һ���ֽ����ݳ�ȥ      
//					}
//					delay_ms(12); //20fps, ֡���ʱ�䡣 ��ͬ�������ü� USB-TTL �豸�����Ӿ���Ӱ���ʱ��ĳ��̣�����ʵ��Ϊ׼�� 
  				
			
	}
}



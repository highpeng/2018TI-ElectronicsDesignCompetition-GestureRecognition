#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"  	
	 
//#define FILTER_N   25			 //�ɼ����ݻ����˲�������
#define InitCap_N  50       //�ɼ����ݳ�ʼ��ֵ�˲�����
#define MinInitCap_N  6       //�ɼ����ݳ�ʼ��ֵ�˲�����
#define CrrentCap_N  4      //ѵ��ʱÿ�βɼ������ܴ���
#define Threshold  150      //��ֵ,����Ƿ�����ָ����ͨ���������ֵ
#define NumberThr  2220     //��ֵ,����Ƿ��������ָ�����������ֵ
#define MusicThr   180      //�����ټ��ĵ��ݼ����ֵ
#define Init_Test  20       //��ֵ,����ʼ���Ƿ�ɹ�[��ʼ������ֵ���ͳ�ʼ������Բ�ֵ�Ƿ�С�ڸ���ֵ]
#define MinInit_Test  35       //��ֵ,����ʼ���Ƿ�ɹ�[��ʼ������ֵ���ͳ�ʼ������Բ�ֵ�Ƿ�С�ڸ���ֵ]


extern float temp[8];                      
extern float rock[8],paper[8],scissors[8];   
extern float number[8];				             

/*
	modeģʽָ��[����ť2����ģʽѡ��]��
							��ģʽ0����ʼ��������
							��ģʽ1��ֱ���о���ȭģʽ�������ɲ�����Աָ��
							��ģʽ2��ֱ���о���ȭģʽ�������ɲ�����Աָ��
							��ģʽ3��ѵ����ȭģʽ��ʯͷ���������������������Ա�Լ�ָ����ѵ��
							��ģʽ4���о���ȭģʽ
							��ģʽ5��ѵ����ȭģʽ��ʯͷ���������������������Ա�Լ�ָ����ѵ��
							��ģʽ6���о���ȭģʽ	
							��ģʽ7�����ټ�ģʽ
							��ģʽ8�����������ж�
							
	modeģʽ����[[����ť3����ģʽ����]]��
							ÿ�Ρ���ʼ������ĳ��ģʽʱ��start_end��1
							ÿ�Ρ�����������ĳ��ģʽʱ��start_end��0

*/
extern u8 mode;      
extern u8 start_end;     //ģʽ����Ϊ0��ģʽ��ʼΪ1

extern u8 roshambo_wait; //��ȭѵ��ʱ�ȴ���־����0����ȫ��δ��ɡ�1������������δ��ɡ�2���� ��δ��ɡ�3����ȫ�����
extern u8 number_wait;   //��ȭѵ��ʱ�ȴ���־����0����ȫ��δ��ɡ�1����234δ���    ��2����34δ��ɡ�3����4δ��ɡ�4��:δ��ɡ�5����ȫ�����
extern u8 usart_num;     //�������ȴ���־

/*
	1��ȭ����ͨ��������¼�Ĵ棬8bit�涨���£���4λΪѵ����1��0-3ͨ��������λΪѵ����2��0-3ͨ�� ����ӦλֵΪ0Ϊ����ָ��ֵΪ1Ϊ����ָ������
	2��ȭ����ͨ��������¼�Ĵ棬number1-5��¼ѵ���߻�ȭ����1-5,����number����λΪѵ����1��ͨ����¼������λΪѵ����2��ͨ����¼
	3��num4_SumCap1/2����¼ѵ����1/2������[number4]���ܵ���ֵ������main�����ж������ָ
*/
extern u8 rock_hand_record,paper_hand_record,scissors_hand_record;
extern u8 rock_hand_record2,paper_hand_record2,scissors_hand_record2;
extern u8 rock_hand_record3,paper_hand_record3,scissors_hand_record3;

extern u8 number1_record,number2_record,number3_record,number4_record,number5_record;
extern u8 number1_record2,number2_record2,number3_record2,number4_record2,number5_record2;
extern u8 number1_record3,number2_record3,number3_record3,number4_record3,number5_record3;
extern float num4_SumCap1,num4_SumCap2; //��¼number4�ĵ��ݺ���Ϊ��ֵ����main���ж��Ƿ��������ơ���5����ָ���ϡ�

/*
	1�õ�ͨ��������ʼ����ֵ
	 ƽʱ�õļ�����С�õ�ͨ��������ʼ����ֵ����
	2�õ�(ѵ��/�о�ʱ)��ǰͨ������ֵ����parameter��a[]Ϊ8ͨ������ֵ����ָ���ַ
	3�����˲�
	4�ⲿ�жϰ�ť��ʼ��
	5,4����

*/
void  Get_CapInit_Value(void);
void  Get_CapCurrent_Value(float cap[]);
void  MinGet_CapInit_Value(void);
//extern float Filter(u8 t);
//extern float Filter2(u8 t);
void EXTIX_Init(void);	//�ⲿ�жϳ�ʼ��	
void KEY_Init(void);	
//�õ���ǰ����ֵ������0Ϊ�����ƣ�����0x80Ϊ1�ң�0x40Ϊ1��0x20Ϊ2�ң�0x10Ϊ2��
u8 Get_gesture_Value(float cap[]); 					    
#endif


























#include "stm32_config.h"
#include "lcd.h"
#include "AD5933.h"
#include "interface.H"
#include "key.h"
#include "math.h"






#define pi 3.1415926
#define R1 199.6
#define R2 10000
#define beta  0.002999                          //0.00271      (pi/1000.0)
#define zurong 600 //�����������ٽ�ֵ
//#define rongkai 1000 //�����뿪·���ٽ�
#define omega 2*pi*100000 //���ٶ�   Ƶ������Ϊ100kHz�������Ƶ�ʼǵ�AD5933�ļ�Ҳ��һ�£�����


int a;
float R_Impedance, R_Rad, realData, imageData;	//���гֵ������ֵ��ʵ�����ݣ��鲿����
char disStr[64];

double theta,L,Cap,temp;
Complex Zin0,Zin,Zl,Z0,j;
extern uint8_t KeyNum;


//��interface.H���޸ĺ궨�壬�Ҹ���ģ������ñλ�ã����Ĳ�������
int main(void)
{
	MY_NVIC_PriorityGroup_Config(NVIC_PriorityGroup_2);	//�����жϷ���
	delay_init(72);	//��ʼ����ʱ����
	initial_lcd();	//��ʼ��LCD

//ע�⣺
//AD5933ģ��Ĭ���ʺϲ�������5.1K-10M���裬�������С����ʱ�����������������㣬��Ҫʹ���ⲿ�˷ŵ�·��ǿ�����������Ӷ�����С��5.1K��С����
//�������ڻ�С��5.1K�ĵ���ʱ�������ģ��������ñλ��
//�ӷ�1����������5.1K( 5.1K-10M)����ʱ,������ñ��ģ��������P6��1��2���ӣ�P6��3,4�Ͽ���P5��1,2�Ͽ���(��interface.H�д򿪺궨��#define _DEF_RSBIGGER5K		1)
//�ӷ�2������С��5.1K(100R-5.1K)����ʱ,������ñ��ģ��������P6��1��2�Ͽ���P6��3,4���ӣ�P5��1,2���ӣ�(��interface.H�д򿪺궨��#define _DEF_RSBIGGER5K		0)
	
	#if _DEF_RSBIGGER5K == 1								//��interface.H�д򿪴˺궨�壬����ע����������ñ���ɲ����ϴ����
	LCD_Show_CEStr(0,0,(u8*)"5K<�������<5M  ");
	#elif DEF_RSBIGGER5K == 0								//��interface.H�д򿪴˺궨�壬����ע����������ñ���ɲ�����С����
	LCD_Show_CEStr(0,0,"Welcome!");//100R<�������<8K
	#endif
	
	
	
	Z0 = Complex_Create(50,0);                                                          //  Z0�������������
	j = Complex_Create(0,1);
	
	
	a = sizeof(long);
	

	
	
	while(1)
	{
			
		Key_GetNum();
	
		
	
		if (KeyNum == 1){                                     // ���ȼ��
			LCD_Clear();		
			sprintf(disStr,"���ڼ�� ");
			LCD_Show_CEStr(30,0,(u8*)disStr);

				
			DA5933_Get_RsRealImage(&R_Impedance, &R_Rad, &realData, &imageData);	//�������гֵ������ֵ����ʵ�����ݣ��鲿����
			Zin0 = Complex_Create(R_Impedance*cos(R_Rad),R_Impedance*sin(R_Rad));  //����ȡ�ĵ翹ģֵת���ɸ�����ʵ��Zreal=Zin*cos��phase���鲿Zimage=Zin*sin��phase��
			Zin = Div(Sub(Con(R1*R2),Pro(Con(R2),Zin0)),Sub(Zin0,Con(R1+R2)));//��ȥ��ӿ�����ֵӰ��
			
			
		

//Zin��ֵ��һ��Ҫ����ֵ�ǵñ��

			
			theta = atan(Div(Z0,Zin).Imagepart);//�û��ȼ���                             //hr +- Image?
			L=theta/beta;	                           //flen_a theta/beta + flen_b , for default a=1 b=0
			L*=100;//תΪcm
			
			if(L<500){
				L=L+L*0.02; // С����У׼ ,����ʵ�⵽1m���ڵĻ���Ҫ����У׼��
			
			}

			sprintf(disStr,"%f cm ",L);
			LCD_Show_CEStr(0,4,(u8*)disStr);
			delay_ms(1000);
			
			sprintf(disStr,"�������");
			LCD_Show_CEStr(30,0,(u8*)disStr);



			sprintf(disStr,"��·");
			LCD_Show_CEStr(30,6,(u8*)disStr);

			
			KeyNum = 9;

		}
		else if(KeyNum == 5){                                   //���ؼ��
		
			if(R_Impedance>zurong){  // |Img|>> |Real| cap, |Real|>>|Img| resistor, hr ����ʱ���������ˣ���Ȼ�����ֱִ��ķ�ʽ�������
				LCD_Clear();


				sprintf(disStr,"���ڼ�� ");
				LCD_Show_CEStr(30,0,(u8*)disStr);
				
				DA5933_Get_RsRealImage(&R_Impedance, &R_Rad, &realData, &imageData);	//�������гֵ������ֵ����ʵ�����ݣ��鲿����
				Zin0 = Complex_Create(R_Impedance*cos(R_Rad),R_Impedance*sin(R_Rad));  //����ȡ�ĵ翹ģֵת���ɸ�����ʵ��Zreal=Zin*cos��phase���鲿Zimage=Zin*sin��phase��
				Zin = Div(Sub(Con(R1*R2),Pro(Con(R2),Zin0)),Sub(Zin0,Con(R1+R2)));//��ȥ��ӿ�����ֵӰ��
				
				

				Zl=Div(Sub(Pro(Zin,Z0),Pro(Pro(Pro(Z0,Z0),Con(tan(theta))),j)),Sub(Z0,Pro(Pro(j,Zin),Con(tan(theta)))));
				
				Cap = -(1/(Zl.Imagepart * omega))*1000000000000;
				
				Cap = 1.0303278*Cap-14.46552;
				
				
				
				
				sprintf(disStr,"�������");
				LCD_Show_CEStr(30,0,(u8*)disStr);

				
				sprintf(disStr,"����");
			    LCD_Show_CEStr(30,4,(u8*)disStr);

				sprintf(disStr,"%f pF ",Cap);
  				LCD_Show_CEStr(0,6,(u8*)disStr);
				
				KeyNum = 9;
			}
			else if(R_Impedance<zurong){
				
				LCD_Clear();			
				sprintf(disStr,"���ڼ�� ");
				LCD_Show_CEStr(30,0,(u8*)disStr);
				
				
				DA5933_Get_RsRealImage(&R_Impedance, &R_Rad, &realData, &imageData);	//�������гֵ������ֵ����ʵ�����ݣ��鲿����
				Zin0 = Complex_Create(R_Impedance*cos(R_Rad),R_Impedance*sin(R_Rad));  //����ȡ�ĵ翹ģֵת���ɸ�����ʵ��Zreal=Zin*cos��phase���鲿Zimage=Zin*sin��phase��
				Zin = Div(Sub(Con(R1*R2),Pro(Con(R2),Zin0)),Sub(Zin0,Con(R1+R2)));//��ȥ��ӿ�����ֵӰ��
								
				Zl=Div(Sub(Pro(Zin,Z0),Pro(Pro(Pro(Z0,Z0),Con(tan(theta))),j)),Sub(Z0,Pro(Pro(j,Zin),Con(tan(theta)))));
				
				
				sprintf(disStr,"�������");
				LCD_Show_CEStr(30,0,(u8*)disStr);

				sprintf(disStr,"����");
			  LCD_Show_CEStr(30,4,(u8*)disStr);

				Zl.Realpart = (1.50645034*Zl.Realpart -19.98632424)*0.96476988+1.13250738;   //��̫��У׼��û�и�Z0��ֵ������У׼�˺ܾã��Ƚϴֲڣ���
				
				
				
				sprintf(disStr,"%.3f �� ",Zl.Realpart);
  				LCD_Show_CEStr(0,6,(u8*)disStr);
				
				KeyNum = 9;

			}









		}		
		
		
		
		
	}
}


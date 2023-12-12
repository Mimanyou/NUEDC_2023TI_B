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
#define zurong 600 //电容与电阻的临界值
//#define rongkai 1000 //电容与开路的临界
#define omega 2*pi*100000 //角速度   频率设置为100kHz，改输出频率记得AD5933文件也改一下！！！


int a;
float R_Impedance, R_Rad, realData, imageData;	//所夹持电阻的阻值，实部数据，虚部数据
char disStr[64];

double theta,L,Cap,temp;
Complex Zin0,Zin,Zl,Z0,j;
extern uint8_t KeyNum;


//在interface.H中修改宏定义，且更改模块跳线帽位置，更改测量量程
int main(void)
{
	MY_NVIC_PriorityGroup_Config(NVIC_PriorityGroup_2);	//设置中断分组
	delay_init(72);	//初始化延时函数
	initial_lcd();	//初始化LCD

//注意：
//AD5933模块默认适合测量测量5.1K-10M电阻，需测量更小电阻时，自身驱动能力不足，需要使用外部运放电路增强驱动能力，从而测量小于5.1K的小电阻
//测量大于或小于5.1K的电阻时，需更改模块上跳线帽位置
//接法1，测量大于5.1K( 5.1K-10M)电阻时,用跳线帽将模块上排针P6的1、2连接，P6的3,4断开，P5的1,2断开；(至interface.H中打开宏定义#define _DEF_RSBIGGER5K		1)
//接法2，测量小于5.1K(100R-5.1K)电阻时,用跳线帽将模块上排针P6的1、2断开，P6的3,4连接，P5的1,2连接；(至interface.H中打开宏定义#define _DEF_RSBIGGER5K		0)
	
	#if _DEF_RSBIGGER5K == 1								//至interface.H中打开此宏定义，按其注释连接跳线帽，可测量较大电阻
	LCD_Show_CEStr(0,0,(u8*)"5K<电阻测量<5M  ");
	#elif DEF_RSBIGGER5K == 0								//至interface.H中打开此宏定义，按其注释连接跳线帽，可测量较小电阻
	LCD_Show_CEStr(0,0,"Welcome!");//100R<电阻测量<8K
	#endif
	
	
	
	Z0 = Complex_Create(50,0);                                                          //  Z0在这里！！！！！
	j = Complex_Create(0,1);
	
	
	a = sizeof(long);
	

	
	
	while(1)
	{
			
		Key_GetNum();
	
		
	
		if (KeyNum == 1){                                     // 长度检测
			LCD_Clear();		
			sprintf(disStr,"正在检测 ");
			LCD_Show_CEStr(30,0,(u8*)disStr);

				
			DA5933_Get_RsRealImage(&R_Impedance, &R_Rad, &realData, &imageData);	//测量所夹持电阻的阻值，及实部数据，虚部数据
			Zin0 = Complex_Create(R_Impedance*cos(R_Rad),R_Impedance*sin(R_Rad));  //将读取的电抗模值转换成复数，实部Zreal=Zin*cos（phase）虚部Zimage=Zin*sin（phase）
			Zin = Div(Sub(Con(R1*R2),Pro(Con(R2),Zin0)),Sub(Zin0,Con(R1+R2)));//除去外接控制阻值影响
			
			
		

//Zin的值看一下要，负值记得变号

			
			theta = atan(Div(Z0,Zin).Imagepart);//用弧度计算                             //hr +- Image?
			L=theta/beta;	                           //flen_a theta/beta + flen_b , for default a=1 b=0
			L*=100;//转为cm
			
			if(L<500){
				L=L+L*0.02; // 小长度校准 ,后面实测到1m以内的还需要进行校准。
			
			}

			sprintf(disStr,"%f cm ",L);
			LCD_Show_CEStr(0,4,(u8*)disStr);
			delay_ms(1000);
			
			sprintf(disStr,"结果保持");
			LCD_Show_CEStr(30,0,(u8*)disStr);



			sprintf(disStr,"开路");
			LCD_Show_CEStr(30,6,(u8*)disStr);

			
			KeyNum = 9;

		}
		else if(KeyNum == 5){                                   //负载检测
		
			if(R_Impedance>zurong){  // |Img|>> |Real| cap, |Real|>>|Img| resistor, hr 不过时间来不及了，仍然用这种粗暴的方式解决问题
				LCD_Clear();


				sprintf(disStr,"正在检测 ");
				LCD_Show_CEStr(30,0,(u8*)disStr);
				
				DA5933_Get_RsRealImage(&R_Impedance, &R_Rad, &realData, &imageData);	//测量所夹持电阻的阻值，及实部数据，虚部数据
				Zin0 = Complex_Create(R_Impedance*cos(R_Rad),R_Impedance*sin(R_Rad));  //将读取的电抗模值转换成复数，实部Zreal=Zin*cos（phase）虚部Zimage=Zin*sin（phase）
				Zin = Div(Sub(Con(R1*R2),Pro(Con(R2),Zin0)),Sub(Zin0,Con(R1+R2)));//除去外接控制阻值影响
				
				

				Zl=Div(Sub(Pro(Zin,Z0),Pro(Pro(Pro(Z0,Z0),Con(tan(theta))),j)),Sub(Z0,Pro(Pro(j,Zin),Con(tan(theta)))));
				
				Cap = -(1/(Zl.Imagepart * omega))*1000000000000;
				
				Cap = 1.0303278*Cap-14.46552;
				
				
				
				
				sprintf(disStr,"结果保持");
				LCD_Show_CEStr(30,0,(u8*)disStr);

				
				sprintf(disStr,"电容");
			    LCD_Show_CEStr(30,4,(u8*)disStr);

				sprintf(disStr,"%f pF ",Cap);
  				LCD_Show_CEStr(0,6,(u8*)disStr);
				
				KeyNum = 9;
			}
			else if(R_Impedance<zurong){
				
				LCD_Clear();			
				sprintf(disStr,"正在检测 ");
				LCD_Show_CEStr(30,0,(u8*)disStr);
				
				
				DA5933_Get_RsRealImage(&R_Impedance, &R_Rad, &realData, &imageData);	//测量所夹持电阻的阻值，及实部数据，虚部数据
				Zin0 = Complex_Create(R_Impedance*cos(R_Rad),R_Impedance*sin(R_Rad));  //将读取的电抗模值转换成复数，实部Zreal=Zin*cos（phase）虚部Zimage=Zin*sin（phase）
				Zin = Div(Sub(Con(R1*R2),Pro(Con(R2),Zin0)),Sub(Zin0,Con(R1+R2)));//除去外接控制阻值影响
								
				Zl=Div(Sub(Pro(Zin,Z0),Pro(Pro(Pro(Z0,Z0),Con(tan(theta))),j)),Sub(Z0,Pro(Pro(j,Zin),Con(tan(theta)))));
				
				
				sprintf(disStr,"结果保持");
				LCD_Show_CEStr(30,0,(u8*)disStr);

				sprintf(disStr,"电阻");
			  LCD_Show_CEStr(30,4,(u8*)disStr);

				Zl.Realpart = (1.50645034*Zl.Realpart -19.98632424)*0.96476988+1.13250738;   //不太好校准，没有改Z0的值，这里校准了很久，比较粗糙（）
				
				
				
				sprintf(disStr,"%.3f Ω ",Zl.Realpart);
  				LCD_Show_CEStr(0,6,(u8*)disStr);
				
				KeyNum = 9;

			}









		}		
		
		
		
		
	}
}


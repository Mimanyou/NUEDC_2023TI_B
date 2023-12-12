#ifndef __INTERFACE_H
#define __INTERFACE_H
#include "stm32f10x.h"
#include "lcd.h"
#include "AD5933.h"
#include "lcd.h"

#define	AD5933_MCLK_USE_OUT	0	//0：内部时钟(16.776MHz)  1：外部时钟


//注意：
//AD5933测量较小电阻时，自身驱动能力不足，需要使用外部运放电路增强驱动能力，从而测量小于5.1K的小电阻
//测量大于或小于5.1K的电阻时，需要切换下面宏定义，且更改模块上跳线帽位置
// 	#define _DEF_RSBIGGER5K		1		//接法1，测量大于5.1K( 5.1K-10M)电阻时,用跳线帽将模块上排针P6的1、2连接，P6的3,4断开，P5的1,2断开；
	#define _DEF_RSBIGGER5K		0		//接法2，测量小于5.1K(100R-5.1K)电阻时,用跳线帽将模块上排针P6的1、2断开，P6的3,4连接，P5的1,2连接；

#if _DEF_RSBIGGER5K == 1
#define REG_MIN	5100
#define REG_MAX	6000000
#define AD5933_Correction 101615461


#elif DEF_RSBIGGER5K == 0
#define REG_MIN	100
#define REG_MAX	8000
#define AD5933_Correction 3219849  //100kHz //hr
#define AD5933_CorrectionPhase -0.376884524 	//100kHz Radium //hr

//#define AD5933_Correction 986558 //10kHz
//#define AD5933_Correction 1718763 //1kHz
#endif

void FormatRsDisStr(float Rs, char str[]);

typedef struct  Complex  //复数类型
{
	float Realpart;  //实部
	float Imagepart; //虚部
}Complex;



Complex Complex_Create(float x, float y);
float GetReal(Complex C);
float GetImag(Complex C);
Complex Add(Complex C1, Complex C2);
Complex Sub(Complex C1, Complex C2);
Complex Pro(Complex C1, Complex C2);
Complex Div(Complex C1, Complex C2);
Complex Con(float x);



#endif

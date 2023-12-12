#include "interface.H"


void FormatRsDisStr(float Rs, char str[])
{
	if(Rs>1000000)
	{
		sprintf(str,"%03.03fMΩ  ",Rs/1000000);
	}else	if(Rs>1000)
	{
		sprintf(str,"%03.03fKΩ  ",Rs/1000);
	}else if(Rs<1000)
	{
		sprintf(str,"%03.03fΩ  ",Rs);
	}
////	sprintf(str,"%08.0fΩ  ",Rs);
}

#include <stdio.h> 
#include <math.h>

/*
complex.h 复数ADT定义

typedef struct  Complex  //复数类型
{
	float Realpart;  //实部
	float Imagepart; //虚部
}Complex;

*/

Complex Complex_Create(float x, float y)//给定复数的虚部和实部，并返回复数
{//构造一个复数
	Complex C;
	C.Realpart = x;
	C.Imagepart = y;
	return C;
}

float GetReal(Complex C)
	{//取复数C=x+yi的实部

    return C.Realpart;

	}
float GetImag(Complex C)
	{//取复数C=x+yi的虚部

    return C.Imagepart;

	}

	
	
	
	
/*
void Print_C(Complex C)
{//复数输出并控制格式
    if(C.Realpart==0&&C.Imagepart==0)   
			printf("0\n");
    else if(C.Realpart!=0&&C.Imagepart==0)  
			printf("%.3f\n");
    else if(C.Realpart==0&&C.Imagepart!=0) 
	{
		if(C.Imagepart>0)
			printf("%.3fi\n",C.Imagepart);
		else if(C.Imagepart<0)
			printf("-%.3fi\n",fabs(C.Imagepart));
	}
	else
	{
		if(C.Imagepart>0)
			printf("%.3f+%.3fi\n", C.Realpart,C.Imagepart);
		else
			printf("%.3f-%.3fi\n", C.Realpart,fabs(C.Imagepart));
	}
}

*/

Complex Add(Complex C1, Complex C2)

{//求两个复数C1和C2的和Sum
    Complex Sum;
    Sum.Realpart = C1.Realpart + C2.Realpart;
    Sum.Imagepart = C1.Imagepart + C2.Imagepart;
    return Sum;
}

Complex Sub(Complex C1, Complex C2)

{//求两个复数C1和C2的差difference
    Complex difference;
    difference.Realpart = C1.Realpart - C2.Realpart;
    difference.Imagepart = C1.Imagepart - C2.Imagepart;
    return difference;
}
Complex Pro(Complex C1, Complex C2)

{//求两个复数C1和C2的积Product
    Complex Product;
    Product.Realpart = C1.Realpart * C2.Realpart - C1.Imagepart * C2.Imagepart;
    Product.Imagepart = C1.Realpart * C2.Imagepart + C1.Imagepart * C2.Realpart;
    return Product;
}

Complex Div(Complex C1, Complex C2)

{//求两个复数C1和C2的商Divide
    Complex Divide;
    Divide.Realpart = (C1.Realpart * C2.Realpart + C1.Imagepart * C2.Imagepart) 
		        /(C2.Realpart * C2.Realpart + C2.Imagepart * C2.Imagepart);

    Divide.Imagepart = (C1.Imagepart * C2.Realpart - C1.Realpart * C2.Imagepart)
			/(C2.Realpart * C2.Realpart + C2.Imagepart * C2.Imagepart);
    return Divide;
}

Complex Con(float x){//实数转复数
		Complex a;
		a.Realpart = x;
		a.Imagepart = 0;
		return a;

}

#include "AD5933.h"
#include "delay.h"
#include <math.h>

#define Delay_IIC 2
#define AD5933 1
#define AD5933_MCLK 16.776

void Ini_I2c( void )      //初始化I2C
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_11 | GPIO_Pin_12); 	//PB10,PB11 输出高
    return;
}
void NOPS(void)
{
    delay_us(20);
}
void SDA_1( void )
{
    //    SDA_OUT();		//将SDA设置为输出模式
    SDA = 1;		//SDA管脚输出为高电平
    NOPS();
    return;
}

void SDA_0 ( void )
{
    //    SDA_OUT();		//将SDA设置为输出模式
    SDA = 0;	//SDA管脚输出为低电平
    NOPS();
    return;
}

void SCL_1( void )
{
    //将SCL设置为输出模式
    SCL = 1;		//SCL管脚输出为高电平
    NOPS();
    return;
}

void SCL_0 ( void )
{
    //将SCL设置为输出模式
    SCL = 0;	//SCL管脚输出为低电平
    NOPS();
    return;
}

void GetACK(void)
{
    u8 ucErrTime = 0;
    SDA_IN();      //SDA设置为输入
    SDA = 1;
    delay_us(Delay_IIC * 1);
    SCL = 1;
    delay_us(Delay_IIC * 1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            STOP();
            return;
        }
    }
    SCL = 0; //时钟输出0
}

void SendNACK(void)
{
    SCL = 0;
    SDA_OUT();
    SDA = 1;
    delay_us(Delay_IIC * 2);
    SCL = 1;
    delay_us(Delay_IIC * 2);
    SCL = 0;
}

void START(void)    // 启动数据总线
{
    SDA_OUT();     //sda线输出
    SDA = 1;
    delay_us(Delay_IIC * 2);
    SCL = 1;
    delay_us(Delay_IIC * 4);
    SDA = 0; //START:when CLK is high,DATA change form high to low
    delay_us(Delay_IIC * 4);
    SCL = 0; //钳住I2C总线，准备发送或接收数据
}

void STOP(void)
{
    SDA_OUT();//sda线输出
    SCL = 0;
    SDA = 0; //STOP:when CLK is high DATA change form low to high
    delay_us(Delay_IIC * 4);
    SCL = 1;
    SDA = 1; //发送I2C总线结束信号
    delay_us(Delay_IIC * 4);
}

void SendByte(u8 txd)	// 发送一个字节数据子函数
{
    u8 t;
	
    SDA_OUT();
    SCL = 0; //拉低时钟开始数据传输
    delay_us(Delay_IIC * 2);
    for(t = 0; t < 8; t++)
    {
        SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(Delay_IIC * 2); //对TEA5767这三个延时都是必须的
        SCL = 1;
        delay_us(Delay_IIC * 2);
        SCL = 0;
        delay_us(Delay_IIC * 2);
    }
}

u8 ReadByte(void)  //读一个字节数据
{
    unsigned char i, receive = 0;
	
    SDA_IN();//SDA设置为输入
    delay_us(Delay_IIC * 2);
    for(i = 0; i < 8; i++ )
    {
        SCL = 0;
        delay_us(Delay_IIC * 2);
        SCL = 1;
        receive <<= 1;
        if(READ_SDA)receive++;
        delay_us(Delay_IIC * 1);
    }
    SendNACK();
    return receive;
}

void Write_Byte(char nAddr, uint nValue) //nAddr中写入字节nValue
{
    int nTemp = 0x1A;      // AD5933的默认地址&写控制位（低）

    START();
    SendByte(nTemp);     // 发送地址
    GetACK();
    SendByte(nAddr);     // 发送地址字节
    GetACK();
    SendByte(nValue);	// 发送数据字节
    GetACK();
    STOP();	// 停止总线
    return;
}

void SetPointer(char nAddr)  //   设置地址指针
{
    int nTemp = 0x1A;      // AD5933的默认地址&写控制位（低）

    START();
    SendByte(nTemp);     // 发送地址
    GetACK();     // 等待 ACK
    SendByte(0xB0);     // 发送指针命令1101 0000
    GetACK();
    SendByte(nAddr);	// 发送地址指针
    GetACK();
    STOP();	// 停止总线
    return;
}

int Rece_Byte(char nAddr)//读取nAddr中的字节到返回值
{
    int nTemp ;

    SetPointer(nAddr);   //地址指针指向nAddr
    nTemp = 0x1B;    // AD5933的默认地址&读控制位（高）
    START();
    SendByte(nTemp);     // 发送地址
    GetACK();
    nTemp = ReadByte(); //读一个字节数据
    SendNACK();//发送NO_ACK
    STOP();	// 停止总线
    return nTemp;
}


u16 AD5933_Tempter(void)
{
    unsigned int Tm;   //温度


    //启动温度测量
    Write_Byte(0x80, 0x93);

    //读出温度，保存在Tm中
    Tm = Rece_Byte(0x92);
    Tm <<= 8;
    Tm += Rece_Byte(0x93);
    Tm <<= 2;

    return Tm;
}

#define kHz 1000
#define swpPNTCnt	500
#define stFreq 100*kHz
#define stepFreq 0 

float resistance[swpPNTCnt];
float rads[swpPNTCnt];
float AD5933_Dat_Re[swpPNTCnt];
float AD5933_Dat_Im[swpPNTCnt];


void Maopao_Paixu(float *dat, u16 leng)
{
    u16 i, j;
    float buf;
    for(j = 0; j < leng - 1; j++)
        for(i = 0; i < leng - j - 1; i++)
            if(dat[i] > dat[i + 1])
            {
                buf = dat[i];
                dat[i] = dat[i + 1];
                dat[i + 1] = buf;
            }
}


float Get_AVE(float src[], u16 num)
{
    u16 i, cnt = 0;
    float sum;
    Maopao_Paixu(src, num);

    sum = 0;
    //for(i = num / 2 - num / 4; i < num / 2 + num / 4; i++)
	  for(i = num / 10; i < num  - num / 10; i++)
    {
        sum += src[i];
        cnt++;
    }
    sum = sum / cnt;
    return sum;
}
void Fre_To_Hex(float fre, u8 *buf)
{
    u32 dat;
    dat = (536870912 / (double)(AD5933_MCLK * 1000000)) * fre;

    buf[0] = dat >> 16;
    buf[1] = dat >> 8;
    buf[2] = dat;

}

/*
函数名：	AD5933_Sweep
功  能：	频率扫描。按设定的频率起点，频率步进，步数及输出幅值，进行增量扫描或重复频率扫描，经内部gain增益后采集实部，虚部，换算阻抗值
输  入：		Fre_Begin 		开始频率；(范围：1k~100k Hz， 精度0.1Hz)
					Fre_UP				步进频率；(范围：1k~100k Hz， 精度0.1Hz)
					UP_Num				步进次数；(范围：0~511)
					OUTPUT_Vatage	输出电压；(范围：AD5933_OUTPUT_2V/AD5933_OUTPUT_1V/AD5933_OUTPUT_400mV/AD5933_OUTPUT_200mV)
					Gain： 				内部增益(范围：AD5933_Gain_1/AD5933_Gain_5)
					SWeep_Rep			扫描模式(范围：AD5933_Fre_UP:递增频率扫描；AD5933_Fre_Rep：重复频率扫描)
输  出：		R_Impedance：	电阻值；
					realData：		实部数据；
					imageData：		虚部数据；
注  释：无
*/
float AD5933_Sweep (float Fre_Begin, float Fre_UP, u16 UP_Num, u16 OUTPUT_Vatage, u16 Gain, u16 SWeep_Rep)
{
    u8 SValue[3], IValue[3], NValue[2], CValue[2];
    u16 buf = 0;
    Fre_To_Hex(Fre_Begin, SValue);
    Fre_To_Hex(Fre_UP, IValue); // 将开始频率、步进频率转换为16进制数，存在后面的变量名里
    NValue[0] = UP_Num >> 8;
    NValue[1] = UP_Num;
#if AD5933_MCLK_USE_OUT == 1
    buf = OUTPUT_Vatage | Gain | SWeep_Rep | AD5933_OUT_MCLK;
#else
    buf = OUTPUT_Vatage | Gain | SWeep_Rep | AD5933_IN_MCLK;
#endif
    CValue[0] = buf >> 8;
    CValue[1] = buf;

    Scale_imp(SValue, IValue, NValue, CValue);
    return 0;
}

/*SValue[3]起始频率，IValue[3]频率增量，NValue[2]增量数，CValue[2]控制字，ki增益系数，Ps扫频为1重复为0*/

long ReadTemp, realArr[3], imageArr[3];


float Scale_imp (u8 *SValue, u8 *IValue, u8 *NValue, u8 *CValue)
{
    static uint16_t repCnt = 0;
    int i, j, AddrTemp;
    u8 Gain = ((~CValue[0]) & 0x01) ? 5 : 1;
    u8 SWeep_Rep = ((CValue[0] & 0xF0) == (AD5933_Fre_UP >> 8)) ? 1 : 0;
    u8 Mode = CValue[0] & 0x0f;

    float magnitude;
    j = 0;
    Ini_I2c();    //初始化I2C

    AddrTemp = 0X82; //初始化 起始频率寄存器
    for(i = 0; i < 3; i++)
    {
        Write_Byte(AddrTemp, SValue[i]);
        AddrTemp++;
    }
    AddrTemp = 0X85; //初始化 频率增量寄存器
    for(i = 0; i < 3; i++)
    {
        Write_Byte(AddrTemp, IValue[i]);
        AddrTemp++;
    }
    AddrTemp = 0X88; //初始化 频率点数寄存器
    for(i = 0; i < 2; i++)
    {
        Write_Byte(AddrTemp, NValue[i]);
        AddrTemp++;
    }

    // Transmit to settling time cycles register (沉降时间周期数)
    // program 15 output cycles at each frequency before a adc conversion
    Write_Byte ( 0x8B, 15);
    Write_Byte ( 0x8A, 0x00);

    //初始化控制寄存器
    AddrTemp = 0X80;
    //                for(i = 0;i <2;i++)
    {
        Write_Byte(AddrTemp, Mode | (AD5933_Standby >> 8));
        AddrTemp++;
        Write_Byte(AddrTemp, CValue[1]);
        AddrTemp++;
    }

    Write_Byte(0x80, Mode | (AD5933_SYS_Init >> 8)); //控制寄存器写入初始化频率扫描命令
    delay_ms(10);
    Write_Byte(0X80, Mode | (AD5933_Begin_Fre_Scan >> 8)); //控制寄存器写入开始频率扫描命令

    repCnt = 0;
    while(1)
    {
        while(1)
        {
            ReadTemp = Rece_Byte(0x8F); //读取状态寄存器检查DFT是否完成
            if (ReadTemp & 0x02)
                break;
        }

        realArr[0] = Rece_Byte(0x94);
        realArr[1] = Rece_Byte(0x95);
        realArr[2] = realArr[0] * 0x100 + realArr[1];

        imageArr[0] = Rece_Byte(0x96);
        imageArr[1] = Rece_Byte(0x97);
        imageArr[2] = imageArr[0] * 0x100 + imageArr[1];


//        rads[j] = atan2((float)imageArr[2], (float)realArr[2]) - 0.00143485062;
			

//        if (realArr[2] >= 0x8000) //计算实部的原码(除符号位外，取反加一) 原数据以16位二进制补码存储在寄存器
//      {
//          realArr[2] ^= 0xFFFF;
//           realArr[2] ^= 0x8000;
//           realArr[2] += 1;
//            realArr[2] ^= 0x8000;
//       } 
//				
//       if (imageArr[2] >= 0x8000) //计算虚部的原码(除符号位外，取反加一)
//        {
//            imageArr[2] ^= 0xFFFF;
//          imageArr[2] ^= 0x8000;
//            imageArr[2] += 1;
//          imageArr[2] ^= 0x8000;
//        }

				if (realArr[2] >= 0x8000) //原数据以16位二进制补码存储在寄存器 ，0x8000代表负数，先对realArr做负数判断
      {
				realArr[2] |= 0xFFFF0000;  //这样可以确保 realArr[2] 的值在大于等于 0x8000 时，高位都为 1,保证正确输出负数（这是32位单片机）

       } 
				
       if (imageArr[2] >= 0x8000) 
        {
            imageArr[2] |= 0xFFFF0000;

        }
				
				rads[j] = atan2((float)imageArr[2], (float)realArr[2]) ;//- 0.00143485062
				
        AD5933_Dat_Re[j] =  realArr[2];			//将实部数据存储到数组AD5933_Dat_Re
        AD5933_Dat_Im[j] =  imageArr[2];			//将虚部数据存储到数组AD5933_Dat_Im
        magnitude = sqrt((float)(realArr[2] * realArr[2] + imageArr[2] * imageArr[2])); //模值计算
        resistance[j++] = 1 / (magnitude * Gain);		 //阻抗计算

        ReadTemp = Rece_Byte(0x8F); //读取状态寄存器检查频率扫描是否完成
        if (ReadTemp & 0x04)
            break;
        if (SWeep_Rep == 1)		//频率递增扫描模式
        {
            Write_Byte(0X80, CValue[0]);	//控制寄存器写入增加频率（跳到下一个频率点)的命令
        }
        else		//频率重复模式
        {
            repCnt++;
            if(repCnt >= *NValue)
            {
                break;
            }
            Write_Byte(0X80, CValue[0]);	//控制寄存器写入重复当前频率点扫描
        }
    }
    Write_Byte(0X80, 0XA1);	//进入掉电模式
    return magnitude;
}


/*
函数名：	DA5933_Get_RsRealImage
功  能：	获得所夹持电阻的阻值，及测得的实部数据及虚部数据
输  入：无
输  出：R_Impedance：电阻值；	realData：实部数据；	imageData：虚部数据；
注  释：无
*/
float DA5933_Get_RsRealImage(float *R_Impedance, float *R_Rad, float *realData, float *imageData)
{
    float RsWord, ReWord, ImWord,RadWord;

		//从频率10240Hz开始扫描，1Hz步进，扫描swpPNTCnt步，输出幅度2V，增益1,扫描模式
    AD5933_Sweep(stFreq-swpPNTCnt*stepFreq, stepFreq, swpPNTCnt, AD5933_OUTPUT_2V, AD5933_Gain_1, AD5933_Fre_UP);//(范围：AD5933_Fre_UP:递增频率扫描；AD5933_Fre_Rep：重复频率扫描)
	
    RsWord = Get_AVE(resistance,    swpPNTCnt);	//阻抗数据的结果都存放在数组resistance， 从该数组求一个均值
    ReWord = Get_AVE(AD5933_Dat_Re, swpPNTCnt);	//实部数据的结果都存放在数AD5933_Dat_Re，从该数组求一个均值
    ImWord = Get_AVE(AD5933_Dat_Im, swpPNTCnt);	//虚部数据的结果都存放在数AD5933_Dat_Im，从该数组求一个均值
		RadWord= Get_AVE(rads, swpPNTCnt);

		//注：
		//AD5933测量值时，模值数据与电阻值具有一定的非线性，校准方法较多，一般比较简单的可采用多线段校正(也可用其他方法)
		//RsWord即为所测得阻抗模值，用户可利用该数据进行多线段校正，如分别记录5k，10k，50k，100k，500k时RsWord的值，
		//后续测得RsWord值时，找到RsWord值在哪个区间及其位置，再将值线性对应到该位置对应的阻值
		//下述例程为仅以10k标准电阻校准的简单校准方法，用户需更大范围或更准确数值时，可参考上述或其他方法
	
    *R_Impedance = RsWord * AD5933_Correction;	//阻抗数据乘以一个校准因子，得到电阻值(fudu)
		*R_Rad = RadWord - AD5933_CorrectionPhase;	//阻抗数据乘以一个校准因子，得到电阻值 //hr
    *realData = ReWord;
    *imageData = ImWord;


    return *R_Impedance;
}







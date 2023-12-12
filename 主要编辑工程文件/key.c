

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "key.h"
#include "stdio.h"
#include "delay.h"

uint8_t KeyNum = 0;

void Key_Init(void)
{
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 |GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

uint8_t Key_GetNum(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
		delay_ms(20);
		KeyNum = 1;
	}
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
		delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);
		delay_ms(20);
		KeyNum = 2;
	}
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
	{
		delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);
		delay_ms(20);
		KeyNum = 3;
	}
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
	{
		delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
		delay_ms(20);
		KeyNum = 4;
	}
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
	{
		delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);
		delay_ms(20);
		KeyNum = 5;
	}
	
	
	return KeyNum;

}

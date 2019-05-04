#include "stm32f10x.h"  
#include "./led/bsp_led.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./Key/bsp_exti.h"
#include "./Key/bsp_key.h"
#include "./SysTick/bsp_SysTick.h"
#include "./engine/engine.h"
#include "./tim/tim.h"
#include <stdio.h>
#include <stdlib.h>

extern struct Bird bird;
extern struct Game game;
extern char display_buff[100];  //计算分数
uint64_t time = 0; //鸟下坠计时器
uint64_t time2 = 0; //柱子移动计时器



//处理按键中断；按键按下，小鸟上升
void KEY1_IRQHandler(void)
{
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{   
		//更新小鸟的高度
		bird.new_height = bird.height - BIRD_UP;
		draw_bird(); //在LCD上绘制新的小鸟
		judge(); //判断小鸟是否死亡
		//清空中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}
//处理TIM6计时器中断；中断达到一定次数，柱子移动一段距离
void BASIC_TIM_IRQHandler(void){
	if (TIM_GetITStatus(BASIC_TIM, TIM_IT_Update) != RESET){
		time++;
		TIM_ClearITPendingBit(BASIC_TIM, TIM_FLAG_Update);
	}
	if(time == PILLAR_REDRAW_TIME_MS){
		time = 0;
		ILI9341_DrawRectangle(0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
		//柱子移动一段距离
		draw_pillar();
		judge(); //判断小鸟是否死亡
	}
}
//处理TIM7计时器中断，中断达到一定次数，小鸟下坠一段距离
void BASIC_TIM_IRQHandler_2(void){
	if (TIM_GetITStatus(BASIC_TIM_2, TIM_IT_Update) != RESET){
		time2++;
		TIM_ClearITPendingBit(BASIC_TIM_2, TIM_FLAG_Update);
	}
	if(time2 == BIRD_DOWN_TIME_MS){
		time2 = 0;
		bird.new_height = bird.height + BIRD_DOWN;
		draw_bird();
		judge(); //判断小鸟是否死亡
	}
}


int main(void){
	//一系列初始化操作
	ILI9341_GramScan(6);
	ILI9341_Init();
	LED_GPIO_Config();
	Key_GPIO_Config();
	SysTick_Init();
	//设置字体大小
	LCD_SetFont(&Font8x16);
	//设置屏幕物件颜色、背景颜色
	LCD_SetColors(RED,BLACK);
	//EXTI_Key_Config();
	ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
	
	ILI9341_DrawRectangle(0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0); 
	game_play();

}




#include "./engine/engine.h"
#include "./led/bsp_led.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./Key/bsp_exti.h"
#include "./Key/bsp_key.h"
#include "./SysTick/bsp_SysTick.h"
#include "stm32f10x.h"
#include "./tim/tim.h"  
#include <stdio.h>
#include <stdlib.h>

struct Bird bird;
struct Game game;
char display_buff[100];  //输出文字缓存

/*画鸟，先用黑色填充原区域，再利用新坐标参数画鸟
*/
void draw_bird(void){
	LCD_SetTextColor(BLACK);
	ILI9341_DrawRectangle(BIRD_X_AXIS - bird.radious, bird.height - bird.radious, 
				          2 * bird.radious + 2, 2 * bird.radious + 2, 1);
    LCD_SetTextColor(BLUE2);
	bird.height = bird.new_height;
	ILI9341_DrawCircle(BIRD_X_AXIS, bird.height, bird.radious, 1);
	LCD_SetTextColor(RED);
}

//把旧柱子消除 -> 计算柱子移动情况 -> 画新柱子
void draw_pillar(void){
	uint16_t i;
	
    LCD_SetTextColor(BLACK);
	//最多智能显示三个柱子
	for (i = game.pillar_pointer; i < game.pillar_pointer + 3; i++){
		ILI9341_DrawRectangle(game.pillar_list[i].longitude, 1, 
					          PILLAR_WIDTH, game.pillar_list[i].length, 1);
		ILI9341_DrawRectangle(game.pillar_list[i].longitude, 
					          game.pillar_list[i].length + PILLAR_OPEN_SIZE,
					          PILLAR_WIDTH, 
					          BACKGROUND_HEIGHT - PILLAR_OPEN_SIZE - game.pillar_list[i].length - 1, 1);
	}
    LCD_SetTextColor(RED);
	//移柱子
	pillar_move();

	for (i = game.pillar_pointer; i < game.pillar_pointer + 3; i++){
		if(game.pillar_list[i].longitude <= BACKGROUND_WIDTH){		
			ILI9341_DrawRectangle(game.pillar_list[i].longitude, 1,
							  	PILLAR_WIDTH, game.pillar_list[i].length, 1);
			ILI9341_DrawRectangle(game.pillar_list[i].longitude,
							  	game.pillar_list[i].length + PILLAR_OPEN_SIZE,
							  	PILLAR_WIDTH, 
							  	BACKGROUND_HEIGHT - PILLAR_OPEN_SIZE - game.pillar_list[i].length - 1, 1);
		}
	}
}

/*计算柱子的位置以及移动
在第1个柱子移出屏幕后生成新柱子 */
void pillar_move(void){
	uint16_t i;
	//计算所有柱子的新开始做表	
	for (i = game.pillar_pointer; i < 100; i++){
		game.pillar_list[i].longitude -= PILLAR_STEP_LENGTH;
	}	
	//pillar_pointer指向会出现在屏幕上的第一个柱子，
	//若某柱子超出屏幕，pillar_pointer指向下一个柱子
	if (game.pillar_list[game.pillar_pointer].longitude + PILLAR_WIDTH <= 0){
		game.pillar_pointer++;
	} 
}



void game_over(void){
	if(bird.life == 0){	
		Delay(10000000);
		ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
		LCD_SetFont(&Font24x32);
		LCD_SetColors(WHITE,BLACK);
		ILI9341_DispStringLine_EN(LINE(0),"          GAME OVER!");
		Delay(2000000);
		ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
		ILI9341_DispStringLine_EN(LINE(1),"          GAME OVER!");
		Delay(2000000);
		ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
		ILI9341_DispStringLine_EN(LINE(2),"          GAME OVER!");
		Delay(2000000);
		ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
		ILI9341_DispStringLine_EN(LINE(3),"          GAME OVER!");
		LED_GPIO_Config();	 //调用4个LED灯初始化函数
 
  	while(1)
  	{	
		GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);
		Delay(800000); //延时
		GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);
		Delay(800000); //延时
		GPIO_ResetBits(LED2_GPIO_PORT,LED2_GPIO_PIN); 
		Delay(800000); //延时
		GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN); 
		Delay(800000); //延时
		GPIO_ResetBits(LED3_GPIO_PORT,LED3_GPIO_PIN); 
		Delay(800000); //延时
		GPIO_SetBits(LED3_GPIO_PORT,LED3_GPIO_PIN); 
		Delay(800000); //延时
  	}
		}
	}

//死亡逻辑
void judge(void){
	uint16_t i;
	//情况1：超出边界
	if((bird.height + bird.radious >= BACKGROUND_HEIGHT) || (bird.height - bird.radious <= 0)){
		bird.life = 0;
		//死亡情况处理
		game_over();
	}

    //情况2：碰到柱子
	for (i = game.pillar_pointer; i < game.pillar_pointer + 2; i++){
		//鸟在碰撞横坐标范围内
		if((BIRD_X_AXIS + bird.radious > game.pillar_list[i].longitude) &&
		  (BIRD_X_AXIS - bird.radious < game.pillar_list[i].longitude + PILLAR_WIDTH)){
			  //鸟在碰撞纵坐标范围内
			  if((bird.height - bird.radious <= game.pillar_list[i].length) || 
			    (bird.height + bird.radious >= game.pillar_list[i].length + PILLAR_OPEN_SIZE)){
					bird.life = 0;
					game_over();
				}
		  }
	}
}

//描述游戏过程
void game_play(void){
	uint16_t i;
	
	//一系列初始化
	bird.height = BIRD_INITIAL_HEIGHT;
	bird.new_height = BIRD_INITIAL_HEIGHT;
	bird.radious = BIRD_RADIOUS;
	bird.life = 1;
	game.score = 0;
	game.pillar_pointer = 0;
	//初始化柱子
	for (i = 0; i < 100; i++){
		game.pillar_list[i].longitude = BACKGROUND_WIDTH + i * PILLAR_DISTANCE;
		game.pillar_list[i].length = rand() % (BACKGROUND_HEIGHT - PILLAR_OPEN_SIZE - 10);
	}
		draw_bird();
		draw_pillar();
		Delay(0XFFFFF);
		EXTI_Key_Config();
    BASIC_TIM_Init();
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);


	while(bird.life == 1){}
		
}

//延时nCount个时钟周期
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );	
}
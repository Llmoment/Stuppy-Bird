#include "stm32f10x.h"

void draw_bird(void);
void draw_pillar(void);
void pillar_move(void);
void game_over(void);
void game_play(void);
void judge(void);
static void Delay(__IO uint32_t nCount);

#define BIRD_RADIOUS        10 //鸟的半径
#define BIRD_X_AXIS         120 //鸟横坐标
#define BIRD_INITIAL_HEIGHT 135 //鸟的初始化高度
#define BIRD_DOWN           15  //一次下降高度
#define BIRD_UP             25  //一次上升高度
#define BIRD_DOWN_TIME_MS   150 //鸟下坠一次的时间间隔，时间到后引发中断

#define BACKGROUND_WIDTH    240 //背景宽度
#define BACKGROUND_HEIGHT   280 //背景高度

#define PILLAR_WIDTH        35 //柱子的宽度
#define PILLAR_STEP_LENGTH  10 //柱子每次移动的像素数
#define PILLAR_DISTANCE     150 //两柱子之间的距离
#define PILLAR_OPEN_SIZE    90  //柱子上下之间的开口大小
#define PILLAR_REDRAW_TIME_MS  110 // 重新画柱子的时间间隔,时间到后引发中断


struct Bird{
	uint16_t height; //鸟的纵坐标
	uint16_t new_height; //记录改变高度后的新纵坐标
	uint8_t life; // 1: alive 0:die
	uint16_t radious;
};

//只需要设置每一柱子对中位于上方的柱子的y轴位置
//预设一间隔，可计算出下方柱子的参数
struct Pillar{
	int longitude; //柱子横坐标
	uint16_t length;    //上方柱子长度
};

//暂定同一屏幕上只会出现三根柱子
struct Game{
	uint16_t score;
	struct Pillar pillar_list[100];
	//指示当前柱子缓存中能够打印在屏幕上的第一根柱子  
	uint16_t pillar_pointer;
};
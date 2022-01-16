#pragma once

#ifndef LOVELYBOY
#define LOVELYBOY
#include"../include/cmd_gmw_tools.h"
#endif

/*边框大小与方格大小*/
#define square_width 6
#define square_height 3
#define score_wide 36
#define default_x 900

/*是否允许动,FAILED代表超出范围，游戏结束*/
#define ALLOWED 1
#define FORBIDDEN 0

/*是否溢出*/
#define T_FAILED 901
#define T_ACCEPT 902
#define T_QUIT 903


/*数字的四个方向*/
#define DIRECTION_UP 0
#define DIRECTION_LEFT 1
#define DIRECTION_DOWN 2
#define DIRECTION_RIGHT 3
#define ALL_DIRECTION 4

/*数字形状*/
const int arr_of_charaters[10][4][5][5] =
{
	/*0四个方向*/
	{
		{0,1,1,1,0,
		0,1,0,1,0,
		0,1,0,1,0,
		0,1,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,0,0,1,
		1,1,1,1,1,
		0,0,0,0,0},

		{0,1,1,1,0,
		0,1,0,1,0,
		0,1,0,1,0,
		0,1,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,0,0,1,
		1,1,1,1,1,
		0,0,0,0,0}},
		/*1四个方向*/
		{
				{0,0,1,0,0,
				0,0,1,0,0,
				0,0,1,0,0,
				0,0,1,0,0,
				0,0,1,0,0},

				{0,0,0,0,0,
				0,0,0,0,0,
				1,1,1,1,1,
				0,0,0,0,0,
				0,0,0,0,0},

				{0,0,1,0,0,
				0,0,1,0,0,
				0,0,1,0,0,
				0,0,1,0,0,
				0,0,1,0,0},

				{0,0,0,0,0,
				0,0,0,0,0,
				1,1,1,1,1,
				0,0,0,0,0,
				0,0,0,0,0}
			},
	/*2四个方向*/
	{
		{0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,0,1,
		1,0,1,0,1,
		1,0,1,1,1,
		0,0,0,0,0},

		{0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,0,1,
		1,0,1,0,1,
		1,0,1,1,1,
		0,0,0,0,0}
	},
	/*3四方向*/
	{
		{0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,1,0,1,
		1,0,1,0,1,
		0,0,0,0,0},

		{0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,0,1,0,1,
		1,0,1,0,1,
		1,1,1,1,1,
		0,0,0,0,0}

	},
	/*4四个方向*/
	{
		{0,1,0,1,0,
		0,1,0,1,0,
		0,1,1,1,0,
		0,0,0,1,0,
		0,0,0,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		0,0,1,0,0,
		1,1,1,0,0,
		0,0,0,0,0},

		{0,1,0,0,0,
		0,1,0,0,0,
		0,1,1,1,0,
		0,1,0,1,0,
		0,1,0,1,0},

		{0,0,0,0,0,
		0,0,1,1,1,
		0,0,1,0,0,
		1,1,1,1,1,
		0,0,0,0,0}
	},
	/*5四个方向*/
	{
		{0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,0,1,1,1,
		1,0,1,0,1,
		1,1,1,0,1,
		0,0,0,0,0},

		{0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,0,1,1,1,
		1,0,1,0,1,
		1,1,1,0,1,
		0,0,0,0,0}
	},
	/*6四个方向*/
	{
		{0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,0,1,1,1,
		1,0,1,0,1,
		1,1,1,1,1,
		0,0,0,0,0},

		{0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,1,0,1,
		1,1,1,0,1,
		0,0,0,0,0},
	},
	/*7*/
	{
		{0,1,1,1,0,
		0,0,0,1,0,
		0,0,0,1,0,
		0,0,0,1,0,
		0,0,0,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,0,0,0,
		1,0,0,0,0,
		0,0,0,0,0},

		{0,1,0,0,0,
		0,1,0,0,0,
		0,1,0,0,0,
		0,1,0,0,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		0,0,0,0,1,
		0,0,0,0,1,
		1,1,1,1,1,
		0,0,0,0,0}
	},
	/*8*/
	{
		{0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,1,0,1,
		1,1,1,1,1,
		0,0,0,0,0},

		{0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,1,0,1,
		1,1,1,1,1,
		0,0,0,0,0}
	},
	/*9*/
	{
		{0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0,
		0,0,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,1,1,1,1,
		1,0,1,0,1,
		1,1,1,0,1,
		0,0,0,0,0},

		{0,1,1,1,0,
		0,1,0,0,0,
		0,1,1,1,0,
		0,1,0,1,0,
		0,1,1,1,0},

		{0,0,0,0,0,
		1,0,1,1,1,
		1,0,1,0,1,
		1,1,1,1,1,
		0,0,0,0,0}
	},
};

struct falling {
	int num;
	int direction;
};


/*偏重于画图的函数*/
void FrameSize(int& M, int& N);
void paint_present_char(const CONSOLE_GRAPHICS_INFO* const pCGI, const int present_char[][5], int arr_x, int arr_y, const char usage[]);

/*偏重于处理内部数组的函数*/
int check_movement(int to_be_char[5][5], int M, int N, int arr_x, int arr_y, int backgd[30][30]);
void update_backgd(const int present_char[5][5], int backgd[30][30], int arr_x, int arr_y, int M, int N);
void check_fall(CONSOLE_GRAPHICS_INFO* pCGI, int present_char[5][5], int M, int N, int arr_x, int& arr_y, int backgd[30][30], int flag);
int control_fall(CONSOLE_GRAPHICS_INFO* pCGI, int present_char[5][5], int backgd[30][30], int M, int N, falling& block, int cmd, int pause);
void paint_all_square(const CONSOLE_GRAPHICS_INFO* const pCGI, int backgd[30][30], int M, int N, const char* usage);
void bring_down_once(int backgd[30][30], int M, int N, int row);
int diminish(CONSOLE_GRAPHICS_INFO* pCGI, int backgd[30][30], int M, int N, int& times);

/*对于完整游戏的功能小函数90-02-b1-tetris_tools当中*/
int get_next_num(const bool new_seed = false, const unsigned int seed = 0);
void get_present_char(int present_char[5][5], int num, int direction);
void show_score(CONSOLE_GRAPHICS_INFO* pCGI, int score, int all_times, int M, int fin);
int get_pause(int all_score);

/*显示下一个数字，因为要在框外画图用到cct,在next_char当中*/
void show_next_char(CONSOLE_GRAPHICS_INFO* pCGI, falling block);
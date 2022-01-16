#pragma once

#ifndef LOVELYBOY
#define LOVELYBOY
#include"../include/cmd_gmw_tools.h"
#endif

/*�߿��С�뷽���С*/
#define square_width 6
#define square_height 3
#define score_wide 36
#define default_x 900

/*�Ƿ�����,FAILED��������Χ����Ϸ����*/
#define ALLOWED 1
#define FORBIDDEN 0

/*�Ƿ����*/
#define T_FAILED 901
#define T_ACCEPT 902
#define T_QUIT 903


/*���ֵ��ĸ�����*/
#define DIRECTION_UP 0
#define DIRECTION_LEFT 1
#define DIRECTION_DOWN 2
#define DIRECTION_RIGHT 3
#define ALL_DIRECTION 4

/*������״*/
const int arr_of_charaters[10][4][5][5] =
{
	/*0�ĸ�����*/
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
		/*1�ĸ�����*/
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
	/*2�ĸ�����*/
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
	/*3�ķ���*/
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
	/*4�ĸ�����*/
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
	/*5�ĸ�����*/
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
	/*6�ĸ�����*/
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


/*ƫ���ڻ�ͼ�ĺ���*/
void FrameSize(int& M, int& N);
void paint_present_char(const CONSOLE_GRAPHICS_INFO* const pCGI, const int present_char[][5], int arr_x, int arr_y, const char usage[]);

/*ƫ���ڴ����ڲ�����ĺ���*/
int check_movement(int to_be_char[5][5], int M, int N, int arr_x, int arr_y, int backgd[30][30]);
void update_backgd(const int present_char[5][5], int backgd[30][30], int arr_x, int arr_y, int M, int N);
void check_fall(CONSOLE_GRAPHICS_INFO* pCGI, int present_char[5][5], int M, int N, int arr_x, int& arr_y, int backgd[30][30], int flag);
int control_fall(CONSOLE_GRAPHICS_INFO* pCGI, int present_char[5][5], int backgd[30][30], int M, int N, falling& block, int cmd, int pause);
void paint_all_square(const CONSOLE_GRAPHICS_INFO* const pCGI, int backgd[30][30], int M, int N, const char* usage);
void bring_down_once(int backgd[30][30], int M, int N, int row);
int diminish(CONSOLE_GRAPHICS_INFO* pCGI, int backgd[30][30], int M, int N, int& times);

/*����������Ϸ�Ĺ���С����90-02-b1-tetris_tools����*/
int get_next_num(const bool new_seed = false, const unsigned int seed = 0);
void get_present_char(int present_char[5][5], int num, int direction);
void show_score(CONSOLE_GRAPHICS_INFO* pCGI, int score, int all_times, int M, int fin);
int get_pause(int all_score);

/*��ʾ��һ�����֣���ΪҪ�ڿ��⻭ͼ�õ�cct,��next_char����*/
void show_next_char(CONSOLE_GRAPHICS_INFO* pCGI, falling block);
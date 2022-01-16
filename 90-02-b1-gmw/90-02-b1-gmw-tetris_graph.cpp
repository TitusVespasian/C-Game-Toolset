#include<iostream>
#include<cstring>
#include<windows.h>
#include<conio.h>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include "../include/cmd_console_tools.h"
#include"90-02-b1-gmw-tetris.h"
#include"../include/cmd_input.h"
#include"../include/cmd_gmw_tools.h"
#endif
using namespace std;

const BLOCK_DISPLAY_INFO bdi_tetris[]{
	{1, COLOR_GREEN,COLOR_BLACK, "★"},  //0不显示，用空格填充即可
	{BDI_VALUE_BLANK,COLOR_HWHITE,COLOR_HWHITE,"  "},//0相当于BDI_VALUE_BLANK
	{BDI_VALUE_END, -1, -1, NULL}
};


/*
* 输入界面大小
*/
void FrameSize(int& M, int& N)
{
	cct_setcolor();
	cct_cls();
	cct_setfontsize("新宋体", 28);
	cct_setconsoleborder(80, 30);
	while (1)
	{
		cout << "请输入宽度M([12..21]且为3的倍数)" << endl;
		cin >> M;
		if (M <= 21 && M >= 12 && M % 3 == 0)
			break;
	}
	while (1)
	{
		cout << "请输入高度N([18..26])" << endl;
		cin >> N;
		if (N <= 26 && N >= 18)
			break;
	}
	cct_cls();
}

/*
* 输出或擦除一个数字
* @param present_char 当前数字形状的数组
* @param arr_x,arr_y 整个数字左上角的横纵坐标，一个数字竖着36*90
*/
void paint_present_char(const CONSOLE_GRAPHICS_INFO* const pCGI, const int present_char[][5], int arr_x, int arr_y, const char usage[])
{
	for (int j = 0; j < 5; j++)
		for (int i = 0; i < 5; i++)
		{
			if (present_char[j][i] != 0 && strcmp(usage, "paint") == 0 && j + arr_y >= 0)
			{
				gmw_draw_block(pCGI, j + arr_y, i + arr_x, present_char[j][i], bdi_tetris);
			}
			else if (present_char[j][i] != 0 && strcmp(usage, "erase") == 0 && j + arr_y >= 0)
			{
				gmw_draw_block(pCGI, j + arr_y, i + arr_x, BDI_VALUE_BLANK, bdi_tetris);
			}
		}
}

/*返回1允许动，返回0禁止动,两种情况不能转，一种有元素了，一种到边界了
* 输入的是动之后的数组
* @param to_be_char 旋转完之后的形状
* @param M 宽度 0~M-1
* @param N 高度 0~N-1
* @param arr_x 背景数组中所在位置(左上角的元素可能小于0
* @param arr_y 背景数组中所在位置
* @param backgd 背景数组
*/
int check_movement(int to_be_char[5][5], int M, int N, int arr_x, int arr_y, int backgd[30][30])
{
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 5; i++)
		{
			/*要先检查arr_y + j，arr_x + i防止越界*/
			if (to_be_char[j][i] == 1)
			{
				if (arr_x + i > M - 1 || arr_x + i < 0 || arr_y + j > N - 1)//处理左右下边界
					return FORBIDDEN;
				else if (arr_y + j >= 0)
				{
					if (backgd[arr_y + j][arr_x + i] == 1)//处理被格子挡住
						return FORBIDDEN;
				}
			}
		}
	}
	return ALLOWED;
}

/*
* 改变背景数组
* 如果arr_x+4>M-1那就不行,防止越界
* @param present_char
* @param backgd
* @param M 宽度
* @param N 高度
*/
void update_backgd(const int present_char[5][5], int backgd[30][30], int arr_x, int arr_y, int M, int N)
{
	for (int j = 0; j < 5; j++)
		for (int i = 0; i < 5; i++)
		{
			if (present_char[j][i] != 0 && arr_y + j >= 0 && arr_x + i >= 0 && arr_y + j <= N - 1 && arr_x + i <= M - 1)
				backgd[arr_y + j][arr_x + i] = present_char[j][i];
		}
}

/*
* 检查下落
* @param present_char
* @param M 宽度
* @param N 高度
* @param arr_x,arr_y 后者引用
* @param backgd
* @param flag 数字的状态
* @param pause 停顿时间
* @param status 是按向下键还是自动下落
*/
void check_fall(CONSOLE_GRAPHICS_INFO* pCGI, int present_char[5][5], int M, int N, int arr_x, int& arr_y, int backgd[30][30], int flag)
{
	if (flag == ALLOWED)
	{
		paint_present_char(pCGI, present_char, arr_x, arr_y, "erase");
		++arr_y;
		paint_present_char(pCGI, present_char, arr_x, arr_y, "paint");
	}
	else if (flag == FORBIDDEN)
	{
		update_backgd(present_char, backgd, arr_x, arr_y, M, N);
	}
}

/*光标控制一个数字下落，
* 先检查能否操作，再改变数字形状，
再进行操作改变背景颜色，然后改变数组状况(如果到底了的话）
* @param present_char 数字的形状
* @param num 某个数字
* @param pause 一层下落的时间，缺省为1
* @param backgd 背景数组，可能用到
* @param M 宽度
* @param N 高度
* @return T_FAILED 溢出
* @return T_ACCEPT 继续堆
*/
int control_fall(CONSOLE_GRAPHICS_INFO* pCGI, int present_char[5][5], int backgd[30][30], int M, int N, falling& block, int cmd, int pause)
{
	int arr_x = M / 2 - square_width / 2;
	int arr_y = -5;
	//输入的数字都是向上的
	clock_t start = clock();
	while (1)
	{
		while (_kbhit())
		{
			int Keycode1 = 0, Keycode2 = 0, MRow, MCol, MAction;
			gmw_read_keyboard_and_mouse(pCGI, MAction, MRow, MCol, Keycode1, Keycode2);
			if (Keycode1 == 224)
			{
				/*向上键*/
				if (Keycode2 == KB_ARROW_UP)
				{
					int temp[5][5];
					get_present_char(temp, block.num, (block.direction + 1) % ALL_DIRECTION);
					int flag = check_movement(temp, M, N, arr_x, arr_y, backgd);
					if (flag == ALLOWED)
					{
						paint_present_char(pCGI, present_char, arr_x, arr_y, "erase");
						block.direction = (block.direction + 1) % ALL_DIRECTION;
						get_present_char(present_char, block.num, block.direction);
						paint_present_char(pCGI, present_char, arr_x, arr_y, "paint");
					}
				}
				/*向左键*/
				else if (Keycode2 == KB_ARROW_LEFT)
				{
					int flag = check_movement(present_char, M, N, arr_x - 1, arr_y, backgd);
					if (flag == ALLOWED)
					{
						paint_present_char(pCGI, present_char, arr_x, arr_y, "erase");
						--arr_x;
						paint_present_char(pCGI, present_char, arr_x, arr_y, "paint");
					}
				}
				/*向右键*/
				else if (Keycode2 == KB_ARROW_RIGHT)
				{
					int flag = check_movement(present_char, M, N, arr_x + 1, arr_y, backgd);
					if (flag == ALLOWED)
					{
						paint_present_char(pCGI, present_char, arr_x, arr_y, "erase");
						++arr_x;
						paint_present_char(pCGI, present_char, arr_x, arr_y, "paint");
					}
				}
				/*向下键*/
				else if (Keycode2 == KB_ARROW_DOWN)
				{
					int flag = check_movement(present_char, M, N, arr_x, arr_y + 1, backgd);
					check_fall(pCGI, present_char, M, N, arr_x, arr_y, backgd, flag);
				}
			}
			else if (Keycode1 == 'p')
				while (_getch() != 'p')
					;
			else if (Keycode1 == 'q')
				return T_QUIT;
		}
		/*自由下落，先检查能否操作，再进行操作改变背景颜色，然后改变数组状况(如果到底了的话）*/

		if (clock() - start >= clock_t(pause))
		{
			int flag = check_movement(present_char, M, N, arr_x, arr_y + 1, backgd);
			check_fall(pCGI, present_char, M, N, arr_x, arr_y, backgd, flag);
			start = clock();
			if (flag == FORBIDDEN)
			{
				if (arr_y <= 0)//检查是否溢出
					return T_FAILED;
				else
					return T_ACCEPT;
			}
		}
	}
	return T_ACCEPT;
}
/*
* 擦除或绘制所有背景数组为1的方块
* @param backgd 背景数组
*/
void paint_all_square(const CONSOLE_GRAPHICS_INFO* const pCGI, int backgd[30][30], int M, int N, const char* usage)
{
	for (int j = 0; j < N; j++)
	{
		for (int i = 0; i < M; i++)
		{
			if (backgd[j][i] == 1)
			{
				if (strcmp(usage, "erase") == 0)
					gmw_draw_block(pCGI, j, i, BDI_VALUE_BLANK, bdi_tetris);
				else if (strcmp(usage, "paint") == 0)
					gmw_draw_block(pCGI, j, i, backgd[j][i], bdi_tetris);
			}
		}
	}
}

/*改变内部数组
* @param row 消除的行号
*/
void bring_down_once(int backgd[30][30], int M, int N, int row)
{
	int temp[30] = {};
	for (int j = row; j > 0; j--)
		for (int i = 0; i < M; i++)
		{
			backgd[j][i] = backgd[j - 1][i];
		}
	for (int i = 0; i < M; i++)
		backgd[0][i] = 0;
}

/*
* 查看是否需要消去一行
* @param M 宽度
* @param N 高度
* @return 分数
*/
int diminish(CONSOLE_GRAPHICS_INFO* pCGI, int backgd[30][30], int M, int N, int& times)
{
	int score = 0;
	for (int j = 0; j < N; j++)
	{
		int flags = 0;
		for (int i = 0; i < M; i++)
		{
			if (backgd[j][i] == 1)
				++flags;
		}
		if (flags == M)
		{
			paint_all_square(pCGI, backgd, M, N, "erase");
			bring_down_once(backgd, M, N, j);
			paint_all_square(pCGI, backgd, M, N, "paint");
			++times;
		}
	}
	switch (times)
	{
	case 1:
		score = 1;
		break;
	case 2:
		score = 3;
		break;
	case 3:
		score = 6;
		break;
	case 4:
		score = 10;
		break;
	case 5:
		score = 15;
		break;
	}
	return score;
}
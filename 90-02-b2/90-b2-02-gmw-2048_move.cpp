#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<fstream>
#include<cstring>
#include<windows.h>
#include<conio.h>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#include"../include/cmd_gmw_tools.h"
#include"90-b2-02-gmw-2048.h"
#endif
using namespace std;

/***************************************************************************
  函数名称：
  功    能：初始化游戏界面
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void intit_play_frame(CONSOLE_GRAPHICS_INFO* pCGI, board_param param_2048)
{
	gmw_init(pCGI, param_2048.rows, param_2048.cols);
	pCGI->lines -= 3;
	gmw_set_color(pCGI, COLOR_BLACK, COLOR_HWHITE);			//整个窗口颜色
	gmw_set_font(pCGI, "新宋体", 16);						//字体
	gmw_set_frame_style(pCGI, 12, 5, true);					//游戏主区域风格：每个色块宽10高5，有分隔线【数字色块带边框，宽度为10(放最多6位数字)，高度为5(为了保持色块为方形)】
	gmw_set_frame_color(pCGI, COLOR_WHITE, COLOR_BLACK);	//游戏主区域颜色
	gmw_set_block_border_switch(pCGI, true);				//小色块带边框
	gmw_set_status_line_switch(pCGI, LOWER_STATUS_LINE, false);
	gmw_set_status_line_color(pCGI, TOP_STATUS_LINE, COLOR_HWHITE, COLOR_BLACK);
	pCGI->delay_of_block_moved = param_2048.delay * 5;//用gmw设置最小是15ms，所以直接设
	gmw_draw_frame(pCGI);                                 // 画出来
}

/*从这里开始，每个函数都有两个功能，一个改变内部数组，一个画图*/

/***************************************************************************
  函数名称：
  功    能：产生一个2或4的色块
  输入参数：bool first_on 判断是否是第一次（只产生2）
  返 回 值：返回代表是否成功，一般都会成功
  说    明：
***************************************************************************/
int random_pos(CONSOLE_GRAPHICS_INFO* pCGI, board_param param_2048, const BLOCK_DISPLAY_INFO bdi_2048[], int backgd[12][12], bool first_on)
{
	srand((unsigned)time(NULL));
	int rand_row = 0;
	int rand_col = 0;
	int four_or_two = 0;
	bool is_full = true;

	/*这一段正常执行不到，但是为了防止写错后面死循环*/
	for (int i = 0; i < param_2048.cols; i++)
		for (int j = 0; j < param_2048.rows; j++)
			if (backgd[j][i] == 0)
			{
				is_full = false;
				break;
			}
	if (is_full == true)
	{
		cout << "错了！" << endl;
		return -1;
	}
	/*这一段正常执行不到，但是为了防止写错后面死循环*/

	/*改变内部数组*/
	while (1)
	{
		rand_row = rand() % param_2048.rows;
		rand_col = rand() % param_2048.cols;
		int temp = rand() % 2;
		if (temp == 0)
			four_or_two = 2;
		else if (temp == 1)
			four_or_two = 4;
		if (first_on)
			four_or_two = 2;

		if (backgd[rand_row][rand_col] == 0)
		{
			backgd[rand_row][rand_col] = four_or_two;
			break;
		}
	}
	/*画图*/
	gmw_draw_block(pCGI, rand_row, rand_col, four_or_two, bdi_2048);
	return 0;
}

/***************************************************************************
  函数名称：
  功    能：将一个色块在一个方向移动到底
  输入参数：row_col代表那一行或那一列
  返 回 值：返回whether_move代表动没动，for_play中要用
  说    明：
***************************************************************************/
int move_to_end(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int backgd[12][12], int direction, const BLOCK_DISPLAY_INFO bdi_2048[], int row_col)
{
	int whether_move = 0;
	if (direction == DOWN_TO_UP)
	{
		/*向上动*/
		for (int j = 0; j < pCGI->row_num; j++)
		{
			int can_up_num = 0;
			while (backgd[j][row_col] != 0 && j - can_up_num - 1 >= 0 && backgd[j - can_up_num - 1][row_col] == 0)
				can_up_num++;
			if (can_up_num > 0)
			{
				gmw_move_block(pCGI, j, row_col, backgd[j][row_col], BDI_VALUE_BLANK, bdi_2048, DOWN_TO_UP, can_up_num);//移动图形化界面
				backgd[j - can_up_num][row_col] = backgd[j][row_col];//改变数组
				backgd[j][row_col] = 0;
				++whether_move;
			}
		}
	}
	else if (direction == UP_TO_DOWN)
	{
		/*向下移动*/
		for (int j = pCGI->row_num - 1; j >= 0; j--)
		{
			int can_down_num = 0;
			while (backgd[j][row_col] != 0 && j + can_down_num + 1 < pCGI->row_num && backgd[j + can_down_num + 1][row_col] == 0)
				can_down_num++;
			if (can_down_num > 0)
			{
				gmw_move_block(pCGI, j, row_col, backgd[j][row_col], BDI_VALUE_BLANK, bdi_2048, UP_TO_DOWN, can_down_num);//移动图形化界面
				backgd[j + can_down_num][row_col] = backgd[j][row_col];//改变数组
				backgd[j][row_col] = 0;
				++whether_move;
			}
		}
	}
	else if (direction == RIGHT_TO_LEFT)
	{
		/*向左移动*/
		for (int i = 0; i < pCGI->col_num; i++)
		{
			int can_left_num = 0;
			while (backgd[row_col][i] != 0 && i - can_left_num - 1 >= 0 && backgd[row_col][i - can_left_num - 1] == 0)
				can_left_num++;
			if (can_left_num > 0)
			{
				gmw_move_block(pCGI, row_col, i, backgd[row_col][i], BDI_VALUE_BLANK, bdi_2048, RIGHT_TO_LEFT, can_left_num);//移动图形
				backgd[row_col][i - can_left_num] = backgd[row_col][i];
				backgd[row_col][i] = 0;
				++whether_move;
			}
		}
	}
	else if (direction == LEFT_TO_RIGHT)
	{
		/*向右移动*/
		for (int i = pCGI->col_num - 1; i >= 0; i--)
		{
			int can_right_num = 0;
			while (backgd[row_col][i] != 0 && i + can_right_num + 1 < pCGI->col_num && backgd[row_col][i + can_right_num + 1] == 0)
				can_right_num++;
			if (can_right_num > 0)
			{
				gmw_move_block(pCGI, row_col, i, backgd[row_col][i], BDI_VALUE_BLANK, bdi_2048, LEFT_TO_RIGHT, can_right_num);//移动图形
				backgd[row_col][i + can_right_num] = backgd[row_col][i];
				backgd[row_col][i] = 0;
				++whether_move;
			}
		}
	}
	return whether_move;
}

/***************************************************************************
  函数名称：
  功    能：将一个色块们向某方向移动好
  输入参数：score代表获取分数，是引用
  返 回 值：返回whether_move代表动没动，for_play中要用
  说    明：
***************************************************************************/
int block_move(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int backgd[12][12], int direction, const BLOCK_DISPLAY_INFO bdi_2048[], int& score)
{
	int whether_move = 0;
	if (direction == DOWN_TO_UP)
	{
		for (int i = 0; i < pCGI->col_num; i++)
		{
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, i);
			/*合并*/
			for (int j = 1; j < pCGI->row_num; j++)
			{
				if (backgd[j][i] != 0 && backgd[j][i] == backgd[j - 1][i])
				{
					gmw_move_block(pCGI, j, i, backgd[j][i], BDI_VALUE_BLANK, bdi_2048, DOWN_TO_UP, 1);//移动图形化界面
					backgd[j - 1][i] += backgd[j][i];//改变数组
					score += backgd[j - 1][i];
					backgd[j][i] = 0;
					gmw_draw_block(pCGI, j - 1, i, backgd[j - 1][i], bdi_2048);
					++whether_move;
				}
			}
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, i);
		}
	}
	else if (direction == UP_TO_DOWN)
	{
		for (int i = 0; i < pCGI->col_num; i++)
		{
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, i);
			/*合并*/
			for (int j = pCGI->row_num - 2; j >= 0; j--)
			{
				if (backgd[j][i] != 0 && backgd[j + 1][i] == backgd[j][i])
				{
					gmw_move_block(pCGI, j, i, backgd[j][i], BDI_VALUE_BLANK, bdi_2048, UP_TO_DOWN, 1);//移动图形化界面
					backgd[j + 1][i] += backgd[j][i];
					backgd[j][i] = 0;
					score += backgd[j + 1][i];
					gmw_draw_block(pCGI, j + 1, i, backgd[j + 1][i], bdi_2048);
					++whether_move;
				}
			}
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, i);
		}//end of transverse cols
	}//end of if direction
	else if (direction == RIGHT_TO_LEFT)
	{
		for (int j = 0; j < pCGI->row_num; j++)
		{
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, j);
			/*合并*/
			for (int i = 1; i < pCGI->col_num; i++)
			{
				if (backgd[j][i] != 0 && backgd[j][i - 1] == backgd[j][i])
				{
					gmw_move_block(pCGI, j, i, backgd[j][i], BDI_VALUE_BLANK, bdi_2048, RIGHT_TO_LEFT, 1);
					backgd[j][i - 1] += backgd[j][i];
					backgd[j][i] = 0;
					score += backgd[j][i - 1];
					gmw_draw_block(pCGI, j, i - 1, backgd[j][i - 1], bdi_2048);
					++whether_move;
				}
			}
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, j);
		}//end of transverse rows
	}//end of if direction
	else if (direction == LEFT_TO_RIGHT)
	{
		for (int j = 0; j < pCGI->row_num; j++)
		{
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, j);
			/*合并*/
			for (int i = pCGI->col_num - 2; i >= 0; i--)
			{
				if (backgd[j][i] != 0 && backgd[j][i + 1] == backgd[j][i])
				{
					gmw_move_block(pCGI, j, i, backgd[j][i], BDI_VALUE_BLANK, bdi_2048, LEFT_TO_RIGHT, 1);//移动图形
					backgd[j][i + 1] += backgd[j][i];
					backgd[j][i] = 0;
					score += backgd[j][i + 1];
					gmw_draw_block(pCGI, j, i + 1, backgd[j][i + 1], bdi_2048);
					++whether_move;
				}
			}
			/*向右移动*/
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, j);
		}//end of transverse rows
	}//end of if directioin
	return whether_move;
}
/***************************************************************************
  函数名称：
  功    能：游戏结束后的提示
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void game_fin(CONSOLE_GRAPHICS_INFO* pCGI, bool success)
{
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2, "╔════╗", COLOR_HGREEN, COLOR_HWHITE);
	if (success == true)
		cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 1, "║游戏成功║", COLOR_HGREEN, COLOR_HWHITE);
	else if (success == false)
		cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 1, "║游戏失败║", COLOR_HGREEN, COLOR_HWHITE);
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 2, "║R：再玩 ║", COLOR_HGREEN, COLOR_HWHITE);
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 3, "║Q：退出 ║", COLOR_HGREEN, COLOR_HWHITE);
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 4, "╚════╝", COLOR_HGREEN, COLOR_HWHITE);
}

/***************************************************************************
  函数名称：
  功    能：判断最大色块是否为所需值
  输入参数：
  返 回 值：max最大色块值
  说    明：
***************************************************************************/
int check_maxblock(int backgd[12][12], board_param param_2048)
{
	int max = 0;
	for (int i = 0; i < param_2048.cols; i++)
		for (int j = 0; j < param_2048.rows; j++)
		{
			if (backgd[j][i] > max)
				max = backgd[j][i];
		}
	return max;
}
/***************************************************************************
  函数名称：
  功    能：判断游戏区域是否已经满了
  输入参数：
  返 回 值：FULL满了，NFULL没满
  说    明：
***************************************************************************/
int check_full(int backgd[12][12], board_param param_2048)
{
	for (int i = 0; i < param_2048.cols; i++)
		for (int j = 0; j < param_2048.rows; j++)
		{
			if (backgd[j][i] == 0)
				return NFULL;
			if (i - 1 >= 0)
				if (backgd[j][i] == backgd[j][i - 1])
					return NFULL;
			if (i + 1 < param_2048.cols)
				if (backgd[j][i] == backgd[j][i + 1])
					return NFULL;
			if (j - 1 >= 0)
				if (backgd[j][i] == backgd[j - 1][i])
					return NFULL;
			if (j + 1 < param_2048.rows)
				if (backgd[j][i] == backgd[j + 1][i])
					return NFULL;
		}
	return FULL;
}

/***************************************************************************
  函数名称：
  功    能：上状态栏显示目标:%d 分数:%d 最高:%d 时间:%d (R:重玩 Q:退出)
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void score_display(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int score, clock_t start, int history_high)
{
	char temp[256] = {};
	int total_time = (clock() - start) / CLOCKS_PER_SEC;
	/* 上状态栏显示内容 */
	sprintf(temp, "目标:%d 分数:%d 最高:%d 时间:%d (R:重玩 Q:退出)", param_2048.goal_score, score, history_high, total_time);
	gmw_status_line(pCGI, TOP_STATUS_LINE, temp);
}

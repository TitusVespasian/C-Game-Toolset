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
  功    能：游戏功能整合，history_high是引用
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int for_play(CONSOLE_GRAPHICS_INFO* pCGI, board_param param_2048, int backgd[12][12], const BLOCK_DISPLAY_INFO bdi_2048[], int& history_high)
{
	int score = 0;
	clock_t start = clock();
	random_pos(pCGI, param_2048, bdi_2048, backgd, true);
	random_pos(pCGI, param_2048, bdi_2048, backgd, true);
	score_display(pCGI, param_2048, score, start, history_high);
	int loop = 1;
	while (1)
	{
		int ret, MAction, MRow, MCol, Keycode1, Keycode2;
		while (1)
		{
			int pass = 0;
			if (check_full(backgd, param_2048) == FULL || check_maxblock(backgd, param_2048) == param_2048.goal_score)
			{
				if (check_maxblock(backgd, param_2048) == param_2048.goal_score)
					game_fin(pCGI, true);
				else if (check_full(backgd, param_2048) == FULL)
					game_fin(pCGI, false);
				while (1)
				{
					ret = gmw_read_keyboard_and_mouse(pCGI, MAction, MRow, MCol, Keycode1, Keycode2);//不显示下状态栏因为没打开过
					if (ret == CCT_KEYBOARD_EVENT)
					{
						if (Keycode1 == 'q' || Keycode1 == 'Q')
						{
							pass = 1;
							loop = 2;
						}
						else if (Keycode1 == 'R' || Keycode1 == 'r')
						{
							pass = 1;
							loop = 0;
						}
						else
							continue;
						break;
					}
				}
			}
			ret = gmw_read_keyboard_and_mouse(pCGI, MAction, MRow, MCol, Keycode1, Keycode2);//不显示下状态栏因为没打开过
			if (ret == CCT_KEYBOARD_EVENT)
			{
				if (Keycode1 == 'q' || Keycode1 == 'Q')
				{
					loop = 2;
					pass = 1;
				}
				else if (Keycode1 == 'R' || Keycode1 == 'r')
				{
					loop = 0;
					pass = 1;
				}
				else if (Keycode1 == 224)
				{
					switch (Keycode2) {
					case KB_ARROW_UP:
						pass += block_move(pCGI, param_2048, backgd, DOWN_TO_UP, bdi_2048, score);//加分也在里面
						break;
					case KB_ARROW_DOWN:
						pass += block_move(pCGI, param_2048, backgd, UP_TO_DOWN, bdi_2048, score);
						break;
					case KB_ARROW_LEFT:
						pass += block_move(pCGI, param_2048, backgd, RIGHT_TO_LEFT, bdi_2048, score);
						break;
					case KB_ARROW_RIGHT:
						pass += block_move(pCGI, param_2048, backgd, LEFT_TO_RIGHT, bdi_2048, score);
						break;
					}//end of switch of Keycode2
				}//end of if of Keycode1
			}//end of CCT_KEYBOARD_EVENT
			score_display(pCGI, param_2048, score, start, history_high);
			if (pass != 0)
				break;
		}//end of input while
		if (loop != 1)
			break;
		random_pos(pCGI, param_2048, bdi_2048, backgd, false);
	}//end of while
	if (score > history_high)
		history_high = score;
	return loop;
}
/*读文件*/
int file_read(for_file file_info[40], board_param param_2048)
{
	ifstream fin;
	int history_high = 0;
	fin.open("HighScore.2048", ios::in);//打开
	if (fin.is_open() == 0)//判断
	{
		int count = 0;
		for (int j = 4; j < 9; j++)
			for (int i = 4; i < 11; i++)
			{
				file_info[count].rows = j;
				file_info[count].cols = i;
				file_info[count].high_score = 0;
				++count;
			}
		return 0;
	}
	for (int i = 0; i < 35; i++)
	{
		fin >> file_info[i].rows;
		fin >> file_info[i].cols;
		fin >> file_info[i].high_score;
		if (file_info[i].rows == param_2048.rows && file_info[i].cols == param_2048.cols)
			history_high = file_info[i].high_score;
	}
	fin.close();//关闭
	return history_high;
}
/*写文件*/
int file_write(for_file file_info[40], board_param param_2048, int history_high)
{
	ofstream fout;
	fout.open("HighScore.2048", ios::out);
	if (fout.is_open() == 0)
		return -1;
	for (int i = 0; i < 35; i++)
	{
		if (file_info[i].rows == param_2048.rows && file_info[i].cols == param_2048.cols)
			file_info[i].high_score = history_high;
		fout << file_info[i].rows << ' ' << file_info[i].cols << ' ' << file_info[i].high_score << endl;
	}
	fout.close();//关闭
	return 0;
}
int main()
{
	const BLOCK_DISPLAY_INFO bdi_2048[] = {
		{BDI_VALUE_BLANK, -1, -1, NULL},  //0不显示，用空格填充即可
		{2, COLOR_HYELLOW, COLOR_BLACK, NULL},
		{4, COLOR_HRED, COLOR_BLACK, NULL},
		{8, COLOR_GREEN, COLOR_BLACK, NULL},
		{16, COLOR_RED, COLOR_BLACK, NULL},
		{32, COLOR_HGREEN, COLOR_BLACK, NULL},
		{64, COLOR_HYELLOW, COLOR_BLACK, NULL},
		{128, COLOR_HRED, COLOR_BLACK, NULL},
		{256, COLOR_HWHITE, COLOR_BLACK, NULL},
		{512, COLOR_HBLACK, COLOR_BLACK, NULL},
		{1024, COLOR_HPINK, COLOR_BLACK, NULL},
		{2048, COLOR_WHITE, COLOR_BLACK, NULL},
		{4096, COLOR_YELLOW, COLOR_BLACK, NULL},
		{8192, COLOR_PINK, COLOR_BLACK, NULL},
		{16384, COLOR_HBLUE, COLOR_BLACK, NULL},
		{32768, COLOR_HCYAN, COLOR_BLACK, NULL},
		{65536, COLOR_HGREEN, COLOR_BLACK, NULL},
		{131072, COLOR_HPINK, COLOR_BLACK, NULL},  //如果开心，还可以继续加
		{BDI_VALUE_END, -1, -1, NULL} //判断结束条件为-999
	};
	while (1)
	{
		int backgd[12][12] = {};
		CONSOLE_GRAPHICS_INFO CGI_2048;
		board_param param_2048;
		int loop = 1;
		int history_high = 0;
		for_file file_info[40] = {};//35行有效数据

		init_choice_frame(CGI_2048);
		input_choice(param_2048);
		history_high = file_read(file_info, param_2048);
		intit_play_frame(&CGI_2048, param_2048);
		clock_t start = clock();
		loop = for_play(&CGI_2048, param_2048, backgd, bdi_2048, history_high);//会改变history_high的值
		file_write(file_info, param_2048, history_high);
		if (loop == 2)
			break;
	}
	cct_setcolor();
	return 0;
}

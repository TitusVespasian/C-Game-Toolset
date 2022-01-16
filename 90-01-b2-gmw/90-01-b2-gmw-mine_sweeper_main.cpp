#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <cstdio>
#include <conio.h>
#include<ctime>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#include"../include/cmd_gmw_tools.h"
#include"../include/cmd_input.h"
#include"90-01-b2-gmw-mine_sweeper.h"
#endif
using namespace std;


const BLOCK_DISPLAY_INFO bdi[] = {
	{'0', COLOR_HWHITE,COLOR_HWHITE, "  "},  //0不显示，用空格填充即可
	{'1',COLOR_HWHITE,COLOR_BLUE,"1"},
	{'2', COLOR_HWHITE, COLOR_GREEN,"2"},
	{'3', COLOR_HWHITE, COLOR_CYAN, "3"},
	{'4', COLOR_HWHITE, COLOR_RED, "4"},
	{'5', COLOR_HWHITE, COLOR_PINK, "5"},
	{'6', COLOR_HWHITE, COLOR_YELLOW,"6"},
	{'7', COLOR_HWHITE,  COLOR_WHITE,"7"},
	{'8',COLOR_HWHITE, COLOR_HBLACK, "8"},
	{'*',COLOR_HWHITE, COLOR_BLACK, "*"},
	{DISPLAY_OFF,COLOR_YELLOW,COLOR_HYELLOW,"  "},//0相当于BDI_VALUE_BLANK
	{DISPLAY_MARK,COLOR_HRED,COLOR_HWHITE,"#"},
	{BDI_VALUE_END, -1, -1, NULL} }; //判断结束条件为-999


/***************************************************************************
  函数名称：
  功    能：初始化扫雷框架
  输入参数：常量CGI
  返 回 值：总雷数//返回0退出
  说    明：
***************************************************************************/
int init_sweeper(CONSOLE_GRAPHICS_INFO& Mine_sweeper_CGI)
{
	int rows = 0, cols = 0, bomb_num = 0;
	int controleins = menu("请选择难度：\n"
		"0.退出\n"
		"1.初级(9 * 9 - 10颗雷\n"
		"2.中级(16 * 16 - 40颗雷)\n"
		"3.高级(16 * 30 - 99颗雷)\n"
		"请输入[1..3]：\n", 3, "mine_sweeper");
	if (controleins == 1)
	{
		rows = 9;//border_line = 35;
		cols = 9;//border_col = 59;
		bomb_num = 10;
	}
	else if (controleins == 2)
	{
		rows = 16;//border_line = 56;
		cols = 16;//border_col = 101;
		bomb_num = 40;
	}
	else if (controleins == 3)
	{
		rows = 16;//border_col = 185;
		cols = 30;//border_line = 56;
		bomb_num = 99;
	}
	else if (controleins == 0)
		return 0;
	gmw_set_font(&Mine_sweeper_CGI, "Terminal", 16, 8);
	gmw_init(&Mine_sweeper_CGI, rows, cols);
	gmw_set_frame_style(&Mine_sweeper_CGI, 4, 2, false);
	gmw_set_ext_rowcol(&Mine_sweeper_CGI);
	gmw_set_rowno_switch(&Mine_sweeper_CGI, true);//显示行号
	gmw_set_colno_switch(&Mine_sweeper_CGI, true);//显示列标
	return bomb_num;
}

/***************************************************************************
  函数名称：
  功    能：按本游戏扫雷显示每个方格
  输入参数：常量pCGI,display,bomb
  返 回 值：
  说    明：
***************************************************************************/
void display_general(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], char bomb[20][34])
{
	for (int j = 0; j < pCGI->row_num; j++)
		for (int i = 0; i < pCGI->col_num; i++)
		{
			if (display[j][i] == DISPLAY_OFF)
				gmw_draw_block(pCGI, j, i, DISPLAY_OFF, bdi);
			else if (display[j][i] == DISPLAY_ON)
				gmw_draw_block(pCGI, j, i, bomb[j][i], bdi);
			else if (display[j][i] == DISPLAY_MARK)
				gmw_draw_block(pCGI, j, i, DISPLAY_MARK, bdi);
		}
	cct_setcolor();
}

/*标记格和显示雷数*/
void mark(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], int MRow, int MCol, int bomb_num, bool change_display)
{
	if (change_display)
	{
		if (display[MRow][MCol] == 2)
			display[MRow][MCol] = 0;
		else if (display[MRow][MCol] == 0)
			display[MRow][MCol] = 2;
	}
	for (int i = 0; i <= 19; i++)
		for (int j = 0; j <= 33; j++)
		{
			if (display[i][j] == DISPLAY_MARK&&bomb_num>0)
				bomb_num--;
		}
	char temp[256] = {};
	sprintf(temp, "剩余雷数：%d", bomb_num);
	gmw_status_line(pCGI, TOP_STATUS_LINE, "ESC退出，空格显示时间", temp);
	return;
}


/***************************************************************************
  函数名称：
  功    能：上工具栏显示时间
  输入参数：常量pCGI,起始时间
  返 回 值：
  说    明：
***************************************************************************/
void display_time(const CONSOLE_GRAPHICS_INFO* const pCGI, clock_t start)
{
	char temp[256] = {};
	sprintf(temp, "当前时间：%lf秒，", (double)(clock() - (double)start) / (double)CLOCKS_PER_SEC);
	gmw_status_line(pCGI, TOP_STATUS_LINE, "按ESC退出,空格显示时间", temp);
}


/*退出游戏*/
void quit_game(int output_pos_x, int output_pos_y)
{
	cct_gotoxy(output_pos_x, output_pos_y);
	cout << "按回车键继续...";
	while (_getch() != '\r')
		;
	cct_setcolor();
	cct_cls();
}
/***************************************************************************
  函数名称：
  功    能：还没数组设雷时的标记，ESC退出，和显示时间
  输入参数：常量pCGI,起始时间
  返 回 值：左键返回MOUSE_LEFT_BUTTON_CLICK，ESC返回VK_ESCAPE
  说    明：按左键的时候需要返回坐标
***************************************************************************/
int not_inited_array(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], char bomb[20][34], int bomb_num, clock_t start, int& MRow, int& MCol)
{
	int MAction, KeyCode1, KeyCode2, ret;
	do
	{
		ret = gmw_read_keyboard_and_mouse(pCGI, MAction, MRow, MCol, KeyCode1, KeyCode2);
		if (ret == CCT_KEYBOARD_EVENT)
		{
			if (KeyCode1 == ' ')
				display_time(pCGI, start);
			else if (KeyCode1 == VK_ESCAPE)
				return VK_ESCAPE;
		}
		else if (ret == CCT_MOUSE_EVENT)
		{
			if (MAction == MOUSE_LEFT_BUTTON_CLICK)
				return MOUSE_LEFT_BUTTON_CLICK;
			else if (MAction == MOUSE_RIGHT_BUTTON_CLICK && MRow != -1 && MCol != -1)
			{
				mark(pCGI, display, MRow, MCol, bomb_num, true);
				display_general(pCGI, display, bomb);
			}
		}
	} while (1);
	return 0;
}

/***************************************************************************
  函数名称：
  功    能：组织所有游戏的函数
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void for_play(CONSOLE_GRAPHICS_INFO* pCGI, int display[20][34], char bomb[20][34], int bomb_num)
{
	clock_t start = clock();
	bool leave = false;
	int MRow = 19, MCol = 34;
	int Action;
	Action = not_inited_array(pCGI, display, bomb, bomb_num, start, MRow, MCol);
	if (Action == MOUSE_LEFT_BUTTON_CLICK)
	{
		generatebomb(bomb, pCGI->row_num, pCGI->col_num, bomb_num, MRow, MCol);
		inarray(bomb, pCGI->row_num, pCGI->col_num, bomb_num);
		BFS(bomb, display, pCGI->row_num, pCGI->col_num, MRow, MCol);
		mark(pCGI, display, MRow, MCol, bomb_num, false);
		display_general(pCGI, display, bomb);
		while (1)
		{
			Action = not_inited_array(pCGI, display, bomb, bomb_num, start, MRow, MCol);
			if (Action == VK_ESCAPE)
				break;
			BFS(bomb, display, pCGI->row_num, pCGI->col_num, MRow, MCol);
			mark(pCGI, display, MRow, MCol, bomb_num, false);
			display_general(pCGI, display, bomb);
			if (display[MRow][MCol] == 1 && bomb[MRow][MCol] == '*')
			{
				char temp[256] = {};
				sprintf(temp, "共用时：%lf秒，", (double)(clock() - (double)start) / (double)CLOCKS_PER_SEC);
				gmw_status_line(pCGI, LOWER_STATUS_LINE, "你输了，游戏结束", temp);
				break;
			}
			int flag = 0;
			for (int i = 0; i < pCGI->row_num; i++)
			{
				for (int j = 0; j < pCGI->col_num; j++)
				{
					if ((display[i][j] == 0 || display[i][j] == 2) && (bomb[i][j] != '*'))
						flag++;
					else if (display[i][j] == 1 && bomb[i][j] == '*')
						flag++;
				}
			}
			if (flag == 0)
			{
				char temp[256] = {};
				sprintf(temp, "共用时：%lf秒，", (double)(clock() - (double)start) / (double)CLOCKS_PER_SEC);
				gmw_status_line(pCGI, LOWER_STATUS_LINE, "恭喜胜利，游戏结束", temp);
				break;
			}
		}
	}
	quit_game(pCGI->SLI.lower_start_x, pCGI->SLI.lower_start_y + 1);
	return;
}
int main()
{
	CONSOLE_GRAPHICS_INFO Mine_sweeper_CGI; //声明一个CGI变量

	while (1)
	{
		char bomb[20][34] = {};
		int display[20][34] = {};
		int bomb_num = 0;
		for (int i = 0; i <= 20 - 1; i++)
			for (int j = 0; j <= 34 - 1; j++)
				bomb[i][j] = '0';
		bomb_num = init_sweeper(Mine_sweeper_CGI);
		if (bomb_num == 0)
			break;
		gmw_draw_frame(&Mine_sweeper_CGI);
		display_general(&Mine_sweeper_CGI, display, bomb);
		gmw_status_line(&Mine_sweeper_CGI, TOP_STATUS_LINE, "按ESC退出,空格显示时间");
		for_play(&Mine_sweeper_CGI, display, bomb, bomb_num);
	}
	cct_setcolor();
}



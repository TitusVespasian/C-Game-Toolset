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
	{'0', COLOR_HWHITE,COLOR_HWHITE, "  "},  //0����ʾ���ÿո���伴��
	{'1',COLOR_HWHITE,COLOR_BLUE,"1"},
	{'2', COLOR_HWHITE, COLOR_GREEN,"2"},
	{'3', COLOR_HWHITE, COLOR_CYAN, "3"},
	{'4', COLOR_HWHITE, COLOR_RED, "4"},
	{'5', COLOR_HWHITE, COLOR_PINK, "5"},
	{'6', COLOR_HWHITE, COLOR_YELLOW,"6"},
	{'7', COLOR_HWHITE,  COLOR_WHITE,"7"},
	{'8',COLOR_HWHITE, COLOR_HBLACK, "8"},
	{'*',COLOR_HWHITE, COLOR_BLACK, "*"},
	{DISPLAY_OFF,COLOR_YELLOW,COLOR_HYELLOW,"  "},//0�൱��BDI_VALUE_BLANK
	{DISPLAY_MARK,COLOR_HRED,COLOR_HWHITE,"#"},
	{BDI_VALUE_END, -1, -1, NULL} }; //�жϽ�������Ϊ-999


/***************************************************************************
  �������ƣ�
  ��    �ܣ���ʼ��ɨ�׿��
  �������������CGI
  �� �� ֵ��������//����0�˳�
  ˵    ����
***************************************************************************/
int init_sweeper(CONSOLE_GRAPHICS_INFO& Mine_sweeper_CGI)
{
	int rows = 0, cols = 0, bomb_num = 0;
	int controleins = menu("��ѡ���Ѷȣ�\n"
		"0.�˳�\n"
		"1.����(9 * 9 - 10����\n"
		"2.�м�(16 * 16 - 40����)\n"
		"3.�߼�(16 * 30 - 99����)\n"
		"������[1..3]��\n", 3, "mine_sweeper");
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
	gmw_set_rowno_switch(&Mine_sweeper_CGI, true);//��ʾ�к�
	gmw_set_colno_switch(&Mine_sweeper_CGI, true);//��ʾ�б�
	return bomb_num;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�������Ϸɨ����ʾÿ������
  �������������pCGI,display,bomb
  �� �� ֵ��
  ˵    ����
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

/*��Ǹ����ʾ����*/
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
	sprintf(temp, "ʣ��������%d", bomb_num);
	gmw_status_line(pCGI, TOP_STATUS_LINE, "ESC�˳����ո���ʾʱ��", temp);
	return;
}


/***************************************************************************
  �������ƣ�
  ��    �ܣ��Ϲ�������ʾʱ��
  �������������pCGI,��ʼʱ��
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void display_time(const CONSOLE_GRAPHICS_INFO* const pCGI, clock_t start)
{
	char temp[256] = {};
	sprintf(temp, "��ǰʱ�䣺%lf�룬", (double)(clock() - (double)start) / (double)CLOCKS_PER_SEC);
	gmw_status_line(pCGI, TOP_STATUS_LINE, "��ESC�˳�,�ո���ʾʱ��", temp);
}


/*�˳���Ϸ*/
void quit_game(int output_pos_x, int output_pos_y)
{
	cct_gotoxy(output_pos_x, output_pos_y);
	cout << "���س�������...";
	while (_getch() != '\r')
		;
	cct_setcolor();
	cct_cls();
}
/***************************************************************************
  �������ƣ�
  ��    �ܣ���û��������ʱ�ı�ǣ�ESC�˳�������ʾʱ��
  �������������pCGI,��ʼʱ��
  �� �� ֵ���������MOUSE_LEFT_BUTTON_CLICK��ESC����VK_ESCAPE
  ˵    �����������ʱ����Ҫ��������
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
  �������ƣ�
  ��    �ܣ���֯������Ϸ�ĺ���
  ���������
  �� �� ֵ��
  ˵    ����
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
				sprintf(temp, "����ʱ��%lf�룬", (double)(clock() - (double)start) / (double)CLOCKS_PER_SEC);
				gmw_status_line(pCGI, LOWER_STATUS_LINE, "�����ˣ���Ϸ����", temp);
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
				sprintf(temp, "����ʱ��%lf�룬", (double)(clock() - (double)start) / (double)CLOCKS_PER_SEC);
				gmw_status_line(pCGI, LOWER_STATUS_LINE, "��ϲʤ������Ϸ����", temp);
				break;
			}
		}
	}
	quit_game(pCGI->SLI.lower_start_x, pCGI->SLI.lower_start_y + 1);
	return;
}
int main()
{
	CONSOLE_GRAPHICS_INFO Mine_sweeper_CGI; //����һ��CGI����

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
		gmw_status_line(&Mine_sweeper_CGI, TOP_STATUS_LINE, "��ESC�˳�,�ո���ʾʱ��");
		for_play(&Mine_sweeper_CGI, display, bomb, bomb_num);
	}
	cct_setcolor();
}



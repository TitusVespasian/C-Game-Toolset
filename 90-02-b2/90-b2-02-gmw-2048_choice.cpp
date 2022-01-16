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

/*��ʼ������߿�*/
void init_choice_frame(CONSOLE_GRAPHICS_INFO& CGI_2048)
{
	cct_setcolor();
	cct_cls();
	cct_setcursor(CURSOR_INVISIBLE);
	gmw_init(&CGI_2048, frame_high1, frame_wide1);

	gmw_set_ext_rowcol(&CGI_2048, 0, 0, 0, 0);
	gmw_set_color(&CGI_2048, COLOR_BLACK, COLOR_HWHITE);	//����������ɫ
	gmw_set_font(&CGI_2048, "������", 16);			    //����
	gmw_set_frame_style(&CGI_2048, 2, 1, false);	//��Ϸ��������ÿ��ɫ���6��3���зָ���
	gmw_set_frame_color(&CGI_2048, COLOR_HWHITE, COLOR_BLACK);		//��Ϸ��������ɫ
	gmw_draw_frame(&CGI_2048);
}

/*������ת��Ϊ�ַ���*/
static void transfer(int num, char num_char[30])
{
	int i = 0;
	while (num > 0)
	{
		if (i < 30)
			num_char[i] = num % 10 + '0';
		num = num / 10;
		++i;
	}
	for (int j = 0; j < i / 2; j++)
	{
		char temp = num_char[j];
		num_char[j] = num_char[i - 1 - j];
		num_char[i - 1 - j] = temp;
	}
	if (i < 30)
		num_char[i] = '\r';
}

/*�൱���Լ�д��cin*/
static int input_param(const char* output, int lowest_goal = 0)
{
	int row_col;
	char num_char_array[5][30] = {};
	if (lowest_goal != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			transfer(lowest_goal * (int)pow(2, i), num_char_array[i]);
		}
	}
	while (1)
	{
		if (strcmp(output, "����������[4-8]") == 0)
			cct_gotoxy(strlen(output) + 2, 3);
		else if (strcmp(output, "����������[4-10]") == 0)
			cct_gotoxy(strlen(output) + 2, 4);
		else if (lowest_goal != 0)
			cct_gotoxy(strlen(output) + 2, 5);
		else if (strcmp(output, "�����붯����ʱ[0-5]") == 0)
			cct_gotoxy(strlen(output) + 2, 6);
		char buffer[20] = {};
		for (int i = 0; i < 10; i++)
		{
			buffer[i] = _getch();
			putchar(buffer[i]);
			if (buffer[i] == '\r')
				break;
		}
		int flag = 0;
		if (lowest_goal != 0)
		{
			for (int i = 0; i < 4; i++)
			{
				if (strcmp(buffer, num_char_array[i]) == 0)
				{
					row_col = lowest_goal * (int)pow(2, i);
					flag = 1;
					break;
				}
			}
		}
		if (strcmp(output, "����������[4-8]") == 0 && buffer[0] >= '4' && buffer[0] <= '8' && buffer[1] == '\r')
			row_col = buffer[0] - '0';
		else if (strcmp(output, "����������[4-10]") == 0 && buffer[0] >= '4' && buffer[0] <= '9' && buffer[1] == '\r')
			row_col = buffer[0] - '0';
		else if (strcmp(output, "����������[4-10]") == 0 && buffer[0] == '1' && buffer[1] == '0' && buffer[2] == '\r')
			row_col = 10;
		else if (flag == 1)
			break;
		else if (strcmp(output, "�����붯����ʱ[0-5]") == 0 && buffer[0] >= '0' && buffer[0] <= '5' && buffer[1] == '\r')
			row_col = buffer[0] - '0';
		else
		{
			if (strcmp(output, "����������[4-8]") == 0)
				cct_showch(strlen(output) + 2, 3, ' ', COLOR_HWHITE, COLOR_BLACK, 20);
			else if (strcmp(output, "����������[4-10]") == 0)
				cct_showch(strlen(output) + 2, 4, ' ', COLOR_HWHITE, COLOR_BLACK, 20);
			else if (lowest_goal != 0)
				cct_showch(strlen(output) + 2, 5, ' ', COLOR_HWHITE, COLOR_BLACK, 20);
			else if (strcmp(output, "�����붯����ʱ[0-5]") == 0)
				cct_showch(strlen(output) + 2, 6, ' ', COLOR_HWHITE, COLOR_BLACK, 20);
			continue;
		}
		break;
	}
	return row_col;
}

/*��������*/
void input_choice(board_param& param_2048)
{
	cct_showstr(2, 2, "��Ϸ2048�������ã�", COLOR_HWHITE, COLOR_BLACK);
	cct_showstr(2, 3, "����������[4-8]", COLOR_HWHITE, COLOR_BLACK);
	param_2048.rows = input_param("����������[4-8]");
	cct_showstr(2, 4, "����������[4-10]", COLOR_HWHITE, COLOR_BLACK);
	param_2048.cols = input_param("����������[4-10]");
	int lowest_goal_exp;
	if ((param_2048.rows + param_2048.cols) % 2 == 0)
		lowest_goal_exp = (param_2048.rows + param_2048.cols) / 2 + 5;
	else
		lowest_goal_exp = (param_2048.rows + param_2048.cols) / 2 + 6;
	int lowest_goal = (int)pow(2, lowest_goal_exp);
	char temp[256];
	sprintf(temp, "������Ŀ�����[%d/%d/%d/%d]", lowest_goal, lowest_goal * 2, lowest_goal * 4, lowest_goal * 8);
	cct_showstr(2, 5, temp, COLOR_HWHITE, COLOR_BLACK);
	param_2048.goal_score = input_param(temp, lowest_goal);
	cct_showstr(2, 6, "�����붯����ʱ[0-5]", COLOR_HWHITE, COLOR_BLACK);
	param_2048.delay = input_param("�����붯����ʱ[0-5]");
}
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
	{1, COLOR_GREEN,COLOR_BLACK, "��"},  //0����ʾ���ÿո���伴��
	{BDI_VALUE_BLANK,COLOR_HWHITE,COLOR_HWHITE,"  "},//0�൱��BDI_VALUE_BLANK
	{BDI_VALUE_END, -1, -1, NULL}
};


/*
* ��������С
*/
void FrameSize(int& M, int& N)
{
	cct_setcolor();
	cct_cls();
	cct_setfontsize("������", 28);
	cct_setconsoleborder(80, 30);
	while (1)
	{
		cout << "��������M([12..21]��Ϊ3�ı���)" << endl;
		cin >> M;
		if (M <= 21 && M >= 12 && M % 3 == 0)
			break;
	}
	while (1)
	{
		cout << "������߶�N([18..26])" << endl;
		cin >> N;
		if (N <= 26 && N >= 18)
			break;
	}
	cct_cls();
}

/*
* ��������һ������
* @param present_char ��ǰ������״������
* @param arr_x,arr_y �����������Ͻǵĺ������꣬һ����������36*90
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

/*����1����������0��ֹ��,�����������ת��һ����Ԫ���ˣ�һ�ֵ��߽���
* ������Ƕ�֮�������
* @param to_be_char ��ת��֮�����״
* @param M ��� 0~M-1
* @param N �߶� 0~N-1
* @param arr_x ��������������λ��(���Ͻǵ�Ԫ�ؿ���С��0
* @param arr_y ��������������λ��
* @param backgd ��������
*/
int check_movement(int to_be_char[5][5], int M, int N, int arr_x, int arr_y, int backgd[30][30])
{
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 5; i++)
		{
			/*Ҫ�ȼ��arr_y + j��arr_x + i��ֹԽ��*/
			if (to_be_char[j][i] == 1)
			{
				if (arr_x + i > M - 1 || arr_x + i < 0 || arr_y + j > N - 1)//���������±߽�
					return FORBIDDEN;
				else if (arr_y + j >= 0)
				{
					if (backgd[arr_y + j][arr_x + i] == 1)//�������ӵ�ס
						return FORBIDDEN;
				}
			}
		}
	}
	return ALLOWED;
}

/*
* �ı䱳������
* ���arr_x+4>M-1�ǾͲ���,��ֹԽ��
* @param present_char
* @param backgd
* @param M ���
* @param N �߶�
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
* �������
* @param present_char
* @param M ���
* @param N �߶�
* @param arr_x,arr_y ��������
* @param backgd
* @param flag ���ֵ�״̬
* @param pause ͣ��ʱ��
* @param status �ǰ����¼������Զ�����
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

/*������һ���������䣬
* �ȼ���ܷ�������ٸı�������״��
�ٽ��в����ı䱳����ɫ��Ȼ��ı�����״��(��������˵Ļ���
* @param present_char ���ֵ���״
* @param num ĳ������
* @param pause һ�������ʱ�䣬ȱʡΪ1
* @param backgd �������飬�����õ�
* @param M ���
* @param N �߶�
* @return T_FAILED ���
* @return T_ACCEPT ������
*/
int control_fall(CONSOLE_GRAPHICS_INFO* pCGI, int present_char[5][5], int backgd[30][30], int M, int N, falling& block, int cmd, int pause)
{
	int arr_x = M / 2 - square_width / 2;
	int arr_y = -5;
	//��������ֶ������ϵ�
	clock_t start = clock();
	while (1)
	{
		while (_kbhit())
		{
			int Keycode1 = 0, Keycode2 = 0, MRow, MCol, MAction;
			gmw_read_keyboard_and_mouse(pCGI, MAction, MRow, MCol, Keycode1, Keycode2);
			if (Keycode1 == 224)
			{
				/*���ϼ�*/
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
				/*�����*/
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
				/*���Ҽ�*/
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
				/*���¼�*/
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
		/*�������䣬�ȼ���ܷ�������ٽ��в����ı䱳����ɫ��Ȼ��ı�����״��(��������˵Ļ���*/

		if (clock() - start >= clock_t(pause))
		{
			int flag = check_movement(present_char, M, N, arr_x, arr_y + 1, backgd);
			check_fall(pCGI, present_char, M, N, arr_x, arr_y, backgd, flag);
			start = clock();
			if (flag == FORBIDDEN)
			{
				if (arr_y <= 0)//����Ƿ����
					return T_FAILED;
				else
					return T_ACCEPT;
			}
		}
	}
	return T_ACCEPT;
}
/*
* ������������б�������Ϊ1�ķ���
* @param backgd ��������
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

/*�ı��ڲ�����
* @param row �������к�
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
* �鿴�Ƿ���Ҫ��ȥһ��
* @param M ���
* @param N �߶�
* @return ����
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
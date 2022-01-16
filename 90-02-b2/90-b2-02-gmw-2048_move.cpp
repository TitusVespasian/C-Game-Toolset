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
  �������ƣ�
  ��    �ܣ���ʼ����Ϸ����
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void intit_play_frame(CONSOLE_GRAPHICS_INFO* pCGI, board_param param_2048)
{
	gmw_init(pCGI, param_2048.rows, param_2048.cols);
	pCGI->lines -= 3;
	gmw_set_color(pCGI, COLOR_BLACK, COLOR_HWHITE);			//����������ɫ
	gmw_set_font(pCGI, "������", 16);						//����
	gmw_set_frame_style(pCGI, 12, 5, true);					//��Ϸ��������ÿ��ɫ���10��5���зָ��ߡ�����ɫ����߿򣬿��Ϊ10(�����6λ����)���߶�Ϊ5(Ϊ�˱���ɫ��Ϊ����)��
	gmw_set_frame_color(pCGI, COLOR_WHITE, COLOR_BLACK);	//��Ϸ��������ɫ
	gmw_set_block_border_switch(pCGI, true);				//Сɫ����߿�
	gmw_set_status_line_switch(pCGI, LOWER_STATUS_LINE, false);
	gmw_set_status_line_color(pCGI, TOP_STATUS_LINE, COLOR_HWHITE, COLOR_BLACK);
	pCGI->delay_of_block_moved = param_2048.delay * 5;//��gmw������С��15ms������ֱ����
	gmw_draw_frame(pCGI);                                 // ������
}

/*�����￪ʼ��ÿ�����������������ܣ�һ���ı��ڲ����飬һ����ͼ*/

/***************************************************************************
  �������ƣ�
  ��    �ܣ�����һ��2��4��ɫ��
  ���������bool first_on �ж��Ƿ��ǵ�һ�Σ�ֻ����2��
  �� �� ֵ�����ش����Ƿ�ɹ���һ�㶼��ɹ�
  ˵    ����
***************************************************************************/
int random_pos(CONSOLE_GRAPHICS_INFO* pCGI, board_param param_2048, const BLOCK_DISPLAY_INFO bdi_2048[], int backgd[12][12], bool first_on)
{
	srand((unsigned)time(NULL));
	int rand_row = 0;
	int rand_col = 0;
	int four_or_two = 0;
	bool is_full = true;

	/*��һ������ִ�в���������Ϊ�˷�ֹд�������ѭ��*/
	for (int i = 0; i < param_2048.cols; i++)
		for (int j = 0; j < param_2048.rows; j++)
			if (backgd[j][i] == 0)
			{
				is_full = false;
				break;
			}
	if (is_full == true)
	{
		cout << "���ˣ�" << endl;
		return -1;
	}
	/*��һ������ִ�в���������Ϊ�˷�ֹд�������ѭ��*/

	/*�ı��ڲ�����*/
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
	/*��ͼ*/
	gmw_draw_block(pCGI, rand_row, rand_col, four_or_two, bdi_2048);
	return 0;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���һ��ɫ����һ�������ƶ�����
  ���������row_col������һ�л���һ��
  �� �� ֵ������whether_move����û����for_play��Ҫ��
  ˵    ����
***************************************************************************/
int move_to_end(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int backgd[12][12], int direction, const BLOCK_DISPLAY_INFO bdi_2048[], int row_col)
{
	int whether_move = 0;
	if (direction == DOWN_TO_UP)
	{
		/*���϶�*/
		for (int j = 0; j < pCGI->row_num; j++)
		{
			int can_up_num = 0;
			while (backgd[j][row_col] != 0 && j - can_up_num - 1 >= 0 && backgd[j - can_up_num - 1][row_col] == 0)
				can_up_num++;
			if (can_up_num > 0)
			{
				gmw_move_block(pCGI, j, row_col, backgd[j][row_col], BDI_VALUE_BLANK, bdi_2048, DOWN_TO_UP, can_up_num);//�ƶ�ͼ�λ�����
				backgd[j - can_up_num][row_col] = backgd[j][row_col];//�ı�����
				backgd[j][row_col] = 0;
				++whether_move;
			}
		}
	}
	else if (direction == UP_TO_DOWN)
	{
		/*�����ƶ�*/
		for (int j = pCGI->row_num - 1; j >= 0; j--)
		{
			int can_down_num = 0;
			while (backgd[j][row_col] != 0 && j + can_down_num + 1 < pCGI->row_num && backgd[j + can_down_num + 1][row_col] == 0)
				can_down_num++;
			if (can_down_num > 0)
			{
				gmw_move_block(pCGI, j, row_col, backgd[j][row_col], BDI_VALUE_BLANK, bdi_2048, UP_TO_DOWN, can_down_num);//�ƶ�ͼ�λ�����
				backgd[j + can_down_num][row_col] = backgd[j][row_col];//�ı�����
				backgd[j][row_col] = 0;
				++whether_move;
			}
		}
	}
	else if (direction == RIGHT_TO_LEFT)
	{
		/*�����ƶ�*/
		for (int i = 0; i < pCGI->col_num; i++)
		{
			int can_left_num = 0;
			while (backgd[row_col][i] != 0 && i - can_left_num - 1 >= 0 && backgd[row_col][i - can_left_num - 1] == 0)
				can_left_num++;
			if (can_left_num > 0)
			{
				gmw_move_block(pCGI, row_col, i, backgd[row_col][i], BDI_VALUE_BLANK, bdi_2048, RIGHT_TO_LEFT, can_left_num);//�ƶ�ͼ��
				backgd[row_col][i - can_left_num] = backgd[row_col][i];
				backgd[row_col][i] = 0;
				++whether_move;
			}
		}
	}
	else if (direction == LEFT_TO_RIGHT)
	{
		/*�����ƶ�*/
		for (int i = pCGI->col_num - 1; i >= 0; i--)
		{
			int can_right_num = 0;
			while (backgd[row_col][i] != 0 && i + can_right_num + 1 < pCGI->col_num && backgd[row_col][i + can_right_num + 1] == 0)
				can_right_num++;
			if (can_right_num > 0)
			{
				gmw_move_block(pCGI, row_col, i, backgd[row_col][i], BDI_VALUE_BLANK, bdi_2048, LEFT_TO_RIGHT, can_right_num);//�ƶ�ͼ��
				backgd[row_col][i + can_right_num] = backgd[row_col][i];
				backgd[row_col][i] = 0;
				++whether_move;
			}
		}
	}
	return whether_move;
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���һ��ɫ������ĳ�����ƶ���
  ���������score�����ȡ������������
  �� �� ֵ������whether_move����û����for_play��Ҫ��
  ˵    ����
***************************************************************************/
int block_move(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int backgd[12][12], int direction, const BLOCK_DISPLAY_INFO bdi_2048[], int& score)
{
	int whether_move = 0;
	if (direction == DOWN_TO_UP)
	{
		for (int i = 0; i < pCGI->col_num; i++)
		{
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, i);
			/*�ϲ�*/
			for (int j = 1; j < pCGI->row_num; j++)
			{
				if (backgd[j][i] != 0 && backgd[j][i] == backgd[j - 1][i])
				{
					gmw_move_block(pCGI, j, i, backgd[j][i], BDI_VALUE_BLANK, bdi_2048, DOWN_TO_UP, 1);//�ƶ�ͼ�λ�����
					backgd[j - 1][i] += backgd[j][i];//�ı�����
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
			/*�ϲ�*/
			for (int j = pCGI->row_num - 2; j >= 0; j--)
			{
				if (backgd[j][i] != 0 && backgd[j + 1][i] == backgd[j][i])
				{
					gmw_move_block(pCGI, j, i, backgd[j][i], BDI_VALUE_BLANK, bdi_2048, UP_TO_DOWN, 1);//�ƶ�ͼ�λ�����
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
			/*�ϲ�*/
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
			/*�ϲ�*/
			for (int i = pCGI->col_num - 2; i >= 0; i--)
			{
				if (backgd[j][i] != 0 && backgd[j][i + 1] == backgd[j][i])
				{
					gmw_move_block(pCGI, j, i, backgd[j][i], BDI_VALUE_BLANK, bdi_2048, LEFT_TO_RIGHT, 1);//�ƶ�ͼ��
					backgd[j][i + 1] += backgd[j][i];
					backgd[j][i] = 0;
					score += backgd[j][i + 1];
					gmw_draw_block(pCGI, j, i + 1, backgd[j][i + 1], bdi_2048);
					++whether_move;
				}
			}
			/*�����ƶ�*/
			whether_move += move_to_end(pCGI, param_2048, backgd, direction, bdi_2048, j);
		}//end of transverse rows
	}//end of if directioin
	return whether_move;
}
/***************************************************************************
  �������ƣ�
  ��    �ܣ���Ϸ���������ʾ
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void game_fin(CONSOLE_GRAPHICS_INFO* pCGI, bool success)
{
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2, "�X�T�T�T�T�[", COLOR_HGREEN, COLOR_HWHITE);
	if (success == true)
		cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 1, "�U��Ϸ�ɹ��U", COLOR_HGREEN, COLOR_HWHITE);
	else if (success == false)
		cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 1, "�U��Ϸʧ�ܨU", COLOR_HGREEN, COLOR_HWHITE);
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 2, "�UR������ �U", COLOR_HGREEN, COLOR_HWHITE);
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 3, "�UQ���˳� �U", COLOR_HGREEN, COLOR_HWHITE);
	cct_showstr((pCGI->cols - 4) / 2 - pCGI->CFI.block_width / 2, (pCGI->lines - 1) / 2 - pCGI->CFI.block_high / 2 + 4, "�^�T�T�T�T�a", COLOR_HGREEN, COLOR_HWHITE);
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ��ж����ɫ���Ƿ�Ϊ����ֵ
  ���������
  �� �� ֵ��max���ɫ��ֵ
  ˵    ����
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
  �������ƣ�
  ��    �ܣ��ж���Ϸ�����Ƿ��Ѿ�����
  ���������
  �� �� ֵ��FULL���ˣ�NFULLû��
  ˵    ����
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
  �������ƣ�
  ��    �ܣ���״̬����ʾĿ��:%d ����:%d ���:%d ʱ��:%d (R:���� Q:�˳�)
  ���������
  �� �� ֵ��
  ˵    ����
***************************************************************************/
void score_display(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int score, clock_t start, int history_high)
{
	char temp[256] = {};
	int total_time = (clock() - start) / CLOCKS_PER_SEC;
	/* ��״̬����ʾ���� */
	sprintf(temp, "Ŀ��:%d ����:%d ���:%d ʱ��:%d (R:���� Q:�˳�)", param_2048.goal_score, score, history_high, total_time);
	gmw_status_line(pCGI, TOP_STATUS_LINE, temp);
}

#include <iostream>
#include<cstring>
#include<windows.h>
#include<conio.h>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cmd_console_tools.h"
#include"../include/cmd_gmw_tools.h"
#include"../include/cmd_input.h"
#include"90-02-b1-gmw-tetris.h"
#endif
using namespace std;

/*
* ����5ȫ����
*/
void for_5(CONSOLE_GRAPHICS_INFO* pCGI, int M, int N)
{
	int present_char[5][5] = {};
	int backgd[30][30] = {};
	falling next_num;
	next_num.num = 0;
	next_num.direction = DIRECTION_UP;
	bool first = true;
	int all_times = 0;
	int all_score = 0;
	int if_spilt = T_ACCEPT;
	int pause = 1000;
	while (1)
	{
		falling block;
		int score = 0;
		int times = 0;
		if (first)
			block.num = get_next_num(true, (unsigned)time(0) + 1);
		else
			block.num = next_num.num;
		first = false;
		next_num.num = get_next_num(true, (unsigned)time(0));
		block.direction = DIRECTION_UP;
		show_next_char(pCGI, next_num);
		show_score(pCGI, all_score, all_times, M, if_spilt);
		get_present_char(present_char, block.num, DIRECTION_UP);
		if_spilt = control_fall(pCGI, present_char, backgd, M, N, block, 5, pause);
		if (if_spilt == T_FAILED)
			break;
		if (if_spilt == T_QUIT)
			break;
		all_score += diminish(pCGI, backgd, M, N, times);
		all_times += times;
		pause = get_pause(all_score);
		show_score(pCGI, all_score, all_times, M, if_spilt);
		//cct_showint(M * square_width + score_wide + 6, 6 * square_height + 7, pause);
	}
	show_score(pCGI, all_score, all_times, M, if_spilt);
	while (int u = _getch() != 'q' && if_spilt == T_FAILED)  
		;
}
/*
* һ��������6x3�Ĵ�С�����һ����Ҫ12x6-21x6�Ŀ�ȣ�18x3-26x3�ĸ߶ȡ�
*/

void init_tetris(CONSOLE_GRAPHICS_INFO& tetris_CGI, int N, int M)
{
	cct_setcolor();
	cct_cls();
	cct_setcursor(CURSOR_INVISIBLE);
	gmw_init(&tetris_CGI, N, M);

	gmw_set_ext_rowcol(&tetris_CGI, 0, 0, score_wide, score_wide);
	gmw_set_color(&tetris_CGI, COLOR_BLACK, COLOR_HWHITE);	//����������ɫ
	gmw_set_font(&tetris_CGI, "������", 8);			    //����
	gmw_set_frame_style(&tetris_CGI, square_width, square_height, true);	//��Ϸ��������ÿ��ɫ���6��3���зָ���
	gmw_set_frame_default_linetype(&tetris_CGI, 2);					//��Ϸ���������ͣ�����
	gmw_set_frame_color(&tetris_CGI, COLOR_HWHITE, COLOR_BLACK);		//��Ϸ��������ɫ
	gmw_set_block_border_switch(&tetris_CGI, true);							//ÿ����Ϸɫ����Ҫ�߿�
	gmw_set_status_line_switch(&tetris_CGI, LOWER_STATUS_LINE);			//��Ҫ��״̬��
	gmw_set_status_line_switch(&tetris_CGI, TOP_STATUS_LINE);			//��Ҫ��״̬��
	tetris_CGI.lines -= 2;
}
int main()
{
	while (1)
	{
		int present_char[5][5] = {};
		int backgd[30][30] = {};
		cct_setcolor();
		cct_cls();
		cct_setconsoleborder(80, 30);
		cct_setfontsize("������", 28);
		int cmd = menu(
			"1.��������Ϸ\n(ÿ�ֽ���ʱ���谴Сдq�����˳�,"
		    "��;�˳���Ҫ��Сдq�ٰ�����������˳�����Сдp��ͣ"
			"��֮������̫С�����ڴ���ʾ)\n"
			"0.�˳���Ϸ\n", 2, "teris");
		if (cmd == 0)
			break;
		int M, N;
		CONSOLE_GRAPHICS_INFO tetris_CGI;
		FrameSize(M, N);
		init_tetris(tetris_CGI, N, M);
		gmw_draw_frame(&tetris_CGI);
		if (cmd == 1)
			for_5(&tetris_CGI, M, N);
	}
	cct_setcolor();
	cct_cls();
	return 0;
}

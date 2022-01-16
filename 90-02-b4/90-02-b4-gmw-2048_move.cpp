#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<fstream>
#include<cstring>
#include<windows.h>
#include<conio.h>
#include<sstream>
#ifndef LOVELYBOY_H_
#define LOVELYBOY_H_
#include"../include/cfg_file_tools.h"
#include"../include/cmd_console_tools.h"
#include"../include/cmd_gmw_tools.h"
#include"90-02-b4-gmw-2048.h"
#endif
const int default_rows = 4;
const int default_cols = 4;
const int default_delay = 0;
const int default_score_goal = 512;
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
BLOCK_DISPLAY_INFO* file_choice(CONSOLE_GRAPHICS_INFO* pCGI, board_param& param_2048, char* file_name)
{
	CFT cfcg;
	cfcg.open(file_name, OPEN_OPT_RDONLY);
	if (!cfcg.is_open())
	{
		cout << "无法打开配置文件(" << file_name << ")" << endl;
		return NULL;
	}
	if (cfcg.item_get_value("区域设置", "行数", param_2048.rows) == 0 || param_2048.rows < 4 || param_2048.rows>8)
		param_2048.rows = 4;
	if (cfcg.item_get_value("区域设置", "列数", param_2048.cols) == 0 || param_2048.cols < 4 || param_2048.cols>10)
		param_2048.cols = 4;
	gmw_init(pCGI, param_2048.rows, param_2048.cols);
	pCGI->lines -= 3;

	int up_lines = 0, down_lines = 0, left_cols = 0, right_cols = 0;
	if (cfcg.item_get_value("区域设置", "起始行号", up_lines) == 0)
		up_lines = 0;
	if (cfcg.item_get_value("区域设置", "起始列号", left_cols) == 0)
		left_cols = 0;
	if (cfcg.item_get_value("区域设置", "下方空行", down_lines) == 0)
		down_lines = 0;
	if (cfcg.item_get_value("区域设置", "右侧空列", right_cols) == 0)
		right_cols = 0;
	gmw_set_ext_rowcol(pCGI, up_lines, down_lines, left_cols, right_cols);

	int bgcolor = 0, fgcolor = 7;
	if (cfcg.item_get_value("区域设置", "背景色", bgcolor) == 0 || bgcolor < 0 || bgcolor>15)
		bgcolor = 0;
	if (cfcg.item_get_value("区域设置", "前景色", fgcolor) == 0 || fgcolor < 0 || fgcolor>15)
		fgcolor = 7;
	gmw_set_color(pCGI, bgcolor, fgcolor);			//整个窗口颜色

	string font = "点阵字体";
	int font_size = 8;
	if (cfcg.item_get_value("区域设置", "字体", font) == 0 || (font != "新宋体" && font != "点阵字体"))
		font = "点阵字体";
	if (cfcg.item_get_value("区域设置", "字号", font_size) == 0)
	{
		if (font == "点阵字体")
			font_size = 8;
		else if (font == "新宋体")
			font_size = 12;
	}
	else
	{
		if (font == "点阵字体" && font_size != 6 && font_size != 8 && font_size != 10)
			font_size = 8;
		else if (font == "新宋体" && (font_size % 2 == 1 || font_size < 8 || font_size>24))
			font_size = 12;
	}
	if (font == "点阵字体")
		font = "Terminal";
	gmw_set_font(pCGI, font.c_str(), font_size);						//字体

	int draw_frame_delay = 0, move_block_delay=3;
	if (cfcg.item_get_value("区域设置", "画边框延时", draw_frame_delay) == 0 || draw_frame_delay < 0 || draw_frame_delay>100)
		draw_frame_delay = 0;
	if (cfcg.item_get_value("区域设置", "色块移动延时", move_block_delay) == 0 || move_block_delay < 0 || move_block_delay>100)
		move_block_delay = 3;
	pCGI->delay_of_block_moved = move_block_delay;
	gmw_set_delay(pCGI, DELAY_OF_DRAW_FRAME, draw_frame_delay);
	gmw_set_delay(pCGI, DELAY_OF_DRAW_BLOCK, 0);

	if (cfcg.item_get_value("区域设置", "合成目标", param_2048.goal_score) == 0)
		param_2048.goal_score = 512;
	int flag = 0;
	for (int i = 9; i <= 17; i++)
	{
		if ((param_2048.goal_score - pow(2, i)) < 1e-6)
			flag = 1;
	}
	if (flag == 0)
		param_2048.goal_score = 512;

	/*; ╔ ═ ╦ ═ ╗
		║    ║    ║
		╠    ╬    ╣
		║    ║    ║
		╚ ═ ╩ ═ ╝*/
	string top_left, lower_left, top_right, lower_right, h_normal, v_normal,
		h_top_separator, h_lower_separator, v_left_separator, v_right_separator, mid_separator;
	if (cfcg.item_get_value("边框设置", "边框线-左上角", top_left) == 0)
		top_left = "╔";
	if (cfcg.item_get_value("边框设置", "边框线-左下角", lower_left) == 0)
		lower_left = "╚";
	if (cfcg.item_get_value("边框设置", "边框线-右上角", top_right) == 0)
		top_right = "╗";
	if (cfcg.item_get_value("边框设置", "边框线-右下角", lower_right) == 0)
		lower_right = "╝";
	if (cfcg.item_get_value("边框设置", "边框线-横线", h_normal) == 0)
		h_normal = "═";
	if (cfcg.item_get_value("边框设置", "边框线-竖线", v_normal) == 0)
		v_normal = "║";
	if (cfcg.item_get_value("边框设置", "边框线-上分隔线", h_top_separator) == 0)
		h_top_separator = "╦";
	if (cfcg.item_get_value("边框设置", "边框线-下分隔线", h_lower_separator) == 0)
		h_lower_separator = "╩";
	if (cfcg.item_get_value("边框设置", "边框线-左分隔线", v_left_separator) == 0)
		v_left_separator = "╠";
	if (cfcg.item_get_value("边框设置", "边框线-右分隔线", v_right_separator) == 0)
		v_right_separator = "╣";
	if (cfcg.item_get_value("边框设置", "边框线-中分隔线", mid_separator) == 0)
		mid_separator = "╬";

	gmw_set_frame_linetype(pCGI, top_left.c_str(), lower_left.c_str(), top_right.c_str(),
		lower_right.c_str(), h_normal.c_str(), v_normal.c_str(), h_top_separator.c_str(),
		h_lower_separator.c_str(), v_left_separator.c_str(), v_right_separator.c_str(), mid_separator.c_str());

	char need_block_separator = 'y', show_line_col_num = 'y';
	bool separator = true, No_show = true;
	if (cfcg.item_get_value("边框设置", "需要分隔线", need_block_separator) == 0)
		separator = true;
	else
	{
		if (need_block_separator == 'N' || need_block_separator == 'n')
			separator = false;
		else
			separator = true;
	}

	if (cfcg.item_get_value("边框设置", "行号列标显示", show_line_col_num) == 0)
		No_show = true;
	else
	{
		if (show_line_col_num == 'N' || show_line_col_num == 'n')
			No_show = false;
		else if (show_line_col_num != 'n')
			No_show = true;//缺省
	}
	gmw_set_rowno_switch(pCGI, No_show);
	gmw_set_colno_switch(pCGI, No_show);

	int width = 12, height = 5;
	if (cfcg.item_get_value("色块设置", "宽度", width) == 0 || width < 12 || width>15)
		width = 12;
	if (cfcg.item_get_value("色块设置", "高度", height) == 0 || height < 5 || height >7)
		height = 5;
	gmw_set_frame_style(pCGI, width, height, separator);
	//游戏主区域风格：每个色块宽12高5，有分隔线【数字色块带边框，宽度为10(放最多6位数字)，高度为5(为了保持色块为方形)】

	gmw_set_block_border_switch(pCGI, true);
	string b_top_left, b_lower_left, b_top_right, b_lower_right, b_h_normal, b_v_normal;
	if (cfcg.item_get_value("色块设置", "边框线-左上角", b_top_left) == 0)
		b_top_left = "╔";
	if (cfcg.item_get_value("色块设置", "边框线-左下角", b_lower_left) == 0)
		b_lower_left = "╚";
	if (cfcg.item_get_value("色块设置", "边框线-右上角", b_top_right) == 0)
		b_top_right = "╗";
	if (cfcg.item_get_value("色块设置", "边框线-右下角", b_lower_right) == 0)
		b_lower_right = "╝";
	if (cfcg.item_get_value("色块设置", "边框线-横线", b_h_normal) == 0)
		b_h_normal = "═";
	if (cfcg.item_get_value("色块设置", "边框线-竖线", b_v_normal) == 0)
		b_v_normal = "║";
	gmw_set_block_linetype(pCGI, b_top_left.c_str(), b_lower_left.c_str(), b_top_right.c_str(), b_lower_right.c_str(), b_h_normal.c_str(), b_v_normal.c_str());

	char upper_status = 'y', lower_status = 'n';
	bool upper_status_bool = true, lower_status_bool = false;
	if (cfcg.item_get_value("状态栏设置", "上状态栏", upper_status) == 0)
		upper_status_bool = true;
	else
	{
		if (upper_status == 'N' || upper_status == 'n')
			upper_status_bool = false;
		else
			upper_status_bool = true;
	}

	if (cfcg.item_get_value("状态栏设置", "下状态栏", lower_status) == 0)
		lower_status_bool = false;
	else
	{
		if (lower_status == 'Y' || lower_status == 'y')
			lower_status_bool = true;
		else
			lower_status_bool = false;
	}

	int normal_char_fgcolor = -1, normal_char_bgcolor = -1, catchy_char_fgcolor = 14, catchy_char_bgcolor = -1;
	if (cfcg.item_get_value("状态栏设置", "正常文字背景色", normal_char_bgcolor) == 0 || normal_char_bgcolor < -1 || normal_char_bgcolor>15)
		normal_char_bgcolor = -1;
	if (cfcg.item_get_value("状态栏设置", "正常文字前景色", normal_char_fgcolor) == 0 || normal_char_fgcolor < -1 || normal_char_fgcolor>15)
		normal_char_fgcolor = -1;
	if (cfcg.item_get_value("状态栏设置", "错误文字背景色", catchy_char_bgcolor) == 0 || catchy_char_bgcolor < -1 || catchy_char_bgcolor>15)
		catchy_char_bgcolor = -1;
	if (cfcg.item_get_value("状态栏设置", "错误文字前景色", catchy_char_fgcolor) == 0 || catchy_char_fgcolor < -1 || catchy_char_fgcolor>15)
		catchy_char_fgcolor = 14;
	gmw_set_status_line_color(pCGI, TOP_STATUS_LINE, normal_char_bgcolor, normal_char_fgcolor, catchy_char_bgcolor, catchy_char_fgcolor);
	gmw_set_status_line_color(pCGI, LOWER_STATUS_LINE, normal_char_bgcolor, normal_char_fgcolor, catchy_char_bgcolor, catchy_char_fgcolor);

	int block_bgcolor[19] = {}, block_fgcolor[19] = {};
	for (int i = 1; i < 18; i++)
	{
		int real_i = (i - 1) % 10 + 1;
		int defa = 0;
		if (real_i >= 1 && real_i <= 5)
			defa = real_i;
		else if (real_i >= 6 && real_i <= 10)
			defa = real_i + 3;
		stringstream sfg, sbg;
		sfg << "色块" << real_i << "-前景色";
		sbg << "色块" << real_i << "-背景色";

		if (cfcg.item_get_value("色块设置", sfg.str().c_str(), block_fgcolor[i]) == 0 || block_fgcolor[i] < 0 || block_fgcolor[i]>15)
			block_fgcolor[i] = 0;
		if (cfcg.item_get_value("色块设置", sbg.str().c_str(), block_bgcolor[i]) == 0 || block_bgcolor[i] < 0 || block_bgcolor[i]>15)
			block_bgcolor[i] = defa;
	}
	BLOCK_DISPLAY_INFO* bdi_2048 = new(nothrow)BLOCK_DISPLAY_INFO[19]{
	    {BDI_VALUE_BLANK, -1, -1, NULL},  //0不显示，用空格填充即可
	    {2, block_bgcolor[1], block_fgcolor[1], NULL},
	    {4, block_bgcolor[2], block_fgcolor[2], NULL},
	    {8, block_bgcolor[3], block_fgcolor[3], NULL},
	    {16, block_bgcolor[4], block_fgcolor[4], NULL},
	    {32, block_bgcolor[5], block_fgcolor[5], NULL},
	    {64, block_bgcolor[6], block_fgcolor[6], NULL},
	    {128, block_bgcolor[7], block_fgcolor[7], NULL},
	    {256, block_bgcolor[8], block_fgcolor[8], NULL},
	    {512, block_bgcolor[9], block_fgcolor[9], NULL},
	    {1024, block_bgcolor[10], block_fgcolor[10], NULL},
	    {2048, block_bgcolor[11], block_fgcolor[11], NULL},
	    {4096, block_bgcolor[12], block_fgcolor[12], NULL},
	    {8192, block_bgcolor[13], block_fgcolor[13], NULL},
	    {16384, block_bgcolor[14], block_fgcolor[14], NULL},
	    {32768, block_bgcolor[15], block_fgcolor[15], NULL},
	    {65536, block_bgcolor[16], block_fgcolor[16], NULL},
	    {131072, block_bgcolor[17], block_fgcolor[17], NULL},  //如果开心，还可以继续加
	    {BDI_VALUE_END, -1, -1, NULL} //判断结束条件为-999
	};
	if (bdi_2048 == NULL)
	{
		cout << "内存申请失败" << endl;
		cfcg.close();
		return NULL;
	}
	gmw_draw_frame(pCGI);                                 // 画出来
	cfcg.close();
	return bdi_2048;
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

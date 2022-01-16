#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <climits>
#include <conio.h>
#include<windows.h>  
#include<ctime>
#include "../include/cmd_console_tools.h" 
#include "../include/cmd_gmw_tools.h"
using namespace std;

/* --------------------------------------------------
		此处可以给出需要的静态全局变量（尽可能少，最好没有）、静态全局只读变量/宏定义（个数不限）等
   -------------------------------------------------- */
const int onechar_right = 2;
const int onechar_down = 1;

/* --------------------------------------------------
		此处可以给出需要的内部辅助工具函数
		1、函数名不限，建议为 gmw_inner_*
		2、个数不限
		3、必须是static函数，确保只在本源文件中使用
   -------------------------------------------------- */

static void zy_strcpy(char* destine, const char* source)
{
	if (strlen(source) >= 2)
	{
		destine[0] = source[0];
		destine[1] = source[1];
		destine[2] = '\0';
	}
	else if (strlen(source) == 1)
	{
		destine[0] = source[0];
		destine[1] = ' ';
		destine[2] = '\0';
	}
	else
	{
		destine[0] = ' ';
		destine[1] = ' ';
		destine[2] = '\0';
	}
}

static void init_array(char* arr, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
		arr[i] = 0;
}

/*返回值代表src半个中文字符的个数*/
int gmw_inner_Mar_num(const char* src, int maxlen)
{
	int count = 0;
	for (int i = 0; i < maxlen; i++)
	{
		if ((unsigned char)src[i] >= 128U)
			count++;
	}
	return count;
}
/*返回值代表dst占用了多少空间*/  
int gmw_inner_slice(char* dst, const char* src, int maxlen)
{
	if (src == NULL)
		return 0;// catch_msg_len相当于为0 
	int take_up = 0;
	if (maxlen > 0)
	{
		if ((int)strlen(src) > maxlen)
		{
			int count = gmw_inner_Mar_num(src, maxlen);  
			if (count % 2 != 0)
			{
				strncpy(dst, src, maxlen - 1);
				take_up = maxlen - 1;
			}
			else
			{
				strncpy(dst, src, maxlen);
				take_up = maxlen;
			}
		}
		else
		{
			strncpy(dst, src, maxlen);
			take_up = strlen(src);  
		}
	}
	return take_up;
}

/*调整窗口大小*/
static void adjust_cmd(CONSOLE_GRAPHICS_INFO* const pCGI)
{
	/*上状态栏位置*/
	pCGI->SLI.top_start_x = pCGI->start_x;
	pCGI->SLI.top_start_y = pCGI->start_y;
	/*下状态栏位置*/
	pCGI->SLI.lower_start_x = pCGI->start_x;
	pCGI->SLI.lower_start_y = pCGI->start_y + pCGI->row_num * pCGI->CFI.bhigh + (pCGI->lower_status_line==true) + (pCGI->draw_frame_with_col_no) + (pCGI->top_status_line) + (pCGI->CFI.separator == false);
	/*状态栏宽度*/
	pCGI->SLI.width = pCGI->col_num * pCGI->CFI.bwidth + 2 * (pCGI->CFI.separator == false) + 2 * (pCGI->draw_frame_with_row_no) + onechar_right/*左边框空出*/;


	pCGI->lines = pCGI->CFI.bhigh * pCGI->row_num + (pCGI->top_status_line == 1) +
		(pCGI->lower_status_line == 1) + 1 + 4/*最下方必须空行*/ +
		pCGI->extern_up_lines + pCGI->extern_down_lines + (pCGI->draw_frame_with_col_no == 1) + (pCGI->CFI.separator == false);
	pCGI->cols = pCGI->CFI.bwidth * pCGI->col_num +
		1 + 2 /*最右方必空空间但没有算在这里*/ +
		pCGI->extern_left_cols + pCGI->extern_right_cols + 2 * (pCGI->draw_frame_with_row_no == 1) + 2 * (pCGI->CFI.separator == false);
}

static char trans_row_num(int j)
{
	if (j <= 'Z' - 'A')
		return j + 'A';
	else if (j > 'Z' - 'A' && j <= 'Z' - 'A' + 'z' - 'a')
		return j - ('Z' - 'A' + 1) + 'a';
	else
		return '*';
}
/*width==-1代表顶格写*/
static void gmw_inner_detect_size(const BLOCK_DISPLAY_INFO bdi_one, int& height, int& width)
{
	int n = bdi_one.value;
	height = 1;
	width = 1;
	if (bdi_one.content != NULL)
	{
		for (int i = 0; i < (int)strlen(bdi_one.content); i++)
		{
			if (bdi_one.content[i] == '\n')
				++height;
		}
		if (height > 1)
			width = -1;
		width = (int)strlen(bdi_one.content);
	}
	else
	{
		while (n > 10)
		{
			n = n / 10;
			++width;
		}
	}
}

static void gmw_inner_draw_temp_block(const CONSOLE_GRAPHICS_INFO* const pCGI, const int start_row_no, const int start_col_no, const int bdi_value, const BLOCK_DISPLAY_INFO* const bdi, const int blank_bdi_value, const int horizontal, const int vertical)
{
	int q = 0;
	int num = 0;
	while (bdi[q].value != BDI_VALUE_END)
	{
		if (bdi[q].value == bdi_value)
		{
			num = q;
			break;
		}
		else
			q++;
	}
	int height = 0;
	int width = 0;
	if (bdi[num].value != blank_bdi_value)
		gmw_inner_detect_size(bdi[num], height, width);
	int start_frame_x = pCGI->start_x + (pCGI->draw_frame_with_row_no == true) * 2;
	int start_frame_y = pCGI->start_y + (pCGI->top_status_line == true) + (pCGI->draw_frame_with_col_no == true);
	int block_bgcolor = bdi[num].bgcolor == -1 ? pCGI->CFI.bgcolor : bdi[num].bgcolor;
	int block_fgcolor = bdi[num].fgcolor == -1 ? pCGI->CFI.fgcolor : bdi[num].fgcolor;
	if (pCGI->CBI.block_border && bdi[num].value != blank_bdi_value)
	{
		Sleep(pCGI->delay_of_draw_block);
		/*╔*/
		cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
			start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + vertical,
			pCGI->CBI.top_left, block_bgcolor, block_fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		/*═*/
		for (int i = 0; i < pCGI->CFI.block_width / 2 - 2; i++)
		{
			cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (i + 1) + horizontal,
				start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + vertical,
				pCGI->CBI.h_normal, block_bgcolor, block_fgcolor);
		}
		Sleep(pCGI->delay_of_draw_block);
		/*╗*/
		cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (pCGI->CFI.block_width / 2 - 1) + horizontal,
			start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + vertical,
			pCGI->CBI.top_right, block_bgcolor, block_fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		for (int j = 0; j < pCGI->CFI.block_high - 2; j++)
		{
			/*║*/
			cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
				start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (j + 1) + vertical,
				pCGI->CBI.v_normal, block_bgcolor, block_fgcolor);
			Sleep(pCGI->delay_of_draw_block);
			/* */
			if (height != 1 && height != 0 && bdi[num].value != blank_bdi_value)//这里保证所有字符串填满
			{
				cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + horizontal,
					start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
					bdi[num].content, block_bgcolor, block_fgcolor);
				j = j + height - 1;
			}
			for (int i = 0; i < pCGI->CFI.block_width - 4; i++)
			{
				if (i % 2 == 1)
					Sleep(pCGI->delay_of_draw_block);
				if (height == 1 && j == ((pCGI->CFI.block_high - 2) / 2) && i == (pCGI->CFI.block_width - 4) / 2 - (width + 1) / 2 && bdi[num].value != blank_bdi_value)
				{
					if (bdi[num].content != NULL)
						cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + i + horizontal,
							start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
							bdi[num].content, block_bgcolor, block_fgcolor);
					else if (bdi[num].content == NULL)
						cct_showint(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + i + horizontal,
							start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
							bdi[num].value, block_bgcolor, block_fgcolor);
					if (width % 2 != 0)
					{
						cout << ' ' << endl;
						i += width;
					}
					else
						i += width - 1;

				}
				else
					cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + i + horizontal,
						start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
						" ", block_bgcolor, block_fgcolor);
			}
			Sleep(pCGI->delay_of_draw_block);
			cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (pCGI->CFI.block_width / 2 - 1) + horizontal,
				start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (j + 1) + vertical,
				pCGI->CBI.v_normal, block_bgcolor, block_fgcolor);
		}
		Sleep(pCGI->delay_of_draw_block);
		/*╚*/
		cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
			start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (pCGI->CFI.block_high - 1) + vertical,
			pCGI->CBI.lower_left, block_bgcolor, block_fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		/*═*/
		for (int i = 0; i < pCGI->CFI.block_width / 2 - 2; i++)
		{
			cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (i + 1) + horizontal,
				start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (pCGI->CFI.block_high - 1) + vertical,
				pCGI->CBI.h_normal, block_bgcolor, block_fgcolor);
		}
		Sleep(pCGI->delay_of_draw_block);
		/*╝*/
		cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (pCGI->CFI.block_width / 2 - 1) + horizontal,
			start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (pCGI->CFI.block_high - 1) + vertical,
			pCGI->CBI.lower_right, block_bgcolor, block_fgcolor);
	}
	else
	{
		for (int j = 0; j < pCGI->CFI.block_high; j++)
		{
			if (height != 1 && height != 0 && bdi[num].value != blank_bdi_value)//这里保证所有字符串填满
			{
				cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
					start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
					bdi[num].content, block_bgcolor, block_fgcolor);
				j = j + height - 1;
			}
			for (int i = 0; i < pCGI->CFI.block_width; i++)
			{
				if (i % 2 == 1)
					Sleep(pCGI->delay_of_draw_block);
				if (height == 1 && j == (pCGI->CFI.block_high / 2) && i == pCGI->CFI.block_width / 2 - (width + 1) / 2 && bdi[num].value != blank_bdi_value)
				{
					if (bdi[num].content != NULL)
						cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + i + horizontal,
							start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
							bdi[num].content, block_bgcolor, block_fgcolor);
					else if (bdi[num].content == NULL)
						cct_showint(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + i + horizontal,
							start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
							bdi[num].value, block_bgcolor, block_fgcolor);
					if (width % 2 != 0)
					{
						cout << ' ' << endl;
						i += width;
					}
					else
						i += width - 1;

				}
				else
					cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + i + horizontal,
						start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
						" ", block_bgcolor, block_fgcolor);
			}
		}
	}
	return;
}

static void gmw_inner_erase_temp_block(const CONSOLE_GRAPHICS_INFO* const pCGI, const int start_row_no, const int start_col_no, const int horizontal, const int vertical)
{
	int start_frame_x = pCGI->start_x + (pCGI->draw_frame_with_row_no == true) * 2;
	int start_frame_y = pCGI->start_y + (pCGI->top_status_line == true) + (pCGI->draw_frame_with_col_no == true);
	for (int j = 0; j < pCGI->CFI.block_high; j++)
	{
		for (int i = 0; i < pCGI->CFI.block_width / 2; i++)
		{
			if ((onechar_right * (i + 1) + horizontal) % pCGI->CFI.bwidth == 0 && pCGI->CFI.separator == 1)
				cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i + horizontal,
					start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			else if ((onechar_right * i + 1 + horizontal) % pCGI->CFI.bwidth == 0 && pCGI->CFI.separator == 1)
			{
				cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i - 1 + horizontal,
					start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				cout << ' ';
			}
			else if ((onechar_down * (j + 1) + vertical) % pCGI->CFI.bhigh == 0 && pCGI->CFI.separator == 1)
				cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i + horizontal,
					start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			else
				cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i + horizontal,
					start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					"  ", pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		}
		if (pCGI->CFI.separator == 1 && horizontal < 0 && (horizontal - 1) % pCGI->CFI.bwidth == 0)
		{
			cct_showstr(start_frame_x + onechar_right/*左边框空下来*/ + start_col_no * pCGI->CFI.bwidth + pCGI->CFI.bwidth - onechar_right - 1 + horizontal,
				start_frame_y + onechar_down/*上边框空下来*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
				pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			cout << ' ';
		}

	}
}
/* -----------------------------------------------
		实现下面给出的函数（函数声明不准动）
   ----------------------------------------------- */
   /***************************************************************************
	 函数名称：
	 功    能：设置游戏主框架的行列数
	 输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			   const int row						：行数(错误则为0，不设上限，人为保证正确性)
			   const int col						：列数(错误则为0，不设上限，人为保证正确性)
	 返 回 值：
	 说    明：1、指消除类游戏的矩形区域的行列值
			   2、行列的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
   ***************************************************************************/
int gmw_set_rowcol(CONSOLE_GRAPHICS_INFO* const pCGI, const int row, const int col)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	pCGI->row_num = row > 0 ? row : 0;
	pCGI->col_num = col > 0 ? col : 0;

	adjust_cmd(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置整个窗口（含游戏区、附加区在内的整个cmd窗口）的颜色
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int bg_color					：前景色（缺省COLOR_BLACK）
		   const int fg_color					：背景色（缺省COLOR_WHITE）
		   const bool cascade					：是否级联（缺省为true-级联）
  返 回 值：
  说    明：1、cascade = true时
				同步修改游戏主区域的颜色
				同步修改上下状态栏的正常文本的背景色和前景色，醒目文本的背景色（前景色不变）
			2、不检查颜色值错误及冲突，需要人为保证
				例：颜色非0-15
					前景色背景色的值一致导致无法看到内容
					前景色正好是状态栏醒目前景色，导致无法看到醒目提示
					...
***************************************************************************/
int gmw_set_color(CONSOLE_GRAPHICS_INFO* const pCGI, const int bgcolor, const int fgcolor, const bool cascade)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->area_bgcolor = bgcolor;
	pCGI->area_fgcolor = fgcolor;
	if (cascade == true)
	{
		pCGI->CFI.bgcolor = bgcolor;
		pCGI->CFI.fgcolor = fgcolor;
		pCGI->SLI.lower_normal_bgcolor = bgcolor;
		pCGI->SLI.lower_normal_fgcolor = fgcolor;
		pCGI->SLI.lower_catchy_bgcolor = bgcolor;
		pCGI->SLI.top_normal_bgcolor = bgcolor;
		pCGI->SLI.top_normal_fgcolor = fgcolor;
		pCGI->SLI.top_catchy_bgcolor = bgcolor;
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置窗口的字体
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const char *fontname					：字体名称（只能是"Terminal"和"新宋体"两种，错误则返回-1，不改变字体）
		   const int fs_high					：字体高度（缺省及错误为16，不设其它限制，人为保证）
		   const int fs_width					：字体高度（缺省及错误为8，不设其它限制，人为保证）
  返 回 值：
  说    明：1、与cmd_console_tools中的setfontsize相似，目前只支持“点阵字体”和“新宋体”
			2、若设置其它字体则直接返回，保持原字体设置不变
***************************************************************************/
int gmw_set_font(CONSOLE_GRAPHICS_INFO* const pCGI, const char* fontname, const int fs_high, const int fs_width)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	/*字体大小*/
	pCGI->CFT.font_size_high = fs_high > 0 ? fs_high : 16;
	pCGI->CFT.font_size_width = fs_width > 0 ? fs_width : 8;
	/*字体类型*/
	if (strcmp(fontname, "Terminal") == 0 || strcmp(fontname, "新宋体") == 0)
	{
		for (int i = 0;; i++)
		{
			pCGI->CFT.font_type[i] = fontname[i];
			if (fontname[i] == '\0')
				break;
		}
	}
	else
		return -1;

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置延时
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int type						：延时的类型（目前为3种）
		   const int delay_ms					：以ms为单位的延时
			   画边框的延时：0 ~ 不设上限，人为保证正确（<0则置0）
			   画色块的延时：0 ~ 不设上限，人为保证正确（<0则置0）
			   色块移动的延时：BLOCK_MOVED_DELAY_MS ~ 不设上限，人为保证正确（ <BLOCK_MOVED_DELAY_MS 则置 BLOCK_MOVED_DELAY_MS）
  返 回 值：
  说    明：
***************************************************************************/
int gmw_set_delay(CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const int delay_ms)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == DELAY_OF_DRAW_FRAME)
		pCGI->delay_of_draw_frame = delay_ms > 0 ? delay_ms : 0;
	else if (type == DELAY_OF_DRAW_BLOCK)
		pCGI->delay_of_draw_block = delay_ms > 0 ? delay_ms : 0;
	else if (type == DELAY_OF_BLOCK_MOVED)
		pCGI->delay_of_block_moved = delay_ms > BLOCK_MOVED_DELAY_MS ? delay_ms : BLOCK_MOVED_DELAY_MS;
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  输入参数：设置游戏主框架结构之外需要保留的额外区域
  功    能：CONSOLE_GRAPHICS_INFO *const pCGI	：
		   const int up_lines					：上部额外的行（缺省及错误为0，不设上限，人为保证）
		   const int down_lines				：下部额外的行（缺省及错误为0，不设上限，人为保证）
		   const int left_cols					：左边额外的列（缺省及错误为0，不设上限，人为保证）
		   const int right_cols				：右边额外的列（缺省及错误为0，不设上限，人为保证）
  返 回 值：
  说    明：额外行列的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_ext_rowcol(CONSOLE_GRAPHICS_INFO* const pCGI, const int up_lines, const int down_lines, const int left_cols, const int right_cols)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->extern_down_lines = down_lines;
	pCGI->extern_up_lines = up_lines;
	pCGI->extern_left_cols = left_cols;
	pCGI->extern_right_cols = right_cols;

	pCGI->start_x = pCGI->extern_left_cols;
	pCGI->start_y = pCGI->extern_up_lines;

	adjust_cmd(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_FRAME_TYPE 结构中的11种线型（缺省4种）
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：1 - 全双线 2 - 全单线 3 - 横双竖单 4 - 横单竖双
  返 回 值：
  说    明：
***************************************************************************/
int gmw_set_frame_default_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const int type)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == 1)
	{
		zy_strcpy(pCGI->CFI.top_left, "╔");
		zy_strcpy(pCGI->CFI.lower_left, "╚");
		zy_strcpy(pCGI->CFI.top_right, "╗");
		zy_strcpy(pCGI->CFI.lower_right, "╝");
		zy_strcpy(pCGI->CFI.h_normal, "═");
		zy_strcpy(pCGI->CFI.v_normal, "║");
		zy_strcpy(pCGI->CFI.h_top_separator, "╦");
		zy_strcpy(pCGI->CFI.h_lower_separator, "╩");
		zy_strcpy(pCGI->CFI.v_left_separator, "╠");
		zy_strcpy(pCGI->CFI.v_right_separator, "╣");
		zy_strcpy(pCGI->CFI.mid_separator, "╬");
	}
	else if (type == 2)
	{
		zy_strcpy(pCGI->CFI.top_left, "┏");
		zy_strcpy(pCGI->CFI.lower_left, "┗");
		zy_strcpy(pCGI->CFI.top_right, "┓");
		zy_strcpy(pCGI->CFI.lower_right, "┛");
		zy_strcpy(pCGI->CFI.h_normal, "━");
		zy_strcpy(pCGI->CFI.v_normal, "┃");
		zy_strcpy(pCGI->CFI.h_top_separator, "┳");
		zy_strcpy(pCGI->CFI.h_lower_separator, "┻");
		zy_strcpy(pCGI->CFI.v_left_separator, "┣");
		zy_strcpy(pCGI->CFI.v_right_separator, "┫");
		zy_strcpy(pCGI->CFI.mid_separator, "╋");
	}
	else if (type == 3)
	{
		zy_strcpy(pCGI->CFI.top_left, "╒");
		zy_strcpy(pCGI->CFI.lower_left, "╘");
		zy_strcpy(pCGI->CFI.top_right, "╕");
		zy_strcpy(pCGI->CFI.lower_right, "╛");
		zy_strcpy(pCGI->CFI.mid_separator, "╪");
		zy_strcpy(pCGI->CFI.h_normal, "═");
		zy_strcpy(pCGI->CFI.v_normal, "┃");
		zy_strcpy(pCGI->CFI.h_lower_separator, "╧");
		zy_strcpy(pCGI->CFI.v_left_separator, "╞");
		zy_strcpy(pCGI->CFI.h_top_separator, "╤");
		zy_strcpy(pCGI->CFI.v_right_separator, "╡");
	}
	else if (type == 4)
	{
		zy_strcpy(pCGI->CFI.top_left, "╓");
		zy_strcpy(pCGI->CFI.lower_left, "╙");
		zy_strcpy(pCGI->CFI.top_right, "╖");
		zy_strcpy(pCGI->CFI.lower_right, "╜");
		zy_strcpy(pCGI->CFI.mid_separator, "╫");
		zy_strcpy(pCGI->CFI.h_normal, "━");
		zy_strcpy(pCGI->CFI.v_normal, "║");
		zy_strcpy(pCGI->CFI.h_lower_separator, "╨");
		zy_strcpy(pCGI->CFI.v_left_separator, "╟");
		zy_strcpy(pCGI->CFI.h_top_separator, "╥");
		zy_strcpy(pCGI->CFI.v_right_separator, "╢");
	}
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_FRAME_TYPE 结构中的11种线型
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const char *...						：共11种，具体见.h，此处略
  返 回 值：
  说    明：约定为一个中文制表符，可以使用其它内容，人为保证2字节
			1、超过2字节则只取前2字节
			2、如果给NULL，用两个空格替代
			3、如果给1字节，则补一个空格，如果因此而导致显示乱，不算错
***************************************************************************/
int gmw_set_frame_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const char* top_left, const char* lower_left, const char* top_right,
	const char* lower_right, const char* h_normal, const char* v_normal, const char* h_top_separator,
	const char* h_lower_separator, const char* v_left_separator, const char* v_right_separator, const char* mid_separator)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	zy_strcpy(pCGI->CFI.top_left, top_left);
	zy_strcpy(pCGI->CFI.lower_left, lower_left);
	zy_strcpy(pCGI->CFI.top_right, top_right);
	zy_strcpy(pCGI->CFI.lower_right, lower_right);
	zy_strcpy(pCGI->CFI.mid_separator, mid_separator);
	zy_strcpy(pCGI->CFI.h_normal, h_normal);
	zy_strcpy(pCGI->CFI.v_normal, v_normal);
	zy_strcpy(pCGI->CFI.h_lower_separator, h_lower_separator);
	zy_strcpy(pCGI->CFI.v_left_separator, v_left_separator);
	zy_strcpy(pCGI->CFI.h_top_separator, h_top_separator);
	zy_strcpy(pCGI->CFI.v_right_separator, v_right_separator);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_FRAME_TYPE 结构中的色块数量大小、是否需要分隔线等
  输入参数：输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int block_width						：宽度（错误及缺省2，因为约定表格线为中文制表符，如果给出奇数，要+1）
			const int block_high						：高度（错误及缺省1）
			const bool separator						：是否需要分隔线（缺省为true，需要分隔线）
  返 回 值：
  说    明：框架大小/是否需要分隔线等的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_frame_style(CONSOLE_GRAPHICS_INFO* const pCGI, const int block_width, const int block_high, const bool separator)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	if (block_width > 0)
		pCGI->CFI.block_width = block_width % 2 == 0 ? block_width : (block_width + 1);
	else
		pCGI->CFI.block_width = 2;
	pCGI->CFI.block_high = block_high > 0 ? block_high : 1;
	pCGI->CFI.separator = separator;

	if (separator)
	{
		pCGI->CFI.block_width_ext = 2;
		pCGI->CFI.block_high_ext = 1;
	}
	else
	{
		pCGI->CFI.block_width_ext = 0;
		pCGI->CFI.block_high_ext = 0;
	}
	pCGI->CFI.bhigh = pCGI->CFI.block_high + pCGI->CFI.block_high_ext;
	pCGI->CFI.bwidth = pCGI->CFI.block_width + pCGI->CFI.block_width_ext;
	adjust_cmd(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_BORDER_TYPE 结构中的颜色
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int bg_color					：背景色（缺省 -1表示用窗口背景色）
			const int fg_color					：前景色（缺省 -1表示用窗口前景色）
  返 回 值：
  说    明：不检查颜色值错误及冲突，需要人为保证
				例：颜色非0-15，前景色背景色的值一致导致无法看到内容等
***************************************************************************/
int gmw_set_frame_color(CONSOLE_GRAPHICS_INFO* const pCGI, const int bgcolor, const int fgcolor)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (bgcolor == -1)
		pCGI->CFI.bgcolor = pCGI->area_bgcolor;
	else
		pCGI->CFI.bgcolor = bgcolor;
	if (fgcolor == -1)
		pCGI->CFI.fgcolor = pCGI->area_fgcolor;
	else
		pCGI->CFI.fgcolor = fgcolor;
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_BLOCK_INFO 结构中的6种线型（缺省4种）
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：1 - 全双线 2 - 全单线 3 - 横双竖单 4 - 横单竖双
  返 回 值：
  说    明：
***************************************************************************/
int gmw_set_block_default_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const int type)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == 1)
	{
		zy_strcpy(pCGI->CBI.top_left, "╔");
		zy_strcpy(pCGI->CBI.lower_left, "╚");
		zy_strcpy(pCGI->CBI.top_right, "╗");
		zy_strcpy(pCGI->CBI.lower_right, "╝");
		zy_strcpy(pCGI->CBI.h_normal, "═");
		zy_strcpy(pCGI->CBI.v_normal, "║");
	}
	else if (type == 2)
	{
		zy_strcpy(pCGI->CBI.top_left, "┏");
		zy_strcpy(pCGI->CBI.lower_left, "┗");
		zy_strcpy(pCGI->CBI.top_right, "┓");
		zy_strcpy(pCGI->CBI.lower_right, "┛");
		zy_strcpy(pCGI->CBI.h_normal, "━");
		zy_strcpy(pCGI->CBI.v_normal, "┃");
	}
	else if (type == 3)
	{
		zy_strcpy(pCGI->CBI.top_left, "╒");
		zy_strcpy(pCGI->CBI.lower_left, "╘");
		zy_strcpy(pCGI->CBI.top_right, "╕");
		zy_strcpy(pCGI->CBI.lower_right, "╛");
		zy_strcpy(pCGI->CBI.h_normal, "═");
		zy_strcpy(pCGI->CBI.v_normal, "┃");
	}
	else if (type == 4)
	{
		zy_strcpy(pCGI->CBI.top_left, "╓");
		zy_strcpy(pCGI->CBI.lower_left, "╙");
		zy_strcpy(pCGI->CBI.top_right, "╖");
		zy_strcpy(pCGI->CBI.lower_right, "╜");
		zy_strcpy(pCGI->CBI.h_normal, "━");
		zy_strcpy(pCGI->CBI.v_normal, "║");
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_BLOCK_INFO 结构中的6种线型
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const char *...					：共6种，具体见.h，此处略
  返 回 值：
  说    明：约定为一个中文制表符，可以使用其它内容，人为保证2字节
			1、超过2字节则只取前2字节
			2、如果给NULL，用两个空格替代
			3、如果给1字节，则补一个空格，如果因此而导致显示乱，不算错
***************************************************************************/
int gmw_set_block_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const char* top_left, const char* lower_left, const char* top_right, const char* lower_right, const char* h_normal, const char* v_normal)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	zy_strcpy(pCGI->CBI.top_left, top_left);
	zy_strcpy(pCGI->CBI.lower_left, lower_left);
	zy_strcpy(pCGI->CBI.top_right, top_right);
	zy_strcpy(pCGI->CBI.lower_right, lower_right);
	zy_strcpy(pCGI->CBI.h_normal, h_normal);
	zy_strcpy(pCGI->CBI.v_normal, v_normal);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置每个游戏色块(彩球)是否需要小边框
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const bool on_off					：true - 需要 flase - 不需要（缺省false）
  返 回 值：
  说    明：边框约定为中文制表符，双线
***************************************************************************/
int gmw_set_block_border_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->CBI.block_border = on_off;
	pCGI->CBI.pad1 = false;
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置是否显示上下状态栏
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：状态栏类型（上/下）
			const bool on_off					：true - 需要 flase - 不需要（缺省true）
  返 回 值：
  说    明：1、状态栏的相关约定如下：
			   1.1、上状态栏只能一行，在主区域最上方框线/列标的上面，为主区域的最开始一行（主区域的左上角坐标就是上状态栏的坐标）
			   1.2、下状态栏只能一行，在主区域最下方框线的下面
			   1.3、状态栏的宽度为主区域宽度，如果信息过长则截断
		   2、行列的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_status_line_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	if (type == TOP_STATUS_LINE)
	{
		pCGI->SLI.is_top_status_line = on_off;
		pCGI->top_status_line = on_off;
	}
	if (type == LOWER_STATUS_LINE)
	{
		pCGI->SLI.is_lower_status_line = on_off;
		pCGI->lower_status_line = on_off;
	}
	pCGI->SLI.width = pCGI->start_x + pCGI->col_num * pCGI->CFI.bwidth + 1;
	adjust_cmd(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置上下状态栏的颜色
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：状态栏类型（上/下）
			const int normal_bgcolor			：正常文本背景色（缺省 -1表示使用窗口背景色）
			const int normal_fgcolor			：正常文本前景色（缺省 -1表示使用窗口前景色）
			const int catchy_bgcolor			：醒目文本背景色（缺省 -1表示使用窗口背景色）
			const int catchy_fgcolor			：醒目文本前景色（缺省 -1表示使用亮黄色）
  输入参数：
  返 回 值：
  说    明：不检查颜色值错误及冲突，需要人为保证
				例：颜色非0-15，前景色背景色的值一致导致无法看到内容等
***************************************************************************/
int gmw_set_status_line_color(CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const int normal_bgcolor, const int normal_fgcolor, const int catchy_bgcolor, const int catchy_fgcolor)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == LOWER_STATUS_LINE)
	{
		/*醒目前景色*/
		if (catchy_fgcolor == -1)
			pCGI->SLI.lower_catchy_fgcolor = COLOR_HYELLOW;
		else
			pCGI->SLI.lower_catchy_fgcolor = catchy_fgcolor;
		/*醒目背景色*/
		if (catchy_bgcolor == -1)
			pCGI->SLI.lower_catchy_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.lower_catchy_bgcolor = catchy_bgcolor;
		/*正常前景色*/
		if (normal_fgcolor == -1)
			pCGI->SLI.lower_normal_fgcolor = pCGI->area_fgcolor;
		else
			pCGI->SLI.lower_normal_fgcolor = normal_fgcolor;
		/*正常背景色*/
		if (normal_bgcolor == -1)
			pCGI->SLI.lower_normal_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.lower_normal_bgcolor = normal_bgcolor;
	}
	if (type == TOP_STATUS_LINE)
	{
		/*醒目前景色*/
		if (catchy_fgcolor == -1)
			pCGI->SLI.top_catchy_fgcolor = COLOR_HYELLOW;
		else
			pCGI->SLI.top_catchy_fgcolor = catchy_fgcolor;
		/*醒目背景色*/
		if (catchy_bgcolor == -1)
			pCGI->SLI.top_catchy_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.top_catchy_bgcolor = catchy_bgcolor;
		/*正常前景色*/
		if (normal_fgcolor == -1)
			pCGI->SLI.top_normal_fgcolor = pCGI->area_fgcolor;
		else
			pCGI->SLI.top_normal_fgcolor = normal_fgcolor;
		/*正常背景色*/
		if (normal_bgcolor == -1)
			pCGI->SLI.top_normal_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.top_normal_bgcolor = normal_bgcolor;
	}
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置是否显示行号
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const bool on_off					：true - 显示 flase - 不显示（缺省false）
  返 回 值：
  说    明：1、行号约定为字母A开始连续排列（如果超过26，则从a开始，超过52的统一为*，实际应用不可能）
			2、是否显示行号的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_rowno_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	pCGI->draw_frame_with_row_no = on_off;
	adjust_cmd(pCGI);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置是否显示列标
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const bool on_off					：true - 显示 flase - 不显示（缺省false）
  返 回 值：
  说    明：1、列标约定为数字0开始连续排列（数字0-99，超过99统一为**，实际应用不可能）
			2、是否显示列标的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_colno_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	pCGI->draw_frame_with_col_no = on_off;
	adjust_cmd(pCGI);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：打印 CONSOLE_GRAPHICS_INFO 结构体中的各成员值
  输入参数：
  返 回 值：
  说    明：1、仅供调试用，打印格式自定义
			2、本函数测试用例中未调用过，可以不实现
***************************************************************************/
int gmw_print(const CONSOLE_GRAPHICS_INFO* const pCGI)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	cout << "inited : " << pCGI->inited << endl;

	/* 整个图形界面的上下左右需要的额外行列数 */
	cout << "整个图形界面的上下左右需要的额外行列数 : " << endl;
	cout << "extern_up_lines : " << pCGI->extern_up_lines << endl;
	cout << "extern_down_lines : " << pCGI->extern_down_lines << endl;
	cout << "extern_left_cols : " << pCGI->extern_left_cols << endl;
	cout << "extern_right_cols : " << pCGI->extern_right_cols << endl;

	/* 游戏主框架区域包含的色块的行列数 */
	cout << "游戏主框架区域包含的色块的行列数 : " << endl;
	cout << "row_num : " << pCGI->row_num << endl;
	cout << "col_num : " << pCGI->col_num << endl;

	/* 整个图形界面（含主框架、上下状态栏、行号列标、附加行列）的背景色和前景色 */
	cout << "整个图形界面（含主框架、上下状态栏、行号列标、附加行列）的背景色和前景色" << endl;

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：将 CONSOLE_GRAPHICS_INFO 结构体用缺省值进行初始化
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI：整体结构指针
		   const int row					：游戏区域色块行数（缺省10）
		   const int col					：游戏区域色块列数（缺省10）
		   const int bgcolor				：整个窗口背景色（缺省 COLOR_BLACK）
		   const int fgcolor				：整个窗口背景色（缺省 COLOR_WHITE）
  返 回 值：
  说    明：窗口背景黑/前景白，点阵16*8，上下左右无额外行列，上下状态栏均有，无行号/列标，框架线型为双线，色块宽度2/高度1/无小边框，颜色略
***************************************************************************/
int gmw_init(CONSOLE_GRAPHICS_INFO* const pCGI, const int row, const int col, const int bgcolor, const int fgcolor)
{
	/* 首先置标记 */
	pCGI->inited = CGI_INITED;
	/*还要初始化一下*/
	pCGI->row_num = row;
	pCGI->col_num = col;
	pCGI->area_bgcolor = bgcolor;
	pCGI->area_fgcolor = fgcolor;
	pCGI->extern_down_lines = 0;
	pCGI->extern_up_lines = 0;
	pCGI->extern_left_cols = 0;
	pCGI->extern_right_cols = 0;
	pCGI->start_x = pCGI->extern_left_cols;
	pCGI->start_y = pCGI->extern_up_lines;
	pCGI->CFI.block_width_ext = 2;
	pCGI->CFI.block_high_ext = 1;

	pCGI->CFI.block_width = 2;
	pCGI->CFI.block_high = 1;
	pCGI->CFI.separator = true;
	pCGI->CFI.bhigh = pCGI->CFI.block_high + pCGI->CFI.block_high_ext;
	pCGI->CFI.bwidth = pCGI->CFI.block_width + pCGI->CFI.block_width_ext;


	pCGI->draw_frame_with_row_no = false;
	pCGI->draw_frame_with_col_no = false;
	/*有上状态栏位置*/
	pCGI->SLI.is_top_status_line = true;
	pCGI->top_status_line = true;
	/*上状态栏位置*/
	pCGI->SLI.top_start_x = pCGI->start_x;
	pCGI->SLI.top_start_y = pCGI->start_y;

	/*有下状态栏位置*/
	pCGI->SLI.is_lower_status_line = true;
	pCGI->lower_status_line = true;
	/*下状态栏位置*/
	pCGI->SLI.lower_start_x = pCGI->start_x;
	pCGI->SLI.lower_start_y = pCGI->start_y + pCGI->row_num * pCGI->CFI.bhigh + 1 + (pCGI->draw_frame_with_col_no) + (pCGI->top_status_line);
	pCGI->SLI.width = pCGI->start_x + pCGI->col_num * pCGI->CFI.bwidth + 2;

	/*设置整个窗口的某些参数*/

	gmw_set_color(pCGI);

	gmw_set_font(pCGI);/*Console Frame Font对字体的初始化*/

	gmw_set_delay(pCGI, DELAY_OF_DRAW_FRAME, 0);
	gmw_set_delay(pCGI, DELAY_OF_DRAW_BLOCK, 0);
	gmw_set_delay(pCGI, DELAY_OF_BLOCK_MOVED, BLOCK_MOVED_DELAY_MS);


	/*设置整个游戏主框架的参数CFI*/
	gmw_set_frame_default_linetype(pCGI, 1);//全双线

	gmw_set_frame_color(pCGI);

	/*设置每个色块的参数*/
	gmw_set_block_default_linetype(pCGI, 1);
	gmw_set_block_border_switch(pCGI);

	/*设置是否需要上下状态栏及颜色*/

	gmw_set_status_line_color(pCGI, TOP_STATUS_LINE);
	gmw_set_status_line_color(pCGI, LOWER_STATUS_LINE);


	/*最后将一些数组初始化*/
	init_array(pCGI->CFI.pad1, sizeof(pCGI->CFI.pad1) / sizeof(char));
	init_array(pCGI->CFI.pad2, sizeof(pCGI->CFI.pad2) / sizeof(char));
	init_array(pCGI->CFI.pad, sizeof(pCGI->CFI.pad) / sizeof(char));

	init_array(pCGI->CBI.pad, sizeof(pCGI->CBI.pad) / sizeof(char));
	init_array(pCGI->SLI.pad1, sizeof(pCGI->SLI.pad1) / sizeof(char));
	init_array(pCGI->SLI.pad, sizeof(pCGI->SLI.pad) / sizeof(char));

	init_array(pCGI->pad, sizeof(pCGI->pad) / sizeof(char));

	adjust_cmd(pCGI);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：画主游戏框架
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
  返 回 值：
  说    明：具体可参考demo的效果
***************************************************************************/
int gmw_draw_frame(const CONSOLE_GRAPHICS_INFO* const pCGI)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	cct_setfontsize(pCGI->CFT.font_type, pCGI->CFT.font_size_high, pCGI->CFT.font_size_width);
	cct_setconsoleborder(pCGI->cols, pCGI->lines, pCGI->cols, pCGI->lines);
	cct_setcolor(pCGI->area_bgcolor, pCGI->area_fgcolor);
	cct_cls();
	int start_frame_x = pCGI->start_x + (pCGI->draw_frame_with_row_no == true) * 2;
	int start_frame_y = pCGI->start_y + (pCGI->top_status_line == true) + (pCGI->draw_frame_with_col_no == true);
	if (pCGI->draw_frame_with_col_no)
		for (int i = 0; i < pCGI->col_num; i++)
		{
			if (i <= 99)
				cct_showint(start_frame_x + pCGI->CFI.bwidth * i + (int)(pCGI->CFI.block_width * 0.5) - 1/*左移一位*/ + onechar_right/*左边框空出*/,
					start_frame_y - 1, i, pCGI->area_bgcolor, pCGI->area_fgcolor);
			else
				cct_showstr(start_frame_x + pCGI->CFI.bwidth * i + (int)(pCGI->CFI.block_width * 0.5) - 1/*左移一位*/ + onechar_right/*左边框空出*/,
					start_frame_y - 1, "**", pCGI->area_bgcolor, pCGI->area_fgcolor);
		}
	for (int j = 0; j < pCGI->row_num; j++)
	{
		for (int i = 0; i < pCGI->col_num; i++)
		{
			/*
			  ╔
			  ║
			  ╟
			*/
			if (i == 0)
			{
				if (j == 0)/*╔*/
					cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.top_left, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				else if (pCGI->CFI.separator == 1)/*╟*/
					cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.v_left_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			}
			Sleep(pCGI->delay_of_draw_frame);

			/*═ ═ ═*/
			if (pCGI->CFI.separator == 1 || j == 0)
				for (int k = 0; k < pCGI->CFI.block_width / 2; k++)
					cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + onechar_right * k, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);

			/*═ ╤ ═ ╗*/
			if (i == pCGI->col_num - 1)
			{
				if (j == 0)/*╗*/
					cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.top_right, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				else if (pCGI->CFI.separator == 1)/*╢*/
					cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.v_right_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			}
			else if (i != pCGI->col_num - 1)
			{
				if (j == 0 && (pCGI->CFI.separator == 1))/*╤*/
					cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.h_top_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				else if (pCGI->CFI.separator == 1)/*┼*/
					cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.mid_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			}

		}
		for (int k = 0; k < pCGI->CFI.block_high; k++)
			for (int i = 0; i < pCGI->col_num; i++)
			{
				/* 	║          ║
					║          ║ */
				if (i == 0)
				{
					if (pCGI->draw_frame_with_row_no && (k == (pCGI->CFI.block_high-1)/ 2))
						cct_showch(start_frame_x - 2, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * j + onechar_down * k, trans_row_num(j), pCGI->area_bgcolor, pCGI->area_fgcolor);
					cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * j + onechar_down * k, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				}
				//Sleep(pCGI->delay_of_draw_frame);
				for (int k2 = 0; k2 < pCGI->CFI.block_width / 2; k2++)
					cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + onechar_right * k2, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * j + onechar_down * k, "  ", pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				if (pCGI->CFI.separator == 1 || i == pCGI->col_num - 1)
				{
					Sleep(pCGI->delay_of_draw_frame);
					cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * j + onechar_down * k, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				}

			}
	}
	for (int i = 0; i < pCGI->col_num; i++)/*只有╧要特殊处理*/
	{
		if (i == 0)/*╚*/
			cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.lower_left, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		Sleep(pCGI->delay_of_draw_frame);
		for (int k = 0; k < pCGI->CFI.block_width / 2; k++)/*═*/
			cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + onechar_right * k, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		if ((i != pCGI->col_num - 1) && (pCGI->CFI.separator == 1))/*╧*/
			cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.h_lower_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		else if (i == pCGI->col_num - 1)/*╝*/
			cct_showstr(start_frame_x + onechar_right/*左边框空出*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + onechar_down/*上边框空出*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.lower_right, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
	}
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：在状态栏上显示信息
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int type							：指定是上/下状态栏
		   const char *msg						：正常信息
		   const char *catchy_msg					：需要特别标注的信息（在正常信息前显示）
  返 回 值：
  说    明：1、输出宽度限定为主框架的宽度（含行号列标位置），超出则截去
			2、如果最后一个字符是某汉字的前半个，会导致后面乱码，要处理
***************************************************************************/
int gmw_status_line(const CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const char* msg, const char* catchy_msg)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	//cct_setfontsize(pCGI->CFT.font_type, pCGI->CFT.font_size_high, pCGI->CFT.font_size_width);

	char* my_catchy_msg = new(nothrow) char[pCGI->SLI.width + 5]{};//申请，初始化
	if (my_catchy_msg == NULL)//判断
		return -2;
	char* my_msg = new(nothrow) char[pCGI->SLI.width + 5]{};//申请，初始化
	if (my_msg == NULL)//判断
		return -2;

	int catchy_msg_len = (catchy_msg == NULL) ? 0 : (int)strlen(catchy_msg);
	int msg_len = (msg == NULL) ? 0 : (int)strlen(msg);

	int take_up = 0;
	bool catchy_full = false;
	take_up = gmw_inner_slice(my_catchy_msg, catchy_msg, pCGI->SLI.width);
	gmw_inner_slice(my_msg, msg, pCGI->SLI.width - take_up);


	if (pCGI->top_status_line == 1 && type == TOP_STATUS_LINE)
	{
		cct_showstr(pCGI->SLI.top_start_x, pCGI->SLI.top_start_y, my_catchy_msg, pCGI->SLI.top_catchy_bgcolor, pCGI->SLI.top_catchy_fgcolor);//, 1, pCGI->SLI.width
		if (pCGI->SLI.width - take_up > 0)
			cct_showstr(pCGI->SLI.top_start_x + take_up, pCGI->SLI.top_start_y, my_msg, pCGI->SLI.top_normal_bgcolor, pCGI->SLI.top_normal_fgcolor, 1, pCGI->SLI.width - take_up);//pCGI->SLI.top_normal_bgcolor, pCGI->SLI.top_normal_fgcolor, 1, pCGI->SLI.width-take_up
	}
	if (pCGI->lower_status_line == 1 && type == LOWER_STATUS_LINE)
	{
		cct_showstr(pCGI->SLI.lower_start_x, pCGI->SLI.lower_start_y, my_catchy_msg, pCGI->SLI.lower_catchy_bgcolor, pCGI->SLI.lower_catchy_fgcolor, 1, pCGI->SLI.width);
		if (pCGI->SLI.width - take_up > 0)
			cct_showstr(pCGI->SLI.lower_start_x + take_up, pCGI->SLI.lower_start_y, my_msg, pCGI->SLI.lower_normal_bgcolor, pCGI->SLI.lower_normal_fgcolor, 1, pCGI->SLI.width - take_up);
	}

	cct_setcolor(pCGI->area_bgcolor, pCGI->area_fgcolor);
	delete[] my_msg;
	delete[] my_catchy_msg;
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：显示某一个色块(内容为字符串，坐标为row/col)
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int row_no						：行号（从0开始，人为保证正确性，程序不检查）
		   const int col_no						：列号（从0开始，人为保证正确性，程序不检查）
		   const int bdi_value						：需要显示的值
		   const BLOCK_DISPLAY_INFO *const bdi		：存放该值对应的显示信息的结构体数组
  返 回 值：
  说    明：1、BLOCK_DISPLAY_INFO 的含义见头文件，用法参考测试样例
			2、bdi_value为 BDI_VALUE_BLANK 表示空白块，要特殊处理
***************************************************************************/
int gmw_draw_block(const CONSOLE_GRAPHICS_INFO* const pCGI, const int row_no, const int col_no, const int bdi_value, const BLOCK_DISPLAY_INFO* const bdi)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, BDI_VALUE_BLANK, 0, 0);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：移动某一个色块
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int row_no						：行号（从0开始，人为保证正确性，程序不检查）
		   const int col_no						：列号（从0开始，人为保证正确性，程序不检查）
		   const int bdi_value						：需要显示的值
		   const int blank_bdi_value				：移动过程中用于动画效果显示时用于表示空白的值（一般为0，此处做为参数代入，是考虑到可能出现的特殊情况）
		   const BLOCK_DISPLAY_INFO *const bdi		：存放显示值/空白值对应的显示信息的结构体数组
		   const int direction						：移动方向，一共四种，具体见cmd_gmw_tools.h
		   const int distance						：移动距离（从1开始，人为保证正确性，程序不检查）
  返 回 值：
  说    明：
***************************************************************************/
int gmw_move_block(const CONSOLE_GRAPHICS_INFO* const pCGI, const int row_no, const int col_no, const int bdi_value, const int blank_bdi_value, const BLOCK_DISPLAY_INFO* const bdi, const int direction, const int distance)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	/*上减下加，左减右加*/
	int dis_real = 0;
	if (direction == UP_TO_DOWN || direction == DOWN_TO_UP)
		dis_real = pCGI->CFI.bhigh * distance;
	else if (direction == LEFT_TO_RIGHT || direction == RIGHT_TO_LEFT)
		dis_real = pCGI->CFI.bwidth * distance;
	for (int p = 0; p < dis_real; p++)
	{
		if (direction == UP_TO_DOWN)
			gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, 0, p);
		else if (direction == DOWN_TO_UP)
			gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, 0, -p);
		else if (direction == LEFT_TO_RIGHT)
			gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, p, 0);
		else if (direction == RIGHT_TO_LEFT)
			gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, -p, 0);

		Sleep(pCGI->delay_of_block_moved);

		if (direction == UP_TO_DOWN)
			gmw_inner_erase_temp_block(pCGI, row_no, col_no, 0, p);
		else if (direction == DOWN_TO_UP)
			gmw_inner_erase_temp_block(pCGI, row_no, col_no, 0, -p);
		else if (direction == LEFT_TO_RIGHT)
			gmw_inner_erase_temp_block(pCGI, row_no, col_no, p, 0);
		else if (direction == RIGHT_TO_LEFT)
			gmw_inner_erase_temp_block(pCGI, row_no, col_no, -p, 0);
	}
	if (direction == UP_TO_DOWN)
		gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, 0, dis_real);
	else if (direction == DOWN_TO_UP)
		gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, 0, -dis_real);
	else if (direction == LEFT_TO_RIGHT)
		gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, dis_real, 0);
	else if (direction == RIGHT_TO_LEFT)
		gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, blank_bdi_value, -dis_real, 0);


	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：读键盘或鼠标
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   int &MAction							：如果返回 CCT_MOUSE_EVENT，则此值有效，为 MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK/MOUSE_RIGHT_BUTTON_CLICK 三者之一
													   如果返回 CCT_KEYBOARD_EVENT，则此值无效
		   int &MRow								：如果返回 CCT_MOUSE_EVENT 且 MAction = MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK，则此值有效，表示左键选择的游戏区域的行号（从0开始）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   int &MCol								：如果返回 CCT_MOUSE_EVENT 且 MAction = MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK，则此值有效，表示左键选择的游戏区域的列号（从0开始）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   int &KeyCode1							：如果返回 CCT_KEYBOARD_EVENT，则此值有效，为读到的键码（如果双键码，则为第一个）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   int &KeyCode2							：如果返回 CCT_KEYBOARD_EVENT，则此值有效，为读到的键码（如果双键码，则为第二个，如果是单键码，则为0）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   const bool update_lower_status_line		：鼠标移动时，是否要在本函数中显示"[当前光标] *行*列/位置非法"的信息（true=显示，false=不显示，缺省为true）
  返 回 值：函数返回约定
		   1、如果是鼠标移动，得到的MRow/MCol与传入的相同(鼠标指针微小的移动)，则不返回，继续读
							  得到行列非法位置，则不返回，根据 update_lower_status_line 的设置在下状态栏显示"[当前光标] 位置非法"
							  得到的MRow/MCol与传入的不同(行列至少一个变化)，根据 update_lower_status_line 的设置在下状态栏显示"[当前光标] *行*列"，再返回MOUSE_ONLY_MOVED（有些游戏返回后要处理色块的不同颜色显示）
		   2、如果是按下鼠标左键，且当前鼠标指针停留在主游戏区域的*行*列上，则返回 CCT_MOUSE_EVENT ，MAction 为 MOUSE_LEFT_BUTTON_CLICK, MRow 为行号，MCol 为列标
								  且当前鼠标指针停留在非法区域（非游戏区域，游戏区域中的分隔线），则不返回，根据 update_lower_status_line 的设置在下状态栏显示"[当前光标] 位置非法"
		   3、如果是按下鼠标右键，则不判断鼠标指针停留区域是否合法，直接返回 CCT_MOUSE_EVENT ，MAction 为 MOUSE_RIGHT_BUTTON_CLICK, MRow、MCol取当前值（因为消灭星星的右键标记需要坐标）
		   4、如果按下键盘上的某键（含双键码按键），则直接返回 CCT_KEYBOARD_EVENT，KeyCode1/KeyCode2中为对应的键码值
 说    明：通过调用 cmd_console_tools.cpp 中的 read_keyboard_and_mouse 函数实现
 按倒右键时位置非法也会返回，所以将MRow和MCol均设为-1，来表示非法位置
***************************************************************************/
int gmw_read_keyboard_and_mouse(const CONSOLE_GRAPHICS_INFO* const pCGI, int& MAction, int& MRow, int& MCol, int& KeyCode1, int& KeyCode2, const bool update_lower_status_line)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	int MX, MY;
	int ret;
	int MRow_new;
	int MCol_new;
	int start_frame_x = pCGI->start_x + (pCGI->draw_frame_with_row_no == true) * 2;
	int start_frame_y = pCGI->start_y + (pCGI->top_status_line == true) + (pCGI->draw_frame_with_col_no == true);
	while (1)
	{
		cct_enable_mouse();
		ret = cct_read_keyboard_and_mouse(MX, MY, MAction, KeyCode1, KeyCode2);
		//cct_showint(0, start_frame_y + pCGI->CFI.bhigh * pCGI->row_num + 2, MX, 0, 7, 1);
		//cct_showint(4, start_frame_y + pCGI->CFI.bhigh * pCGI->row_num + 2, MY, 0, 7, 1);
		if (ret == CCT_KEYBOARD_EVENT)
			return ret;
		else if (ret == CCT_MOUSE_EVENT)
		{
			/*不超过边框有无边框均适用*/
			if (MX >= start_frame_x + onechar_right 
				&& MX < start_frame_x + pCGI->col_num * pCGI->CFI.bwidth+onechar_right*(pCGI->CFI.separator==false)
				&& MY >= start_frame_y + onechar_down 
				&& MY < start_frame_y + pCGI->row_num * pCGI->CFI.bhigh+onechar_down * (pCGI->CFI.separator == false))
			{
				int col_temp_extra = (MX - start_frame_x - onechar_right/*左边框*/)
					- ((MX - start_frame_x - onechar_right/*左边框*/) / pCGI->CFI.bwidth) * pCGI->CFI.bwidth;
				int row_temp_extra = (MY - start_frame_y - onechar_down/*下边框*/)
					- ((MY - start_frame_y - onechar_down/*上边框*/) / pCGI->CFI.bhigh) * pCGI->CFI.bhigh;
				if (col_temp_extra < pCGI->CFI.block_width && row_temp_extra < pCGI->CFI.block_high)//不在边框上,合法区域
				{
					MCol_new = (MX - start_frame_x - onechar_right/*左边框*/) / pCGI->CFI.bwidth;
					MRow_new = (MY - start_frame_y - onechar_down/*上边框*/) / pCGI->CFI.bhigh;
					if (MAction == MOUSE_LEFT_BUTTON_CLICK)//左键
					{
						char temp[256] = {};
						MCol = MCol_new;
						MRow = MRow_new;
						sprintf(temp, "[当前光标] %c行%d列", trans_row_num(MRow), MCol); //未考虑mcol超过99的情况，如有需要，请自行处理
						gmw_status_line(pCGI, LOWER_STATUS_LINE, temp);
						return  CCT_MOUSE_EVENT;
					}
					else if (MAction == MOUSE_ONLY_MOVED && (MCol != MCol_new || MRow != MRow_new))//变动
					{
						if (update_lower_status_line)
						{
							char temp[256] = {};
							MCol = MCol_new;
							MRow = MRow_new;
							sprintf(temp, "[当前光标] %c行%d列", trans_row_num(MRow), MCol); //未考虑mcol超过99的情况，如有需要，请自行处理
							gmw_status_line(pCGI, LOWER_STATUS_LINE, temp);
							return  ret;
						}
					}
					else if (MAction == MOUSE_RIGHT_BUTTON_CLICK)
						return ret;
					else if (MAction == MOUSE_ONLY_MOVED && (MCol == MCol_new && MRow == MRow_new))
						continue;
				}
				else
				{
					MRow = -1;
					MCol = -1;
					gmw_status_line(pCGI, LOWER_STATUS_LINE, "[当前光标] 位置非法");
					if (MAction == MOUSE_RIGHT_BUTTON_CLICK)
						return ret;
				}
			}
			else
			{
				MRow = -1;
				MCol = -1;
				gmw_status_line(pCGI, LOWER_STATUS_LINE, "[当前光标] 位置非法");
				if (MAction == MOUSE_RIGHT_BUTTON_CLICK)
					return ret;
			}
		}
	}
	return 0; //此句可根据需要修改
}

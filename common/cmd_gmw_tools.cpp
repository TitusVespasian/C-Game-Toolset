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
		�˴����Ը�����Ҫ�ľ�̬ȫ�ֱ������������٣����û�У�����̬ȫ��ֻ������/�궨�壨�������ޣ���
   -------------------------------------------------- */
const int onechar_right = 2;
const int onechar_down = 1;

/* --------------------------------------------------
		�˴����Ը�����Ҫ���ڲ��������ߺ���
		1�����������ޣ�����Ϊ gmw_inner_*
		2����������
		3��������static������ȷ��ֻ�ڱ�Դ�ļ���ʹ��
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

/*����ֵ����src��������ַ��ĸ���*/
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
/*����ֵ����dstռ���˶��ٿռ�*/  
int gmw_inner_slice(char* dst, const char* src, int maxlen)
{
	if (src == NULL)
		return 0;// catch_msg_len�൱��Ϊ0 
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

/*�������ڴ�С*/
static void adjust_cmd(CONSOLE_GRAPHICS_INFO* const pCGI)
{
	/*��״̬��λ��*/
	pCGI->SLI.top_start_x = pCGI->start_x;
	pCGI->SLI.top_start_y = pCGI->start_y;
	/*��״̬��λ��*/
	pCGI->SLI.lower_start_x = pCGI->start_x;
	pCGI->SLI.lower_start_y = pCGI->start_y + pCGI->row_num * pCGI->CFI.bhigh + (pCGI->lower_status_line==true) + (pCGI->draw_frame_with_col_no) + (pCGI->top_status_line) + (pCGI->CFI.separator == false);
	/*״̬�����*/
	pCGI->SLI.width = pCGI->col_num * pCGI->CFI.bwidth + 2 * (pCGI->CFI.separator == false) + 2 * (pCGI->draw_frame_with_row_no) + onechar_right/*��߿�ճ�*/;


	pCGI->lines = pCGI->CFI.bhigh * pCGI->row_num + (pCGI->top_status_line == 1) +
		(pCGI->lower_status_line == 1) + 1 + 4/*���·��������*/ +
		pCGI->extern_up_lines + pCGI->extern_down_lines + (pCGI->draw_frame_with_col_no == 1) + (pCGI->CFI.separator == false);
	pCGI->cols = pCGI->CFI.bwidth * pCGI->col_num +
		1 + 2 /*���ҷ��ؿտռ䵫û����������*/ +
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
/*width==-1������д*/
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
		/*�X*/
		cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
			start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + vertical,
			pCGI->CBI.top_left, block_bgcolor, block_fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		/*�T*/
		for (int i = 0; i < pCGI->CFI.block_width / 2 - 2; i++)
		{
			cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (i + 1) + horizontal,
				start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + vertical,
				pCGI->CBI.h_normal, block_bgcolor, block_fgcolor);
		}
		Sleep(pCGI->delay_of_draw_block);
		/*�[*/
		cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (pCGI->CFI.block_width / 2 - 1) + horizontal,
			start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + vertical,
			pCGI->CBI.top_right, block_bgcolor, block_fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		for (int j = 0; j < pCGI->CFI.block_high - 2; j++)
		{
			/*�U*/
			cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
				start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (j + 1) + vertical,
				pCGI->CBI.v_normal, block_bgcolor, block_fgcolor);
			Sleep(pCGI->delay_of_draw_block);
			/* */
			if (height != 1 && height != 0 && bdi[num].value != blank_bdi_value)//���ﱣ֤�����ַ�������
			{
				cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + horizontal,
					start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
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
						cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + i + horizontal,
							start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
							bdi[num].content, block_bgcolor, block_fgcolor);
					else if (bdi[num].content == NULL)
						cct_showint(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + i + horizontal,
							start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
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
					cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right + i + horizontal,
						start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down + j + vertical,
						" ", block_bgcolor, block_fgcolor);
			}
			Sleep(pCGI->delay_of_draw_block);
			cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (pCGI->CFI.block_width / 2 - 1) + horizontal,
				start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (j + 1) + vertical,
				pCGI->CBI.v_normal, block_bgcolor, block_fgcolor);
		}
		Sleep(pCGI->delay_of_draw_block);
		/*�^*/
		cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
			start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (pCGI->CFI.block_high - 1) + vertical,
			pCGI->CBI.lower_left, block_bgcolor, block_fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		/*�T*/
		for (int i = 0; i < pCGI->CFI.block_width / 2 - 2; i++)
		{
			cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (i + 1) + horizontal,
				start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (pCGI->CFI.block_high - 1) + vertical,
				pCGI->CBI.h_normal, block_bgcolor, block_fgcolor);
		}
		Sleep(pCGI->delay_of_draw_block);
		/*�a*/
		cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * (pCGI->CFI.block_width / 2 - 1) + horizontal,
			start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * (pCGI->CFI.block_high - 1) + vertical,
			pCGI->CBI.lower_right, block_bgcolor, block_fgcolor);
	}
	else
	{
		for (int j = 0; j < pCGI->CFI.block_high; j++)
		{
			if (height != 1 && height != 0 && bdi[num].value != blank_bdi_value)//���ﱣ֤�����ַ�������
			{
				cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + horizontal,
					start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
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
						cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + i + horizontal,
							start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
							bdi[num].content, block_bgcolor, block_fgcolor);
					else if (bdi[num].content == NULL)
						cct_showint(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + i + horizontal,
							start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
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
					cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + i + horizontal,
						start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + j + vertical,
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
				cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i + horizontal,
					start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			else if ((onechar_right * i + 1 + horizontal) % pCGI->CFI.bwidth == 0 && pCGI->CFI.separator == 1)
			{
				cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i - 1 + horizontal,
					start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				cout << ' ';
			}
			else if ((onechar_down * (j + 1) + vertical) % pCGI->CFI.bhigh == 0 && pCGI->CFI.separator == 1)
				cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i + horizontal,
					start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			else
				cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + onechar_right * i + horizontal,
					start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
					"  ", pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		}
		if (pCGI->CFI.separator == 1 && horizontal < 0 && (horizontal - 1) % pCGI->CFI.bwidth == 0)
		{
			cct_showstr(start_frame_x + onechar_right/*��߿������*/ + start_col_no * pCGI->CFI.bwidth + pCGI->CFI.bwidth - onechar_right - 1 + horizontal,
				start_frame_y + onechar_down/*�ϱ߿������*/ + start_row_no * pCGI->CFI.bhigh + onechar_down * j + vertical,
				pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			cout << ' ';
		}

	}
}
/* -----------------------------------------------
		ʵ����������ĺ���������������׼����
   ----------------------------------------------- */
   /***************************************************************************
	 �������ƣ�
	 ��    �ܣ�������Ϸ����ܵ�������
	 ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			   const int row						������(������Ϊ0���������ޣ���Ϊ��֤��ȷ��)
			   const int col						������(������Ϊ0���������ޣ���Ϊ��֤��ȷ��)
	 �� �� ֵ��
	 ˵    ����1��ָ��������Ϸ�ľ������������ֵ
			   2�����еı仯�ᵼ��CONSOLE_GRAPHICS_INFO�ṹ����������Աֵ�ı仯��Ҫ����
   ***************************************************************************/
int gmw_set_rowcol(CONSOLE_GRAPHICS_INFO* const pCGI, const int row, const int col)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	pCGI->row_num = row > 0 ? row : 0;
	pCGI->col_num = col > 0 ? col : 0;

	adjust_cmd(pCGI);

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ������������ڣ�����Ϸ�������������ڵ�����cmd���ڣ�����ɫ
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const int bg_color					��ǰ��ɫ��ȱʡCOLOR_BLACK��
		   const int fg_color					������ɫ��ȱʡCOLOR_WHITE��
		   const bool cascade					���Ƿ�����ȱʡΪtrue-������
  �� �� ֵ��
  ˵    ����1��cascade = trueʱ
				ͬ���޸���Ϸ���������ɫ
				ͬ���޸�����״̬���������ı��ı���ɫ��ǰ��ɫ����Ŀ�ı��ı���ɫ��ǰ��ɫ���䣩
			2���������ɫֵ���󼰳�ͻ����Ҫ��Ϊ��֤
				������ɫ��0-15
					ǰ��ɫ����ɫ��ֵһ�µ����޷���������
					ǰ��ɫ������״̬����Ŀǰ��ɫ�������޷�������Ŀ��ʾ
					...
***************************************************************************/
int gmw_set_color(CONSOLE_GRAPHICS_INFO* const pCGI, const int bgcolor, const int fgcolor, const bool cascade)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
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

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ����ô��ڵ�����
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const char *fontname					���������ƣ�ֻ����"Terminal"��"������"���֣������򷵻�-1�����ı����壩
		   const int fs_high					������߶ȣ�ȱʡ������Ϊ16�������������ƣ���Ϊ��֤��
		   const int fs_width					������߶ȣ�ȱʡ������Ϊ8�������������ƣ���Ϊ��֤��
  �� �� ֵ��
  ˵    ����1����cmd_console_tools�е�setfontsize���ƣ�Ŀǰֻ֧�֡��������塱�͡������塱
			2������������������ֱ�ӷ��أ�����ԭ�������ò���
***************************************************************************/
int gmw_set_font(CONSOLE_GRAPHICS_INFO* const pCGI, const char* fontname, const int fs_high, const int fs_width)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	/*�����С*/
	pCGI->CFT.font_size_high = fs_high > 0 ? fs_high : 16;
	pCGI->CFT.font_size_width = fs_width > 0 ? fs_width : 8;
	/*��������*/
	if (strcmp(fontname, "Terminal") == 0 || strcmp(fontname, "������") == 0)
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

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�������ʱ
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const int type						����ʱ�����ͣ�ĿǰΪ3�֣�
		   const int delay_ms					����msΪ��λ����ʱ
			   ���߿����ʱ��0 ~ �������ޣ���Ϊ��֤��ȷ��<0����0��
			   ��ɫ�����ʱ��0 ~ �������ޣ���Ϊ��֤��ȷ��<0����0��
			   ɫ���ƶ�����ʱ��BLOCK_MOVED_DELAY_MS ~ �������ޣ���Ϊ��֤��ȷ�� <BLOCK_MOVED_DELAY_MS ���� BLOCK_MOVED_DELAY_MS��
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int gmw_set_delay(CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const int delay_ms)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == DELAY_OF_DRAW_FRAME)
		pCGI->delay_of_draw_frame = delay_ms > 0 ? delay_ms : 0;
	else if (type == DELAY_OF_DRAW_BLOCK)
		pCGI->delay_of_draw_block = delay_ms > 0 ? delay_ms : 0;
	else if (type == DELAY_OF_BLOCK_MOVED)
		pCGI->delay_of_block_moved = delay_ms > BLOCK_MOVED_DELAY_MS ? delay_ms : BLOCK_MOVED_DELAY_MS;
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ���������������Ϸ����ܽṹ֮����Ҫ�����Ķ�������
  ��    �ܣ�CONSOLE_GRAPHICS_INFO *const pCGI	��
		   const int up_lines					���ϲ�������У�ȱʡ������Ϊ0���������ޣ���Ϊ��֤��
		   const int down_lines				���²�������У�ȱʡ������Ϊ0���������ޣ���Ϊ��֤��
		   const int left_cols					����߶�����У�ȱʡ������Ϊ0���������ޣ���Ϊ��֤��
		   const int right_cols				���ұ߶�����У�ȱʡ������Ϊ0���������ޣ���Ϊ��֤��
  �� �� ֵ��
  ˵    �����������еı仯�ᵼ��CONSOLE_GRAPHICS_INFO�ṹ����������Աֵ�ı仯��Ҫ����
***************************************************************************/
int gmw_set_ext_rowcol(CONSOLE_GRAPHICS_INFO* const pCGI, const int up_lines, const int down_lines, const int left_cols, const int right_cols)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->extern_down_lines = down_lines;
	pCGI->extern_up_lines = up_lines;
	pCGI->extern_left_cols = left_cols;
	pCGI->extern_right_cols = right_cols;

	pCGI->start_x = pCGI->extern_left_cols;
	pCGI->start_y = pCGI->extern_up_lines;

	adjust_cmd(pCGI);

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���� CONSOLE_FRAME_TYPE �ṹ�е�11�����ͣ�ȱʡ4�֣�
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const int type						��1 - ȫ˫�� 2 - ȫ���� 3 - ��˫���� 4 - �ᵥ��˫
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int gmw_set_frame_default_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const int type)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == 1)
	{
		zy_strcpy(pCGI->CFI.top_left, "�X");
		zy_strcpy(pCGI->CFI.lower_left, "�^");
		zy_strcpy(pCGI->CFI.top_right, "�[");
		zy_strcpy(pCGI->CFI.lower_right, "�a");
		zy_strcpy(pCGI->CFI.h_normal, "�T");
		zy_strcpy(pCGI->CFI.v_normal, "�U");
		zy_strcpy(pCGI->CFI.h_top_separator, "�j");
		zy_strcpy(pCGI->CFI.h_lower_separator, "�m");
		zy_strcpy(pCGI->CFI.v_left_separator, "�d");
		zy_strcpy(pCGI->CFI.v_right_separator, "�g");
		zy_strcpy(pCGI->CFI.mid_separator, "�p");
	}
	else if (type == 2)
	{
		zy_strcpy(pCGI->CFI.top_left, "��");
		zy_strcpy(pCGI->CFI.lower_left, "��");
		zy_strcpy(pCGI->CFI.top_right, "��");
		zy_strcpy(pCGI->CFI.lower_right, "��");
		zy_strcpy(pCGI->CFI.h_normal, "��");
		zy_strcpy(pCGI->CFI.v_normal, "��");
		zy_strcpy(pCGI->CFI.h_top_separator, "��");
		zy_strcpy(pCGI->CFI.h_lower_separator, "��");
		zy_strcpy(pCGI->CFI.v_left_separator, "��");
		zy_strcpy(pCGI->CFI.v_right_separator, "��");
		zy_strcpy(pCGI->CFI.mid_separator, "��");
	}
	else if (type == 3)
	{
		zy_strcpy(pCGI->CFI.top_left, "�V");
		zy_strcpy(pCGI->CFI.lower_left, "�\");
		zy_strcpy(pCGI->CFI.top_right, "�Y");
		zy_strcpy(pCGI->CFI.lower_right, "�_");
		zy_strcpy(pCGI->CFI.mid_separator, "�n");
		zy_strcpy(pCGI->CFI.h_normal, "�T");
		zy_strcpy(pCGI->CFI.v_normal, "��");
		zy_strcpy(pCGI->CFI.h_lower_separator, "�k");
		zy_strcpy(pCGI->CFI.v_left_separator, "�b");
		zy_strcpy(pCGI->CFI.h_top_separator, "�h");
		zy_strcpy(pCGI->CFI.v_right_separator, "�e");
	}
	else if (type == 4)
	{
		zy_strcpy(pCGI->CFI.top_left, "�W");
		zy_strcpy(pCGI->CFI.lower_left, "�]");
		zy_strcpy(pCGI->CFI.top_right, "�Z");
		zy_strcpy(pCGI->CFI.lower_right, "�`");
		zy_strcpy(pCGI->CFI.mid_separator, "�o");
		zy_strcpy(pCGI->CFI.h_normal, "��");
		zy_strcpy(pCGI->CFI.v_normal, "�U");
		zy_strcpy(pCGI->CFI.h_lower_separator, "�l");
		zy_strcpy(pCGI->CFI.v_left_separator, "�c");
		zy_strcpy(pCGI->CFI.h_top_separator, "�i");
		zy_strcpy(pCGI->CFI.v_right_separator, "�f");
	}
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���� CONSOLE_FRAME_TYPE �ṹ�е�11������
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const char *...						����11�֣������.h���˴���
  �� �� ֵ��
  ˵    ����Լ��Ϊһ�������Ʊ��������ʹ���������ݣ���Ϊ��֤2�ֽ�
			1������2�ֽ���ֻȡǰ2�ֽ�
			2�������NULL���������ո����
			3�������1�ֽڣ���һ���ո������˶�������ʾ�ң������
***************************************************************************/
int gmw_set_frame_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const char* top_left, const char* lower_left, const char* top_right,
	const char* lower_right, const char* h_normal, const char* v_normal, const char* h_top_separator,
	const char* h_lower_separator, const char* v_left_separator, const char* v_right_separator, const char* mid_separator)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
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
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���� CONSOLE_FRAME_TYPE �ṹ�е�ɫ��������С���Ƿ���Ҫ�ָ��ߵ�
  ������������������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const int block_width						����ȣ�����ȱʡ2����ΪԼ�������Ϊ�����Ʊ�����������������Ҫ+1��
			const int block_high						���߶ȣ�����ȱʡ1��
			const bool separator						���Ƿ���Ҫ�ָ��ߣ�ȱʡΪtrue����Ҫ�ָ��ߣ�
  �� �� ֵ��
  ˵    ������ܴ�С/�Ƿ���Ҫ�ָ��ߵȵı仯�ᵼ��CONSOLE_GRAPHICS_INFO�ṹ����������Աֵ�ı仯��Ҫ����
***************************************************************************/
int gmw_set_frame_style(CONSOLE_GRAPHICS_INFO* const pCGI, const int block_width, const int block_high, const bool separator)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
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

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���� CONSOLE_BORDER_TYPE �ṹ�е���ɫ
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const int bg_color					������ɫ��ȱʡ -1��ʾ�ô��ڱ���ɫ��
			const int fg_color					��ǰ��ɫ��ȱʡ -1��ʾ�ô���ǰ��ɫ��
  �� �� ֵ��
  ˵    �����������ɫֵ���󼰳�ͻ����Ҫ��Ϊ��֤
				������ɫ��0-15��ǰ��ɫ����ɫ��ֵһ�µ����޷��������ݵ�
***************************************************************************/
int gmw_set_frame_color(CONSOLE_GRAPHICS_INFO* const pCGI, const int bgcolor, const int fgcolor)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
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
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���� CONSOLE_BLOCK_INFO �ṹ�е�6�����ͣ�ȱʡ4�֣�
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const int type						��1 - ȫ˫�� 2 - ȫ���� 3 - ��˫���� 4 - �ᵥ��˫
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int gmw_set_block_default_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const int type)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == 1)
	{
		zy_strcpy(pCGI->CBI.top_left, "�X");
		zy_strcpy(pCGI->CBI.lower_left, "�^");
		zy_strcpy(pCGI->CBI.top_right, "�[");
		zy_strcpy(pCGI->CBI.lower_right, "�a");
		zy_strcpy(pCGI->CBI.h_normal, "�T");
		zy_strcpy(pCGI->CBI.v_normal, "�U");
	}
	else if (type == 2)
	{
		zy_strcpy(pCGI->CBI.top_left, "��");
		zy_strcpy(pCGI->CBI.lower_left, "��");
		zy_strcpy(pCGI->CBI.top_right, "��");
		zy_strcpy(pCGI->CBI.lower_right, "��");
		zy_strcpy(pCGI->CBI.h_normal, "��");
		zy_strcpy(pCGI->CBI.v_normal, "��");
	}
	else if (type == 3)
	{
		zy_strcpy(pCGI->CBI.top_left, "�V");
		zy_strcpy(pCGI->CBI.lower_left, "�\");
		zy_strcpy(pCGI->CBI.top_right, "�Y");
		zy_strcpy(pCGI->CBI.lower_right, "�_");
		zy_strcpy(pCGI->CBI.h_normal, "�T");
		zy_strcpy(pCGI->CBI.v_normal, "��");
	}
	else if (type == 4)
	{
		zy_strcpy(pCGI->CBI.top_left, "�W");
		zy_strcpy(pCGI->CBI.lower_left, "�]");
		zy_strcpy(pCGI->CBI.top_right, "�Z");
		zy_strcpy(pCGI->CBI.lower_right, "�`");
		zy_strcpy(pCGI->CBI.h_normal, "��");
		zy_strcpy(pCGI->CBI.v_normal, "�U");
	}

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���� CONSOLE_BLOCK_INFO �ṹ�е�6������
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const char *...					����6�֣������.h���˴���
  �� �� ֵ��
  ˵    ����Լ��Ϊһ�������Ʊ��������ʹ���������ݣ���Ϊ��֤2�ֽ�
			1������2�ֽ���ֻȡǰ2�ֽ�
			2�������NULL���������ո����
			3�������1�ֽڣ���һ���ո������˶�������ʾ�ң������
***************************************************************************/
int gmw_set_block_linetype(CONSOLE_GRAPHICS_INFO* const pCGI, const char* top_left, const char* lower_left, const char* top_right, const char* lower_right, const char* h_normal, const char* v_normal)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	zy_strcpy(pCGI->CBI.top_left, top_left);
	zy_strcpy(pCGI->CBI.lower_left, lower_left);
	zy_strcpy(pCGI->CBI.top_right, top_right);
	zy_strcpy(pCGI->CBI.lower_right, lower_right);
	zy_strcpy(pCGI->CBI.h_normal, h_normal);
	zy_strcpy(pCGI->CBI.v_normal, v_normal);
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�����ÿ����Ϸɫ��(����)�Ƿ���ҪС�߿�
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const bool on_off					��true - ��Ҫ flase - ����Ҫ��ȱʡfalse��
  �� �� ֵ��
  ˵    �����߿�Լ��Ϊ�����Ʊ����˫��
***************************************************************************/
int gmw_set_block_border_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const bool on_off)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->CBI.block_border = on_off;
	pCGI->CBI.pad1 = false;
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ������Ƿ���ʾ����״̬��
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const int type						��״̬�����ͣ���/�£�
			const bool on_off					��true - ��Ҫ flase - ����Ҫ��ȱʡtrue��
  �� �� ֵ��
  ˵    ����1��״̬�������Լ�����£�
			   1.1����״̬��ֻ��һ�У������������Ϸ�����/�б�����棬Ϊ��������ʼһ�У�����������Ͻ����������״̬�������꣩
			   1.2����״̬��ֻ��һ�У������������·����ߵ�����
			   1.3��״̬���Ŀ��Ϊ�������ȣ������Ϣ������ض�
		   2�����еı仯�ᵼ��CONSOLE_GRAPHICS_INFO�ṹ����������Աֵ�ı仯��Ҫ����
***************************************************************************/
int gmw_set_status_line_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const bool on_off)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
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

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���������״̬������ɫ
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const int type						��״̬�����ͣ���/�£�
			const int normal_bgcolor			�������ı�����ɫ��ȱʡ -1��ʾʹ�ô��ڱ���ɫ��
			const int normal_fgcolor			�������ı�ǰ��ɫ��ȱʡ -1��ʾʹ�ô���ǰ��ɫ��
			const int catchy_bgcolor			����Ŀ�ı�����ɫ��ȱʡ -1��ʾʹ�ô��ڱ���ɫ��
			const int catchy_fgcolor			����Ŀ�ı�ǰ��ɫ��ȱʡ -1��ʾʹ������ɫ��
  ���������
  �� �� ֵ��
  ˵    �����������ɫֵ���󼰳�ͻ����Ҫ��Ϊ��֤
				������ɫ��0-15��ǰ��ɫ����ɫ��ֵһ�µ����޷��������ݵ�
***************************************************************************/
int gmw_set_status_line_color(CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const int normal_bgcolor, const int normal_fgcolor, const int catchy_bgcolor, const int catchy_fgcolor)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	if (type == LOWER_STATUS_LINE)
	{
		/*��Ŀǰ��ɫ*/
		if (catchy_fgcolor == -1)
			pCGI->SLI.lower_catchy_fgcolor = COLOR_HYELLOW;
		else
			pCGI->SLI.lower_catchy_fgcolor = catchy_fgcolor;
		/*��Ŀ����ɫ*/
		if (catchy_bgcolor == -1)
			pCGI->SLI.lower_catchy_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.lower_catchy_bgcolor = catchy_bgcolor;
		/*����ǰ��ɫ*/
		if (normal_fgcolor == -1)
			pCGI->SLI.lower_normal_fgcolor = pCGI->area_fgcolor;
		else
			pCGI->SLI.lower_normal_fgcolor = normal_fgcolor;
		/*��������ɫ*/
		if (normal_bgcolor == -1)
			pCGI->SLI.lower_normal_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.lower_normal_bgcolor = normal_bgcolor;
	}
	if (type == TOP_STATUS_LINE)
	{
		/*��Ŀǰ��ɫ*/
		if (catchy_fgcolor == -1)
			pCGI->SLI.top_catchy_fgcolor = COLOR_HYELLOW;
		else
			pCGI->SLI.top_catchy_fgcolor = catchy_fgcolor;
		/*��Ŀ����ɫ*/
		if (catchy_bgcolor == -1)
			pCGI->SLI.top_catchy_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.top_catchy_bgcolor = catchy_bgcolor;
		/*����ǰ��ɫ*/
		if (normal_fgcolor == -1)
			pCGI->SLI.top_normal_fgcolor = pCGI->area_fgcolor;
		else
			pCGI->SLI.top_normal_fgcolor = normal_fgcolor;
		/*��������ɫ*/
		if (normal_bgcolor == -1)
			pCGI->SLI.top_normal_bgcolor = pCGI->area_bgcolor;
		else
			pCGI->SLI.top_normal_bgcolor = normal_bgcolor;
	}
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ������Ƿ���ʾ�к�
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const bool on_off					��true - ��ʾ flase - ����ʾ��ȱʡfalse��
  �� �� ֵ��
  ˵    ����1���к�Լ��Ϊ��ĸA��ʼ�������У��������26�����a��ʼ������52��ͳһΪ*��ʵ��Ӧ�ò����ܣ�
			2���Ƿ���ʾ�кŵı仯�ᵼ��CONSOLE_GRAPHICS_INFO�ṹ����������Աֵ�ı仯��Ҫ����
***************************************************************************/
int gmw_set_rowno_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const bool on_off)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	pCGI->draw_frame_with_row_no = on_off;
	adjust_cmd(pCGI);
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ������Ƿ���ʾ�б�
  ���������CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
			const bool on_off					��true - ��ʾ flase - ����ʾ��ȱʡfalse��
  �� �� ֵ��
  ˵    ����1���б�Լ��Ϊ����0��ʼ�������У�����0-99������99ͳһΪ**��ʵ��Ӧ�ò����ܣ�
			2���Ƿ���ʾ�б�ı仯�ᵼ��CONSOLE_GRAPHICS_INFO�ṹ����������Աֵ�ı仯��Ҫ����
***************************************************************************/
int gmw_set_colno_switch(CONSOLE_GRAPHICS_INFO* const pCGI, const bool on_off)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	pCGI->draw_frame_with_col_no = on_off;
	adjust_cmd(pCGI);
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���ӡ CONSOLE_GRAPHICS_INFO �ṹ���еĸ���Աֵ
  ���������
  �� �� ֵ��
  ˵    ����1�����������ã���ӡ��ʽ�Զ���
			2������������������δ���ù������Բ�ʵ��
***************************************************************************/
int gmw_print(const CONSOLE_GRAPHICS_INFO* const pCGI)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	cout << "inited : " << pCGI->inited << endl;

	/* ����ͼ�ν��������������Ҫ�Ķ��������� */
	cout << "����ͼ�ν��������������Ҫ�Ķ��������� : " << endl;
	cout << "extern_up_lines : " << pCGI->extern_up_lines << endl;
	cout << "extern_down_lines : " << pCGI->extern_down_lines << endl;
	cout << "extern_left_cols : " << pCGI->extern_left_cols << endl;
	cout << "extern_right_cols : " << pCGI->extern_right_cols << endl;

	/* ��Ϸ��������������ɫ��������� */
	cout << "��Ϸ��������������ɫ��������� : " << endl;
	cout << "row_num : " << pCGI->row_num << endl;
	cout << "col_num : " << pCGI->col_num << endl;

	/* ����ͼ�ν��棨������ܡ�����״̬�����к��бꡢ�������У��ı���ɫ��ǰ��ɫ */
	cout << "����ͼ�ν��棨������ܡ�����״̬�����к��бꡢ�������У��ı���ɫ��ǰ��ɫ" << endl;

	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ��� CONSOLE_GRAPHICS_INFO �ṹ����ȱʡֵ���г�ʼ��
  ���������CONSOLE_GRAPHICS_INFO *const pCGI������ṹָ��
		   const int row					����Ϸ����ɫ��������ȱʡ10��
		   const int col					����Ϸ����ɫ��������ȱʡ10��
		   const int bgcolor				���������ڱ���ɫ��ȱʡ COLOR_BLACK��
		   const int fgcolor				���������ڱ���ɫ��ȱʡ COLOR_WHITE��
  �� �� ֵ��
  ˵    �������ڱ�����/ǰ���ף�����16*8�����������޶������У�����״̬�����У����к�/�б꣬�������Ϊ˫�ߣ�ɫ����2/�߶�1/��С�߿���ɫ��
***************************************************************************/
int gmw_init(CONSOLE_GRAPHICS_INFO* const pCGI, const int row, const int col, const int bgcolor, const int fgcolor)
{
	/* �����ñ�� */
	pCGI->inited = CGI_INITED;
	/*��Ҫ��ʼ��һ��*/
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
	/*����״̬��λ��*/
	pCGI->SLI.is_top_status_line = true;
	pCGI->top_status_line = true;
	/*��״̬��λ��*/
	pCGI->SLI.top_start_x = pCGI->start_x;
	pCGI->SLI.top_start_y = pCGI->start_y;

	/*����״̬��λ��*/
	pCGI->SLI.is_lower_status_line = true;
	pCGI->lower_status_line = true;
	/*��״̬��λ��*/
	pCGI->SLI.lower_start_x = pCGI->start_x;
	pCGI->SLI.lower_start_y = pCGI->start_y + pCGI->row_num * pCGI->CFI.bhigh + 1 + (pCGI->draw_frame_with_col_no) + (pCGI->top_status_line);
	pCGI->SLI.width = pCGI->start_x + pCGI->col_num * pCGI->CFI.bwidth + 2;

	/*�����������ڵ�ĳЩ����*/

	gmw_set_color(pCGI);

	gmw_set_font(pCGI);/*Console Frame Font������ĳ�ʼ��*/

	gmw_set_delay(pCGI, DELAY_OF_DRAW_FRAME, 0);
	gmw_set_delay(pCGI, DELAY_OF_DRAW_BLOCK, 0);
	gmw_set_delay(pCGI, DELAY_OF_BLOCK_MOVED, BLOCK_MOVED_DELAY_MS);


	/*����������Ϸ����ܵĲ���CFI*/
	gmw_set_frame_default_linetype(pCGI, 1);//ȫ˫��

	gmw_set_frame_color(pCGI);

	/*����ÿ��ɫ��Ĳ���*/
	gmw_set_block_default_linetype(pCGI, 1);
	gmw_set_block_border_switch(pCGI);

	/*�����Ƿ���Ҫ����״̬������ɫ*/

	gmw_set_status_line_color(pCGI, TOP_STATUS_LINE);
	gmw_set_status_line_color(pCGI, LOWER_STATUS_LINE);


	/*���һЩ�����ʼ��*/
	init_array(pCGI->CFI.pad1, sizeof(pCGI->CFI.pad1) / sizeof(char));
	init_array(pCGI->CFI.pad2, sizeof(pCGI->CFI.pad2) / sizeof(char));
	init_array(pCGI->CFI.pad, sizeof(pCGI->CFI.pad) / sizeof(char));

	init_array(pCGI->CBI.pad, sizeof(pCGI->CBI.pad) / sizeof(char));
	init_array(pCGI->SLI.pad1, sizeof(pCGI->SLI.pad1) / sizeof(char));
	init_array(pCGI->SLI.pad, sizeof(pCGI->SLI.pad) / sizeof(char));

	init_array(pCGI->pad, sizeof(pCGI->pad) / sizeof(char));

	adjust_cmd(pCGI);
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ�������Ϸ���
  ���������const CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
  �� �� ֵ��
  ˵    ��������ɲο�demo��Ч��
***************************************************************************/
int gmw_draw_frame(const CONSOLE_GRAPHICS_INFO* const pCGI)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
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
				cct_showint(start_frame_x + pCGI->CFI.bwidth * i + (int)(pCGI->CFI.block_width * 0.5) - 1/*����һλ*/ + onechar_right/*��߿�ճ�*/,
					start_frame_y - 1, i, pCGI->area_bgcolor, pCGI->area_fgcolor);
			else
				cct_showstr(start_frame_x + pCGI->CFI.bwidth * i + (int)(pCGI->CFI.block_width * 0.5) - 1/*����һλ*/ + onechar_right/*��߿�ճ�*/,
					start_frame_y - 1, "**", pCGI->area_bgcolor, pCGI->area_fgcolor);
		}
	for (int j = 0; j < pCGI->row_num; j++)
	{
		for (int i = 0; i < pCGI->col_num; i++)
		{
			/*
			  �X
			  �U
			  �c
			*/
			if (i == 0)
			{
				if (j == 0)/*�X*/
					cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.top_left, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				else if (pCGI->CFI.separator == 1)/*�c*/
					cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.v_left_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			}
			Sleep(pCGI->delay_of_draw_frame);

			/*�T �T �T*/
			if (pCGI->CFI.separator == 1 || j == 0)
				for (int k = 0; k < pCGI->CFI.block_width / 2; k++)
					cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + onechar_right * k, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);

			/*�T �h �T �[*/
			if (i == pCGI->col_num - 1)
			{
				if (j == 0)/*�[*/
					cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.top_right, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				else if (pCGI->CFI.separator == 1)/*�f*/
					cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.v_right_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			}
			else if (i != pCGI->col_num - 1)
			{
				if (j == 0 && (pCGI->CFI.separator == 1))/*�h*/
					cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.h_top_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				else if (pCGI->CFI.separator == 1)/*��*/
					cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + pCGI->CFI.bhigh * j, pCGI->CFI.mid_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			}

		}
		for (int k = 0; k < pCGI->CFI.block_high; k++)
			for (int i = 0; i < pCGI->col_num; i++)
			{
				/* 	�U          �U
					�U          �U */
				if (i == 0)
				{
					if (pCGI->draw_frame_with_row_no && (k == (pCGI->CFI.block_high-1)/ 2))
						cct_showch(start_frame_x - 2, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * j + onechar_down * k, trans_row_num(j), pCGI->area_bgcolor, pCGI->area_fgcolor);
					cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * j + onechar_down * k, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				}
				//Sleep(pCGI->delay_of_draw_frame);
				for (int k2 = 0; k2 < pCGI->CFI.block_width / 2; k2++)
					cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + onechar_right * k2, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * j + onechar_down * k, "  ", pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				if (pCGI->CFI.separator == 1 || i == pCGI->col_num - 1)
				{
					Sleep(pCGI->delay_of_draw_frame);
					cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * j + onechar_down * k, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				}

			}
	}
	for (int i = 0; i < pCGI->col_num; i++)/*ֻ�ШkҪ���⴦��*/
	{
		if (i == 0)/*�^*/
			cct_showstr(start_frame_x + pCGI->CFI.bwidth * i, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.lower_left, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		Sleep(pCGI->delay_of_draw_frame);
		for (int k = 0; k < pCGI->CFI.block_width / 2; k++)/*�T*/
			cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + onechar_right * k, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		if ((i != pCGI->col_num - 1) && (pCGI->CFI.separator == 1))/*�k*/
			cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.h_lower_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
		else if (i == pCGI->col_num - 1)/*�a*/
			cct_showstr(start_frame_x + onechar_right/*��߿�ճ�*/ + pCGI->CFI.bwidth * i + pCGI->CFI.block_width, start_frame_y + onechar_down/*�ϱ߿�ճ�*/ + pCGI->CFI.bhigh * (pCGI->row_num - 1) + pCGI->CFI.block_high, pCGI->CFI.lower_right, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
	}
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���״̬������ʾ��Ϣ
  ���������const CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const int type							��ָ������/��״̬��
		   const char *msg						��������Ϣ
		   const char *catchy_msg					����Ҫ�ر��ע����Ϣ����������Ϣǰ��ʾ��
  �� �� ֵ��
  ˵    ����1���������޶�Ϊ����ܵĿ�ȣ����к��б�λ�ã����������ȥ
			2��������һ���ַ���ĳ���ֵ�ǰ������ᵼ�º������룬Ҫ����
***************************************************************************/
int gmw_status_line(const CONSOLE_GRAPHICS_INFO* const pCGI, const int type, const char* msg, const char* catchy_msg)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	//cct_setfontsize(pCGI->CFT.font_type, pCGI->CFT.font_size_high, pCGI->CFT.font_size_width);

	char* my_catchy_msg = new(nothrow) char[pCGI->SLI.width + 5]{};//���룬��ʼ��
	if (my_catchy_msg == NULL)//�ж�
		return -2;
	char* my_msg = new(nothrow) char[pCGI->SLI.width + 5]{};//���룬��ʼ��
	if (my_msg == NULL)//�ж�
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
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ���ʾĳһ��ɫ��(����Ϊ�ַ���������Ϊrow/col)
  ���������const CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const int row_no						���кţ���0��ʼ����Ϊ��֤��ȷ�ԣ����򲻼�飩
		   const int col_no						���кţ���0��ʼ����Ϊ��֤��ȷ�ԣ����򲻼�飩
		   const int bdi_value						����Ҫ��ʾ��ֵ
		   const BLOCK_DISPLAY_INFO *const bdi		����Ÿ�ֵ��Ӧ����ʾ��Ϣ�Ľṹ������
  �� �� ֵ��
  ˵    ����1��BLOCK_DISPLAY_INFO �ĺ����ͷ�ļ����÷��ο���������
			2��bdi_valueΪ BDI_VALUE_BLANK ��ʾ�հ׿飬Ҫ���⴦��
***************************************************************************/
int gmw_draw_block(const CONSOLE_GRAPHICS_INFO* const pCGI, const int row_no, const int col_no, const int bdi_value, const BLOCK_DISPLAY_INFO* const bdi)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	gmw_inner_draw_temp_block(pCGI, row_no, col_no, bdi_value, bdi, BDI_VALUE_BLANK, 0, 0);
	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ��ƶ�ĳһ��ɫ��
  ���������const CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   const int row_no						���кţ���0��ʼ����Ϊ��֤��ȷ�ԣ����򲻼�飩
		   const int col_no						���кţ���0��ʼ����Ϊ��֤��ȷ�ԣ����򲻼�飩
		   const int bdi_value						����Ҫ��ʾ��ֵ
		   const int blank_bdi_value				���ƶ����������ڶ���Ч����ʾʱ���ڱ�ʾ�հ׵�ֵ��һ��Ϊ0���˴���Ϊ�������룬�ǿ��ǵ����ܳ��ֵ����������
		   const BLOCK_DISPLAY_INFO *const bdi		�������ʾֵ/�հ�ֵ��Ӧ����ʾ��Ϣ�Ľṹ������
		   const int direction						���ƶ�����һ�����֣������cmd_gmw_tools.h
		   const int distance						���ƶ����루��1��ʼ����Ϊ��֤��ȷ�ԣ����򲻼�飩
  �� �� ֵ��
  ˵    ����
***************************************************************************/
int gmw_move_block(const CONSOLE_GRAPHICS_INFO* const pCGI, const int row_no, const int col_no, const int bdi_value, const int blank_bdi_value, const BLOCK_DISPLAY_INFO* const bdi, const int direction, const int distance)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
	if (pCGI->inited != CGI_INITED)
		return -1;
	/*�ϼ��¼ӣ�����Ҽ�*/
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


	return 0; //�˾�ɸ�����Ҫ�޸�
}

/***************************************************************************
  �������ƣ�
  ��    �ܣ������̻����
  ���������const CONSOLE_GRAPHICS_INFO *const pCGI	������ṹָ��
		   int &MAction							��������� CCT_MOUSE_EVENT�����ֵ��Ч��Ϊ MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK/MOUSE_RIGHT_BUTTON_CLICK ����֮һ
													   ������� CCT_KEYBOARD_EVENT�����ֵ��Ч
		   int &MRow								��������� CCT_MOUSE_EVENT �� MAction = MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK�����ֵ��Ч����ʾ���ѡ�����Ϸ������кţ���0��ʼ��
												  ���������ֵ��Ч�����������Чֵ���´��󣬲��Ǳ������Ĵ�!!!��
		   int &MCol								��������� CCT_MOUSE_EVENT �� MAction = MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK�����ֵ��Ч����ʾ���ѡ�����Ϸ������кţ���0��ʼ��
												  ���������ֵ��Ч�����������Чֵ���´��󣬲��Ǳ������Ĵ�!!!��
		   int &KeyCode1							��������� CCT_KEYBOARD_EVENT�����ֵ��Ч��Ϊ�����ļ��루���˫���룬��Ϊ��һ����
												  ���������ֵ��Ч�����������Чֵ���´��󣬲��Ǳ������Ĵ�!!!��
		   int &KeyCode2							��������� CCT_KEYBOARD_EVENT�����ֵ��Ч��Ϊ�����ļ��루���˫���룬��Ϊ�ڶ���������ǵ����룬��Ϊ0��
												  ���������ֵ��Ч�����������Чֵ���´��󣬲��Ǳ������Ĵ�!!!��
		   const bool update_lower_status_line		������ƶ�ʱ���Ƿ�Ҫ�ڱ���������ʾ"[��ǰ���] *��*��/λ�÷Ƿ�"����Ϣ��true=��ʾ��false=����ʾ��ȱʡΪtrue��
  �� �� ֵ����������Լ��
		   1�����������ƶ����õ���MRow/MCol�봫�����ͬ(���ָ��΢С���ƶ�)���򲻷��أ�������
							  �õ����зǷ�λ�ã��򲻷��أ����� update_lower_status_line ����������״̬����ʾ"[��ǰ���] λ�÷Ƿ�"
							  �õ���MRow/MCol�봫��Ĳ�ͬ(��������һ���仯)������ update_lower_status_line ����������״̬����ʾ"[��ǰ���] *��*��"���ٷ���MOUSE_ONLY_MOVED����Щ��Ϸ���غ�Ҫ����ɫ��Ĳ�ͬ��ɫ��ʾ��
		   2������ǰ������������ҵ�ǰ���ָ��ͣ��������Ϸ�����*��*���ϣ��򷵻� CCT_MOUSE_EVENT ��MAction Ϊ MOUSE_LEFT_BUTTON_CLICK, MRow Ϊ�кţ�MCol Ϊ�б�
								  �ҵ�ǰ���ָ��ͣ���ڷǷ����򣨷���Ϸ������Ϸ�����еķָ��ߣ����򲻷��أ����� update_lower_status_line ����������״̬����ʾ"[��ǰ���] λ�÷Ƿ�"
		   3������ǰ�������Ҽ������ж����ָ��ͣ�������Ƿ�Ϸ���ֱ�ӷ��� CCT_MOUSE_EVENT ��MAction Ϊ MOUSE_RIGHT_BUTTON_CLICK, MRow��MColȡ��ǰֵ����Ϊ�������ǵ��Ҽ������Ҫ���꣩
		   4��������¼����ϵ�ĳ������˫���밴��������ֱ�ӷ��� CCT_KEYBOARD_EVENT��KeyCode1/KeyCode2��Ϊ��Ӧ�ļ���ֵ
 ˵    ����ͨ������ cmd_console_tools.cpp �е� read_keyboard_and_mouse ����ʵ��
 �����Ҽ�ʱλ�÷Ƿ�Ҳ�᷵�أ����Խ�MRow��MCol����Ϊ-1������ʾ�Ƿ�λ��
***************************************************************************/
int gmw_read_keyboard_and_mouse(const CONSOLE_GRAPHICS_INFO* const pCGI, int& MAction, int& MRow, int& MCol, int& KeyCode1, int& KeyCode2, const bool update_lower_status_line)
{
	/* ��ֹ��δ���� gmw_init ǰ������������ */
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
			/*�������߿����ޱ߿������*/
			if (MX >= start_frame_x + onechar_right 
				&& MX < start_frame_x + pCGI->col_num * pCGI->CFI.bwidth+onechar_right*(pCGI->CFI.separator==false)
				&& MY >= start_frame_y + onechar_down 
				&& MY < start_frame_y + pCGI->row_num * pCGI->CFI.bhigh+onechar_down * (pCGI->CFI.separator == false))
			{
				int col_temp_extra = (MX - start_frame_x - onechar_right/*��߿�*/)
					- ((MX - start_frame_x - onechar_right/*��߿�*/) / pCGI->CFI.bwidth) * pCGI->CFI.bwidth;
				int row_temp_extra = (MY - start_frame_y - onechar_down/*�±߿�*/)
					- ((MY - start_frame_y - onechar_down/*�ϱ߿�*/) / pCGI->CFI.bhigh) * pCGI->CFI.bhigh;
				if (col_temp_extra < pCGI->CFI.block_width && row_temp_extra < pCGI->CFI.block_high)//���ڱ߿���,�Ϸ�����
				{
					MCol_new = (MX - start_frame_x - onechar_right/*��߿�*/) / pCGI->CFI.bwidth;
					MRow_new = (MY - start_frame_y - onechar_down/*�ϱ߿�*/) / pCGI->CFI.bhigh;
					if (MAction == MOUSE_LEFT_BUTTON_CLICK)//���
					{
						char temp[256] = {};
						MCol = MCol_new;
						MRow = MRow_new;
						sprintf(temp, "[��ǰ���] %c��%d��", trans_row_num(MRow), MCol); //δ����mcol����99�������������Ҫ�������д���
						gmw_status_line(pCGI, LOWER_STATUS_LINE, temp);
						return  CCT_MOUSE_EVENT;
					}
					else if (MAction == MOUSE_ONLY_MOVED && (MCol != MCol_new || MRow != MRow_new))//�䶯
					{
						if (update_lower_status_line)
						{
							char temp[256] = {};
							MCol = MCol_new;
							MRow = MRow_new;
							sprintf(temp, "[��ǰ���] %c��%d��", trans_row_num(MRow), MCol); //δ����mcol����99�������������Ҫ�������д���
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
					gmw_status_line(pCGI, LOWER_STATUS_LINE, "[��ǰ���] λ�÷Ƿ�");
					if (MAction == MOUSE_RIGHT_BUTTON_CLICK)
						return ret;
				}
			}
			else
			{
				MRow = -1;
				MCol = -1;
				gmw_status_line(pCGI, LOWER_STATUS_LINE, "[��ǰ���] λ�÷Ƿ�");
				if (MAction == MOUSE_RIGHT_BUTTON_CLICK)
					return ret;
			}
		}
	}
	return 0; //�˾�ɸ�����Ҫ�޸�
}

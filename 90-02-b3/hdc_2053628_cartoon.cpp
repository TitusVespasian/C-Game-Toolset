#include<iostream>
#include <Windows.h>
#include"../include/cmd_hdc_tools.h"
const int wide = 400;
const int high = 400;
static void head(const int start_x,const int start_y)
{
	hdc_circle(start_x + 183, start_y + 144,143,true,2,RGB(10,10,10));
	hdc_ellipse(start_x + 148, start_y + 134, 31, 45, -5, true, 2, RGB(255, 255, 255));//eye left
	hdc_ellipse(start_x + 220, start_y + 124, 31, 45, -5, true, 2, RGB(255, 255, 255));//eye
	hdc_ellipse(start_x + 160, start_y + 120, 9, 12, -5, true, 2, RGB(10, 10, 10));//eyeball
	hdc_ellipse(start_x + 206, start_y + 115, 9, 12, -5, true, 2, RGB(10, 10, 10));//eyeball
	hdc_ellipse(start_x + 198, start_y + 210, 86, 28, -5, true, 2, RGB(255, 225, 0));//eyeball
}
static void scarf(const int start_x, const int start_y)
{
	hdc_sector(start_x + 183, start_y + 164, 143, 90, 90 + 165, true, 30, RGB(255, 0, 0));
	hdc_circle(start_x + 57, start_y + 200, 28, true, 2, RGB(255, 0, 0));
	hdc_ellipse(start_x + 330, start_y + 156, 10,27,0, true, 2, RGB(255, 0, 0));
	hdc_rectangle(start_x + 123, start_y + 296, 46, 38, 0, true, 2, RGB(255, 0, 0));
	hdc_ellipse(start_x + 145, start_y + 337, 23, 9, 0, true, 2, RGB(255, 0, 0));
}
static void arm(const int start_x, const int start_y)
{
	hdc_ellipse(start_x +53, start_y + 270,24,44,0, true, 2, RGB(10, 10, 10));
	hdc_ellipse(start_x + 352, start_y + 222, 24, 44, -60, true, 2, RGB(10, 10, 10));
}
static void body(const int start_x, const int start_y)
{
	hdc_ellipse(start_x + 99, start_y + 337, 34, 36, 0, true, 2, RGB(225, 225, 0));
	hdc_ellipse(start_x + 318, start_y + 310, 34, 36, 0, true, 2, RGB(225, 225, 0));
	hdc_ellipse(start_x + 192, start_y + 258, 135, 110, 0, true, 2, RGB(225, 225, 225));
}

void hdc_draw_cartoon_2053628(const int base_x, const int base_y)
{  
	hdc_cls();//Í¼Æ¬400*400ÏñËØ
	int start_x=base_x- wide/2, start_y=base_y-high/2;
	hdc_rectangle(start_x, start_y-15 , 400, 400, 0, true, 2, RGB(128, 225, 225));
	body(start_x, start_y);
	arm(start_x, start_y);
	scarf(start_x, start_y);
	head(start_x, start_y);
	
	
}
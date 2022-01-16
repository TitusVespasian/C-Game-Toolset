/*2053628 大数据 许芷瑜*/
#pragma once

#define VK_ESCAPE 27 
#define TEMP_NO_MOVE -2
#define DISPLAY_ON 1
#define DISPLAY_OFF 0
#define DISPLAY_MARK 2

/*main中*/
/*有关游戏界面*/
int init_sweeper(CONSOLE_GRAPHICS_INFO& Mine_sweeper_CGI);
void display_general(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], char bomb[20][34]);
/*文字显示栏*/
void display_time(const CONSOLE_GRAPHICS_INFO* const pCGI, clock_t start);
void mark(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], int MRow, int MCol, int bomb_num, bool change_display);
/*处理判断*/
void quit_game(int output_pos_x, int output_pos_y);
int not_inited_array(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], char bomb[20][34], int bomb_num, clock_t start, int& MRow, int& MCol);
void for_play(CONSOLE_GRAPHICS_INFO* pCGI, int display[20][34], char bomb[20][34], int bomb_num);

/*tools中*/
void generatebomb(char bomb[20][34], const int hang, const int lie, const int bomb_num, const int inputy, const int inputx);
void inarray(char bomb[20][34], int hang, int lie, int bomb_num);
void BFS(char bomb[20][34], int display[20][34], int hang, int lie, int y, int x);

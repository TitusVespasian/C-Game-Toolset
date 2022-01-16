/*2053628 ������ �����*/
#pragma once

#define VK_ESCAPE 27 
#define TEMP_NO_MOVE -2
#define DISPLAY_ON 1
#define DISPLAY_OFF 0
#define DISPLAY_MARK 2

/*main��*/
/*�й���Ϸ����*/
int init_sweeper(CONSOLE_GRAPHICS_INFO& Mine_sweeper_CGI);
void display_general(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], char bomb[20][34]);
/*������ʾ��*/
void display_time(const CONSOLE_GRAPHICS_INFO* const pCGI, clock_t start);
void mark(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], int MRow, int MCol, int bomb_num, bool change_display);
/*�����ж�*/
void quit_game(int output_pos_x, int output_pos_y);
int not_inited_array(const CONSOLE_GRAPHICS_INFO* const pCGI, int display[20][34], char bomb[20][34], int bomb_num, clock_t start, int& MRow, int& MCol);
void for_play(CONSOLE_GRAPHICS_INFO* pCGI, int display[20][34], char bomb[20][34], int bomb_num);

/*tools��*/
void generatebomb(char bomb[20][34], const int hang, const int lie, const int bomb_num, const int inputy, const int inputx);
void inarray(char bomb[20][34], int hang, int lie, int bomb_num);
void BFS(char bomb[20][34], int display[20][34], int hang, int lie, int y, int x);

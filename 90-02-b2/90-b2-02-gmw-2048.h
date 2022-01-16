/*2053628 ����� ������*/
#pragma
#define NFULL 901
#define FULL  902
const int frame_wide1 = 38;
const int frame_high1 = 20;

struct board_param {
	int rows;
	int cols;
	int goal_score;
	int delay;
};

struct for_file {
	int rows;
	int cols;
	int high_score;
};

void init_choice_frame(CONSOLE_GRAPHICS_INFO& CGI_2048);

/*move.cpp�еĺ���*/
void intit_play_frame(CONSOLE_GRAPHICS_INFO* pCGI, board_param param_2048);
/*for_play���õ��ƶ����жϵ�С����*/
int random_pos(CONSOLE_GRAPHICS_INFO* pCGI, board_param param_2048, const BLOCK_DISPLAY_INFO bdi_2048[], int backgd[12][12], bool first_on);
int move_to_end(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int backgd[12][12], int direction, const BLOCK_DISPLAY_INFO bdi_2048[], int row_col);
int block_move(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int backgd[12][12], int direction, const BLOCK_DISPLAY_INFO bdi_2048[], int& score);
void game_fin(CONSOLE_GRAPHICS_INFO* pCGI, bool success);
int check_maxblock(int backgd[12][12], board_param param_2048);
int check_full(int backgd[12][12], board_param param_2048);
void score_display(const CONSOLE_GRAPHICS_INFO* const pCGI, board_param param_2048, int score, clock_t start, int history_high);

/*choice�еĺ���*/
void init_choice_frame(CONSOLE_GRAPHICS_INFO& CGI_2048);
void input_choice(board_param& param_2048);







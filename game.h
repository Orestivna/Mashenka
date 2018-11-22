#if ! defined(_GAME_KUDUSHTEEV_H_)
#define _GAME_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif
#define STAGE_WIDTH    5800
#define PAGE_COLS      16
#define MAX_USERS      2
#define U_LEFT_STOP    0
#define U_LEFT_STEP    1
#define U_LEFT_JUMP    2
#define U_RIGHT_STOP   3
#define U_RIGHT_STEP   4
#define U_RIGHT_JUMP   5

#define CELL_NONE      0
#define CELL_VERT      1
#define CELL_CLIP_OBJ  15
#define CELL_MAGNIT    18
#define CELL_FIRE      19
#define CELL_POWER     20

#define STEP_VELOCITY  2
#define JUMP_VELOCITY  3

#define LIFT_A         5
#define LIFT_B         6

#define CELL_BOOM_2    2
#define CELL_BOOM_5    5

#define ALIEN_SIZE     32
#define ALIEN_RADIUS   16
#define CRASH_CX       63
#define CRASH_CY       60

#define LAZER_WIDTH    7

#define MAX_ALIENS     3

//точка
struct point {
	int x, y;
};

//лифт
struct slift {
	short int x, y, velocity;
	short int pos_start, pos_end;
	BYTE type;
	BYTE dir;
};

//взрыв
struct sboom {
	sprite sp;
	short int x, y;
};

//пули
struct sbullet {
	short int x, y, dir_x;
	sbullet(void){}
	sbullet(short int _x, short int _y, short int _dir):x(_x), y(_y), dir_x(_dir){}	
};

//враги
struct salien {
	short int x, y, start_x, end_x, velocity;
	BYTE dir, type;
};

//вражеские окружности
struct scircle {
	int   x1, y1, x0, y0, off;
	float velocity;
};

//лазер
struct slazer {
	DWORD delay, tick;
	short int x, y, height, check;
};


//класс игры
class quest_game: public base_form {
private:
	int       state_user;
	int       vel_user;
	int       stage_pos;
	int       stage_edge;
	int       jump_top;
	int       rc_page;
	int       move_water;
	DWORD     time_water;
	DWORD     time_move;
	int       dir_water;
	int       top_water;
	e_menu    state_game;
	UINT      life;
	BOOL      jump_user;
	BOOL      fly_user;
	BOOL      move_right;
	DWORD     level;
	game_menu menu;
	gdi_pen   pen_life;
	gdi_pen   pen_uron;
	gdi_brush brush_water;
	point     pos_user;

	BYTE     fmap[MAX_ROWS][MAX_COLS];
	q_levels levels;
	bitmap   bmp_users[MAX_USERS];
	bitmap   tiles;
	bitmap   tiles2;
	bitmap   bmp_map;
	bitmap   bmp_view;
	bitmap   bmp_back;
	bmp_mask mask_map;
	bmp_mask mask_t2;
	bmp_mask mask_users[MAX_USERS];
	sprite   sp_fire;
	sprite   sp_power;
	sprite   sp_crash;
	sprite   sp_aliens[MAX_ALIENS];

	dsound   snd_taran;
	dsound   snd_down;
	dsound   snd_boom;
	dsound   snd_lazer;
	dsound   snd_jump;
	dsound   snd_head;
	dsound   snd_music;

	sblock<sprite, 6>  users;
	sblock<sboom,  5>  booms;
	pod_array<salien>  aliens;
	pod_array<scircle> circles;
	pod_array<slazer>  lazers;
	pslist<slift>      lifts;
public:
	quest_game(void);
	virtual ~quest_game();
private:
	virtual void onCreate(HINSTANCE hinst);
	virtual void onKeyPress(LPBYTE keys);
	virtual void onKeyDown(WORD key);
	virtual void onUpdateFrame(DWORD msec);
	virtual void onMouseMove(int x, int y);
	virtual void onLeftMouseDown(int x, int y);
	virtual void onDrawing(HDC hDC);
	virtual void onDestroy(void);
private:
	void activate_level(void);
	void update(DWORD msec);
	void controls(LPBYTE keys);
	int  isBlock(int x, int y, DWORD* pinx = NULL);
	void flyDown(void);
	BOOL isUpDown(void);
	void draw_game(HDC hDC);
	bool typeBlock(int type);
	void boom_block(int row, int col);
	BOOL isDownFire(void);
	bool isGameOver(void);
	void putBoom(int x, int y, int size);
	bool exec_uron(void);
	void copy_view(void);
	void exec_menu(int cmd);
	void stop_game(void);
	void draw_life(HDC hDC);
};


#endif

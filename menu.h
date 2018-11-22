#if ! defined(_MENU_KUDUSHTEEV_H_)
#define _MENU_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

enum e_menu { 
	GAME_MENU = 0, GAME_CONTROL = 1, GAME_HELP = 2, GAME_LEVEL = 3,
	GAME_OVER = 4, GAME_PAUSE   = 5, GAME_PLAY = 6, GAME_WIN   = 7, GAME_NOT = 8
};

//Ã≈Õﬁ √–»
class game_menu {
private:
	rus_font font;
	int      sel_cmd;
	int      top;
	DWORD    ltime;
public:
	game_menu(void);
	~game_menu();

	game_menu(const game_menu&);
	game_menu& operator = (const game_menu&);
public:
	void initialize(HINSTANCE hinst);
	void destroy(void);
	void draw_menu(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask);
	void draw_control(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask);
	void draw_help(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask, bitmap* bmp1, bmp_mask* mask1);
	void draw_level(HDC hDC, DWORD level, DWORD msec, int width, int height);
	void draw_pause(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask);
	void draw_over(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask, bitmap* bmp1, bmp_mask* mask1);
	void draw_victory(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask);

	bool mouse_move(e_menu state_game, int x, int y, int swidth);
	void key_down(e_menu state_game, WORD key);

	void setTimeout(DWORD tick){
		ltime = tick + 1500UL;
	}

	void setSelected(int sel = 0){
		sel_cmd = sel;
	}

	int getSelected(void) const {
		return sel_cmd;
	}

	bool isTimeout(DWORD msec){
		return (ltime < msec);
	}

private:
	void draw_button(HDC hDC, DWORD msec, const char* cmds[], int num, int swidth, bitmap* bmp, bmp_mask* mask);
	int  int_to_str(LPSTR s, DWORD n);
};


#endif

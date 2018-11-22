#include "headers.h"
#define BUTTON_WIDTH   300
#define BUTTON_HEIGHT  40
#define s_len(s) static_cast<int>(sizeof((s))/sizeof((s[0])) - 1)
#define d_len(s) static_cast<int>(strlen((s)))


game_menu::game_menu(void):sel_cmd(0),
                           top(0), 
                           ltime(0){}

game_menu::~game_menu(){
	destroy();
}


void game_menu::initialize(HINSTANCE hinst){
	font.load(hinst, IDB_BMP_ALPHA);
}

void game_menu::destroy(void){
	font.destroy();
}


//БШБНД ЛЕМЧ ХЦПШ
void game_menu::draw_menu(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask){
	top = 160;
	PatBlt(hDC, 0, 0, width, height, BLACKNESS);

	const char game[] = "опхцндх люьемэйх";
	font.drawLine(hDC, (width - s_len(game) * font.getWidth())/2 + 80 * _tsin(static_cast<float>(msec) * 0.001f), 50, game);

	int i, k = width / 40;
	for(i = 0; i < k; ++i){
		tiles->draw(hDC, i*40,  0, 40, 40, 280, 0);
		tiles->draw(hDC, i*40, 80, 40, 40, 280, 0);
		tiles->draw(hDC, i*40, height - 40, 40, 40, 280, 0);
	}

	const int _N = 4;
	const char* cmds[_N] = {"онвюрх цпюрх", "опюбхкю цпх", "яопюбйю", "бхирх"};
	draw_button(hDC, msec, cmds, _N, width, bmp, mask);

	const char author[] = "юбрнп люьемэйю";
	font.drawLine(hDC, (width - s_len(author)*10)/2, height - 70, 10, 11, author);
}


//бхб╡д лема опюбхкю
void game_menu::draw_control(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask){
	top = 390;
	PatBlt(hDC, 0, 0, width, height, BLACKNESS);

	const char game[] = "опюбхкю цпх";
	font.drawLine(hDC, (width - s_len(game) * font.getWidth())/2, 5, game);

	int i, k = width / 40;
	for(i = 0; i < k; ++i){
		tiles->draw(hDC, i*40, 30, 40, 40, 240, 0);
		tiles->draw(hDC, i*40, height - 40, 40, 40, 240, 0);
	}

	const char ctrl[] = "псу бкебн ймнойю бкебн юан ю\nпсу бопюбн ймнойю бопюбн юан б\nярпханй ймнойю опна╡к юан окчя";
	font.drawText(hDC, 48, 110, ctrl);
						
	const char desc[] = "псу ббепу рю бмхг кхье он яундюу";
	font.drawLine(hDC, (width - s_len(desc)*13)/2, 210, 13, 15, desc);

	const char ctrl1[] = "псу бепу ймнойю бепу юан ж\nпсу бмхг ймнойю бмхг юан ╡";
	font.drawText(hDC, 48, 240, ctrl1);

	const char ctrl2[] = "оюсгю - еяйеио";
	font.drawText(hDC, (width - s_len(ctrl2)*font.getWidth())/2, 300, ctrl2);

	const char* cmds[1] = {"мюгюд б лемч"};
	draw_button(hDC, msec, cmds, 1, width, bmp, mask);
}


//бхб╡д лемч яопюбйю
void game_menu::draw_help(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask, bitmap* bmp1, bmp_mask* mask1){
	top = 390;
	PatBlt(hDC, 0, 0, width, height, BLACKNESS);

	const char game[] = "яопюбйю";
	font.drawLine(hDC, (width - s_len(game) * font.getWidth())/2, 5, game);

	int i, k = width / 40;
	for(i = 0; i < k; ++i){
		tiles->draw(hDC, i*40, 30, 40, 40, 320, 0);
		tiles->draw(hDC, i*40, height - 40, 40, 40, 320, 0);
	}

	const char help[] = "акнйх ын опнахбючряъ цнкнбнч";
	font.drawLine(hDC, (width - s_len(help)*font.getWidth())/2, 80, help);

	int left = (width - 4*50) / 2;
	for(i = 0; i < 4; ++i)
		tiles->draw(hDC, left + i*50, 105, 40, 40, 40*i, 0);

	const char help1[] = "акнй люцмхр ярпханй мелнфкхбхи";
	font.drawLine(hDC, (width - s_len(help1)*font.getWidth())/2, 160, help1);	

	bmp1->draw_clip(hDC, (width - 40)/2, 185, 40, 40, 160, 0, mask1);

	const char help2[] = "дкъ оепелеыемхъ ббепу х бмхг";
	font.drawLine(hDC, (width - s_len(help2)*font.getWidth())/2, 240, help2);	

	bmp1->draw_clip(hDC, (width - 40)/2 - 30, 265, 40, 40, 200, 0, mask1);
	bmp1->draw_clip(hDC, (width - 40)/2 + 30, 265, 40, 40, 240, 0, mask1);

	const char  desc[] = "бюью гюдювю опнирх ерюох\nг онвюрйс дн й╡мжъ";
	font.drawCenter(hDC, 173, 325, 10, 12, desc);

	const char* cmds[1] = {"мюгюд б лемч"};
	draw_button(hDC, msec, cmds, 1, width, bmp, mask);
}


//мнлеп п╡бмъ
void game_menu::draw_level(HDC hDC, DWORD level, DWORD msec, int width, int height){
	PatBlt(hDC, 0, 0, width, height, BLACKNESS);

	const char lvl[] = "ерюо";
	font.drawLine(hDC, (width - s_len(lvl)*font.getWidth())/2, height / 2 - 40, lvl);

	int      ret = SetBkMode(hDC, TRANSPARENT);
	COLORREF col = SetTextColor(hDC, RGB(0xFF, 0xFF, 0));

	char s[16];
	int  n = int_to_str(s, level + 1);

	SIZE sz = {0};
	if(!GetTextExtentPoint32A(hDC, s, n, &sz))
		sz.cx = sz.cy = 10;

	TextOutA(hDC, (width - sz.cx * n) / 2, (height - sz.cy) / 2, s, n);

	SetTextColor(hDC, col);
	SetBkMode(hDC, ret);
}


//оюсгю
void game_menu::draw_pause(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask){
	top = 300;
	PatBlt(hDC, 0, 0, width, height, BLACKNESS);

	const char lvl[] = "оюсгю";
	font.drawLine(hDC, (width - s_len(lvl)*font.getWidth())/2, height / 2 - 120, lvl);

	const char* cmds[2] = {"опнднбфхрх цпс", "бхирх б лемч"};
	draw_button(hDC, msec, cmds, 2, width, bmp, mask);
}


//опнцпюь
void game_menu::draw_over(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask, bitmap* bmp1, bmp_mask* mask1){
	top = 300;
	PatBlt(hDC, 0, 0, width, height, BLACKNESS);

	const char game[] = "бх опнцпюкх";
	font.drawLine(hDC, (width - s_len(game) * font.getWidth())/2, 95, game);

	int i, k = width / 40;
	for(i = 0; i < k; ++i){
		tiles->draw(hDC, i*40, 0, 40, 40, 40, 0);
		tiles->draw(hDC, i*40, height - 40, 40, 40, 40, 0);
	}

	float t = _tsin(static_cast<float>(msec) * 0.003f);
	bmp1->draw_clip(hDC, ((width - 23) / 2) + 100 *  t, 180, 23, 21, 17, 1, mask1);
	bmp1->draw_clip(hDC, ((width - 23) / 2) + 100 * -t, 210, 23, 21, 17, 1, mask1);

	const char* cmds[2] = {"онвюрх яонвюрйс", "бхирх б лемч"};
	draw_button(hDC, msec, cmds, 2, width, bmp, mask);
}


//оепелнфежэ
void game_menu::draw_victory(HDC hDC, DWORD msec, int width, int height, bitmap* tiles, bitmap* bmp, bmp_mask* mask){
	top = 300;
	PatBlt(hDC, 0, 0, width, height, BLACKNESS);

	const char game[] = "бх опниькх бяч цпс";
	font.drawLine(hDC, (width - s_len(game) * font.getWidth())/2, 95, game);

	int i, k = width / 40;
	for(i = 0; i < k; ++i){
		tiles->draw(hDC, i*40, 0, 40, 40, 120, 0);
		tiles->draw(hDC, i*40, height - 40, 40, 40, 120, 0);
	}

	const char* cmds[2] = {"онвюрх яонвюрйс", "бхирх б лемч"};
	draw_button(hDC, msec, cmds, 2, width, bmp, mask);	
}


//псу лхьйх
bool game_menu::mouse_move(e_menu state_game, int x, int y, int swidth){
	int h = BUTTON_HEIGHT / 3;
	int n = 0;

	switch(state_game){
	case GAME_MENU:
		n = 4;
		break;
	case GAME_CONTROL:
	case GAME_HELP:
		n = 1;
		break;
	case GAME_LEVEL:
		return false;
	default:
		n = 2;
		break;
	}

	int left = (swidth - BUTTON_WIDTH) >> 1;
	for(int i = 0; i < n; ++i, top += font.getHeight() + 50){
		if((x >= left && x <= left + BUTTON_WIDTH) && (y >= top - h && y <= top - h + BUTTON_HEIGHT)){
			sel_cmd = i;
			return true;
		}
	}
	return false;
}


//напнайю йкюб╡ь
void game_menu::key_down(e_menu state_game, WORD key){
	int n = 0;
	switch(state_game){
	case GAME_MENU:
		n = 4;
		break;
	case GAME_CONTROL:
	case GAME_HELP:
		n = 1;
		break;
	case GAME_LEVEL:
		return;
	default:
		n = 2;
		break;
	}

	switch(key){
	case VK_UP:
	case 'W':
		if(sel_cmd > 0)
			--sel_cmd;
		break;
	case VK_DOWN:
	case 'S':
		if(sel_cmd < (n - 1))
			++sel_cmd;
		break;
	}
}


//бхб╡д ймнонй
void game_menu::draw_button(HDC hDC, DWORD msec, const char* cmds[], int num, int swidth, bitmap* bmp, bmp_mask* mask){
	float t;
	int   k = BUTTON_HEIGHT / 3;
	int   x = (swidth - BUTTON_WIDTH) >> 1, h = font.getHeight() + 50;
	int   y = top;
	for(int i = 0; i < num; ++i, y += h){
		if(i == sel_cmd){
			bmp->draw_size(hDC, x, y - k, BUTTON_WIDTH, BUTTON_HEIGHT, 319, 141, 20, 26);

			t = static_cast<float>(msec) * 0.2f;
			bmp->draw_rotate(hDC,  t, x - 40, y - 9, ALIEN_SIZE, ALIEN_SIZE, 348, 168, mask);
			bmp->draw_rotate(hDC, -t, x + BUTTON_WIDTH + 8, y - 9, ALIEN_SIZE, ALIEN_SIZE, 348, 168, mask);
		} else
			bmp->draw_size(hDC, x, y - k, BUTTON_WIDTH, BUTTON_HEIGHT, 340, 141, 20, 26);

		font.drawLine(hDC, (swidth - d_len(cmds[i]) * font.getWidth())/2, y, cmds[i], SRCPAINT);
	}
}


//йнмбепюж╡ъ б ярп╡вйс
int game_menu::int_to_str(LPSTR s, DWORD n){
	LPSTR t = s;
	do {
		*s++ = static_cast<CHAR>(n % 10) + '0';
	} while((n /= 10) != 0);

	int m = static_cast<int>(s - t);
	*s--  = '\0';

	CHAR c;
	for(LPSTR p = t; p < s; --s, ++p){
		c  = *p;
		*p = *s;
		*s = c;
	}
	return m;
}

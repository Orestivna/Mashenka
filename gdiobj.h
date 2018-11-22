#if ! defined(_GDI_OBJECTS_KUDUSHTEEV_H_)
#define _GDI_OBJECTS_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif


//карандаш
class gdi_pen {
private:
	HPEN pen;
public:
	gdi_pen(void);
	~gdi_pen();

	gdi_pen(const gdi_pen&);
	gdi_pen& operator = (const gdi_pen&);
public:
	bool create(int style, int bold, COLORREF color);
	void destroy(void);

	HGDIOBJ selectObj(HDC hDC){
		return SelectObject(hDC, pen);
	}

	static void line(HDC hDC, int x1, int y1, int x2, int y2){
		MoveToEx(hDC, x1, y1, NULL);
		LineTo(hDC, x2, y2);
	}
};


//кисть
class gdi_brush {
private:
	HBRUSH hbr;
public:
	gdi_brush(void);
	~gdi_brush();

	gdi_brush(const gdi_brush&);
	gdi_brush& operator = (const gdi_brush&);
public:
	bool create(COLORREF color);
	bool create(HINSTANCE hinst, UINT id);
	void destroy(void);

	HGDIOBJ selectObj(HDC hDC){
		return SelectObject(hDC, hbr);
	}

	void fill(HDC hDC, int x, int y, int w, int h){
		RECT rc = { x, y, x + w, y + h };
		FillRect(hDC, &rc, hbr);
	}
};


//класс-монохромная маска
class bmp_mask {
private:
	HBITMAP hbm;
public:
	bmp_mask(void);
	~bmp_mask();

	bmp_mask(const bmp_mask&);
	bmp_mask& operator = (const bmp_mask&);
public:
	bool create(int width, int height);
	void destroy(void);

	HBITMAP getHandle(void) const {
		return hbm;
	}
};


//класс-битмап
class bitmap {
private:
	HBITMAP hbm;
	HDC     mdc;
	int     width;
	int     height;
public:
	bitmap(void);
	~bitmap();

	bitmap(const bitmap&);
	bitmap& operator = (const bitmap&);
public:
	bool load(LPCSTR filename);
	bool load(HINSTANCE hinst, UINT id);
	bool create(int _width, int _height);
	bool from_mask(bmp_mask* mask);
	bool from_mask(bmp_mask* mask, COLORREF color);
	void destroy(void);

	void draw(HDC hDC, int x, int y, DWORD rop = SRCCOPY);
	void draw(HDC hDC, int x, int y, int w, int h, DWORD rop = SRCCOPY);
	void draw(HDC hDC, int x, int y, int w, int h, int ox, int oy, DWORD rop = SRCCOPY);

	void draw_size(HDC hDC, int x, int y, int w, int h, DWORD rop = SRCCOPY);
	void draw_size(HDC hDC, int x, int y, int w, int h, int ox, int oy, int cx, int cy, DWORD rop = SRCCOPY);

	void draw_clip(HDC hDC, int x, int y, bmp_mask* mask, DWORD rop = MAKEROP4(SRCCOPY, SRCPAINT));
	void draw_clip(HDC hDC, int x, int y, int cx, int cy, bmp_mask* mask, DWORD rop = MAKEROP4(SRCCOPY, SRCPAINT));
	void draw_clip(HDC hDC, int x, int y, int cx, int cy, int ox, int oy, bmp_mask* mask, DWORD rop = MAKEROP4(SRCCOPY, SRCPAINT));

	void draw_rotate(HDC hDC, float degree, int x, int y, bmp_mask* mask);
	void draw_rotate(HDC hDC, float degree, int x, int y, int cx, int cy, bmp_mask* mask);
	void draw_rotate(HDC hDC, float degree, int x, int y, int cx, int cy, int ox, int oy, bmp_mask* mask);

	HDC getDC(void) const {
		return mdc;
	}

	HBITMAP getHandle(void) const {
		return hbm;
	}

	int getWidth(void) const {
		return width;
	}

	int getHeight(void) const {
		return height;
	}

private:
	bool _init_dbuf(void);
};


#define SP_STOP           0
#define SP_PLAY           1
#define SP_LOOP           2
#define SP_PLAYBACK       3
#define SP_LOOPBACK       4
#define SP_NEXT_PREV      5
#define SP_NEXT_PREV_LOOP 6

//анимационный спрайт
class sprite {
private:
	DWORD     ltime;
	DWORD     delay;
	short int state;
	short int dir;
	short int width;
	short int height;
	short int px;
	short int py;
	short int sizex;
	short int sizey;
	short int start_row;
	short int start_col;
	short int off_x;
	short int off_y;
public:
	sprite(void);
public:
	void create(int w, int h, DWORD time_delay, int row1 = 0, int col1 = 0, int row2 = 0, int col2 = 0);
	void create(const bitmap* hobj, int num_cols, int num_rows, DWORD time_delay, int row1 = 0, int col1 = 0, int row2 = 0, int col2 = 0);
	void draw(HDC hDC, const bitmap* hobj, int x, int y, DWORD rop = SRCCOPY);
	void draw_size(HDC hDC, const bitmap* hobj, int x, int y, int w, int h, DWORD rop = SRCCOPY);
	void draw_clip(HDC hDC, const bitmap* hobj, const bmp_mask* mask, int x, int y, DWORD rop = MAKEROP4(SRCCOPY, SRCPAINT));
	void draw_rotate(HDC hDC, const bitmap* hobj, const bmp_mask* mask, int x, int y, float degree);

	void play(int flag = SP_PLAY);
	void stop(void);

	void setOffset(int ox, int oy){
		off_x = ox;
		off_y = oy;
	}

	BOOL updateFrame(DWORD msec);

	int getWidth(void) const {
		return sizex;
	}

	int getHeight(void) const {
		return sizey;
	}

	bool isPlay(void) const {
		return (state != SP_STOP);
	}

	bool isStop(void) const {
		return (state == SP_STOP);
	}
};


//класс русский шрифт, алфавит
class rus_font {
private:
	bitmap bmp;
	int    width;
public:
	rus_font(void);
	~rus_font();

	rus_font(const rus_font&);
	rus_font& operator = (const rus_font&);
public:
	bool load(HINSTANCE hinst, UINT id);
	void destroy(void);

	void drawLine(HDC hDC, int x, int y, const char* s, DWORD rop = SRCCOPY);
	void drawLine(HDC hDC, int x, int y, int w, int h, const char* s, DWORD rop = SRCCOPY);

	void drawText(HDC hDC, int x, int y, const char* s, DWORD rop = SRCCOPY);
	void drawText(HDC hDC, int x, int y, int w, int h, const char* s, DWORD rop = SRCCOPY);

	void drawCenter(HDC hDC, int x, int y, const char* s, DWORD rop = SRCCOPY);
	void drawCenter(HDC hDC, int x, int y, int w, int h, const char* s, DWORD rop = SRCCOPY);

	int getWidth(void) const {
		return width;
	}

	int getHeight(void) const {
		return bmp.getHeight();
	}

private:
	int max_line(const char* s);
};


#endif

#include "headers.h"
static void _calc_angle(LPPOINT ds, float angle, int x, int y, int width, int height);


gdi_pen::gdi_pen(void):pen(NULL){}

gdi_pen::~gdi_pen(){
	destroy();
}

bool gdi_pen::create(int style, int bold, COLORREF color){
	pen = CreatePen(style, bold, color);
	return (pen != NULL);
}
	
void gdi_pen::destroy(void){
	if(pen != NULL)
		DeleteObject(pen);
	pen = NULL;
}


//------------------------------------------------------------------------------------------------------


gdi_brush::gdi_brush(void):hbr(NULL){}

gdi_brush::~gdi_brush(){
	destroy();
}

bool gdi_brush::create(COLORREF color){
	destroy();
	hbr = CreateSolidBrush(color);
	return (hbr != NULL);
}

bool gdi_brush::create(HINSTANCE hinst, UINT id){
	HBITMAP bmp = LoadBitmapA(hinst, MAKEINTRESOURCEA(id));
	if(bmp == NULL)
		return false;

	destroy();
	hbr = CreatePatternBrush(bmp);
	DeleteObject(bmp);
	return (hbr != NULL);
}

void gdi_brush::destroy(void){
	if(hbr != NULL)
		DeleteObject(hbr);
	hbr = NULL;
}


//------------------------------------------------------------------------------------------------------


bmp_mask::bmp_mask(void):hbm(NULL){}

bmp_mask::~bmp_mask(){
	destroy();
}

//создание
bool bmp_mask::create(int width, int height){
	destroy();
	hbm = CreateBitmap(width, height, 1, 1, NULL);
	return (hbm != NULL);
}

//удаление
void bmp_mask::destroy(void){
	if(hbm != NULL)
		DeleteObject(hbm);
	hbm = NULL;
}


//------------------------------------------------------------------------------------------------------


bitmap::bitmap(void):hbm(NULL),
                     mdc(NULL),
                     width(0),
                     height(0){}

bitmap::~bitmap(){
	destroy();
}

//загрузка из файла
bool bitmap::load(LPCSTR filename){
	destroy();
	hbm = reinterpret_cast<HBITMAP>(LoadImageA(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	if(hbm != NULL)
		return _init_dbuf();
	return true;
}

//загрузка из ресурсов
bool bitmap::load(HINSTANCE hinst, UINT id){
	destroy();
	hbm = LoadBitmapA(hinst, MAKEINTRESOURCEA(id));
	if(hbm != NULL)
		return _init_dbuf();
	return true;
}

//создание
bool bitmap::create(int _width, int _height){
	destroy();
	HDC hdc = GetDC(NULL);
	if((mdc = CreateCompatibleDC(hdc)) == NULL){
		ReleaseDC(NULL, hdc);
		return false;
	}

	if((hbm = CreateCompatibleBitmap(hdc, _width, _height)) == NULL){
		ReleaseDC(NULL, hdc);
		destroy();
		return false;
	}
	ReleaseDC(NULL, hdc);

	width  = _width;
	height = _height;
	SelectObject(mdc, hbm);

	PatBlt(mdc, 0, 0, width, height, BLACKNESS);
	return true;
}

//создание маски
bool bitmap::from_mask(bmp_mask* mask){
	return from_mask(mask, GetPixel(mdc, 0, 0));
}

bool bitmap::from_mask(bmp_mask* mask, COLORREF color){
	int   x, y, j, i;
	LONG  size, ch;
	BYTE* ptr, *mbs = NULL;

	if(hbm == NULL)
		return false;

	mask->destroy();
	if(!mask->create(width, height))
		return false;

	BITMAP inf;
	if(! GetObject(mask->getHandle(), sizeof(inf), (LPVOID)&inf))
		goto err;

	size = GetBitmapBits(mask->getHandle(), 0, NULL);
	if(size <= 0)
		goto err;

	mbs = (BYTE*)HeapAlloc(GetProcessHeap(), 0, static_cast<DWORD>(size));
	if(mbs == NULL)
		goto err;

	size = GetBitmapBits(mask->getHandle(), size, (LPVOID)mbs);
	if(!size)
		goto err;

	ch = (inf.bmHeight < 0) ? -inf.bmHeight : inf.bmHeight;
	for(y = 0; y < ch; ++y){
		if(inf.bmHeight > 0)
			ptr = mbs + (inf.bmWidthBytes * y);
		else
			ptr = mbs + (inf.bmWidthBytes * (ch - 1 - y));

		for(x = 0; x < inf.bmWidth; ++x){
			j = x / 8;
			i = 7 - x % 8;
			if(GetPixel(mdc, x, y) == color){
				ptr[j] &= ~(1 << i);
				SetPixelV(mdc, x, y, RGB(0, 0, 0));
			} else
				ptr[j] |= 1 << i;
		}
	}

	SetBitmapBits(mask->getHandle(), (DWORD)size, (LPCVOID)mbs);
	HeapFree(GetProcessHeap(), 0, mbs);
	return true;
err:
	if(mbs != NULL)
		HeapFree(GetProcessHeap(), 0, mbs);
	return false;
}

//уничтожение
void bitmap::destroy(void){
	if(hbm != NULL)
		DeleteObject(hbm);
	hbm = NULL;

	if(mdc != NULL)
		DeleteDC(mdc);
	mdc = NULL;

	width = height = 0;
}

//вывод на дисплей
void bitmap::draw(HDC hDC, int x, int y, DWORD rop){
	BitBlt(hDC, x, y, width, height, mdc, 0, 0, rop);
}

void bitmap::draw(HDC hDC, int x, int y, int w, int h, DWORD rop){
	BitBlt(hDC, x, y, w, h, mdc, 0, 0, rop);
}

void bitmap::draw(HDC hDC, int x, int y, int w, int h, int ox, int oy, DWORD rop){
	BitBlt(hDC, x, y, w, h, mdc, ox, oy, rop);
}

void bitmap::draw_size(HDC hDC, int x, int y, int w, int h, DWORD rop){
	StretchBlt(hDC, x, y, w, h, mdc, 0, 0, width, height, rop);
}

void bitmap::draw_size(HDC hDC, int x, int y, int w, int h, int ox, int oy, int cx, int cy, DWORD rop){
	StretchBlt(hDC, x, y, w, h, mdc, ox, oy, cx, cy, rop);
}

//вывод с отсечением + маска
void bitmap::draw_clip(HDC hDC, int x, int y, bmp_mask* mask, DWORD rop){
	MaskBlt(hDC, x, y, width, height, mdc, 0, 0, mask->getHandle(), 0, 0, rop);
}

void bitmap::draw_clip(HDC hDC, int x, int y, int cx, int cy, bmp_mask* mask, DWORD rop){
	MaskBlt(hDC, x, y, cx, cy, mdc, 0, 0, mask->getHandle(), 0, 0, rop);
}

void bitmap::draw_clip(HDC hDC, int x, int y, int cx, int cy, int ox, int oy, bmp_mask* mask, DWORD rop){
	MaskBlt(hDC, x, y, cx, cy, mdc, ox, oy, mask->getHandle(), ox, oy, rop);
}

//вывод с вращением
void bitmap::draw_rotate(HDC hDC, float degree, int x, int y, bmp_mask* mask){
	POINT ds[3];
	_calc_angle(ds, degree * (M_PI / 180.0f), x, y, width, height);
	PlgBlt(hDC, ds, mdc, 0, 0, width, height, mask->getHandle(), 0, 0);	
}

void bitmap::draw_rotate(HDC hDC, float degree, int x, int y, int cx, int cy, bmp_mask* mask){
	POINT ds[3];
	_calc_angle(ds, degree * (M_PI / 180.0f), x, y, cx, cy);
	PlgBlt(hDC, ds, mdc, 0, 0, cx, cy, mask->getHandle(), 0, 0);	
}

void bitmap::draw_rotate(HDC hDC, float degree, int x, int y, int cx, int cy, int ox, int oy, bmp_mask* mask){
	POINT ds[3];
	_calc_angle(ds, degree * (M_PI / 180.0f), x, y, cx, cy);
	PlgBlt(hDC, ds, mdc, ox, oy, cx, cy, mask->getHandle(), ox, oy);	
}


bool bitmap::_init_dbuf(void){
	BITMAP inf;
	HDC hdc = GetDC(NULL);
	if((mdc = CreateCompatibleDC(hdc)) == NULL){
		ReleaseDC(NULL, hdc);
		return false;
	}
	ReleaseDC(NULL, hdc);

	if(GetObject(hbm, sizeof(inf), (LPVOID)&inf)){
		width  = inf.bmWidth;
		height = (inf.bmHeight < 0) ? -inf.bmHeight : inf.bmHeight; 
	}
	SelectObject(mdc, hbm);
	return true;
}


//---------------------------------------------------------------------------------------------------------


sprite::sprite(void):ltime(0),
                     delay(0),
                     state(SP_STOP),
                     dir(0),
                     width(0),
                     height(0),
                     px(0),
                     py(0),
                     sizex(0),
                     sizey(0),
                     start_row(0),
                     start_col(0),
                     off_x(0),
                     off_y(0) {}


//инициализация
void sprite::create(int w, int h, DWORD time_delay, int row1, int col1, int row2, int col2){
	sizex  = w;
	sizey  = h;
	delay  = time_delay;
	px     = 0;
	py     = 0;

	start_row = row1 * sizey;
	start_col = col1 * sizex;

	width  = col2 * sizex;
	height = row2 * sizey;
	off_x  = off_y = 0;
}

//инициализация
void sprite::create(const bitmap* hobj, int num_cols, int num_rows, DWORD time_delay, int row1, int col1, int row2, int col2){
	sizex  = (hobj->getWidth()  / num_cols);
	sizey  = (hobj->getHeight() / num_rows);
	delay  = time_delay;
	px     = 0;
	py     = 0;

	start_row = row1 * sizey;
	start_col = col1 * sizex;

	if(col2 > 0)
		width = col2 * sizex;
	else
		width = hobj->getWidth();

	if(row2 > 0)
		height = row2 * sizey;
	else
		height = hobj->getHeight();

	off_x  = off_y = 0;
}

//вывод
void sprite::draw(HDC hDC, const bitmap* hobj, int x, int y, DWORD rop){
	BitBlt(hDC, x, y, sizex, sizey, hobj->getDC(), off_x + px, off_y + py, rop);
}

void sprite::draw_size(HDC hDC, const bitmap* hobj, int x, int y, int w, int h, DWORD rop){
	StretchBlt(hDC, x, y, w, h, hobj->getDC(), off_x + px, off_y + py, sizex, sizey, rop);
}

void sprite::draw_clip(HDC hDC, const bitmap* hobj, const bmp_mask* mask, int x, int y, DWORD rop){
	int ox = off_x + px;
	int oy = off_y + py;
	MaskBlt(hDC, x, y, sizex, sizey, hobj->getDC(), ox, oy, mask->getHandle(), ox, oy, rop);
}

void sprite::draw_rotate(HDC hDC, const bitmap* hobj, const bmp_mask* mask, int x, int y, float degree){
	POINT ds[3];
	int ox = off_x + px;
	int oy = off_y + py;
	_calc_angle(ds, degree * (M_PI / 180.0f), x, y, sizex, sizey);
	PlgBlt(hDC, ds, hobj->getDC(), ox, oy, sizex, sizey, mask->getHandle(), ox, oy);
}

//начать проигрывание
void sprite::play(int flag){
	state = flag;
	switch(state){
	case SP_PLAY:
	case SP_LOOP:
	case SP_NEXT_PREV:
	case SP_NEXT_PREV_LOOP:
		px  = start_col;
		py  = start_row;
		dir = 0;
		break;
	case SP_PLAYBACK: 
	case SP_LOOPBACK:
		px = width  - sizex;
		py = height - sizey;
		break;
	}
}

//остановка
void sprite::stop(void){
	switch(state){
	case SP_PLAY:
	case SP_LOOP:
	case SP_NEXT_PREV:
	case SP_NEXT_PREV_LOOP:
		px  = start_col;
		py  = start_row;
		dir = 0;
		break;
	case SP_PLAYBACK:
	case SP_LOOPBACK:
		px = width  - sizex;
		py = height - sizey;
		break;
	}
	state = SP_STOP;
}

//обновление анимации
BOOL sprite::updateFrame(DWORD msec){
	switch(state){
	case SP_PLAY:
	case SP_LOOP:

		if((msec - ltime) < delay)
			break;
		ltime = msec;

		px += sizex;
		if(px >= (width - 1)){
			px  = start_col;
			py += sizey;
			if(py >= (height - 1)){
				py = start_row;
				if(state == SP_PLAY)
					stop();
				return TRUE;
			}
		}
		break;
	case SP_PLAYBACK:
	case SP_LOOPBACK:

		if((msec - ltime) < delay)
			break;
		ltime = msec;

		px -= sizex;
		if(px < start_col){
			px  = width - sizex;
			py -= sizey;
			if(py < start_row){
				px = width  - sizex;
				py = height - sizey;
				if(state == SP_PLAYBACK)
					stop();
				return TRUE;
			}
		}
		break;
	case SP_NEXT_PREV:
	case SP_NEXT_PREV_LOOP:

		if((msec - ltime) < delay)
			break;
		ltime = msec;

		if(!dir){
			px += sizex;
			if(px >= (width - 1)){
				px  = start_col;
				py += sizey;
				if(py >= (height - 1)){
					px  = width  - sizex * 2;
					py  = height - sizey;
					dir = 1;
				}
			}
		} else {
			px -= sizex;
			if(px < start_col){
				px  = width - sizex;
				py -= sizey;
				if(py < start_row){
					px  = sizex;
					py  = start_row;
					dir = 0;
					if(state == SP_NEXT_PREV)
						stop();
					return TRUE;
				}
			}
		}
		break;
	}
	return FALSE;
}


//---------------------------------------------------------------------------------------------------------


rus_font::rus_font(void):width(0){}

rus_font::~rus_font(){
	destroy();
}


bool rus_font::load(HINSTANCE hinst, UINT id){
	if(bmp.load(hinst, id)){
		width = bmp.getWidth() / 32;
		return true;
	}
	return false;
}

void rus_font::destroy(void){
	bmp.destroy();
}


//вывод однострочного текста
void rus_font::drawLine(HDC hDC, int x, int y, int w, int h, const char* s, DWORD rop){
	const UCHAR* p = reinterpret_cast<const UCHAR*>(s);
	while(*p){
		if(*p >= UCHAR('А') && *p <= UCHAR('Я'))
			bmp.draw_size(hDC, x, y, w, h, (*p - UCHAR('А')) * width, 0, width, bmp.getHeight(), rop);
		
		x += w;
		++p;
	}
}

void rus_font::drawLine(HDC hDC, int x, int y, const char* s, DWORD rop){
	const UCHAR* p = reinterpret_cast<const UCHAR*>(s);
	while(*p){
		if(*p >= UCHAR('А') && *p <= UCHAR('Я'))
			bmp.draw(hDC, x, y, width, bmp.getHeight(), (*p - UCHAR('А')) * width, 0, rop);
		x += width;
		++p;
	}
}


//вывод многострочного текста
void rus_font::drawText(HDC hDC, int x, int y, const char* s, DWORD rop){
	int px = x;
	const UCHAR* p = reinterpret_cast<const UCHAR*>(s);
	while(*p){
		if(*p >= UCHAR('А') && *p <= UCHAR('Я'))
			bmp.draw(hDC, px, y, width, bmp.getHeight(), (*p - UCHAR('А')) * width, 0, rop);
		else if(*p == '\n'){
			y += bmp.getHeight() + 2;
			px = x;
			++p;
			continue;
		}
		px += width;
		++p;
	}
}

void rus_font::drawText(HDC hDC, int x, int y, int w, int h, const char* s, DWORD rop){
	int px = x;
	const UCHAR* p = reinterpret_cast<const UCHAR*>(s);
	while(*p){
		if(*p >= UCHAR('А') && *p <= UCHAR('Я'))
			bmp.draw_size(hDC, px, y, w, h, (*p - UCHAR('А')) * width, 0, width, bmp.getHeight(), rop);
		else if(*p == '\n'){
			y += h + 2;
			px = x;
			++p;
			continue;
		}
		px += w;
		++p;
	}
}


//вывод многострочного текста с выравниванием по центру
void rus_font::drawCenter(HDC hDC, int x, int y, const char* s, DWORD rop){
	int m, px, max_w = max_line(s);

	const UCHAR* t, *p = reinterpret_cast<const UCHAR*>(s);
	while(*p){
		t = p;
		while(*p && (*p != '\n'))
			++p;

		if((m = static_cast<int>(p - t)) == 0){
			y += bmp.getHeight() + 2;
			if(*p)
				++p;
			continue;
		}

		if(m < max_w)
			px = x + (max_w * width - m * width)/2;
		else
			px = x;

		while(t != p){
			if(*t >= UCHAR('А') && *t <= UCHAR('Я'))
				bmp.draw(hDC, px, y, width, bmp.getHeight(), (*t - UCHAR('А')) * width, 0, rop);
			px += width;
			++t;
		}
	}
}

void rus_font::drawCenter(HDC hDC, int x, int y, int w, int h, const char* s, DWORD rop){
	int m, px, max_w = max_line(s);

	const UCHAR* t, *p = reinterpret_cast<const UCHAR*>(s);
	while(*p){
		t = p;
		while(*p && (*p != '\n'))
			++p;

		if((m = static_cast<int>(p - t)) == 0){
			y += h + 2;
			if(*p)
				++p;
			continue;
		}

		if(m < max_w)
			px = x + (max_w * w - m * w)/2;
		else
			px = x;

		while(t != p){
			if(*t >= UCHAR('А') && *t <= UCHAR('Я'))
				bmp.draw_size(hDC, px, y, w, h, (*t - UCHAR('А')) * width, 0, width, bmp.getHeight(), rop);
			px += w;
			++t;
		}
	}
}


//max - строка
int rus_font::max_line(const char* s){
	int m = 0, i = 0;
	for(; *s; ++s){
		if(*s == '\n'){
			if(i > m)
				m = i;
			i = 0;
		} else
			++i;
	}

	if(i > m)
		m = i;
	return m;
}


//---------------------------------------------------------------------------------------------------------


static void _calc_angle(LPPOINT ds, float angle, int x, int y, int width, int height){
	const float cosa = _tcos(angle);
	const float sina = _tsin(angle);
	const int     dx = x + (width  >> 1);
	const int     dy = y + (height >> 1);

	ds[0].x = long((x - dx) * cosa - (y - dy) * sina + dx);
	ds[0].y = long((y - dy) * cosa + (x - dx) * sina + dy);
	ds[1].x = long((x + width - dx) * cosa - (y - dy) * sina + dx);
	ds[1].y = long((y - dy) * cosa + (x + width  - dx) * sina + dy);
	ds[2].x = long((x - dx) * cosa - (y + height - dy) * sina + dx);
	ds[2].y = long((y + height - dy) * cosa + (x - dx) * sina + dy);
}

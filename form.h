#if ! defined(_FORM_KUDUSHTEEV_H_)
#define _FORM_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif


//¡¿«Œ¬»…  À¿— ‘Œ–Ã»
class base_form {
private:
	HINSTANCE b_hinst;
	HWND      b_hwnd;
	HBITMAP   b_hbm;
	HDC       b_mdc;
	HDC       b_hdc;
	DWORD     b_delay;
	DWORD     elapsed;
	BOOL      b_draw;
protected:
	int b_width;
	int b_height;

	base_form(void);
	virtual ~base_form();

	base_form(const base_form&);
	base_form& operator = (const base_form&);
public:
	bool create(HINSTANCE hinst, LPCSTR caption, int width, int height, UINT id_icon);
	void destroy(void);
	int  run(void);

	void close(void){
		PostQuitMessage(0);
	}

	void setDelay(DWORD delay){
		b_delay = delay;
	}

	DWORD getDelay(void) const {
		return b_delay;
	}

	BOOL invalidate(CONST RECT* lprc = NULL, BOOL bErase = TRUE){
		return InvalidateRect(b_hwnd, lprc, bErase);
	}

	HDC getDC(void) const {
		return b_mdc;
	}

	DWORD getElapsed(void) const {
		return elapsed;
	}

	int getWidth(void) const {
		return b_width;
	}

	int getHeight(void) const {
		return b_height;
	}

	HWND getHandle(void) const {
		return b_hwnd;
	}

	HINSTANCE getModule(void) const {
		return b_hinst;
	}

protected:
	virtual void onCreate(HINSTANCE hinst){}
	virtual void onKeyPress(LPBYTE key){}
	virtual void onKeyDown(WORD key){}
	virtual void onUpdateFrame(DWORD msec){}
	virtual void onDrawing(HDC hDC){}
	virtual void onMouseMove(int x, int y){}
	virtual void onLeftMouseDown(int x, int y){}
	virtual void onDestroy(void){}
private:
	static LRESULT CALLBACK _wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


#endif

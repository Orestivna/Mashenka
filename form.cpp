#include "headers.h"
#define BF_TIME_DELAY  15
#define BF_CLASS_NAME  "GameP"
static base_form* __ptr_bform = NULL;


base_form::base_form(void):b_hinst(NULL),
                           b_hwnd(NULL),
                           b_hbm(NULL),
                           b_mdc(NULL),
                           b_hdc(NULL),
                           b_delay(1),
                           b_width(0),
                           b_height(0),
                           elapsed(0),
                           b_draw(TRUE) {
	__ptr_bform = this;
}

base_form::~base_form(){
	destroy();
}


//—“¬Œ–≈ÕÕﬂ
bool base_form::create(HINSTANCE hinst, LPCSTR caption, int width, int height, UINT id_icon){
	DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX);

	if(b_hwnd != NULL)
		return false;

	WNDCLASSEXA   cls = {0};
	cls.cbSize        = sizeof(cls);
	cls.style         = CS_OWNDC;
	cls.lpfnWndProc   = reinterpret_cast<WNDPROC>(&base_form::_wnd_proc);
	cls.hInstance	  = hinst;
	cls.hIcon         = (!id_icon) ? NULL: LoadIconA(hinst, MAKEINTRESOURCEA(id_icon));
	cls.hCursor       = LoadCursor(NULL, IDC_ARROW);
	cls.lpszClassName = BF_CLASS_NAME;

	if(! RegisterClassExA(&cls))
		return false;

	RECT rc = { 0, 0, width, height };
	AdjustWindowRectEx(&rc, style, FALSE, 0);

	b_hwnd = CreateWindowExA(0, BF_CLASS_NAME, caption, style, 
	                        (GetSystemMetrics(SM_CXSCREEN) - (rc.right  - rc.left)) / 2,
	                        (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top))  / 2,
		                     rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hinst, NULL);
	if(b_hwnd == NULL){
		UnregisterClassA(BF_CLASS_NAME, hinst);
		return false;
	}

	b_hinst  = hinst;
	b_width  = width;
	b_height = height;
	b_hdc    = GetDC(b_hwnd);

	if((b_mdc = CreateCompatibleDC(b_hdc)) == NULL)
		return false;

	if((b_hbm = CreateCompatibleBitmap(b_hdc, width, height)) == NULL)
		return false;

	SelectObject(b_mdc, b_hbm);
	PatBlt(b_mdc, 0, 0, width, height, BLACKNESS);

	onCreate(hinst);

	ShowWindow(b_hwnd, SW_SHOW);
	UpdateWindow(b_hwnd);
	return true;
}


//«¿œ”— 
int base_form::run(void){
	BYTE  keys[256];
	BOOL  ret;
	DWORD cur, res;
	MSG   msg = {0};

	elapsed = timeGetTime();

	while(msg.message != WM_QUIT) {

		if(b_draw)
			ret = PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
		else
			ret = GetMessageA(&msg, NULL, 0, 0);

		if(ret){
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		} else {

			if(b_draw){
				cur = timeGetTime();
				res = cur - elapsed;
				if(res >= BF_TIME_DELAY){
					if(GetKeyboardState(keys))
						onKeyPress(keys);

					elapsed = cur;
					onUpdateFrame(cur);
				}

				onDrawing(b_mdc);
				BitBlt(b_hdc, 0, 0, b_width, b_height, b_mdc, 0, 0, SRCCOPY);

				Sleep(b_delay);
			}
		}
	}

	onDestroy();
	return static_cast<int>(msg.wParam);
}


//«Õ»Ÿ≈ÕÕﬂ
void base_form::destroy(void){
	if(b_hdc != NULL)
		ReleaseDC(b_hwnd, b_hdc);
	b_hdc = NULL;

	if(b_hbm != NULL)
		DeleteObject(b_hbm);
	b_hbm = NULL;

	if(b_mdc != NULL)
		DeleteDC(b_mdc);
	b_mdc = NULL;

	if(b_hwnd != NULL){
		DestroyWindow(b_hwnd);
		UnregisterClassA(BF_CLASS_NAME, b_hinst);
	}
	b_hwnd = NULL;
}


//Œ¡–Œ¡ ¿
LRESULT CALLBACK base_form::_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_SETFOCUS:
		__ptr_bform->b_draw = TRUE;
		break;
	case WM_KILLFOCUS:
		__ptr_bform->b_draw = FALSE;
		break;
	case WM_MOUSEMOVE:
		__ptr_bform->onMouseMove(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
		break;
	case WM_LBUTTONDOWN:
		__ptr_bform->onLeftMouseDown(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
		break;
	case WM_KEYDOWN:
		__ptr_bform->onKeyDown(static_cast<WORD>(wParam));
		return DefWindowProcA(hwnd, msg, wParam, lParam);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcA(hwnd, msg, wParam, lParam);
	}
	return 0;
}

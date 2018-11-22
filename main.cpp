#include "headers.h"
#ifdef __BORLANDC__
#pragma resource "Script.res"
#endif


int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int){
	srand(static_cast<unsigned int>(time(NULL)));

	quest_game* app = new quest_game();
	int ret;

	if(app->create(hinst, "ÏÐÈÃÎÄÈ ÌÀØÅÍÜÊÈ", 640, 480, IDI_ICON_APP))
		ret = app->run();
	else
		ret = 0;

	delete app;
	return ret;
}


void* operator new (size_t size){
	return HeapAlloc(GetProcessHeap(), 0, static_cast<DWORD>(size));
}

void* operator new [] (size_t size){
	return HeapAlloc(GetProcessHeap(), 0, static_cast<DWORD>(size));
}

void operator delete (void* ptr){
	HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete [] (void* ptr){
	HeapFree(GetProcessHeap(), 0, ptr);
}

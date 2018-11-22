#if ! defined(_UTIL_KUDUSHTEEV_H_)
#define _UTIL_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

enum e_intersect {
	inter_not, inter_top, inter_left, inter_right, inter_bottom
};

extern bool isSquareToSquare(int x1, int y1, int s1, int x2, int y2, int s2);
extern bool isRectToRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern bool isSquareToCircle(int rx, int ry, int s, int cx, int cy, int r);
extern bool isPointToCircle(int x, int y, int cx, int cy, int r);
extern bool isPointToSquare(int x, int y, int rx, int ry, int s);
extern bool isRectToCircle(int rx, int ry, int w, int h, int cx, int cy, int r);
extern e_intersect SquareToRect(int& x, int& y, int s, int rx, int ry, int w, int h);
extern e_intersect SquareToSquare(int& x1, int& y1, int s1, int x2, int y2, int s2);

#endif

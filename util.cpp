#include "headers.h"


//ÏÅĞÅÑ²ÊÀÍÍß ÊÂÀÄĞÀÒÀ Ç ÊÂÀÄĞÀÒÎÌ
bool isSquareToSquare(int x1, int y1, int s1, int x2, int y2, int s2){
	int r1 = x1 + s1;
	int b1 = y1 + s1;
	int r2 = x2 + s2;
	int b2 = y2 + s2;
	return (x1 < r2) && (x2 < r1) && (y1 < b2) && (y2 < b1);
}


//ÏÅĞÅÑ²ÊÀÍÍß ÏĞßÌÎÊÓÒÍÈÊÀ Ç ÏĞßÌÎÊÓÒÍÈÊÀ
bool isRectToRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
	int r1 = x1 + w1;
	int b1 = y1 + h1;
	int r2 = x2 + w2;
	int b2 = y2 + h2;
	return (x1 < r2) && (x2 < r1) && (y1 < b2) && (y2 < b1);
}


//ÏÅĞÅÑ²ÊÀÍÍß ÏĞßÌÎÊÓÒÍÈÊÀ Ç ÊÎËÎÌ
bool isSquareToCircle(int rx, int ry, int s, int cx, int cy, int r){
	int x = cx;
	int y = cy;
	if(x < rx)
		x = rx;
	else if(x > (rx + s))
		x = rx + s;

	if(y < ry)
		y = ry;
	else if(y > (ry + s))
		y = ry + s;

	x = cx - x;
	y = cy - y;
	return (x*x+y*y <= r*r);
}


//ÏÅĞÅÑ²ÊÀÍÍß ÒÎ×ÊÈ Ç ÊÎËÎÌ
bool isPointToCircle(int x, int y, int cx, int cy, int r){
	int dx = x - cx;
	int dy = y - cy;
	return ((dx*dx + dy*dy) <= (r*r));
}


//ÏÅĞÅÑ²ÊÀÍÍß ÒÎ×ÊÈ Ç ÊÂÀÄĞÀÒÎÌ
bool isPointToSquare(int x, int y, int rx, int ry, int s){
	return ((x >= rx) && (x <= (rx + s)) && (y >= ry) && (y <= (ry + s)));
}


//ÏÅĞÅÑ²ÊÀÍÍß ÏĞßÌÎÊÓÒÍÈÊÀ Ç ÊÎËÎÌ
bool isRectToCircle(int rx, int ry, int w, int h, int cx, int cy, int r){
	int x = cx;
	int y = cy;
	if(x < rx)
		x = rx;
	else if(x > (rx + w))
		x = rx + w;

	if(y < ry)
		y = ry;
	else if(y > (ry + h))
		y = ry + h;

	x = cx - x;
	y = cy - y;
	return (x*x+y*y <= r*r);
}


//ÂÈØÒÎÂÕÓÂÀÍÍß ÊÂÀÄĞÀÒÀ Ç ÏĞßÌÎÊÓÒÍÈÊÀ
e_intersect SquareToRect(int& x, int& y, int s, int rx, int ry, int w, int h){
	e_intersect ret = inter_not;
	if(! isRectToRect(x, y, s, s, rx, ry, w, h))
		return ret;

	const int eps = 5;

	int a = rx - x;
	int b = ry - y;
	

	if((a >= 0) && (b > 0)){ //ÂÅĞÕÍ²É Ë²ÂÈÉ ÊÓÒ

		if(((y + s) < (ry + (eps << 1))) && ((x + s) > (rx + eps))){
			y   = ry - s - 1;
			ret = inter_top;
		} else {
			x   = rx - s - 1;
			ret = inter_left;
		}

	} else if((a < 0) && (b <= 0)){ //ÍÈÆÍ²É ÏĞÀÂÈÉ ÊÓÒ

		if(((x + eps) > (rx + w)) && ((y + eps) < (ry + h))){
			x   = rx + w + 1;
			ret = inter_right; 
		} else {
			y   = ry + h + 1;
			ret = inter_bottom; 
		}

	} else if((a >= 0) && (b < 0)){ //ÍÈÆÍ²É Ë²ÂÈÉ ÊÓÒ

		if(((x + s) < (rx + eps)) && ((y + eps) < (ry + h))){
			x   = rx - s - 1;
			ret = inter_left; 
		} else {
			y   = ry + h + 1;
			ret = inter_bottom;
		}

	} else if((a < 0) && (b >= 0)){ //ÏĞÀÂÈÉ ÂÅĞÕÍ²É ÊÓÒ

		if(((y + s) < (ry + (eps << 1))) && ((x + eps) < (rx + w))){
			y   = ry - s - 1;
			ret = inter_top; 
		} else {
			x   = rx + w + 1;
			ret = inter_right;
		}
	}
	return ret;
}


//ÂÈØÒÎÂÕÓÂÀÍÍß ÊÂÀÄĞÀÒÀ Ç ÊÂÀÄĞÀÒÀ
e_intersect SquareToSquare(int& x1, int& y1, int s1, int x2, int y2, int s2){
	return SquareToRect(x1, y1, s1, x2, y2, s2, s2);
}

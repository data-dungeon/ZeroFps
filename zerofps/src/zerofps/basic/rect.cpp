// RECT.cpp: implementation of the RECT class.
//
//////////////////////////////////////////////////////////////////////

#include "rect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Rect::Rect(int l, int t, int r, int b)
{
	Left = l;
	Top = t;
	Right = r;
	Bottom = b;
}

Rect::~Rect()
{

}

bool Rect::Inside(int x, int y)
{
	return (x >= Left && x <= Right &&
		y >= Top && y <= Bottom) ? true : false;
}

bool Rect::Inside(Rect d)
{
	if(d.Left < Left) return false;
	if(d.Right > Right) return false;
	if(d.Top < Top) return false;
	if(d.Bottom > Bottom) return false;
	return true;
}

Rect Rect::Contract(int iPixels) const
{
	Rect res = *this;
	res.Left += iPixels;
	res.Right -= iPixels;
	res.Top += iPixels;
	res.Bottom -= iPixels;
	return res;
}

Rect Rect::Move(int dx, int dy) const
{
	Rect res = *this;
	res.Left += dx;
	res.Right += dx;
	res.Top += dy;
	res.Bottom += dy;
	return res;
}




// RECT.h: interface for the RECT class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASIC_RECT_H_
#define _BASIC_RECT_H_

#include "basic_x.h"

class BASIC_API Rect  
{
public:
	Rect(int l=0, int t=0, int r=0, int b=0);
	virtual ~Rect();

	int Left;
	int Top;
	int Right;
	int Bottom;

	Rect Move(int dx, int dy) const; // påverkar inte objektet i sig utan returnerar ett nytt objekt
	bool Inside(int x, int y);
	bool Inside(Rect d); 
	Rect Contract(int iPixels) const; // påverkar inte objektet i sig utan returnerar ett nytt objekt

	inline Rect Rect::operator +=(Rect d)
	{
		Left += d.Left;
		Top += d.Top;
		Right += d.Left;
		Bottom += d.Top;
		return (*this);
	}

	inline Rect Rect::operator +(Rect d)
	{
		Rect rc;
		rc.Left = Left+d.Left;
		rc.Top = Top+d.Top;
		rc.Right = Right+d.Right;
		rc.Bottom = Bottom+d.Bottom;
		return rc;
	}

	inline bool Rect::operator==(Rect d) 
	{   
		return (Left==d.Left && Top==d.Top
			&& Right==d.Right && Bottom==d.Bottom); 
	} 

	inline bool Rect::operator!=(Rect d) 
	{   
		return (Left!=d.Left || Top==d.Top
			|| Right!=d.Right || Bottom!=d.Bottom); 
	} 

	inline int Width() { return (Right-Left); }
	inline int Height() { return (Bottom-Top); }
};

class BASIC_API Point  
{
public:
	Point() { }
	Point(int nx, int ny) { x=nx, y=ny; }

	inline bool operator==(Point a)
	{
		return (a.x == x && a.y == y);
	}

	inline bool operator!=(Point a)
	{
		return (a.x != x || a.y != y);
	}

	int x,y;
};

#endif // !defined(AFX_RECT_H__9D6EA35C_226A_4A91_BE8F_5E90ADCEC058__INCLUDED_)

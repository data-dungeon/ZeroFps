#ifndef _SELECTWND_H
#define _SELECTWND_H

class ZGuiWnd;

// A singleton to ensure that there is only one instance of the selected window.
class SelectWnd
{
public:
	static SelectWnd* GetInstance();

	ZGuiWnd* m_pkWnd;

private:
	SelectWnd();
	static SelectWnd* m_pkInstance;
};

#endif // #ifndef _SELECTWND_H

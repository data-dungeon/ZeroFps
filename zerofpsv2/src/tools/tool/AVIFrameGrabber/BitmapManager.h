#pragma once

class BitmapManager
{
public:
	BitmapManager(void);
	~BitmapManager(void);
	int SaveFile(const char* szFileName, char* data, int width, int height);
};

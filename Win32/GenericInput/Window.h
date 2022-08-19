#pragma once
class Window
{
public:
	BOOL RegisterWindow(HWND hWindow);
	void UnregisterWindow();
private:
	std::vector <DEV_BROADCAST_HANDLE>  btFilters;
	std::vector <HDEVNOTIFY> hDevNotify;
};


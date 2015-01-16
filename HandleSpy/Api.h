#pragma once

class CApi
{
public:
	static TCHAR* GetNameByIndex(DWORD dwIndex);
private:
	static TCHAR* Name[]; 

	static TCHAR* invalidIndex;
};

#pragma once
#include <string>
#include <iostream>
//#include <Windows.h>

class Debug
{
public:
	static void Error(std::string message, bool doError = false)
	{
		std::cout << "[ERROR]: " << message << std::endl;
		if (doError) 
		{
			abort();
		}
	}

	static void Warning(std::string message, bool doWarning = false)
	{
		std::cout << "[WARNING]: " << message << std::endl;
		if (doWarning)
		{
			//MessageBox(NULL, L"This is a warning message", L"Warning", MB_OK);
		}
	}

private:

};


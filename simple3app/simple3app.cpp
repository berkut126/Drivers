// simple3app.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Win32 API
#include<Windows.h>
// Smaller header
#define WIN32_LEAN_AND_MEAN
#include <iostream>

int main()
{
	auto driveSymLink = CreateFile(
		L"\\\\.\\Simple3Link",
		GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (driveSymLink != INVALID_HANDLE_VALUE) {

		std::cout << "Successfully opened driver symlink!" << std::endl;
		CloseHandle(driveSymLink);
		return 0;

	}
	
	std::cout << "Failed opening driver symlink with error: " << GetLastError() << std::endl;
	return 1;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

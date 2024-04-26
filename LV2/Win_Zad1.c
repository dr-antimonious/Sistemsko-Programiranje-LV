// Sistemsko programiranje
// Druga laboratorijska vjezba
// Zadatak 1.

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

int _tmain(int argc, LPTSTR argv[])
{
    WIN32_FIND_DATA data;
    int countDirs = 0, countFiles = 0;
    const HANDLE handle = FindFirstFile(argv[1], &data);

    // Preskakanje . i .. elemenata
	FindNextFile(handle, &data);

    while (FindNextFile(handle, &data))
    {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) countDirs++;
        else countFiles++;
    }

    _tprintf(TEXT("Pronadjeno je %d direktorija i %d datoteka."), countDirs, countFiles);
    return 0;
}
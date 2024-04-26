// Sistemsko programiranje
// Druga laboratorijska vjezba
// Zadatak 2.

/*
 * Prije pokretanja osigurati da su sve ostale .c datoteke iskljucene iz builda.
 * - desni klik na datoteku koju se zeli izuzeti
 * - Properties -> Configuration Properties -> General
 * - Excluded From Build: Yes
 * Istovremeno treba biti samo JEDNA .c datoteka ukljucena u build.
 */

/*
 * Za pokretanje:
 * - Build Solution
 * - Otvoriti direktorij u kojem se nalazi .exe
 * - Otvoriti u terminalu
 * - Naredba "LV2.exe [direktorij]"
 * - [direktorij] kao npr. "C:\Users\"
 * - ne unositi direktorije koji sadrze razmake u imenu
 * Sve naredbe i putanje se unose BEZ navodnika
 */

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

/*
 * Pretpostavka da je velika datoteka >=4GB
 * (4 * 1024 * 1024 * 1024) = 4294967296
 */
#define LARGE_FILE_SIZE 4294967296

#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

int largeFileCount = 0;
FILETIME oldestFile;

void EnterDirectory(LPTSTR partialPath, LPCTSTR suffix);
void TestFile(LPTSTR partialPath, LPCTSTR suffix);

int _tmain(int argc, LPTSTR argv[])
{
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);
    SystemTimeToFileTime(&currentTime, &oldestFile);

    _TCHAR path[MAX_PATH];
    _tcscpy(path, argv[1]);

    _TCHAR fullPath[MAX_PATH];
    _tcscpy(fullPath, path);
    _tcscat(fullPath, TEXT("*"));

    WIN32_FIND_DATA data;
    const HANDLE handle = FindFirstFile(fullPath, &data);

    // Preskakanje . i .. elemenata
    FindNextFile(handle, &data);

    while (FindNextFile(handle, &data))
    {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            EnterDirectory(path, data.cFileName);
        else
            TestFile(path, data.cFileName);
    }

    _tprintf(TEXT("Pronadjeno je %d 'velikih' datoteka.\n"), largeFileCount);

    SYSTEMTIME oldestFileTime;
    FileTimeToSystemTime(&oldestFile, &oldestFileTime);

    _tprintf(TEXT("Najstarija datoteka stvorena je %d.%d.%d.\n"),
        oldestFileTime.wDay, oldestFileTime.wMonth, oldestFileTime.wYear);

    return 0;
}

void EnterDirectory(LPTSTR partialPath, LPCTSTR suffix)
{
    // Ako je ime direktorija predugacko, preskoci.
    if ((_tcslen(partialPath) + _tcslen(suffix) + 2) > MAX_PATH)
        return;

    _TCHAR path[MAX_PATH];
    _tcscpy(path, partialPath);
    _tcscat(path, suffix);
    _tcscat(path, TEXT("\\"));

	_TCHAR fullPath[MAX_PATH];
    _tcscpy(fullPath, path);
    _tcscat(fullPath, TEXT("*"));

    WIN32_FIND_DATA data;
    const HANDLE handle = FindFirstFile(fullPath, &data);

    // Preskakanje . i .. elemenata
    FindNextFile(handle, &data);

    while (FindNextFile(handle, &data))
    {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            EnterDirectory(path, data.cFileName);
        else
            TestFile(path, data.cFileName);
    }
}

void TestFile(LPTSTR partialPath, LPCTSTR suffix)
{
    // Ako je ime datoteke predugacko, preskoci.
    if ((_tcslen(partialPath) + _tcslen(suffix) + 2) > MAX_PATH)
        return;

    _TCHAR path[MAX_PATH];
    _tcscpy(path, partialPath);
    _tcscat(path, suffix);

    WIN32_FIND_DATA data;
    const HANDLE handle = FindFirstFile(path, &data);

    if ((((long long)data.nFileSizeHigh * ((long long)MAXDWORD + 1)) + (long long)data.nFileSizeLow) >= LARGE_FILE_SIZE)
        largeFileCount++;

    if (CompareFileTime(&data.ftCreationTime, &oldestFile) == -1)
        oldestFile = data.ftCreationTime;
}
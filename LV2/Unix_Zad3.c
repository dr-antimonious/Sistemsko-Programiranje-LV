// Sistemsko programiranje
// Druga laboratorijska vjezba
// Zadatak 3.

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _GNU_SOURCE

/*
 * Pretpostavka da je velika datoteka >=4GB
 * (4 * 1024 * 1024 * 1024) = 4294967296
 */
#define LARGE_FILE_SIZE 4294967296

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

int largeFileCount = 0;
time_t oldestFile;

void EnterDirectory(char* partialPath, char* suffix);
void TestFile(char* partialPath, char* suffix);

int main(int argc, char* argv[])
{
    oldestFile = time(NULL);

    char path[PATH_MAX];
    strcpy(path, argv[1]);

    if (path[strlen(path) - 1] != '/')
        strcat(path, "/");
    
    DIR* directory;
    struct dirent* dirEntry;
    directory = opendir(path);

    if (directory != NULL)
    {
        int countDirs = 0, countFiles = 0;

        while (dirEntry = readdir(directory))
            // Preskakanje . i .. elemenata
            if (!((strlen(dirEntry->d_name) == 1 && dirEntry->d_name[0] == '.') ||
                  (strlen(dirEntry->d_name) == 2 && dirEntry->d_name[0] == '.' 
                   && dirEntry->d_name[1] == '.')))
                {
                    if (dirEntry->d_type == DT_DIR)
                        countDirs++;
                    if (dirEntry->d_type == DT_REG)
                        countFiles++;
                }
                
        (void)closedir(directory);

        printf("Pronadjeno je %d direktorija i %d datoteka.\n", countDirs, countFiles);
    }
    else perror("Couldn't open directory");

    directory = opendir(path);

    if (directory != NULL)
    {
        while (dirEntry = readdir(directory))
            // Preskakanje . i .. elemenata
            if (!((strlen(dirEntry->d_name) == 1 && dirEntry->d_name[0] == '.') ||
                  (strlen(dirEntry->d_name) == 2 && dirEntry->d_name[0] == '.' 
                   && dirEntry->d_name[1] == '.')))
                {
                    if (dirEntry->d_type == DT_DIR)
                        EnterDirectory(path, dirEntry->d_name);
                    if (dirEntry->d_type == DT_REG)
                        TestFile(path, dirEntry->d_name);
                }

        (void)closedir(directory);
    }
    else perror("Couldn't open directory");

    printf("Pronadjeno je %d 'velikih' datoteka.\n", largeFileCount);
    printf("Najstarija datoteka stvorena je %s", ctime(&oldestFile));

    return 0;
}

void EnterDirectory(char* partialPath, char* suffix)
{
    // Ako je ime datoteke predugacko, preskoci.
    if ((strlen(partialPath) + strlen(suffix) + 2) > PATH_MAX)
        return;

    char path[PATH_MAX];
    strcpy(path, partialPath);
    strcat(path, suffix);
    strcat(path, "/");

    DIR* directory;
    struct dirent* dirEntry;
    directory = opendir(path);

    if (directory != NULL)
    {
        while (dirEntry = readdir(directory))
            // Preskakanje . i .. elemenata
            if (!((strlen(dirEntry->d_name) == 1 && dirEntry->d_name[0] == '.') ||
                  (strlen(dirEntry->d_name) == 2 && dirEntry->d_name[0] == '.' 
                   && dirEntry->d_name[1] == '.')))
                {
                    if (dirEntry->d_type == DT_DIR)
                        EnterDirectory(path, dirEntry->d_name);
                    if (dirEntry->d_type == DT_REG)
                        TestFile(path, dirEntry->d_name);
                }
                
        (void)closedir(directory);
    }
    else perror("Couldn't open directory");
}

void TestFile(char* partialPath, char* suffix)
{
    // Ako je ime datoteke predugacko, preskoci.
    if ((strlen(partialPath) + strlen(suffix) + 2) > PATH_MAX)
        return;

    char path[PATH_MAX];
    strcpy(path, partialPath);
    strcat(path, suffix);

    struct stat statBuffer;
    stat(path, &statBuffer);

    if ((__intmax_t)statBuffer.st_size >= LARGE_FILE_SIZE)
        largeFileCount++;

    if (statBuffer.st_ctim.tv_sec < oldestFile)
        oldestFile = statBuffer.st_ctim.tv_sec;
}
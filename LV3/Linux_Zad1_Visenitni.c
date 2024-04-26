// Sistemsko programiranje
// Treca laboratorijska vjezba
// Zadatak 3. - VISENITNA VERZIJA

#undef UNICODE
#undef UNICODE_

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

/* Broj niti koristenih za racunanje
 * Mijenjati prema izvjestaju (1-4) */
#define MAX_THREADS 4

/* Definira velicinu kvadratne matrice
 * Velicina matrice je MATRIX_SIZE * MATRIX_SIZE
 * Za sve kolicine koristenih niti ova konstanta mora biti iste vrijednosti
 * tj. moze se konstanta promijeniti,
 *     ali ne da je za 1 nit vrijednost n,
 *     a za 3 niti vrijednost m
 * Vrijednost ove konstante mora biti jednaka za sekvencijalni mod i ovaj mod,
 *     inace se ne moze provjeriti tocnost preko zbroja */
#define MATRIX_SIZE 1500

void* FillMatrix(void* startIndex);
double** matrix;

void main()
{
    struct timespec start, finish;
    double elapsed = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    matrix = (double**)malloc(sizeof(double*) * MATRIX_SIZE);
    pthread_t threads[MAX_THREADS];

    if (matrix == NULL)
        exit(1);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
	    matrix[i] = (double*)malloc(sizeof(double) * MATRIX_SIZE);
        if (matrix[i] == NULL)
            exit(2);
    }

    int** startIndex = (int**)malloc(sizeof(int*) * MAX_THREADS);

    if (startIndex == NULL)
        exit(3);

    for (int i = 0; i < MAX_THREADS; i++)
    {
        startIndex[i] = (int*)malloc(sizeof(int));
        if (startIndex[i] == NULL)
            exit(4);
        startIndex[i][0] = i;
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_create((threads + i), NULL, FillMatrix, startIndex[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (startIndex[i] != NULL)
        {
            free(startIndex[i]);
            startIndex[i] = NULL;
        }
    }

    if (startIndex != NULL)
    {
        free(startIndex);
        startIndex = NULL;
    }

    double matrixSum = 0;
    for (int i = 0; i < MATRIX_SIZE; i++)
	    for (int j = 0; j < MATRIX_SIZE; j++)
            matrixSum += matrix[i][j];

    printf("Zbroj: %lf\n", matrixSum);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        if (matrix[i] != NULL)
        {
            free(matrix[i]);
            matrix[i] = NULL;
        }
    }

    if (matrix != NULL)
    {
        free(matrix);
        matrix = NULL;
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);
    
    elapsed = (finish.tv_sec - start.tv_sec) * 1000;
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1e6;
    
    printf("Vrijeme (ms): %lf\n", elapsed);
}

void* FillMatrix(void* startIndex)
{
    for (int i = *(int*)startIndex; i < MATRIX_SIZE; i += MAX_THREADS)
    {
	    for (int j = 0; j < MATRIX_SIZE; j++)
	    {
            double temp = 0;
		    for (int k = 0; k <= i; k++)
                temp += k * sin(j) - j * cos(k);
            matrix[i][j] = temp;
	    }
    }
}

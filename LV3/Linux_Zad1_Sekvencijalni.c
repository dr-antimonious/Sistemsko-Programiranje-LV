// Sistemsko programiranje
// Treca laboratorijska vjezba
// Zadatak 3. - SEKVENCIJALNA VERZIJA

#undef UNICODE
#undef UNICODE_

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

 /* Definira velicinu kvadratne matrice
  * Velicina matrice je MATRIX_SIZE * MATRIX_SIZE
  * Za sve kolicine koristenih niti ova konstanta mora biti iste vrijednosti
  * tj. moze se konstanta promijeniti,
  *     ali ne da je za 1 nit vrijednost n,
  *     a za 3 niti vrijednost m
  * Vrijednost ove konstante mora biti jednaka za sekvencijalni mod i ovaj mod,
  *     inace se ne moze provjeriti tocnost preko zbroja */
#define MATRIX_SIZE 1500

double** matrix;

void main()
{
    clock_t time = clock();

    matrix = (double**)malloc(sizeof(double*) * MATRIX_SIZE);

    if (matrix == NULL)
        exit(1);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        matrix[i] = (double*)malloc(sizeof(double) * MATRIX_SIZE);
        if (matrix[i] == NULL)
            exit(2);
    }

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            double temp = 0;
            for (int k = 0; k <= i; k++)
                temp += k * sin(j) - j * cos(k);
            matrix[i][j] = temp;
        }
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

    printf("Vrijeme (ms): %ld\n", (clock() - time) / 1000);
}

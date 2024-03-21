#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "stdbool.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define NB_F 10                                                                  // размер блока
#define PATH "/home/dmitriy/Desktop/comp-math/1-WaveParallelizer/build/grid.txt" // файл куда сетку сохранять

typedef int (*alg)(int, double, double **, double **);
typedef void (*prepare)(int, double **, double **);

/// @brief Последовательный алгоритм Гаусса-Зейделя
/// @param N количество узлов по каждой из координат области D
/// @param eps граница (k), после которой приближаться дальше бессмысленно
/// @param u аппроксимация функции u(x, y)
/// @param f производная
int default_alg(int N, double eps, double **u, double **f);

/// @brief 11.6. Волновая схема вычислений (кэш)
int async_alg5(int N, double eps, double **u, double **f);

/// @brief Несколько раз запускает алгоритм и фиксирует скорость выполнения (через omp).
/// @param repeats Кол-во повторных запусков алгоритма
/// @param threads Число потоков
/// @param alg Сам алгоритм (и дальше аргументы его)
void test(int repeats, int threads, bool save_grid, alg run, prepare p, int N, double eps, double **u, double **f);

/// @brief Вычисление каждого блока
/// @return Разница
double eval(int N, int bsize, double **u, double **f, int i, int j);

/// @brief Приминение условий из книги
void book_cond(int N, double **u, double **f);

/// @brief Простое краевое условие
void trig_cond(int N, double **u, double **f);

int main(int argc, char *argv[])
{
    int N = 100;
    double eps = 0.1;

    int threads[] = {1, 2, 4, 8, 16};
    int repeats = 3;

    double **u = malloc((N + 2) * sizeof(double *));
    double **f = malloc((N + 2) * sizeof(double *));

    for (int i = 0; i <= N + 1; i++)
    {
        u[i] = malloc((N + 2) * sizeof(double));
        f[i] = malloc((N + 2) * sizeof(double));
    }

    for (int i = 0; i < 5; i++)
    {
        printf("\nThreads: %i\n", threads[i]);

        // printf("### Iterative algorithm (book conditions)\n");
        // test(repeats, threads[i], i == 0, &default_alg, book_cond, N, eps, u, f);

        printf("### Parallel alg 11.6 (book conditions)\n");
        test(repeats, threads[i], true, &async_alg5, book_cond, N, eps, u, f);

        // ---

        // printf("### Iterative algorithm (my conditions)\n");
        // test(repeats, threads[i], &default_alg, trig_cond, N, eps, u, f);

        // printf("### Parallel alg 11.6 (my conditions)\n");
        // test(repeats, threads[i], &async_alg5, trig_cond, N, eps, u, f);
    }

    for (int i = 0; i <= N + 1; i++)
    {
        free(u[i]);
        free(f[i]);
    }

    free(u);
    free(f);
}

int default_alg(int N, double eps, double **u, double **f)
{
    double h = 1.0 / (N + 1);
    double dmax;
    int iters = 0;

    // Значения u при индексах i,j = 0 || i,j = N+1 являются граничными, задаются при постановке задачи

    do
    {
        dmax = 0;                       // максимальное изменение значений u
        for (int i = 1; i < N + 1; i++) // граничные значения не изменяются
        {
            for (int j = 1; j < N + 1; j++)
            {
                double temp = u[i][j];
                // уточняем значение
                u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * f[i][j]);
                // сравниваем на сколько мы отошли
                double dm = fabs(temp - u[i][j]);
                if (dmax < dm)
                    dmax = dm;
            }
        }
        iters++;
    } while (dmax > eps);

    return iters;
}
int async_alg5(int N, double eps, double **u, double **f)
{
    omp_lock_t dmax_lock;
    omp_init_lock(&dmax_lock);
    double h = 1.0 / (N + 1);
    double temp, dmax = 0;

    const int bsize = 64;
    int NB = 100; // количество блоков
    double dm[N + 1];

    int iters = 0;

    do
    {
        dmax = 0;
        // нарастание волны (размер волны равен nx+1), двигаемся по блокам
        for (int nx = 0; nx < NB; nx++)
        {
            dm[nx] = 0;
            int i, j;
            double d;

#pragma omp parallel for shared(dm, nx, u, f, NB, N, bsize) private(i, j, d)
            for (i = 0; i < nx + 1; i++) // отн. блока
            {
                j = nx - i; // отн. блока

                d = eval(N, bsize, u, f, i, j);

                if (dm[i] < d)
                    dm[i] = d;
            }
        }

        // затухание волны
        for (int nx = NB; nx > -1; nx--)
        {
            int i, j;
            double d;
#pragma omp parallel for shared(dm, nx, u, f, NB, N, bsize) private(i, j, d)
            for (i = 0; i < nx + 1; i++)
            {
                j = 2 * NB - nx - i;

                d = eval(N, bsize, u, f, i, j);

                if (dm[i] < d)
                    dm[i] = d;
            } // конец параллельной области
        }

        for (int i = 0; i < NB; i++)
        {
            if (dmax < dm[i])
                dmax = dm[i];
        }

        iters++;
    } while (dmax > eps);

    return iters;
}
double eval(int N, int bsize, double **u, double **f, int i, int j)
{
    double dmax = 0, dm, temp, h = 1 / (N + 1);

    int x_s = 1 + i * bsize;
    int x_f = min(x_s + bsize, N + 1);
    int y_s = 1 + j * bsize;
    int y_f = min(y_s + bsize, N + 1);

    for (int x = x_s; x < x_f; x++)
        for (int y = y_s; y < y_f; y++)
        {
            temp = u[x][y];
            u[x][y] = 0.25 * (u[x - 1][y] + u[x + 1][y] + u[x][y - 1] + u[x][y + 1] - h * h * f[x][y]);
            dm = fabs(temp - u[x][y]);
            if (dmax < dm)
                dmax = dm;
        }

    return dmax;
}
void test(int repeats, int threads, bool save_grid, alg run, prepare p, int N, double eps, double **u, double **f)
{
    double results[repeats];
    double average = 0;
    int iters;

    omp_set_num_threads(threads);

    for (int i = 0; i < repeats; i++)
    {
        p(N, u, f);
        double time = omp_get_wtime();
        iters = run(N, eps, u, f);
        results[i] = omp_get_wtime() - time;
        average += results[i];

        printf("%f\t", results[i]);
    }

    if (save_grid)
    {
        FILE *file = fopen(PATH, "w");

        for (int i = 0; i <= N + 1; i++)
        {
            for (int j = 0; j <= N + 1; j++)
            {
                fprintf(file, "%f\t", u[i][j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }

    printf("Result: %fms, %i iterations\n", average / repeats, iters);
}
void book_cond(int N, double **u, double **f)
{
    // Пусть будет квадратная область с граничными условиями (ниже)
    // Для рандома
    double max = 100.0;
    double min = -100.0;
    double range = (max - min);
    double div = RAND_MAX / range;

    int h = 1 / (N + 1);

    // заполнение значениями
    for (int x = 0; x <= N + 1; x++)
    {
        for (int y = 0; y <= N + 1; y++)
        {
            // f(x,y) = 0 на всём D
            f[x][y] = 0.0;

            if (y == 0) // нижняя грань
            {
                u[x][y] = 100 - 200 * (x * h);
            }
            else if (x == 0) // левая грань
            {
                u[x][y] = 100 - 200 * (y * h);
            }
            else if (y == N + 1) // верхняя грань
            {
                u[x][y] = -100 + 200 * (x * h);
            }
            else if (x == N + 1) // правая грань
            {
                u[x][y] = -100 + 200 * (y * h);
            }
            else
            {
                u[x][y] = min + (rand() / div);
            }
        }
    }
}
void trig_cond(int N, double **u, double **f)
{
    double max = 100.0;
    double min = -100.0;
    double range = (max - min);
    double div = RAND_MAX / range;

    double const e10 = pow(2.71, 10);

    // заполнение значениями
    for (int x = 0; x <= N + 1; x++)
    {
        for (int y = 0; y <= N + 1; y++)
        {
            // e^x + e^y
            f[x][y] = pow(2.71, x) + pow(2.71, y);

            // 100*sin(x)^3 + cos(y)*e^10
            u[x][y] = 100 * pow(sin(x), 3) + cos(y) * e10;
        }
    }
}
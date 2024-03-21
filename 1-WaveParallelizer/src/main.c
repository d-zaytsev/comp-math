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

        printf("### Iterative algorithm (book conditions)\n");
        test(repeats, threads[i], i == 0, &default_alg, book_cond, N, eps, u, f);

        // printf("### Parallel alg 11.6 (book conditions)\n");
        // test(repeats, threads[i], true, &async_alg5, book_cond, N, eps);

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

int async_alg5(int N, double eps, double **u, double **f)
{
    omp_lock_t dmax_lock;
    omp_init_lock(&dmax_lock);
    double h = 1.0 / (N + 1);
    double d, temp, dmax = 0;
    int i, j;
    int nx;

    const int chunk = N / 10; // размер последовательного участка
    const int bsize = NB_F;
    const int NB = (N + 1) / bsize; // количество блоков
    double dm[N + 1];

    int x, y;
    int iters = 0;

    do
    {
        dmax = 0;
        // нарастание волны (размер волны равен nx+1), двигаемся по блокам
        for (nx = 0; nx < NB; nx++)
        {
#pragma omp parallel for shared(dm, nx) private(i, j, x, y)
            for (i = 0; i <= nx; i++) // отн. блока
            {
                j = nx - i; // отн. блока

                const int x_s = i * bsize + 1;
                const int y_s = j * bsize + 1;

                const int x_e = min((x + bsize), N - 1);
                const int y_e = min((y + bsize), N - 1);

                for (x = x_s; x < x_e; x++)
                    for (y = y_s; y < y_e; y++) // проходимся по границе каждого куска
                    {
                        temp = u[x][y];
                        u[x][y] = 0.25 * (u[x - 1][y] + u[x + 1][y] + u[x][y - 1] + u[x][y + 1] - h * h * f[x][y]);
                        d = fabs(temp - u[x][y]);
                        if (dm[x] < d)
                            dm[x] = d;
                    }
            }
        }

        // затухание волны
        for (nx = NB - 2; nx >= 0; nx--)
        {
#pragma omp parallel for shared(nx) private(i, j, x, y)
            for (i = 0; i < nx + 1; i++)
            {
                j = 2 * (NB - 1) - nx - i;

                const int x_s = i * bsize + 1;
                const int y_s = j * bsize + 1;

                const int x_e = min((x + bsize), N - 1);
                const int y_e = min((y + bsize), N - 1);

                for (x = x_s; x < x_e; x++)
                    for (y = y_s; y < y_e; y++)
                    {
                        temp = u[x][y];
                        u[x][y] = 0.25 * (u[x - 1][y] + u[x + 1][y] + u[x][y - 1] + u[x][y + 1] - h * h * f[x][y]);
                        d = fabs(temp - u[x][y]);
                        if (dm[x] < d)
                            dm[x] = d;
                    }
            } // конец параллельной области
        }
#pragma omp parallel for shared(dm, dmax) private(i, d)
        for (i = 1; i < nx + 1; i += chunk)
        {
            d = 0; // наиб. значение на куске
            for (j = i; j < i + chunk; j++)
                if (d < dm[j])
                    d = dm[j];
            omp_set_lock(&dmax_lock);
            if (dmax < d)
                dmax = d;
            omp_unset_lock(&dmax_lock);
        }

        iters++;
    } while (dmax > eps);

    return iters;
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

    // заполнение значениями
    for (int x = 0; x <= N + 1; x++)
    {
        for (int y = 0; y <= N + 1; y++)
        {
            // f(x,y) = 0 на всём D
            f[x][y] = 0.0;

            if (y == 0) // нижняя грань
            {
                u[x][y] = x * x;
            }
            else if (x == 0) // левая грань
            {
                u[x][y] = y;
            }
            else if (y == N + 1) // верхняя грань
            {
                u[x][y] = 2 * x + 1;
            }
            else if (x == N + 1) // правая грань
            {
                u[x][y] = 3 * y * y;
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
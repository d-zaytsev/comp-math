#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define NB_F 10 // делитель для размеров блока

typedef void (*alg)(int, double, double **, double **);
typedef void (*prepare)(int, double **, double **);

/// @brief Последовательный алгоритм Гаусса-Зейделя
/// @param N количество узлов по каждой из координат области D
/// @param eps граница (k), после которой приближаться дальше бессмысленно
/// @param u аппроксимация функции u(x, y)
/// @param f производная
void default_alg(int N, double eps, double **u, double **f);

/// @brief Первая попытка распараллелить
void async_alg1(int N, double eps, double **u, double **f);

/// @brief Уменьшенная частота блокировок потоков
void async_alg2(int N, double eps, double **u, double **f);

/// @brief 11.4. Метод Гаусса-Якоби (считаем изменения локально)
void async_alg3(int N, double eps, double **u, double **f);

/// @brief 11.5. Волновая схема вычислений
void async_alg4(int N, double eps, double **u, double **f);

/// @brief 11.6. Волновая схема вычислений (кэш)
void async_alg5(int N, double eps, double **u, double **f);

/// @brief Несколько раз запускает алгоритм и фиксирует скорость выполнения (через omp).
/// @param repeats Кол-во повторных запусков алгоритма
/// @param threads Число потоков
/// @param alg Сам алгоритм (и дальше аргументы его)
void test(int repeats, int threads, alg run, prepare p, int N, double eps, double **u, double **f);

/// @brief Приминение условий из книги
void book_cond(int N, double **u, double **f);

/// @brief Простое краевое условие
void prepare2(int N, double **u, double **f);

int main(int argc, char *argv[])
{
    int N = 128;
    double eps = 0.1;
    double **u = malloc((N + 2) * sizeof(double *));
    double **f = malloc((N + 2) * sizeof(double *));

    // fill with random values [-100;100]
    for (int i = 0; i <= N + 1; i++)
    {
        u[i] = malloc((N + 2) * sizeof(double));
        f[i] = malloc((N + 2) * sizeof(double));
    }

    int threads[] = {1, 2, 4, 8, 16};
    int repeats = 3;

    for (int i = 0; i < 5; i++)
    {
        printf("\n|Threads: %i|\n", threads[i]);

        printf("### Algorithm\n");
        test(repeats, threads[i], &default_alg, book_cond, N, eps, u, f);

        printf("\n### Parallel algorithm (11.2)\n");
        test(repeats, threads[i], &async_alg1, book_cond, N, eps, u, f);

        printf("\n### Parallel algorithm (11.3)\n");
        test(repeats, threads[i], &async_alg2, book_cond, N, eps, u, f);

        printf("\n### Parallel algorithm (11.4)\n");
        test(repeats, threads[i], &async_alg3, book_cond, N, eps, u, f);

        printf("\n### Parallel algorithm (11.5)\n");
        test(repeats, threads[i], &async_alg4, book_cond, N, eps, u, f);

        printf("\n### Parallel algorithm (11.6)\n");
        test(repeats, threads[i], &async_alg5, book_cond, N, eps, u, f);
    }

    for (int i = 0; i <= N + 1; i++)
    {
        free(u[i]);
        free(f[i]);
    }

    free(u);
    free(f);
}

void async_alg5(int N, double eps, double **u, double **f)
{
    omp_lock_t dmax_lock;
    omp_init_lock(&dmax_lock);
    double h = 1.0 / (N + 1);
    double d, temp, dmax = 0;
    int i, j;
    int nx;

    const int chunk = N / 10; // размер последовательного участка
    const int NB = N * NB_F;  // количество блоков
    double dm[N + 1];

    int x, y;

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

                const int x_s = i * NB + 1;
                const int y_s = j * NB + 1;

                const int x_e = min((x + NB), N - 1);
                const int y_e = min((y + NB), N - 1);

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

                const int x_s = i * NB + 1;
                const int y_s = j * NB + 1;

                const int x_e = (x + NB) > N - 1 ? N - 1 : x + NB; // min
                const int y_e = (x + NB) > N - 1 ? N - 1 : x + NB;

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

    } while (dmax > eps);
}
void async_alg4(int N, double eps, double **u, double **f)
{
    // Как выполнять те же действия, что и в 11.1?
    // Будем вычислять волнами (т.е. значения пересчитываются так, чтобы друг на друга не наталкиваться)

    omp_lock_t dmax_lock;
    omp_init_lock(&dmax_lock);
    double h = 1.0 / (N + 1);
    double d, temp, dmax = 0;
    int i, j;

    double dm[N + 1]; // изменения (массив потому что иначе конфликты)
    int nx;           // размер волны (отсчёт для j)

    do
    {
        dmax = 0; // максимальное изменение u
        // нарастание волны (nx пробегает границу)
        for (nx = 1; nx < N + 1; nx++)
        {
            dm[nx] = 0;
#pragma omp parallel for shared(u, nx, dm) private(i, j, temp, d)
            for (i = 1; i <= nx; i++)
            {
                j = nx + 1 - i;

                temp = u[i][j];
                u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * f[i][j]);
                d = fabs(temp - u[i][j]);
                if (dm[i] < d)
                    dm[i] = d;
            }
        }
        // дошли до пика волны, теперь затухание
        for (nx = N - 1; nx > 0; nx--)
        {
#pragma omp parallel for shared(u, nx, dm) private(i, j, temp, d)
            for (i = N - nx + 1; i < N + 1; i++)
            {
                j = 2 * N - nx - i + 1;
                temp = u[i][j];
                u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] +
                                  u[i][j - 1] + u[i][j + 1] - h * h * f[i][j]);
                d = fabs(temp - u[i][j]);
                if (dm[i] < d)
                    dm[i] = d;
            }
        }

        // находим dmax
#pragma omp parallel for shared(dm, dmax) private(i)
        for (i = 1; i < nx + 1; i++)
        {
            omp_set_lock(&dmax_lock);
            if (dmax < dm[i])
                dmax = dm[i];
            omp_unset_lock(&dmax_lock);
        }

    } while (dmax > eps);
}
void async_alg3(int N, double eps, double **u, double **f)
{
    // Одна из проблем прошлых решений (11.2, 11.3) - race condition,
    // У нас u считается криво => больше итераций чем нужно,
    // Можно попробовать изменять не общий u, а считать локально и потом обновлять данные.

    omp_lock_t dmax_lock;
    omp_init_lock(&dmax_lock);
    double h = 1.0 / (N + 1);
    double d, dm, temp, dmax = 0;
    int i, j;
    double ucopy[N + 1][N + 1];

    do
    {
        dmax = 0;

#pragma omp parallel for shared(u, N, dmax, h, ucopy) private(i, j, d, dm, temp)
        for (i = 1; i < N + 1; i++)
        {
            dm = 0;
            for (j = 1; j < N + 1; j++)
            {
                temp = u[i][j];
                ucopy[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * f[i][j]);

                d = fabs(temp - ucopy[i][j]);

                if (dm < d)
                    dm = d;
            }
            omp_set_lock(&dmax_lock);
            if (dmax < dm)
                dmax = dm;
            omp_unset_lock(&dmax_lock);
        }

        // update u
        for (i = 1; i < N + 1; i++)
            for (j = 1; j < N + 1; j++)
                u[i][j] = ucopy[i][j];

    } while (dmax > eps);
}
void async_alg2(int N, double eps, double **u, double **f)
{

    omp_lock_t dmax_lock;
    omp_init_lock(&dmax_lock);
    double h = 1.0 / (N + 1);
    double temp, dmax = 0;
    int i, j;

    do
    {
        dmax = 0;
        double dm = 0;

#pragma omp parallel for shared(u, N, dmax, h) private(i, j, temp, dm)

        for (int i = 1; i < N + 1; i++)
        {
            dm = 0; // локальная (для каждого потока) оценка погрешности
            for (int j = 1; j < N + 1; j++)
            {
                double temp = u[i][j];

                u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1] - h * h * f[i][j]);

                double d = fabs(temp - u[i][j]);

                if (dm < d)
                    dm = d;
            }
            omp_set_lock(&dmax_lock);
            if (dmax < dm) // с помощью dm снижаем частоту блокировок
                dmax = dm;
            omp_unset_lock(&dmax_lock);
        }

    } while (dmax > eps);
}
void async_alg1(int N, double eps, double **u, double **f)
{
    omp_lock_t dmax_lock;
    omp_init_lock(&dmax_lock);

    double h = 1.0 / (N + 1);
    double temp, dmax = 0;
    int i, j;
    do
    {
        dmax = 0;
#pragma omp parallel for shared(u, N, dmax, h) private(i, j, temp)
        // общими(shared) являются те переменные, которые изменяться не будут
        // private переменные будут копироваться для каждого отдельного потока
        for (int i = 1; i < N + 1; i++)
        {
            // второй параллельный for можно убрать, он тут только ресурсы потребляет
#pragma omp parallel for shared(u, N, dmax, h) private(j, temp)
            for (int j = 1; j < N + 1; j++)
            {
                double temp = u[i][j];
                u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] +
                                  u[i][j - 1] + u[i][j + 1] - h * h * f[i][j]);
                double d = fabs(temp - u[i][j]);
                omp_set_lock(&dmax_lock); // блокировка во время изменения общих ресурсов
                // на ожидание впустую тратится много времени
                if (dmax < d)
                    dmax = d;
                omp_unset_lock(&dmax_lock);
            }
        }
    } while (dmax > eps);
}
void default_alg(int N, double eps, double **u, double **f)
{
    double h = 1.0 / (N + 1);
    double dmax;

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

    } while (dmax > eps);
}
void test(int repeats, int threads, alg run, prepare p, int N, double eps, double **u, double **f)
{
    double results[repeats];
    double average = 0;

    omp_set_num_threads(threads);

    for (int i = 0; i < repeats; i++)
    {
        p(N, u, f);
        double time = omp_get_wtime();
        run(N, eps, u, f);
        results[i] = omp_get_wtime() - time;
        average += results[i];
        printf("%f\t", results[i]);
    }

    printf("Result: %f\n", average / repeats);
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
void prepare2(int N, double **u, double **f)
{
    // Простое краевое условие

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
                u[x][y] = 0;
            }
            else if (x == 0) // левая грань
            {
                u[x][y] = y;
            }
            else if (y == N + 1) // верхняя грань
            {
                u[x][y] = 1;
            }
            else if (x == N + 1) // правая грань
            {
                u[x][y] = 2;
            }
            else
            {
                u[x][y] = min + (rand() / div);
            }
        }
    }
}
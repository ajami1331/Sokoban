#ifndef UTILS_H
#define UTILS_H 1

#define utils_get_tile_rect(x, y)                                                                                      \
    (Rectangle)                                                                                                        \
    {                                                                                                                  \
        x * 64, y * 64, 64, 64                                                                                         \
    }
#define utils_get_draw_rect(x, y)                                                                                      \
    (Rectangle)                                                                                                        \
    {                                                                                                                  \
        x * 32, y * 32, 32, 32                                                                                         \
    }

#define utils_max(a, b)                                                                                                \
    _Generic((a), int                                                                                                  \
             : utils_max_int, long long                                                                                \
             : utils_max_long_long, float                                                                              \
             : utils_max_float, double                                                                                 \
             : utils_max_double, long double                                                                           \
             : utils_max_long_double, default                                                                          \
             : utils_max_int)(a, b)

#define utils_min(a, b) _Generic((a), int : utils_min_int, default : utils_min_int)(a, b)

int utils_max_int(int a, int b);
long long utils_max_long_long(long long a, long long b);
float utils_max_float(float a, float b);
double utils_max_double(double a, double b);
long double utils_max_long_double(long double a, long double b);

int utils_min_int(int a, int b);

#endif

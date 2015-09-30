/**
 * ECE 353 Lab 1: Modeling Thermal Issues in Multicore Systems
 * Sarah Mangels, Matteo Puzella, Aaron Lucia
 * Sept 13, 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

double test(double, double);
double rk(double, double, double, double (*)(double, double));

int main(int argc, char *argv[]) {
    int i;
    double h = 0.01;
    double x = 0.0;
    double y = 0.0;
    for (i = 0; i < 100; i++) {
        y = rk(h, x, y, &test);
        x += h;
        printf("x=%f, y=%f\n", x, y);
    }
}

double test(double x, double y) {
    return (x * x) + (y * y);
}

/**
 * Runge-Kutta algorithm
 *
 * @param h step size
 * @param x_n previous x
 * @param y_n previous y
 * @param f pointer to function
 * @returns y(t+h)
 */
double rk(double h, double x_n, double y_n, double (*f)(double, double)) {
    double k1 = h * f(x_n, y_n);
    double k2 = h * f(x_n + h / 2, y_n + k1 / 2);
    double k3 = h * f(x_n + h / 2, y_n + k2 / 2);
    double k4 = h * f(x_n + h, y_n + k3);
    return y_n + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
}

/**
 * ECE 353 Lab 1: Modeling Thermal Issues in Multicore Systems
 * Sarah Mangels, Matteo Puzella, Aaron Lucia
 * Sept 13, 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

void usage(int argc, char *argv[]);
double *f(double, double[]);
double *rk(double, double, double[]);
void printParams();

double c[4];
double r[4][4];
double w[4];

int main(int argc, char *argv[]) {
    FILE *paramFile;
    FILE *powerTraceFile;
    FILE *outputFile;
    
    if (argc < 3) {
        usage(argc, argv);
        return 1;
    }
    
    char* paramFileName = argv[1];
    char* powerTraceFileName = argv[2];
    char* outputFileName = argv[3];
    double ambient = 300;
    if (argc > 4) {
        ambient = strtod(argv[3], NULL);
        outputFileName = argv[4];
    }
    
    paramFile = fopen(paramFileName, "r");
    powerTraceFile = fopen(powerTraceFileName, "r");
    outputFile = fopen(outputFileName, "w");
    
    assert(paramFile != NULL);
    assert(powerTraceFile != NULL);
    assert(outputFile != NULL);
    
    char *line = malloc(sizeof(char) * 136);
    assert(fgets(line, 136, paramFile) != NULL);
    assert(sscanf(line, "%lf%lf%lf%lf", &c[0], &c[1], &c[2], &c[3]) == 4);
    int i;
    for (i = 0; i < 4; i++) {
        assert(fgets(line, 136, paramFile) != NULL);
        assert(sscanf(line, "%lf%lf%lf%lf", &r[i][0], &r[i][1], &r[i][2], &r[i][3]) == 4);
    }
    
    printParams();
    
    double x = 0.0;
    double y[] = {ambient, ambient, ambient, ambient};
    double prevt;
    while (fgets(line, 136, powerTraceFile) != NULL) {
        double t;
        assert(sscanf(line, "%lf%lf%lf%lf%lf", &t, &w[0], &w[1], &w[2], &w[3]) == 5);
        double h = t - prevt;
        prevt = t;
        rk(h, x, y);
        x += h;
    }
    
    free(line);

    fclose(paramFile);
    fclose(powerTraceFile);
    fclose(outputFile);
}

void usage(int argc, char *argv[]) {
    printf("Usage: %s takes 3 or 4 arguments, you provided %d\n", argv[0], argc - 1);
    printf("%s paramFile powerFile optionalAmbientInput outputFile\n", argv[0]);
}

double *f(double x, double y[]) {
    double *result = malloc(sizeof(double) * 4);
    
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i != j) {
                result[i] -= (y[i] - y[j]) / (r[i][j] * c[i]);
            }
        }
        
        result[i] += w[i] / c[i];
    }
    
    return result;
}

/**
 * Runge-Kutta algorithm
 *
 * @param h step size
 * @param x_n previous x
 * @param y_n previous ys
 * @returns y(t+h)
 */
double *rk(double h, double x_n, double y_n[]) {
    int i;
    double *k1 = f(x_n, y_n);
    for (i = 0; i < 4; i++) {
        k1[i] *= h;
    }
    
    double ya[4] = {y_n[0] + k1[0] / 2, y_n[1] + k1[1] / 2, y_n[2] + k1[2] / 2, y_n[3] + k1[3] / 2};
    double *k2 = f(x_n + h / 2, ya);
    for (i = 0; i < 4; i++) {
        k2[i] *= h;
    }
    
    double yb[4] = {y_n[0] + k2[0] / 2, y_n[1] + k2[1] / 2, y_n[2] + k2[2] / 2, y_n[3] + k2[3] / 2};
    double *k3 = f(x_n + h / 2, yb);
    for (i = 0; i < 4; i++) {
        k3[i] *= h;
    }
    
    double yc[4] = {y_n[0] + k3[0], y_n[1] + k3[1], y_n[2] + k3[2], y_n[3] + k3[3]};
    double *k4 = f(x_n + h, yc);
    for (i = 0; i < 4; i++) {
        k4[i] *= h;
    }
    
    for (i = 0; i < 4; i++) {
        y_n[i] += (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]) / 6;
    }
    
    free(k1);
    free(k2);
    free(k3);
    free(k4);
    
    return y_n;
}

void printParams() {
    printf("C: %f %f %f %f\n", c[0], c[1], c[2], c[3]);
    int i;
    for (i = 0; i < 4; i++) {
        printf("R: %f %f %f %f\n", r[i][0], r[i][1], r[i][2], r[i][3]);
    }
}

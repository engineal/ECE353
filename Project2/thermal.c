/**
 * ECE 353 Lab 1: Modeling Thermal Issues in Multicore Systems
 * Sarah Mangels, Matteo Puzella, Aaron Lucia
 * Sept 13, 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define k 8.617e-5
#define Ea 0.8

//#define DEBUG

typedef double *(*func)(double, double[], double[]);

void usage(int, char*[]);
double *temp(double, double[], double[]);
double *age(double, double[], double[]);
double *rk(double, double[], double[], func);
void printParams();

double h = 0.005;
double c[4];
double r[4][5];

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
    int j;
    for (j = 0; j < 5; j++) {
        assert(fgets(line, 136, paramFile) != NULL);
        assert(sscanf(line, "%lf%lf%lf%lf", &r[0][j], &r[1][j], &r[2][j], &r[3][j]) == 4);
    }
    
    #ifdef DEBUG
        printParams();
    #endif
    
    double x = 0.0;
    double coretemp[] = {ambient, ambient, ambient, ambient, ambient};
    double coreage[] = {0, 0, 0, 0, 0};
    
    while (fgets(line, 136, powerTraceFile) != NULL) {
        double t;
        double w[4];
        assert(sscanf(line, "%lf%lf%lf%lf%lf", &t, &w[0], &w[1], &w[2], &w[3]) == 5);
        while (x < t) {
            rk(x, coretemp, w, temp);
            rk(x, coreage, coretemp, age);
            x += h;
        }
        
        fprintf(outputFile, "%f %f %f %f %f %f %f %f %f\n", t, coretemp[0], coreage[0],
            coretemp[1], coreage[1], coretemp[2], coreage[2], coretemp[3], coreage[3]);
    }
    
    free(line);

    fclose(paramFile);
    fclose(powerTraceFile);
    fclose(outputFile);
}


/**
 * Prints out helpful information if there are incorect arguments
 */
void usage(int argc, char *argv[]) {
    printf("Usage: %s takes 3 or 4 arguments, you provided %d\n", argv[0], argc - 1);
    printf("%s paramFile powerFile optionalAmbientInput outputFile\n", argv[0]);
}

/**
 * Calculate the temperature of each core
 */
double *temp(double x, double y[], double w[]) {
    double *result = malloc(sizeof(double) * 4);
    
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        result[i] = 0;
        
        for (j = 0; j < 5; j++) {
            if (i != j) {
                result[i] -= (y[i] - y[j]) / (r[i][j] * c[i]);
            }
        }
        
        result[i] += w[i] / c[i];
    }
    
    return result;
}

/**
 * Calculate the age of each core
 */
double *age(double x, double y[], double t[]) {
    double *result = malloc(sizeof(double) * 4);
    
    int i;
    for (i = 0; i < 4; i++) {
        result[i] = exp(-Ea/(k*t[i])) / exp(-Ea/(k*t[4]));
    }
    
    return result;
}

/**
 * Runge-Kutta algorithm
 *
 * @param x_n previous x
 * @param y_n previous ys
 * @param f the function to run Runge-Kutta on
 * @returns y(t+h)
 */
double *rk(double x_n, double y_n[], double extra[], func f) {
    int i;
    double *k1 = f(x_n, y_n, extra);
    for (i = 0; i < 4; i++) {
        k1[i] *= h;
    }
    
    double ya[] = {y_n[0] + k1[0] / 2, y_n[1] + k1[1] / 2, y_n[2] + k1[2] / 2, y_n[3] + k1[3] / 2, y_n[4]};
    double *k2 = f(x_n + h / 2, ya, extra);
    for (i = 0; i < 4; i++) {
        k2[i] *= h;
    }
    
    double yb[] = {y_n[0] + k2[0] / 2, y_n[1] + k2[1] / 2, y_n[2] + k2[2] / 2, y_n[3] + k2[3] / 2, y_n[4]};
    double *k3 = f(x_n + h / 2, yb, extra);
    for (i = 0; i < 4; i++) {
        k3[i] *= h;
    }
    
    double yc[] = {y_n[0] + k3[0], y_n[1] + k3[1], y_n[2] + k3[2], y_n[3] + k3[3], y_n[4]};
    double *k4 = f(x_n + h, yc, extra);
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

/**
 * Provide helpful debug information
 */
void printParams() {
    printf("h: %f\n", h);
    printf("C: %f %f %f %f\n", c[0], c[1], c[2], c[3]);
    int j;
    for (j = 0; j < 5; j++) {
        printf("R%d: %f %f %f %f\n", j, r[0][j], r[1][j], r[2][j], r[3][j]);
    }
}

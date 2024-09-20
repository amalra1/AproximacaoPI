#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <fenv.h>
#include <gmp.h>

#define M_PI 3.14159265358979323846

void fatorial(mpf_t resultado, int n) 
{
    mpf_set_ui(resultado, 1);

    for (int i = 1; i <= n; i++)
        mpf_mul_ui(resultado, resultado, i);
}

int main() 
{
    double tolerancia;
    mpf_set_default_prec(1024); // Precisão padrão - 1024

    mpf_t pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima, numerador, denominador, termo, mpf_tolerancia;
    mpf_inits(pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima, numerador, denominador, termo, mpf_tolerancia, NULL);

    printf("Digite a tolerância: ");
    scanf("%lf", &tolerancia);
    mpf_set_d(mpf_tolerancia, tolerancia);

    // Arredondamento para cima
    fesetround(FE_UPWARD);
    int n = 0;
    do 
    {
        mpf_set(pi_anterior_cima, pi_aprox_cima);

        // Numerador
        // 2^n
        mpf_set_ui(numerador, 1);
        for (int i = 0; i < n; i++)
            mpf_mul_ui(numerador, numerador, 2);

        // n!
        mpf_t fatorial_n;
        mpf_init(fatorial_n);
        fatorial(fatorial_n, n);

        // 2^n*k!*k!
        mpf_mul(numerador, numerador, fatorial_n);
        mpf_mul(numerador, numerador, fatorial_n);

        // Denominador - (2n + 1)!
        fatorial(denominador, 2 * n + 1);

        // Numerador / Denominador
        mpf_div(termo, numerador, denominador);

        // Soma o termo obtido no somatório na aprox total
        mpf_add(pi_aprox_cima, pi_aprox_cima, termo);

        // Erro aproximado de N com N - 1
        mpf_sub(erro_aproximado_cima, pi_aprox_cima, pi_anterior_cima);
        mpf_abs(erro_aproximado_cima, erro_aproximado_cima);

        n++;

        gmp_printf("------Iteração: %d\n", n - 1);
        gmp_printf("Numerador: %.Ff\n", numerador);
        gmp_printf("Denominador: %.Ff\n", denominador);
        gmp_printf("Pi aprox: %.15Fe\n", pi_aprox_cima);
        gmp_printf("Erro relativo: %.15Fe\n\n", erro_aproximado_cima);

        mpf_clear(fatorial_n);
    } 
    while (mpf_cmp(erro_aproximado_cima, mpf_tolerancia) > 0);

    mpf_mul_ui(pi_aprox_cima, pi_aprox_cima, 2); // Porque a fórmula é pi/2
    mpf_set_d(erro_exato_cima, M_PI);
    mpf_sub(erro_exato_cima, erro_exato_cima, pi_aprox_cima);
    mpf_abs(erro_exato_cima, erro_exato_cima);

    // mpf -> double
    double pi_aprox_cima_d = mpf_get_d(pi_aprox_cima);
    double erro_aproximado_cima_d = mpf_get_d(erro_aproximado_cima);
    double erro_exato_cima_d = mpf_get_d(erro_exato_cima);

    // double -> unsigned long long int
    unsigned long long int pi_aprox_cima_hex = *(unsigned long long int*)&pi_aprox_cima_d;
    unsigned long long int erro_aproximado_cima_hex = *(unsigned long long int*)&erro_aproximado_cima_d;
    unsigned long long int erro_exato_cima_hex = *(unsigned long long int*)&erro_exato_cima_d;

    printf("%d\n", n);
    printf("%.15e %llX\n", erro_aproximado_cima_d, erro_aproximado_cima_hex);
    printf("%.15e %llX\n", erro_exato_cima_d, erro_exato_cima_hex);
    printf("%.15e %llX\n", pi_aprox_cima_d, pi_aprox_cima_hex);

    mpf_clears(pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima, numerador, denominador, termo, mpf_tolerancia, NULL);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <fenv.h>
#include <gmp.h>

#define M_PI 3.14159265358979323846

void fatorial(mpf_t resultado, int n, long long int *flops) 
{
    mpf_set_ui(resultado, 1);

    for (int i = 1; i <= n; i++)
    {
        mpf_mul_ui(resultado, resultado, i);
        (*flops)++;
    }
}

int main() 
{
    long long int flops = 0;
    double tolerancia;
    mpf_set_default_prec(1024); // Precisão padrão - 1024

    mpf_t pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima, numerador, numerador_ant, denominador, termo, mpf_tolerancia;
    mpf_t fatorial_n, fatorial_n_ant;
    mpf_t pi_aprox_baixo, pi_anterior_baixo, erro_aproximado_baixo, erro_exato_baixo;

    mpf_inits(pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima, numerador, numerador_ant, fatorial_n, fatorial_n_ant, denominador, termo, mpf_tolerancia, NULL);
    mpf_inits(pi_aprox_baixo, pi_anterior_baixo, erro_aproximado_baixo, erro_exato_baixo, NULL);

    printf("Digite a tolerância: ");
    scanf("%lf", &tolerancia);
    mpf_set_d(mpf_tolerancia, tolerancia);

    // Arredondamento para baixo
    fesetround(FE_DOWNWARD);
    int n = 0;
    do 
    {
        mpf_set(pi_anterior_baixo, pi_aprox_baixo);

        // Numerador
        // 2^n
        mpf_set_ui(numerador, 1);
        for (int i = 0; i < n; i++)
            mpf_mul_ui(numerador, numerador, 2);

        // n!
        mpf_t fatorial_n;
        mpf_init(fatorial_n);
        fatorial(fatorial_n, n, &flops);

        // 2^n*n!*n!
        mpf_mul(numerador, numerador, fatorial_n);
        mpf_mul(numerador, numerador, fatorial_n);

        // Denominador - (2n + 1)!
        fatorial(denominador, 2 * n + 1, &flops);

        // Numerador / Denominador
        mpf_div(termo, numerador, denominador);

        // Soma o termo obtido no somatório na aprox total
        mpf_add(pi_aprox_baixo, pi_aprox_baixo, termo);

        // Erro aproximado de N com N - 1
        mpf_sub(erro_aproximado_baixo, pi_aprox_baixo, pi_anterior_baixo);
        mpf_abs(erro_aproximado_baixo, erro_aproximado_baixo);

        n++;

        // gmp_printf("------Iteração: %d\n", n - 1);
        // gmp_printf("Numerador: %.Ff\n", numerador);
        // gmp_printf("Denominador: %.Ff\n", denominador);
        // gmp_printf("Pi aprox: %.15Fe\n", pi_aprox_baixo);
        // gmp_printf("Erro relativo: %.15Fe\n\n", erro_aproximado_baixo);

        mpf_clear(fatorial_n);
    } 
    while (mpf_cmp(erro_aproximado_baixo, mpf_tolerancia) > 0);

    mpf_mul_ui(pi_aprox_baixo, pi_aprox_baixo, 2); // Porque a fórmula é pi/2
    mpf_set_d(erro_exato_baixo, M_PI);
    mpf_sub(erro_exato_baixo, erro_exato_baixo, pi_aprox_baixo);
    mpf_abs(erro_exato_baixo, erro_exato_baixo);

    // Arredondamento para cima
    fesetround(FE_UPWARD);
    n = 0;
    flops = 0;
    mpf_set_ui(numerador, 1);
    mpf_set(numerador_ant, numerador);
    mpf_set_ui(fatorial_n, 1);
    mpf_set(fatorial_n_ant, fatorial_n);
    do 
    {
        mpf_set(pi_anterior_cima, pi_aprox_cima);

        // Numerador
        // 2^n
        if (n > 0)
        {
            mpf_mul_ui(numerador, numerador_ant, 2);
            flops++;
            mpf_set(numerador_ant, numerador);
        }

        // n!
        if (n > 0)
        {
            mpf_mul_ui(fatorial_n, fatorial_n_ant, n);
            flops++;
            mpf_set(fatorial_n_ant, fatorial_n);
        }

        // 2^n*n!*n!
        mpf_mul(numerador, numerador, fatorial_n);
        flops++;
        mpf_mul(numerador, numerador, fatorial_n);
        flops++;

        // Denominador - (2n + 1)!
        fatorial(denominador, 2 * n + 1, &flops);

        // Numerador / Denominador
        mpf_div(termo, numerador, denominador);
        flops++;

        // Soma o termo obtido no somatório na aprox total
        mpf_add(pi_aprox_cima, pi_aprox_cima, termo);
        flops++;

        // Erro aproximado de N com N - 1
        mpf_sub(erro_aproximado_cima, pi_aprox_cima, pi_anterior_cima);
        flops++;
        mpf_abs(erro_aproximado_cima, erro_aproximado_cima);

        n++;

        // gmp_printf("------Iteração: %d\n", n - 1);
        // gmp_printf("Numerador: %.Ff\n", numerador);
        // gmp_printf("Denominador: %.Ff\n", denominador);
        // gmp_printf("Pi aprox: %.15Fe\n", pi_aprox_cima);
        // gmp_printf("Erro relativo: %.15Fe\n\n", erro_aproximado_cima);

        flops++; // Da comparação feita no while
    } 
    while (mpf_cmp(erro_aproximado_cima, mpf_tolerancia) > 0);

    mpf_mul_ui(pi_aprox_cima, pi_aprox_cima, 2); // Porque a fórmula é pi/2
    flops++;
    mpf_set_d(erro_exato_cima, M_PI);
    mpf_sub(erro_exato_cima, erro_exato_cima, pi_aprox_cima);
    flops++;
    mpf_abs(erro_exato_cima, erro_exato_cima);

    // mpf -> double
    double pi_aprox_cima_d = mpf_get_d(pi_aprox_cima);
    double erro_aproximado_cima_d = mpf_get_d(erro_aproximado_cima);
    double erro_exato_cima_d = mpf_get_d(erro_exato_cima);
    double pi_aprox_baixo_d = mpf_get_d(pi_aprox_baixo);

    // double -> unsigned long long int
    unsigned long long int pi_aprox_cima_hex = *(unsigned long long int*)&pi_aprox_cima_d;
    unsigned long long int erro_aproximado_cima_hex = *(unsigned long long int*)&erro_aproximado_cima_d;
    unsigned long long int erro_exato_cima_hex = *(unsigned long long int*)&erro_exato_cima_d;
    unsigned long long int pi_aprox_baixo_hex = *(unsigned long long int*)&pi_aprox_baixo_d;

    printf("%d\n", n);
    printf("%.15e %llX\n", erro_aproximado_cima_d, erro_aproximado_cima_hex);
    printf("%.15e %llX\n", erro_exato_cima_d, erro_exato_cima_hex);
    printf("%.15e %llX\n", pi_aprox_cima_d, pi_aprox_cima_hex);
    printf("%.15e %llX\n", pi_aprox_baixo_d, pi_aprox_baixo_hex);
    printf("%lld\n", flops);

    mpf_clears(pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima, numerador, denominador, termo, mpf_tolerancia, NULL);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <fenv.h>
#include <gmp.h>

#define M_PI 3.14159265358979323846 // M_PI da math.h não tava funcionando, copiei a linha e defini aqui

// Função que arredonda o último bit da tolerância pra cima
void arredonda_cima_tolerancia(mpf_t resultado, const mpf_t input, const mpf_t tolerancia) 
{
    mp_exp_t exp;
    mpf_get_str(NULL, &exp, 10, 0, tolerancia);
    unsigned int precision = -exp;

    mpf_t escala, temp;
    mpf_init(escala);
    mpf_init(temp);

    // Define a escala como 10^precisão
    mpf_set_ui(escala, 10);
    mpf_pow_ui(escala, escala, precision);

    // Multiplica o valor pela escala
    mpf_mul(temp, input, escala);

    // Arredonda para o inteiro mais próximo para cima
    mpf_ceil(temp, temp);

    // Divide de volta pela escala
    mpf_div(temp, temp, escala);

    // Define o resultado
    mpf_set(resultado, temp);
}

// Função que arredonda o último bit da tolerância pra baixo
void arredonda_baixo_tolerancia(mpf_t resultado, const mpf_t input, const mpf_t tolerancia) 
{
    mp_exp_t exp;
    mpf_get_str(NULL, &exp, 10, 0, tolerancia);
    unsigned int precisao = -exp;

    mpf_t escala, temp;
    mpf_init(escala);
    mpf_init(temp);

    // Define a escala como 10^precisão
    mpf_set_ui(escala, 10);
    mpf_pow_ui(escala, escala, precisao);

    // Multiplica o valor pela escala
    mpf_mul(temp, input, escala);

    // Arredonda para o inteiro mais próximo para cima
    mpf_floor(temp, temp);

    // Divide de volta pela escala
    mpf_div(temp, temp, escala);

    // Define o resultado
    mpf_set(resultado, temp);
}

// União para pegar a representação inteira de um ponto flutuante
typedef union 
{
    double d;
    uint64_t i;
} DoubleIntUnion;

// Função para converter mpf_t para uint64_t
uint64_t mpf_to_uint64(mpf_t num)
{
    DoubleIntUnion u;
    u.d = mpf_get_d(num);
    return u.i;
}

// Função para calcular a diferença em ULPs
uint64_t calcula_dif_ulps(mpf_t a, mpf_t b) 
{
    uint64_t int_a = mpf_to_uint64(a);
    uint64_t int_b = mpf_to_uint64(b);
    return int_a - int_b;
}

int main() 
{
    long long int flops = 0;
    double tolerancia;
    int n;
    mpf_set_default_prec(1024); // Precisão padrão - 1024

    mpf_t numerador, numerador_ant, denominador, termo, mpf_tolerancia;
    mpf_t fatorial_n, fatorial_n_ant, fatorial_denominador, fatorial_denominador_ant;
    mpf_t pi_aprox_baixo, pi_anterior_baixo, erro_aproximado_baixo, erro_exato_baixo;
    mpf_t pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima;

    mpf_inits(numerador, numerador_ant, denominador, termo, mpf_tolerancia, NULL);
    mpf_inits(fatorial_n, fatorial_n_ant, fatorial_denominador, fatorial_denominador_ant, NULL);
    mpf_inits(pi_aprox_baixo, pi_anterior_baixo, erro_aproximado_baixo, erro_exato_baixo, NULL);
    mpf_inits(pi_aprox_cima, pi_anterior_cima, erro_aproximado_cima, erro_exato_cima, NULL);

    scanf("%lf", &tolerancia);
    mpf_set_d(mpf_tolerancia, tolerancia);

    // Arredondamento para baixo
    fesetround(FE_DOWNWARD);
    n = 0;
    mpf_set_ui(numerador, 1);
    mpf_set(numerador_ant, numerador);
    mpf_set_ui(fatorial_n, 1);
    mpf_set(fatorial_n_ant, fatorial_n);
    mpf_set_ui(fatorial_denominador, 1);
    mpf_set(fatorial_denominador_ant, fatorial_denominador);
    do 
    {
        mpf_set(pi_anterior_baixo, pi_aprox_baixo);

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
        if (n > 0)
        {
           // Faz duas vezes porque o fatorial do denominador vai subindo de 2 em 2 a cada n

            mpf_mul_ui(fatorial_denominador, fatorial_denominador_ant, 2*n+1);
            flops++;
            mpf_set(fatorial_denominador_ant, fatorial_denominador);

            mpf_mul_ui(fatorial_denominador, fatorial_denominador_ant, 2*n);
            flops++;
            mpf_set(fatorial_denominador_ant, fatorial_denominador);
        }

        // Numerador / Denominador
        mpf_div(termo, numerador, fatorial_denominador);
        flops++;

        // Soma o termo obtido no somatório na aprox total
        mpf_add(pi_aprox_baixo, pi_aprox_baixo, termo);
        flops++;

        // Erro aproximado de N com N - 1
        mpf_sub(erro_aproximado_baixo, pi_aprox_baixo, pi_anterior_baixo);
        flops++;
        mpf_abs(erro_aproximado_baixo, erro_aproximado_baixo);

        // Verifica se o erro está dentro da tolerância antes de arredondar
        if (mpf_cmp(erro_aproximado_baixo, mpf_tolerancia) <= 0)
            break;

        // Arredonda para cima com base na tolerância
        arredonda_baixo_tolerancia(pi_aprox_baixo, pi_aprox_baixo, mpf_tolerancia);

        n++;
        flops++; // Da comparação feita no while
    } 
    while (1);

    mpf_mul_ui(pi_aprox_baixo, pi_aprox_baixo, 2); // Porque a fórmula é pi/2
    flops++;
    mpf_set_d(erro_exato_baixo, M_PI);
    mpf_sub(erro_exato_baixo, erro_exato_baixo, pi_aprox_baixo);
    flops++;
    mpf_abs(erro_exato_baixo, erro_exato_baixo);

    // Arredondamento para cima
    fesetround(FE_UPWARD);
    n = 0;
    flops = 0;
    mpf_set_ui(numerador, 1);
    mpf_set(numerador_ant, numerador);
    mpf_set_ui(fatorial_n, 1);
    mpf_set(fatorial_n_ant, fatorial_n);
    mpf_set_ui(fatorial_denominador, 1);
    mpf_set(fatorial_denominador_ant, fatorial_denominador);
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
        if (n > 0)
        {
           // Faz duas vezes porque o fatorial do denominador vai subindo de 2 em 2 a cada n

            mpf_mul_ui(fatorial_denominador, fatorial_denominador_ant, 2*n+1);
            flops++;
            mpf_set(fatorial_denominador_ant, fatorial_denominador);

            mpf_mul_ui(fatorial_denominador, fatorial_denominador_ant, 2*n);
            flops++;
            mpf_set(fatorial_denominador_ant, fatorial_denominador);
        }

        // Numerador / Denominador
        mpf_div(termo, numerador, fatorial_denominador);
        flops++;

        // Soma o termo obtido no somatório na aprox total
        mpf_add(pi_aprox_cima, pi_aprox_cima, termo);
        flops++;

        // Erro aproximado de N com N - 1
        mpf_sub(erro_aproximado_cima, pi_aprox_cima, pi_anterior_cima);
        flops++;
        mpf_abs(erro_aproximado_cima, erro_aproximado_cima);

        // Verifica se o erro está dentro da tolerância antes de arredondar
        if (mpf_cmp(erro_aproximado_cima, mpf_tolerancia) <= 0)
            break;

        // Arredonda para cima com base na tolerância
        arredonda_cima_tolerancia(pi_aprox_cima, pi_aprox_cima, mpf_tolerancia);

        n++;
        flops++; // Da comparação feita no while
    } 
    while (1);

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

    int64_t ulps_int = calcula_dif_ulps(pi_aprox_cima, pi_aprox_baixo);

    printf("%d\n", n);
    printf("%.15e %llX\n", erro_aproximado_cima_d, erro_aproximado_cima_hex);
    printf("%.15e %llX\n", erro_exato_cima_d, erro_exato_cima_hex);
    printf("%.25e %llX\n", pi_aprox_baixo_d, pi_aprox_baixo_hex);
    printf("%.25e %llX\n", pi_aprox_cima_d, pi_aprox_cima_hex);
    printf("%ld\n", ulps_int);
    printf("%lld\n", flops);

    return 0;
}

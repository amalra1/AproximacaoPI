#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {

  int n, ULPs;
  double EAapD, EAexD, PI1D, PI2D;
  int64_t EAapH, EAexH, PI1H, PI2H;

  if (scanf("%d\n", &n) != 1 ||
      scanf("%lf %llX\n", &EAapD, &EAapH) != 2 ||
      scanf("%lf %llX\n", &EAexD, &EAexH) != 2 ||
      scanf("%lf %llX\n", &PI1D, &PI1H)   != 2 ||
      scanf("%lf %llX\n", &PI2D, &PI2H)   != 2 ||
      scanf("%d", &ULPs) != 1) {
    printf("Formato de saída INCORRETO\n");
    exit(1);
  }

  printf("n = %d\n", n);
  printf("EA aprox = %.15e (%llX)\n", EAapD, EAapH);
  printf("EA exato = %.15e (%llX)\n", EAexD, EAexH);
  printf("PI'  = %.15e (%llX)\n", PI1D, PI1H);
  printf("PI'' = %.15e (%llX)\n", PI2D, PI2H);
  printf("ULPs = %d\n", ULPs);
  printf("Formato de saída correto!\n");
}

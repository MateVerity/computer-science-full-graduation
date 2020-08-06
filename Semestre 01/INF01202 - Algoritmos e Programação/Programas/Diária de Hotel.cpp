/*
  Disciplina de Algoritmos de Programa��o (INF01202) - Turma H
  Aluno: Jo�o Luiz Grave Gross 	
  Matr�cula: 180171
  Atividade: Exerc�cio de Avalia��o 2.4
  Data: 08/03/09 16:49
  Descri��o: Um hotel com 75 apartamentos deseja fazer uma promo��o especial de final de semana, concedendo um desconto 
de 25% na di�ria. Com isto, espera aumentar sua taxa de ocupa��o de 50 para 80%. Sendo dado o valor normal da di�ria, 
calcular e imprimir:
         a) o valor da di�ria promocional;
         b) o valor total arrecadado com 80% de ocupa��o e di�ria promocional;
         c) o valor total arrecadado com 50% de ocupa��o e di�ria normal;
         d) a diferen�a entre estes dois valores.
*/

#include<stdlib.h>
#include<stdio.h>

int main ()
{
    float diaria_normal, diaria_prom, valor_80_prom, valor_50_normal, diferenca;
    printf("Digite o valor da diaria: ");
    scanf("%f",&diaria_normal);
    diaria_prom = diaria_normal*0.75;
    valor_80_prom = diaria_prom*75*0.8;
    valor_50_normal = diaria_normal*38;                         // 75*0,5 = 37,5 -> arredondei para mais -> 38
    diferenca = valor_80_prom - valor_50_normal;
    printf("\nDiaria Promocional: %.2f\n", diaria_prom);
    printf("Valor total arrecadado com 80 porcento de ocupacao e diaria promocional: %.2f", valor_80_prom);    
    printf("Valor total arrecadado com 50 porcento de ocupacao e diaria normal: %.2f\n", valor_50_normal);
    printf("Diferenca: %.2f\n", diferenca);    
    system("pause");
    return 0;
}

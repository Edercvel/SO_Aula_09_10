#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para verificar se um número é primo.
static int eh_primo(long long numero) {
    if (numero < 2) {
        return 0;
    }

    if (numero == 2) {
        return 1;
    }

    if (numero % 2 == 0) {
        return 0;
    }

    for (long long divisor = 3; divisor <= numero / divisor; divisor += 2) {
        if (numero % divisor == 0) {
            return 0;
        }
    }

    return 1;
}

// Função para calcular a diferença de tempo em milissegundos entre dois instantes.
static double diferenca_ms(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) * 1000.0 +
           (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
}
    // Função principal do programa.
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *arquivo = fopen(argv[1], "r");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }

    int quantidade;

    if (fscanf(arquivo, "%d", &quantidade) != 1 || quantidade <= 0) {
        fprintf(stderr, "Formato de entrada invalido.\n");
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    long long *numeros =
        malloc((size_t)quantidade * sizeof(long long));

    if (numeros == NULL) {
        fprintf(stderr, "Erro ao alocar memoria.\n");
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < quantidade; i++) {
        if (fscanf(arquivo, "%lld", &numeros[i]) != 1) {
            fprintf(stderr, "Erro ao ler o numero %d.\n", i + 1);
            free(numeros);
            fclose(arquivo);
            return EXIT_FAILURE;
        }
    }

    fclose(arquivo);

    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    int quantidade_primos = 0;

    for (int i = 0; i < quantidade; i++) {
        if (eh_primo(numeros[i])) {
            quantidade_primos++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo_ms = diferenca_ms(inicio, fim);

    printf("Versao: sequencial\n");
    printf("Quantidade de threads: 1\n");
    printf("Quantidade de numeros: %d\n", quantidade);
    printf("Quantidade de primos: %d\n", quantidade_primos);
    printf("Tempo de execucao: %.6f ms\n", tempo_ms);

    free(numeros);

    return EXIT_SUCCESS;
}   
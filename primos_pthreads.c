 // Necessário para clock_gettime()
#define _POSIX_C_SOURCE 199309L


#include <errno.h> 
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Estrutura para armazenar os dados de cada thread.
 */
typedef struct {
    const long long *numeros;
    int inicio;
    int fim;
    int quantidade_primos;
} DadosThread;

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
    
    for (long long divisor = 3;
         divisor <= numero / divisor;
         divisor += 2) {
        // Se encontrar um divisor, o número não é primo.
        if (numero % divisor == 0) {
            return 0;
        }
    }

    return 1;
}

// Função executada por cada thread para contar os números primos no intervalo designado.
static void *contar_primos(void *argumento) {
    DadosThread *dados = (DadosThread *)argumento;

        dados->quantidade_primos = 0;

        for (int i = dados->inicio; i < dados->fim; i++) {
        if (eh_primo(dados->numeros[i])) {
            dados->quantidade_primos++;
        }
    }

    return NULL;
}
// Função para calcular a diferença de tempo em milissegundos entre dois instantes.
static double diferenca_ms(struct timespec inicio,
                           struct timespec fim) {
    time_t segundos = fim.tv_sec - inicio.tv_sec;
    long nanossegundos = fim.tv_nsec - inicio.tv_nsec;

    return segundos * 1000.0 +
           nanossegundos / 1000000.0;
}
// Função para converter a quantidade de threads a partir de uma string. Retorna 1 em caso de
// sucesso e 0 em caso de erro.
static int converter_threads(const char *texto, int *resultado) {
    char *fim;
    long valor;

    errno = 0;
    valor = strtol(texto, &fim, 10);

    if (errno != 0 ||
        *texto == '\0' ||
        *fim != '\0' ||
        valor <= 0 ||
        valor > INT_MAX) {
        return 0;
    }

    *resultado = (int)valor;
    return 1;
}

// Função principal do programa.
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(
            stderr,
            "Uso: %s <arquivo_de_entrada> <quantidade_de_threads>\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    int quantidade_threads;

    if (!converter_threads(argv[2], &quantidade_threads)) {
        fprintf(
            stderr,
            "Erro: a quantidade de threads deve ser um inteiro positivo.\n"
        );
        return EXIT_FAILURE;
    }

    FILE *arquivo = fopen(argv[1], "r");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }

    int quantidade;

    if (fscanf(arquivo, "%d", &quantidade) != 1 ||
        quantidade <= 0) {
        fprintf(stderr, "Erro: formato de entrada invalido.\n");
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    long long *numeros =
        malloc((size_t)quantidade * sizeof(long long));

    if (numeros == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para os numeros.\n");
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < quantidade; i++) {
        if (fscanf(arquivo, "%lld", &numeros[i]) != 1) {
            fprintf(
                stderr,
                "Erro ao ler o numero da posicao %d.\n",
                i
            );

            free(numeros);
            fclose(arquivo);
            return EXIT_FAILURE;
        }
    }

    fclose(arquivo);

    // Não faz sentido criar mais threads do que elementos.
    if (quantidade_threads > quantidade) {
        quantidade_threads = quantidade;
    }

    pthread_t *threads =
        malloc((size_t)quantidade_threads * sizeof(pthread_t));

    DadosThread *dados =
        malloc((size_t)quantidade_threads * sizeof(DadosThread));

    if (threads == NULL || dados == NULL) {
        fprintf(
            stderr,
            "Erro ao alocar memoria para as threads.\n"
        );

        free(numeros);
        free(threads);
        free(dados);
        return EXIT_FAILURE;
    }

    struct timespec inicio;
    struct timespec fim;

    /*
     * A leitura do arquivo e a alocação de memória ficam fora da medição.
     * O tempo medido inclui a criação das threads, o processamento e o join.
     */
    if (clock_gettime(CLOCK_MONOTONIC, &inicio) != 0) {
        perror("Erro ao iniciar a medicao de tempo");

        free(numeros);
        free(threads);
        free(dados);
        return EXIT_FAILURE;
    }

    int base = quantidade / quantidade_threads;
    int restante = quantidade % quantidade_threads;
    int indice_atual = 0;
    int threads_criadas = 0;

    for (int i = 0; i < quantidade_threads; i++) {
        int tamanho_faixa = base;

        /*
         * As primeiras threads recebem um elemento adicional
         * quando a divisão não é exata.
         */
        if (i < restante) {
            tamanho_faixa++;
        }

        dados[i].numeros = numeros;
        dados[i].inicio = indice_atual;
        dados[i].fim = indice_atual + tamanho_faixa;
        dados[i].quantidade_primos = 0;

        int erro = pthread_create(
            &threads[i],
            NULL,
            contar_primos,
            &dados[i]
        );

        if (erro != 0) {
            fprintf(
                stderr,
                "Erro ao criar a thread %d: codigo %d.\n",
                i,
                erro
            );

            /*
             * Aguarda as threads que já foram criadas antes de encerrar.
             */
            for (int j = 0; j < threads_criadas; j++) {
                pthread_join(threads[j], NULL);
            }

            free(numeros);
            free(threads);
            free(dados);
            return EXIT_FAILURE;
        }

        threads_criadas++;
        indice_atual += tamanho_faixa;
    }

    int quantidade_primos = 0;

    for (int i = 0; i < threads_criadas; i++) {
        int erro = pthread_join(threads[i], NULL);

        if (erro != 0) {
            fprintf(
                stderr,
                "Erro ao aguardar a thread %d: codigo %d.\n",
                i,
                erro
            );

            free(numeros);
            free(threads);
            free(dados);
            return EXIT_FAILURE;
        }

        quantidade_primos += dados[i].quantidade_primos;
    }
    // Calcula o tempo total de execução.
    if (clock_gettime(CLOCK_MONOTONIC, &fim) != 0) {
        perror("Erro ao finalizar a medicao de tempo");

        free(numeros);
        free(threads);
        free(dados);
        return EXIT_FAILURE;
    }

    double tempo_ms = diferenca_ms(inicio, fim);

    printf("Versao: pthreads\n");
    printf("Quantidade de threads: %d\n", quantidade_threads);
    printf("Quantidade de numeros: %d\n", quantidade);
    printf("Quantidade de primos: %d\n", quantidade_primos);
    printf("Tempo de execucao: %.6f ms\n", tempo_ms);

    free(numeros);
    free(threads);
    free(dados);

    return EXIT_SUCCESS;
}
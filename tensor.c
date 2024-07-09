#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 4
int *matriz_a, *matriz_b;
int *resultado;
int linhas_a, colunas_a, linhas_b, colunas_b;
int result_l, result_c;

int* alocar_matriz(int linhas, int colunas) {
    return (int*) malloc(linhas * colunas * sizeof(int));
}

void desalocar_matriz(int* matriz) {
    free(matriz);
}

void ler_matriz(const char* nome_arquivo, int** matriz, int* linhas, int* colunas) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        exit(EXIT_FAILURE);
    }

    char* linha = NULL;
    size_t len = 0;
    ssize_t nread;

    // matriz temporaria
    int* matriz_temp = NULL;
    *linhas = 0;
    *colunas = 0;

    while ((nread = getline(&linha, &len, arquivo)) != -1) {
        char* pos;
        //removendo os caracteres de nova linha
        if ((pos = strchr(linha, '\n')) != NULL) *pos = '\0';
        if ((pos = strchr(linha, '\r')) != NULL) *pos = '\0';

        if (strlen(linha) > 0) {
            // realocando matriz temporária para adicionar nova linha
            matriz_temp = realloc(matriz_temp, (*linhas + 1) * len * sizeof(int));
            if (!matriz_temp) exit(EXIT_FAILURE);
            char* token = strtok(linha, " ");
            int colunas_atual = 0;
            while (token) {
                matriz_temp[(*linhas) * (*colunas) + colunas_atual++] = atoi(token);
                token = strtok(NULL, " ");
            }
            if (*colunas == 0) *colunas = colunas_atual;
            (*linhas)++;
        }
    }

    free(linha);
    fclose(arquivo);

    *matriz = alocar_matriz(*linhas, *colunas);
    memcpy(*matriz, matriz_temp, (*linhas) * (*colunas) * sizeof(int));
    free(matriz_temp);
}

void salvar_result(const char* nome_arquivo, int* matriz, int linhas, int colunas) {
    FILE* arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {

        exit(EXIT_FAILURE);
    }

    //armazenando o número máximo de dígitos em cada coluna
    int max_digitos[colunas];
    for (int j = 0; j < colunas; ++j) {
        max_digitos[j] = 1;
        for (int i = 0; i < linhas; ++i) {
            int num_digitos = (matriz[i * colunas + j] == 0) ? 1 : 0;
            for (int num = matriz[i * colunas + j]; num != 0; num /= 10) {
                num_digitos++;
            }
            if (num_digitos > max_digitos[j]) {
                max_digitos[j] = num_digitos;
            }
        }
    }

    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            // add um espaço entre os números na mesma linha (menos antes do primeiro número) 
            if (j > 0) {
                fprintf(arquivo, " ");
            }
            fprintf(arquivo, "%*d", max_digitos[j], matriz[i * colunas + j]);
        }
        if (i < linhas - 1) {
            fprintf(arquivo, "\n");
        }
    }

    fclose(arquivo);
}

void* produto_tensorial(void* arg) {

    // id da thread
    int tid = *(int*)arg;

    // num de linhas que cada thread deve processar
    int qtd_linhas_por_thread = (linhas_a + NUM_THREADS - 1) / NUM_THREADS;
    int comeco = tid * qtd_linhas_por_thread;
    int fim = (tid + 1) * qtd_linhas_por_thread;

    //se o fim for maior que a qtd de linhas, fim é igual a linhas_a
    if (fim > linhas_a) fim = linhas_a;

    for (int i = comeco; i < fim; i++) {
        for (int j = 0; j < colunas_a; j++) {
            for (int k = 0; k < linhas_b; k++) {
                for (int l = 0; l < colunas_b; l++) {
                    resultado[(i * linhas_b + k) * colunas_a * colunas_b + (j * colunas_b + l)] = matriz_a[i * colunas_a + j] * matriz_b[k * colunas_b + l];
                }
            }
        }
    }
    return NULL;
}

void criar_threads(){
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    //criando as threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, produto_tensorial, &thread_ids[i]);
    }

    //esperando que  cada thread termine sua execução
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
}


void matriz_resultado() {
    result_l = linhas_a * linhas_b;
    result_c = colunas_a * colunas_b;
    resultado = alocar_matriz(result_l, result_c);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo da matriz_a> <arquivo da matriz_b>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ler_matriz(argv[1], &matriz_a, &linhas_a, &colunas_a);
    ler_matriz(argv[2], &matriz_b, &linhas_b, &colunas_b);

    //verificar o tamanho da matriz e alocar memória para ela
    matriz_resultado();

    //cria as threads que vao calcular o produto tensorial
    criar_threads();
    
    //salva a matriz de resultado no arquivo de saida
    salvar_result("tensor.out", resultado, result_l, result_c);
    
    desalocar_matriz(matriz_a);
    desalocar_matriz(matriz_b);
    desalocar_matriz(resultado);

    return EXIT_SUCCESS;
}

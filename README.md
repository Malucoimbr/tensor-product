# tensor-product

Este projeto implementa a operação de produto tensorial entre duas matrizes fornecidas em arquivos de entrada, produzindo um arquivo de saída com o resultado.

- Multithreading: A implementação deve usar no mínimo 2 threads em paralelo para o cálculo do produto tensorial. As threads devem estar associadas ao processamento da validação, e não às etapas de entrada e saída. Soluções que não cumpram este requisito serão descartadas.

# Arquivo de Saída:

O arquivo de saída será nomeado tensor.out

# Compilação

Para compilar o projeto, utilize o comando:

make
Isso irá gerar o executável tensor.

Execução
Para executar o programa, utilize o comando:

./tensor [namefile1] [namefile2]
onde [namefile1] e [namefile2] são os nomes dos arquivos contendo as matrizes de entrada.

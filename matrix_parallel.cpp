#include <iostream>
#include <random>
#include <fstream>
#include <time.h>
#include <string>
#include <omp.h>

/* Alocar vector<vector<int>>
de tamanho M x N  que pode ser acessado
como uma matriz normalmente alocada */
void alloc(int m, int n, std::vector<std::vector<int>> &mat){
    mat.resize(m);
    for (int i = 0; i < m; i++) {
        mat[i].resize(n);
    }
}

/*Recebe uma matriz por referência de tamanho
M x N e preenche com valores aleatoriamente
gerados com limites de [0, max[
*/
void random_matrix(int m, int n, std::vector<std::vector<int>> &mat, int max){
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            mat[i][j] = (rand() % (int)(max - 1));
        }
    }
}

/* Imprime a matriz recebida como parâmetro */
void print(int n, int m, std::vector<std::vector<int>> mat){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            std::cout << mat[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char const *argv[]) {

    srand(time(NULL));

    /* Alocação de Variáveis */
    int m, k, n, w, j, i;
    std::vector<std::vector<int>> mat1, mat2, mat;
    int aux = 0, chunks = 1;
    double start_omp, end_omp, wtick_omp, start_sin, end_sin, wtick_sin;

    /*Verifica os argumentos recebidos*/
    if (!argv[1] || !argv[2] || !argv[3] || !argv[4])
        std::cout << "Erro nos argumentos\n";
    else{
        /*Transforma o argumentos recebidos em inteiro*/
        m = atoi(argv[1]);
        k = atoi(argv[2]);
        n = atoi(argv[3]);

        /*Alocar as matrizes necessárias para o cálculo*/
        alloc(m, k, mat1);
        alloc(k, n, mat2);
        alloc(m, n, mat);

        /*Preenche as matrizes com números aleatórios*/
        random_matrix(m, k, mat1, 10);
        random_matrix(k, n, mat2, 10);
        random_matrix(m, n, mat, 0);
    }

    if(atoi(argv[4]) == 1){ //executa sequencial
        omp_set_num_threads(0);// mudar o número de threads para apenas 1(sequencial)
        omp_set_dynamic(0);
        omp_set_nested(0);
        start_sin = omp_get_wtime( );
        //calcula a multiplicação com apenas 1 thread sequencial
        #pragma omp parallel for num_threads(1) schedule(static, chunks) private(w) collapse(2)
        for (i = 0; i < m; ++i){
            for (j = 0; j < n; j++) {
                for (w = 0; w < k; w++) {
                    mat[i][j] += mat1[i][w] * mat2[w][j];
                } //fim for w
            }//fim for j
        } //fim for i
        end_sin = omp_get_wtime( );
        wtick_sin = omp_get_wtick( );

        //abre arquivo apenas para saida
        std::ofstream sin_time_mm ("mm_single_time.txt", std::ios::out);

        if(sin_time_mm.is_open()) { // escreve o tempo se arquivo estiver aberto
            sin_time_mm << "Tempo: " << (end_sin-start_sin)/wtick_sin << "(seg)\n";
            sin_time_mm.close();
        }
    }//fim if

    else if(atoi(argv[4]) == 0){ //executa parallel
        start_omp = omp_get_wtime( );

        /*  - #pragma omp parallel for espcifica para o omp que será paralelizado um for
            - num_threads(M*N) mudar o numero de threads para N*M
            - schedule(static., chunks) determina como as iterações do loop são divididas entre threads do time.
            static: iterações divididas em bloco de tamanho chunk, no caso 1 para 1.
            colocado como 1 para que cada valor da matriz resultado seja calculado por 1 thread
            - private(w) indica que a variável w será privada para cada thread em especifico
            - collapse(2) indica quantos loops em um aninhamento de loops (nested loops)
            devem ser agrupados (collapsed) em um bloco de iteração maior dividido de acordo com
            a cláusula schedule.
        */
        #pragma omp parallel for num_threads(m*n) schedule(static, chunks) private(w) collapse(2)
        for (i = 0; i < m; ++i){
            for (j = 0; j < n; j++) {
                for (w = 0; w < k; w++) {
                    mat[i][j] += mat1[i][w] * mat2[w][j];
                } //fim for w
            }//fim for j
        } //fim for i
        end_omp = omp_get_wtime( );
        wtick_omp = omp_get_wtick( );

        // abre aruqivo apenas para saida
        std::ofstream omp_time_mm ("omp_mm_time.txt", std::ios::out);

        if(omp_time_mm.is_open()) { // escreve o tempo se arquivo estiver aberto
            omp_time_mm << "Tempo: " << (end_omp-start_omp)/wtick_omp << "(seg)\n";
            omp_time_mm.close();
        }
    } //fim do else-if

    //abre arquivo apenas para saida
    std::ofstream res_mm("res_mm.txt", std::ios::out);


    if (res_mm.is_open()) {// se arquivo aberto
        //escreve a matriz resultado no arquivo aberto
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                res_mm << mat[i][j] << " ";
            }//fim for j
            res_mm << std::endl;
        }//fim for i
        res_mm.close();
    }

    return 0;
}

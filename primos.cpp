#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>

bool is_prime(int n){
    if(n < 2)
        return false;

    for (int i = 2; i < n; i++) {
        if ((n % i) == 0) {
            return false;
        }
    }

    return true;
}

int main(int argc, char const *argv[]) {
    int n = atoi(argv[1]), i;

    double start, end;

    bool *vet = new bool[n];

    start = omp_get_wtime();
    #pragma omp parallel for private(i) num_threads(atoi(argv[2])) collapse(1)
    for (i = 0; i < n; i++) {
        vet[i] = is_prime(i);
        #pragma omp critical
        {
            std::cout << "Thread :" << omp_get_thread_num() << "\t i: " << i << "\t n: " << n << std::endl;
        }
    }
    end = omp_get_wtime();

    std::ofstream primos_res ("primos_res.txt", std::ios::out);
    std::ofstream primos_time ("primos_time.txt", std::ios::out);

    for (int i = 0; i < n; i++)
        if (vet[i])
            primos_res << i << "\t";

    primos_time << "Tempo: " << (end - start) << " (seg)";

    primos_time.close();
    primos_res.close();

    return 0;
}

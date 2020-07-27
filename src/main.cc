#include <global.h>
#include <checkArgs.hpp>

uint64_t* arreglo = nullptr;

int main(int argc, char** argv){

	uint64_t totalElementos;
	uint32_t numThreads;
	uint32_t limInferior;
	uint32_t limSuperior;

	auto argumentos = (std::shared_ptr<checkArgs>) new checkArgs(argc, argv);

	totalElementos = argumentos->getArgs().tamProblema;
	numThreads = argumentos->getArgs().numThreads;
	limInferior = argumentos->getArgs().limInferior;
	limSuperior = argumentos->getArgs().limSuperior;

	std::cout << "Elementos: " << totalElementos << std::endl;
	std::cout << "Threads: " << numThreads << std::endl;
	std::cout << "Limite inferior: " << limInferior << std::endl;
	std::cout << "Limite superior: " << limSuperior << std::endl;

	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<> unif(limInferior,limSuperior);

	//Etapa de llenado

	//Secuencial
	arreglo = new uint64_t[totalElementos];

	auto start = std::chrono::high_resolution_clock::now();

	for(size_t i = 0; i < totalElementos; ++i){
		arreglo[i] = unif(rng);
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoLlenadoTotal_S = elapsed.count();

	delete[] arreglo;
	//Paralela
	arreglo = new uint64_t[totalElementos];

	start = std::chrono::high_resolution_clock::now();
	//Directiva de formato de OpenMP
	#pragma omp parallel for num_threads(numThreads)
	for(size_t i=0; i < totalElementos; ++i){
		arreglo[i] = unif(rng);
	}

	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
	auto tiempoLlenadoTotal_P = elapsed.count();

	//Etapa de suma

	//Secuencial
	uint64_t sumaSecuencial=0;

	start = std::chrono::high_resolution_clock::now();

	for(size_t i = 0; i < totalElementos; ++i){
		sumaSecuencial += arreglo[i];
	}

	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSuma_S = elapsed.count();

	//Paralela
	uint64_t sumaParalela=0;
	start = std::chrono::high_resolution_clock::now();

	//Directiva de formato de OpenMP
	#pragma omp parallel for reduction(+:sumaParalela) num_threads(numThreads)
	for(size_t i=0; i<totalElementos; ++i){
		sumaParalela += arreglo[i];
	}

	end     = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSuma_P = elapsed.count();

	//Etapa de Resultados

	std::cout << "Resultados suma: \n" << std::endl;
	std::cout << "Suma secuencial total: " << sumaSecuencial << std::endl;
	std::cout << "Suma en paralelo total: " << sumaParalela << "\n" << std::endl;
	std::cout << "Tiempos de ejecución módulo de llenado: \n" << std::endl;
	std::cout << "Tiempo Llenado Secuencial: " << tiempoLlenadoTotal_S << "[ms]" << std::endl;
	std::cout << "Tiempo Llenado Paralelo: " << tiempoLlenadoTotal_P << "[ms]\n" << std::endl;
	std::cout << "Tiempos de ejecución módulo de suma: \n" << std::endl;
	std::cout << "Tiempo Suma Secuencial: " << tiempoTotalSuma_S << "[ms]" << std::endl;
	std::cout << "Tiempo Suma Paralela: " << tiempoTotalSuma_P << "[ms]\n" << std::endl;

	return(EXIT_SUCCESS);
}


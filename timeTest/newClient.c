#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <stdlib.h>
//int limit = 80150;
//int limit = 2;
int limit = 20;

int getPrimes(){
	int num = 1,primes = 0;


#pragma omp parallel for schedule(dynamic) reduction(+ : primes)
	for (num = 1; num <= limit; num++) { 
		int i = 2; 
		while(i <= num) { 
			if(num % i == 0)
				break;
			i++; 
		}
		if(i == num)
			primes++;
	}
	return primes;
}

int main( int argc, char *argv[]) {
	double start, end;
	double runTime;
	long int  maxL =(long int) atof(argv[1]);
	start = omp_get_wtime();
	while(maxL-- > 0 ){
		getPrimes();
	}
	end = omp_get_wtime();
	runTime = end - start;
	printf("This machine calculated all prime numbers under %d in %g seconds\n",limit,runTime);

	return 0;
}

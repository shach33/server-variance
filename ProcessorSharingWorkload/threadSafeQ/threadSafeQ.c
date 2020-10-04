#include <pthread.h>
#include <stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<math.h>
#include<sys/time.h>
#include<stdlib.h>
#include <inttypes.h>
#include <omp.h>

#define DURATION 10000
#define RPS 100
#define MAX_THREADS 100
#define NUM_REQS 3*RPS*DURATION 
typedef unsigned long long ticks;

pthread_mutex_t access_q;
pthread_mutex_t access_qe;
pthread_mutex_t access_thrA;
pthread_cond_t qNotEmpty_cv;
int threadsActive = 0;

double reqs[NUM_REQS];
long int head= -1, tail = -1;
int actT = 0;
int qEmpty = 1;
int endProc = 0;
int limit = 10;
int client_sock;
struct thPar{
	int sock;
	int i;
}thPar;

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

int tmain(int loops){
	double start, end, runtime;
	int l = loops;
	char *arg = malloc(50);
	sprintf(arg, "/home/compassys/timeTest/./test %d", loops);
	start = omp_get_wtime();
	while(loops-->0){
		getPrimes();
//	int status = system(arg);
//		 execv("/home/compassys/timetest/test",arg);

	}
	end = omp_get_wtime();
	runtime = end - start;
//	printf("Lopps: %d, TIme: %g\n", l, runtime);
}

static inline ticks get_cycles()
{
	unsigned a, d;
	asm volatile("rdtsc" : "=a" (a), "=d" (d));

	return (((ticks)a) | (((ticks)d) << 32));
}

/*functionrun by all the threads */
void *inc_x(void *thParam)//void *x_void_ptr)
{
	struct thPar *thP = (struct thPar *)thParam;

	double serveTime;
	//int client_sock = thP->sock;//*((int *)sock);
	int thID = thP->i;
	int rcvd= 0;
	while(1){
		//	printf("In loop : %d\n", thID);

	//	if(!qEmpty){
			pthread_mutex_lock(&access_q);
			pthread_cond_wait(&qNotEmpty_cv, &access_q);
			//if(!qEmpty){
				pthread_mutex_lock(&access_thrA);
				threadsActive++;	
				pthread_mutex_unlock(&access_thrA);
				if(threadsActive ==MAX_THREADS){
					printf("All active: %d\n",++actT);	
				}
				if(head<tail){
				head = head+1;

				serveTime = reqs[head];
				//		printf("Thrd: %d\n", thID);
		                //printf("%d-- Removed: %f from head: %d, tail: %d\n" ,thID,serveTime, head, tail); 
			//	pthread_mutex_lock(&access_qe);
				if(head<tail)
					pthread_cond_signal(&qNotEmpty_cv);
		//			qEmpty = 1;
		/*		else
					qEmpty = 0;
		*/		pthread_mutex_unlock(&access_qe);
				rcvd = 1;
				}
			//}	
			pthread_mutex_unlock(&access_q);
	//	}
		//Call user function to get 100% cpu usage
		if(rcvd){
			tmain(serveTime);
			//Send reply on the socket
			//printf("Sending reply %d  tail: %d\n", head, tail );
			if(send(client_sock , "Done\0" , 5,0) < 0){
				puts("Reply not sent");
			}
		}
		pthread_mutex_lock(&access_thrA);
		threadsActive--;
		pthread_mutex_unlock(&access_thrA);
		rcvd = 0;
	}	
	return NULL;

}

int main()
{

	int x = 0, y = 0;
	int i = 0 , j = 0;
	pthread_t i_thread[MAX_THREADS];
	ticks c1, c2;
	int tsccost, tot, a;
	tot=0;
	struct timeval t1, t2;
	unsigned time = 0;
	double elapsedTime;
	double start, end, runTime;

	int num_measures = 1;
/*	for(i=0; i<num_measures; i++){
		//gettimeofday(&t1, NULL); 
		c1 = get_cycles();
		//gettimeofday(&t2, NULL); 
		while(j++<2000000)
			getPrimes();
		c2 = get_cycles();
		j=0;
		start = omp_get_wtime();
		while(j++<2000000)
			getPrimes();
		end = omp_get_wtime();

		tsccost=(int)(c2-c1);
		if(tsccost<0){
			printf("####  ERROR!!!   ");
		}

		time = (unsigned)((c2-c1)/2000000);
		printf("rdtsc took %d clock cycles in %uMS\n", tsccost, time);
		runTime = end-start;
		//    printf("This machine calculated all prime numbers under %d in %g seconds\n",limit,runTime);
		tot = tot+tsccost;
	}
*/


	int socket_desc , c , read_size;
	struct sockaddr_in server , client;
	struct timeval tv,tv2;
	float runtime;
	int usec;
	int req_id = 0;
	char client_message[15];
	int *arg = malloc(sizeof(*arg)); 
	struct thPar thPar1;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("10.22.17.36");
	server.sin_port = htons( 7921 );
	int enable = 1;
	if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		error("setsockopt(SO_REUSEADDR) failed");
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	i = 0;
	while(i<MAX_THREADS){
		thPar1.i=i;
		if(pthread_create(&i_thread[i], NULL, inc_x,(void*) &thPar1 )) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		else{
			//    printf("created %d \n",i);
		}
		i++;
	}

	//Listen
	listen(socket_desc , 3);

	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	*arg = client_sock;
	thPar1.sock = client_sock;

	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");
	//Receive a message from client
	while( (read_size = recv(client_sock , client_message , 8 , 0)) > 0 ){
		//gettimeofday(&tv, NULL);
		runtime = (float)atof(client_message);
		//pthread_mutex_lock(&access_q);
		tail++;
		reqs[tail] = runtime;
		//pthread_mutex_lock(&access_qe);
		//qEmpty = 0;
		pthread_cond_signal(&qNotEmpty_cv);
		//pthread_mutex_unlock(&access_qe);
	//	printf("Rcvd: %f---  %s--- %d\n", reqs[tail], client_message, tail);
		//printf("Rcvd: %s\n", client_message);
		//pthread_mutex_unlock(&access_q);

		if(read_size == 0){
			puts("Client disconnected");
			fflush(stdout);
		}
		else if(read_size == -1){
			perror("recv failed");
		}
	}
	return 0;
}




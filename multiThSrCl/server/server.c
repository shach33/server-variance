#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<stddef.h>
#include<sys/time.h>
#include<sched.h>
#include<signal.h>
#include<unistd.h>
#include<errno.h>
#include<omp.h>


#define NUMCPU 8

int stick2cpu=-2;
pthread_mutex_t mutex_cpu;
//the thread function
void *connection_handler(void *);
int limit = 10;

int stick_this_thread_to_core(int core_id) {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    //printf("Num cores: %d\n", num_cores);
    //if (core_id < 0 || core_id >= num_cores)
    if (core_id < 0 || core_id >= 12)
        return EINVAL;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    pthread_t current_thread = pthread_self();
    //printf("%d -- 4\n",current_thread);
    return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}


double rand_val(int seed){
    static long x;               // Random int value
    x = rand();
    //const long  a =      16807;  // Multiplier
    const long  a =      26807;  // Multiplier
    const long  m = 2147483647;  // Modulus
    const long  q =     127773;  // m div a
    const long  r =       2836;  // m mod a
    long        x_div_q;         // x divided by q
    long        x_mod_q;         // x modulo q
    long        x_new;           // New x value
    if (seed > 0){
        x = seed;
        return(0.0);
    }
    //printf("x: %ld\n" , x);
    x_div_q = x / q;
    x_mod_q = x % q;
    x_new = (a * x_mod_q) - (r * x_div_q);
    if (x_new > 0)
        x = x_new;
    else
        x = x_new + m;
    // Return a random value between 0.0 and 1.0
    return((double) x / m);
}

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
        start = omp_get_wtime();
        while(loops-->0){
                getPrimes();
        }
        end = omp_get_wtime();
        runtime = end - start;

//	printf("itme: %g\n", runtime);
}




int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 3000 );
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
    int enable = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed");
    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    c=sizeof(struct sockaddr_in);
    while(client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
    {
        //puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}
/*
   This will handle connection for each client
 */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int n,wait,loops;	
    struct timeval tv0,tv1;
    int my_cpu,ret=5 ;
    char    sendBuff[100], client_message[2000];

    pthread_mutex_lock(&mutex_cpu);
    stick2cpu = (stick2cpu+2)%NUMCPU;
    my_cpu = stick2cpu;
    pthread_mutex_unlock(&mutex_cpu);
	ret = stick_this_thread_to_core(my_cpu);
	printf("Core: %d ret: %d - %s\n",my_cpu, ret,strerror(ret));
    
	while((n=recv(sock,client_message,2000,0))>0){
	loops = 350;//50 us 	rand_val(0)*10000;
	//loops = 1400;//50 us 	rand_val(0)*10000;
	//loops = 1100;//100 us 	rand_val(0)*10000;
	//loops = 2200;//200 us 	rand_val(0)*10000;
	//loops = 5500;//500 us 	rand_val(0)*10000;
	//loops = 11000;//1000 us 	rand_val(0)*10000;
        gettimeofday(&tv0,NULL);
	tmain(loops);
        gettimeofday(&tv1,NULL);
        //printf("time: %d - %d\n",(int)((tv1.tv_sec*1000000 + (int)tv1.tv_usec)- (tv0.tv_sec*1000000 + (int)tv0.tv_usec)),sock);
/*      wait = rand_val(0)*10000;
        gettimeofday(&tv0,NULL);
        do{
            gettimeofday(&tv1,NULL);
        }while(((tv1.tv_sec*1000000 + (int)tv1.tv_usec)- (tv0.tv_sec*1000000 + (int)tv0.tv_usec))<wait);
*/
        //printf("Rcvd: %s wait: %d - %d\n",client_message, wait,sock);
        send(sock,client_message,n,0);
    }
    close(sock);

    if(n==0)
    {
        //puts("Client Disconnected ");
    }
    else
    {
        perror("recv failed");
    }
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_SIZE 50
#define NUM_CLIENT 5
#define DURATION 500
#define REQRATE 10	//50us
//#define REQRATE 2000	//100us
//#define REQRATE 1000	//200us
//#define REQRATE 400	//500us
//#define REQRATE 200	//1000us
#define IAT 1000000/REQRATE
void *connection_handler(void *socket_desc);

int main()
{
    int socket_desc , new_socket , c , *new_sock, i;
    pthread_t sniffer_thread;
    for (i=1; i<=NUM_CLIENT; i++) {
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) i) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        //sleep(3);
    }
    pthread_exit(NULL);
    return 0;
}

int gen_IAT(){
	


}
double rand_val(int seed){
    static long x;               // Random int value
    x = rand();
    const long  a =      16807;  // Multiplier
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
    // RNG using integer arithmetic
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

void *connection_handler(void *threadid)
{
    int threadnum = (int)threadid;
    int sock_desc;
    int sumResp=0;
    double avgResp;
    int numReq=0;
	struct sockaddr_in serv_addr;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    int wait;
    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Failed creating socket\n");
    struct timeval tv0,tv1,tv2,tv3, tv4,tv5;
    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.22.17.102");
    serv_addr.sin_port = htons(3000);

    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Failed to connect to server\n");
    }
    gettimeofday(&tv4,NULL);
    //gettimeofday(&tv5,NULL);
    tv5 = tv4;
    //printf("Connected successfully client:%d\n", threadnum);
    sprintf(sbuff, "ClntMsg--%d",threadnum);
    while((tv5.tv_sec-tv4.tv_sec)<DURATION )
    {
        //printf("tv5: %d, tv4: %d - thread: %d\n",tv5.tv_sec,tv4.tv_sec,threadnum);
        //printf("For thread : %d\n", threadnum);
        //fgets(sbuff, MAX_SIZE , stdin);
        send(sock_desc,sbuff,strlen(sbuff),0);
        gettimeofday(&tv2,NULL);
        
        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
            printf("Error");
        //else
        //    fputs(rbuff,stdout);
        gettimeofday(&tv3,NULL);
        numReq++;
        //printf("rcvd: %s: %d" , rbuff,threadnum);
        bzero(rbuff,MAX_SIZE);
        //wait = 50;//rr: 2000			rand_val(0)*1000;
        //wait = 25;//rr: 4000	rand_val(0)*1000;
        wait = (tv3.tv_sec*1000000 + (int)tv3.tv_usec)- (tv2.tv_sec*1000000 + (int)tv2.tv_usec);

	sumResp +=wait;// (tv3.tv_sec*1000000 + (int)tv3.tv_usec)- (tv2.tv_sec*1000000 + (int)tv2.tv_usec);
        //printf("resp: %d, wait: %d in thread: %d\n",(tv3.tv_sec*1000000 + (int)tv3.tv_usec)- (tv2.tv_sec*1000000 + (int)tv2.tv_usec), wait, threadnum);
        //printf("avg: %d, wait: %d in thread: %d\n",sumResp/numReq, wait, threadnum);
        do{
            gettimeofday(&tv5, NULL);
        }while((tv5.tv_sec*1000000 + (int)tv5.tv_usec) < ((tv4.tv_sec*1000000 + (int)tv4.tv_usec) + numReq*IAT));
	//printf("thread:%d:%d --- %d.%d\n",threadnum,numReq,tv5.tv_sec,tv5.tv_usec);
        //sleep(2);
    }
    //printf("thread: %d req: %d avg: %f\n" ,threadnum,numReq,((double)sumResp)/numReq);
    //printf("thread: %d avg: %f\n" ,threadnum,((double)sumResp)/numReq);
    printf("%f\n" ,((double)sumResp)/numReq);
    close(sock_desc);
    return 0;
}

/*
   C ECHO client example using sockets
 */
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<math.h>
#include<sys/time.h>
#include<limits.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include <pthread.h>
#define REQ_RATE 100
#define DURATION 100000
#define NUM_REQS REQ_RATE*DURATION*2
//#define NUM_REQS 10 //REQ_RATE*DURATION*2
#define IAT 1000000/REQ_RATE
#define NUMFILES 1
double exp_rv[NUM_REQS];
struct timeval recv_time[NUM_REQS];
double par_rv[NUM_REQS];
//double exp_time[] = {0,50000,150000,250000,350000,450000,550000,650000,750000,850000};

double exp_time[NUM_REQS];
//double serve_time[NUM_REQS];// = {ITRN,ITRN,ITRN,ITRN,ITRN,ITRN,ITRN,ITRN,ITRN,ITRN};

int serve_time[NUM_REQS];
long int resp_time_usec[NUM_REQS];
long long int recv_ptr=0, send_ptr=0;
struct timeval tv,tv2,tv3,tv4,tv5;
long long int sumResp = 0;
int iter[] ={1,10,100,200,300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000,2500,3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,8000,8500,9000,9500,10000,10500,11000,11500,12000,12500,13500,14500,15500,16500,17500,18500,19500,20500,21500,22500,23500,24500,25500,26500,27500,28500,29500,30500,40500,45000,50000,55000,60000,65000,70000,75000,80000,85000,90000,95000,100000,105000,110000,115000,120000,125000,130000,135000,140000,145000,150000,155000,165000,175000,185000,195000,205000,215000,225000,325000,425000,525000,625000,725000,825000,925000,1025000,1125000,1225000,1325000};

double duration[] = {0.000001,0.000004,0.00002,0.00004,0.00006,0.00008,0.0001,0.00012,0.00014,0.00015,0.00016,0.00018,0.00021,0.00023,0.00025,0.00027,0.00029,0.00030,0.00032,0.00033,0.00037,0.00039,0.00048,0.00057,0.00068,0.00074,0.00079,0.00089,0.001,0.00115,0.00118,0.00125,0.00128,0.0014,0.0015,0.0017,0.0018,0.0019,0.002,0.0021,0.0023,0.00235,0.0024,0.00245,0.0027,0.0028,0.0029,0.003,0.0031,0.0033,0.0035,0.00355,0.0036,0.0038,0.0041,0.0042,0.0045,0.0048,0.0049,0.0051,0.0052,0.006,0.0072,0.008,0.009,0.0096,0.0108,0.0112,0.0115,0.0125,0.0132,0.0135,0.014,0.0145,0.015,0.0155,0.0165,0.017,0.0175,0.018,0.0185,0.02,0.021,0.022,0.0225,0.023,0.025,0.0255,0.026,0.029,0.0295,0.03,0.04,0.05,0.06,0.08,0.13,0.25,0.42,0.6,0.75,0.85,1.06};

double rand_val(int seed)
{
    /*
       static long x;               // Random int value
    //const long  a =      16807;  // Multiplier
    const long  a =      14477;  // Multiplier
    //const long  m = 2147483647;  // Modulus

    //const long  m = 3267000013;  // Modulus
    const long  m = 2319131071;
    //const long  q =     127773;  // m div a
    //const long  r =       2836;  // m mod a
    const long  q =     194383;  // m div a
    const long  r =       4932;  // m mod a
    //const long  q =    (long)(m/a);  // m div a
    //const long  r =    (long)(m%a);  // m mod a
    static long y;               // Random int value
    long x_div_q;         // x divided by q
    long x_mod_q;         // x modulo q
    long x_new;           // New x value
    //194383--d: 4932--m:
    //printf("\nm: %ld--", m/a);
    //printf("d: %ld--", m%a);
    // Set the seed if argument is non-zero and then return zero
    //printf("%ld\n",x );
    //printf("1.: %ld -- %ld", m/a , (m-10)/a);
    //printf("2.: %d -- %d", m%a , (m-10)%a);*/
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

//===========================================================================
//=  Function to generate bounded Pareto distributed RVs using              =
//=    - Input:  a, k, and p                                                =
//=    - Output: Returns with bounded Pareto RV                             =
//===========================================================================
//double bpareto(double a, double k, double p)
double bpareto(double a, double k)
{
    double z;     // Uniform random number from 0 to 1
    double rv;    // RV to be returned

    // Pull a uniform RV (0 < z < 1)
    do{
        //        z = rand_val(0);
        z = rand();

        //printf("z: %f ---\n ", z);
    }while ((z == 0) || (z == 1));
    //printf("z: %f ---\n ", z);
    z = z/RAND_MAX;
    //printf("z: %f\n ", z);
    //  Generate the bounded Pareto rv using the inversion method
    //    rv = pow((pow(k, a) / (z*pow((k/p), a) - z + 1)), (1.0/a));
    //  rv = pow(k*(1-z),(-1.0/a));
    //   printf("rv: %f ---\n ", rv);
    rv = k / pow(z, (1.0 / a));
    //printf("k = %f, rv: %f ---\n ",k, rv);

    return(rv);
}

double expon(double x)
{
    double z;                     // Uniform random number (0 < z < 1)
    double exp_value;             // Computed exponential value to be returned
    do{
        z = rand_val(0);
    }while ((z == 0) || (z == 1));
    exp_value = -x * log(z);
    return(exp_value);
}

int sock;

//void *send_t(void *sock_i){
void *send_t(){
    char runtime[9];

    gettimeofday(&tv,NULL);
    gettimeofday(&tv4,NULL);
    printf("send_thead\n");
    while((tv4.tv_sec - tv.tv_sec) < DURATION){// && send_ptr < NUM_REQS){
   	 
	runtime[0] = '\0';
        snprintf(runtime, 9, "%8d", serve_time[send_ptr]);
        if( send(sock , runtime , strlen(runtime) , 0) < 0){
            puts("Send failed");
        }
        //printf("Sent: %lld --- %s --%f\n",tv4.tv_sec*1000000 + (int)tv4.tv_usec, runtime, exp_time[send_ptr]);
        //printf("Sent: %s\n",runtime);
	send_ptr++;
        
        while((tv4.tv_sec*1000000 + (int)tv4.tv_usec)< (tv.tv_sec*1000000 + (int)tv.tv_usec + exp_time[send_ptr])){
            gettimeofday(&tv4,NULL);
        }
	//printf("time elapsed: %d",(tv4.tv_sec - tv.tv_sec));

    }


}

//void *recv_t(void *sock_i){
void *recv_t(){
//    int sock = *((int*)sock_i);
    char server_reply[5];
    //while(recv_ptr != send_ptr || !recv_ptr){
    while(1){//recv_ptr != send_ptr || !recv_ptr){
        if( recv(sock , server_reply , 5 , 0) < 0){
            puts("recv failed");
            break;
        }
	//printf("Rcvd: %s\n", server_reply);
        gettimeofday(&tv3,NULL);
        //recv_time[recv_ptr] = tv3;
        resp_time_usec[recv_ptr] = (tv3.tv_sec*1000000 + (int)tv3.tv_usec ) - ((tv.tv_sec*1000000 + (int)tv.tv_usec + exp_time[recv_ptr]));
	sumResp+=resp_time_usec[recv_ptr];
	//printf("Rcvd:: %lld ptr: %lld ts: %lld\n",resp_time_usec[recv_ptr], recv_ptr, tv3.tv_sec*1000000 + (int)tv3.tv_usec  );
	//:printf("sent:: %ld, %f\n",tv.tv_sec*1000000 + (int)tv.tv_usec , exp_time[recv_ptr]);
	//printf("Avg: %f, recvd: %d, sent: %d\n",((double)sumResp)/((double)(recv_ptr+1)), recv_ptr, send_ptr);
	printf("Avg: %f\n",((double)sumResp)/((double)(recv_ptr+1)));
        recv_ptr++;
        // (tv4.tv_sec - tv3.tv_sec) * 1000000 + ((int)tv4.tv_usec - (int)tv3.tv_usec);
        //printf("\nresp[%ld] - %ld", ind, resp_time_usec[ind]);        
    }

}

int main(int argc , char *argv[])
{
    //int sock;
    struct sockaddr_in server;
    long int ind = 0;
    double alpha = 2.0001, par_k, par_p;
    double par_mean = 0.005;
    long int i=0,sum;
    FILE *fp, *fp2;
    char pFile[20], eFile[20];
    char fInd[5];
    pthread_t i_thread[2];
    //    system("rm -f IAT*");
    int fileNum = 0;
    int durn, j = 0;

    par_k = (par_mean*(alpha-1))/alpha;
    //par_p = INT_MAX;
    srand(time(NULL));
    printf("k: %f\n",par_k );
    printf("alpha: %f\n",alpha );

    for(ind=0;ind<NUM_REQS;ind++){
        //printf("1\n");
        exp_rv[ind] = expon(IAT);
        //par_rv[ind] = bpareto(alpha, par_k, par_p);
        par_rv[ind] = bpareto(alpha, par_k);
    }
    exp_time[0] = 0;
    for(ind=1;ind<NUM_REQS;ind++){
        exp_time[ind] = exp_time[ind-1] + exp_rv[ind];
	
    }

    int index = 0;
    while(fileNum < NUMFILES){
        i = 0;
        strcpy(pFile, "parNums");
        snprintf(fInd, 5, "%d",fileNum);
        strcat(pFile,fInd);
        strcat(pFile,".txt");
        strcpy(eFile, "IAT");
        strcat(eFile,fInd);
        strcat(eFile,".txt");
        //printf("\nOpening %s -- %s for %d",pFile, eFile, fileNum);
        fp = fopen(pFile, "w");
        fp2 = fopen(eFile, "w");
        //printf("\nOpened  %d -- %d for %d",(int)fp, (int)fp2, errno);
        i=0;
        while(i<((2*REQ_RATE*DURATION)/NUMFILES)){
            j = 1;
            ind = fileNum*REQ_RATE+i;
            durn = -1;
            while(j<103){
                if((par_rv[ind] >= duration[j-1]) && (par_rv[ind] <= duration[j])  ){
                    if((par_rv[ind] - duration[j-1]) > (duration[j] - par_rv[ind])){
                        durn = iter[j];
                    }
                    else{
                        durn = iter[j-1];
                    }
                    break;
                }
                j++;

            }
            if(durn == -1){
                durn = (par_rv[ind]/1.06)*1325000;
            }	
            serve_time[index] = durn;
            fprintf(fp, "%f -- %d-- %ld\n",par_rv[fileNum*REQ_RATE+i],durn, serve_time[index]);
            //fprintf(fp, "%d\n",durn);
            fprintf(fp2, "%f\n",exp_time[fileNum*REQ_RATE+i] );
            i++;
            index++;	
        }
        fileNum++;
        fclose(fp);
        fclose(fp2);
    }
    printf("\nStarting communication");
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("10.22.17.36");
    server.sin_family = AF_INET;
    server.sin_port = htons( 7921 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //if(pthread_create(&i_thread[0], NULL, send_t,(void *)sock )){
    if(pthread_create(&i_thread[0], NULL, send_t,NULL )){
        fprintf(stderr, "Send: Error creating thread\n");  
        return 1;
    }
    else
        printf("Send created \n");
    //if(pthread_create(&i_thread[1], NULL, recv_t,(void *)sock )){
    if(pthread_create(&i_thread[1], NULL, recv_t,NULL )){
        fprintf(stderr, "Recv: Error creating thread\n");  
        return 1;
    }
    else
        printf("Recv created \n");

    (void) pthread_join(i_thread[0], NULL);
    (void) pthread_join(i_thread[1], NULL);

    //keep communicating with server
    sum= 0;
    for(i=0;i<(send_ptr-1);i++){
        sum+=resp_time_usec[i];
        //printf("\nsum:%ld",sum);
    }
    //printf("\n%ld --sum:%ld, avg: %f", send_ptr,sum,(float)(sum/(send_ptr-1)));


    close(sock);
    return 0;
}

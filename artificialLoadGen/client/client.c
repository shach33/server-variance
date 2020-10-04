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
#define REQ_RATE 100
#define DURATION 100000
#define NUM_REQS REQ_RATE*DURATION*2
#define IAT 1000000/REQ_RATE
double exp_rv[NUM_REQS];
double exp_time[NUM_REQS];
double par_rv[NUM_REQS];
long int resp_time_usec[NUM_REQS];
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
        z = rand_val(0);
        //z = rand();
	
    	//printf("z: %f ---\n ", z);
    }while ((z == 0) || (z == 1));
    //printf("z: %f ---\n ", z);
    //z = z/RAND_MAX;
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
    // Pull a uniform random number (0 < z < 1)
	//printf("test\n");
    do{
        z = rand_val(0);
        //z = rand();
	//printf("%f\n",z);
    }while ((z == 0) || (z == 1));
	//z = z/RAND_MAX;
	//printf("%f\n",z);
	
    // Compute exponential random variable using inversion method
    exp_value = -x * log(z);
    return(exp_value);
}

 
int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    long int ind = 0;
    double alpha = 2.1, par_k, par_p;
    double par_mean = 0.005;
    struct timeval tv,tv2,tv3,tv4,tv5;
    char runtime[15],server_reply[15];
    long int i,sum;
	FILE *fp;

    fp = fopen("parNums.txt", "w");
    
    par_k = (par_mean*(alpha-1))/alpha;
    //par_p = INT_MAX;
    srand(time(NULL));
	printf("par_k : %f\n", par_k);

    for(ind=0;ind<NUM_REQS;ind++){
    	//printf("1\n");
        exp_rv[ind] = expon(IAT);
        //par_rv[ind] = bpareto(alpha, par_k, par_p);
        par_rv[ind] = bpareto(alpha, par_k);
        fprintf(fp, "%f\n",par_rv[ind]);
    	//printf("mean: %f\n", par_mean);
        //printf("exp: %f, par: %f\n", exp_rv[ind], par_rv[ind]);
    }
    exp_time[0] = 0;
    for(ind=1;ind<NUM_REQS;ind++){
        exp_time[ind] = exp_time[ind-1] + exp_rv[ind];        
	}
    printf("\nStarting communication"); 
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 7921 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //keep communicating with server
    ind = 0;
    gettimeofday(&tv,NULL);
    gettimeofday(&tv4,NULL);
    while((tv4.tv_sec - tv.tv_sec) < DURATION)
    {
        snprintf(runtime, 15, "%f", par_rv[ind]);
        if( send(sock , runtime , strlen(runtime) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

         
        //Receive a reply from the server
        if( recv(sock , server_reply , 15 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
        gettimeofday(&tv4,NULL);
        resp_time_usec[ind] = (tv4.tv_sec*1000000 + (int)tv4.tv_usec ) - ((tv.tv_sec*1000000 + (int)tv.tv_usec + exp_time[ind]));// (tv4.tv_sec - tv3.tv_sec) * 1000000 + ((int)tv4.tv_usec - (int)tv3.tv_usec);
	//printf("\nresp[%ld] - %ld", ind, resp_time_usec[ind]);	
	    ind++;
	
        while((tv4.tv_sec*1000000 + (int)tv4.tv_usec)< (tv.tv_sec*1000000 + (int)tv.tv_usec + exp_time[ind])){
            gettimeofday(&tv4,NULL);
        }
	
    }
    sum= 0;
    for(i=0;i<(ind-1);i++){
        sum+=resp_time_usec[i];
	//printf("\nsum:%ld",sum);
    }
    printf("\n%ld --sum:%ld, avg: %f", ind,sum,(float)(sum/(ind-1)));
    
     
    close(sock);
    return 0;
}

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
#define REQ_RATE 10000
#define DURATION 10
#define NUM_REQS REQ_RATE*DURATION*2
#define IAT 1000000/REQ_RATE
#define NUMFILES 10
double exp_rv[NUM_REQS];
double exp_time[NUM_REQS];
double par_rv[NUM_REQS];
double serve_time[NUM_REQS];
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
	// Pull a uniform random number (0 < z < 1)
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
	long int i=0,sum;
	FILE *fp, *fp2;
	char pFile[20], eFile[20];
	char fInd[5];
	//    system("rm -f IAT*");
	int fileNum = 0;
	//Find the closest number of iterations for a request's service time	
	int iter[] ={1,10,100,200,300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000,2500,3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,8000,8500,9000,9500,10000,10500,11000,11500,12000,12500,13500,14500,15500,16500,17500,18500,19500,20500,21500,22500,23500,24500,25500,26500,27500,28500,29500,30500,40500,45000,50000,55000,60000,65000,70000,75000,80000,85000,90000,95000,100000,105000,110000,115000,120000,125000,130000,135000,140000,145000,150000,155000,165000,175000,185000,195000,205000,215000,225000,325000,425000,525000,625000,725000,825000,925000,1025000,1125000,1225000,1325000,1425000,1525000,1625000,1725000,1825000,1950000,2050000,2150000};

	//Duration of service time
	double duration[] = {0.000001,0.000006,0.00005,0.0001,0.000155,0.00021,0.00026,0.00031,0.00037,0.00042,0.00047,0.00052,0.00058,0.00063,0.00068,0.00073,0.00079,0.00084,0.00089,0.00095,0.001,0.00105,0.0013,0.0016,0.0019,0.0021,0.0024,0.0026,0.0029,0.0032,0.0034,0.0037,0.004,0.0042,0.0045,0.0048,0.005,0.0053,0.0055,0.0058,0.006,0.0063,0.0066,0.0072,0.0075,0.0079,0.0083,0.0088,0.0092,0.0097,0.0103,0.0109,0.01104,0.0116,0.012,0.0125,0.013,0.0133,0.014,0.0141,0.015,0.019,0.021,0.024,0.026,0.028,0.031,0.033,0.035,0.038,0.04,0.043,0.045,0.048,0.0495,0.052,0.054,0.057,0.059,0.062,0.064,0.066,0.068,0.071,0.073,0.076,0.082,0.087,0.092,0.097,0.101,0.106,0.153,0.201,0.248,0.294,0.34,0.388,0.434,0.48,0.529,0.574,0.620,0.67,0.71,0.76,0.81,0.856,0.91,0.96,1.0081};
	int durn, j = 0;

	par_k = (par_mean*(alpha-1))/alpha;
	//par_p = INT_MAX;
	srand(time(NULL));
	printf("k: %f\n",par_k );
	printf("alpha: %f\n",alpha );

	//Pre-generate interarrival time and servie times for requests
	for(ind=0;ind<NUM_REQS;ind++){
		//printf("1\n");
		exp_rv[ind] = expon(IAT);
		//par_rv[ind] = bpareto(alpha, par_k, par_p);
		par_rv[ind] = bpareto(alpha, par_k);
		//printf("mean: %f\n", par_mean);
		//printf("%d--- exp: %f, par: %f\n", ind,exp_rv[ind], par_rv[ind]);
	}
	exp_time[0] = 0;
	//Calculate time w.r.t first request 
	for(ind=1;ind<NUM_REQS;ind++){
		exp_time[ind] = exp_time[ind-1] + exp_rv[ind];
		//fprintf(fp2, "%f ---- %f\n",exp_time[ind], exp_rv[ind]);
	}
	int index = 0;
	//Write arrival time to text file
	//Convert service time to number of cycles required at server
	while(fileNum < NUMFILES){
		i = 0;
		strcpy(pFile, "parNums");
		snprintf(fInd, 5, "%d",fileNum);
		strcat(pFile,fInd);
		strcat(pFile,".txt");
		strcpy(eFile, "IAT");
		strcat(eFile,fInd);
		strcat(eFile,".txt");
		printf("\nOpening %s -- %s for %d",pFile, eFile, fileNum);
		fp = fopen(pFile, "w");
		fp2 = fopen(eFile, "w");
		printf("\nOpened  %d -- %d for %d",(int)fp, (int)fp2, errno);
		i=0;
		while(i<((2*REQ_RATE*DURATION)/NUMFILES)){
			j = 1;
			ind = fileNum*REQ_RATE+i;
			durn = -1;
			while(j<111){
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
				durn = (par_rv[ind]/1.0081)*2100000;
			}	
			serve_time[index++] = durn;
			//fprintf(fp, "%f -- %d\n",par_rv[fileNum*REQ_RATE+i],durn);
			fprintf(fp, "%d\n",durn);
			fprintf(fp2, "%f\n",exp_time[fileNum*REQ_RATE+i]/1000);
			i++;
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
		runtime[0] = '\0';
		snprintf(runtime, 15, "%f", serve_time[ind]);
		if( send(sock , runtime , strlen(runtime) , 0) < 0)
		{
			puts("Send failed");
			return 1;
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
	}
	printf("\n%ld --sum:%ld, avg: %f", ind,sum,(float)(sum/(ind-1)));


	close(sock);
	return 0;
}

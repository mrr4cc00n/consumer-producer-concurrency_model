#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>       
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <poll.h>

struct ProductionInfo
{
   char warehouse_ip[16];
   int warehouse_port;
   int wareid;
};

struct product {
    char provider_id[3];     // identificador del productor
    int  product_id;         // numero secuencial del productor
    char id[3];              // tipo de producto generado
    char product_data[4084]; // esto es solo para que todos los productos tengan 4096 bytes de tamaño 
};

#define Max_Size 1024
struct ProductionInfo producer_info[Max_Size];
sem_t sem;
int count_port=0;
struct product producer_production [Max_Size];
int count_types = 0; 
int max_product[Max_Size];

void * ConsumeFromWarehouse(void * args);

void take_entry(int argc, char const *argv[])
{
    // ip:port, type:production
    if(argc < 2)
    {
        printf("Argument NULL Exception \n");
        exit(-1);
    }
    char * ip_producer;
    int port_producer;
    if(argc==2)
    {   
        char * aux = (char *)argv[1];
        ip_producer = strtok(aux,":");
        port_producer= atoi(strtok(NULL, ":"));
        producer_info[0].warehouse_port= port_producer;      
        sprintf( producer_info[0].warehouse_ip,"%s",ip_producer);
        count_port++;   
    }
    else if(argc > 2)
    {       
        int products_total;
        char * name_type;
        for (int i = 0; i < argc-1; ++i)
        {
            char * aux = (char *)argv[1+i];

            if(( products_total= atoi(argv[1+i]))==0)
            {             
                name_type = strtok(aux,":");
                products_total= atoi(strtok(NULL, ":"));
                max_product[count_types] = products_total;
                sprintf(producer_production[count_types].id,"%s",name_type);
                count_types++;
            }
            else
            {
                ip_producer = strtok(aux,":");
                port_producer= atoi(strtok(NULL, ":"));
                producer_info[count_port].warehouse_port= port_producer;     
                sprintf( producer_info[count_port].warehouse_ip,"%s",ip_producer);
                count_port++;           
            }
            
        }
    }
}

int main(int argc, char const *argv[])
{
    take_entry(argc,argv);
    
	pthread_t thread[count_port];
    sem_init(&sem,0,1);

    for (int i = 0; i < count_port; i++)
    {
    	producer_info[i].wareid = i;
    	pthread_create(&thread[i],NULL,ConsumeFromWarehouse,&producer_info[i]);
    }

    for (int i = 0; i < count_port; i++)
       pthread_join(thread[i],NULL);

}

void * ConsumeFromWarehouse(void * args)
{
	struct ProductionInfo * a = (struct ProductionInfo *)args;

	int socketid = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(a->warehouse_port);
	inet_aton(a->warehouse_ip, &client_addr.sin_addr);
	
	if(connect(socketid,(struct sockaddr *) &client_addr,sizeof(client_addr)) < 0)
		pthread_exit(0);

	write(socketid,"Consumer",8);
    
	while(1)
	{
        sem_wait(&sem);
		for (int i = 0; i < count_types; i++)
		   for (int j = 0; j < max_product[i]; j++)	
    		{          	
                write(socketid,&producer_production[i].id,3);
                
                char m[4096];

                bzero(&m,sizeof(m));
                struct pollfd set[1];
                set[0].fd = socketid;
                set[0].events = POLLIN;
                set[0].revents = 0;

                poll(set,1,-1);

                if((set[0].revents & POLLNVAL) || (set[0].revents & POLLHUP) || (set[0].revents & POLLERR))
                {
                   sem_post(&sem);
                   pthread_exit(0);
                }

                read(socketid,&m,sizeof(struct product));

                struct product * a = (struct product *)m;

                if(strncmp(m,"UNAVAILABLE",11) == 0)
                    continue;

                printf("%s %s %d %s\n",a->id,a->provider_id,a->product_id,a->product_data);
            }
        printf("--------------------------------------------------------\n");    
        sem_post(&sem);
        sleep(5);
	}
}

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
int id;
sem_t sem;
struct ProductionInfo producer_info[Max_Size];
int count_port=0;
struct product producer_production [Max_Size];
int count_types = 0; 
int max_product[Max_Size];
int production_persecond[Max_Size];
int products_finished = 0;
char provider_id[3];
int share_persecond = 0;

void * ProduceToWarehouse(void * args);

void take_entry(int argc, char const *argv[])
{
    // provider_id , ip:port, type:production
    if(argc < 3)
    {
        printf("Argument NULL Exception \n");
        exit(-1);
    }
    char * ip_producer;
    int port_producer;
    if(argc==3)
    {   
        char * aux = (char *)argv[2];
        ip_producer = strtok(aux,":");
        port_producer= atoi(strtok(NULL, ":"));
        producer_info[0].warehouse_port= port_producer;      
        sprintf( producer_info[0].warehouse_ip,"%s",ip_producer);
        count_port++;   
    }
    else if(argc > 3)
    {       
        int products_total;
        char * name_type;
        for (int i = 0; i < argc-2; ++i)
        {
            char * aux = (char *)argv[2+i];

            if(( products_total= atoi(argv[2+i]))==0)
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
    bzero(provider_id,sizeof(provider_id));
    sprintf(provider_id,"%s",(char *)argv[1]);
}

int main(int argc, char const *argv[])
{
    take_entry(argc,argv);

	pthread_t thread[count_port];
    id = 1;
    sem_init(&sem,0,1);

    for (int i = 0; i < count_types; i++)
        production_persecond[i] = max_product[i];

    for (int i = 0; i < count_port; i++)
    {
    	producer_info[i].wareid = i;
    	pthread_create(&thread[i],NULL,ProduceToWarehouse,&producer_info[i]);
    }

    for (int i = 0; i < count_port; i++)
       pthread_join(thread[i],NULL);
}

void * ProduceToWarehouse(void * args)
{
	struct ProductionInfo * a = (struct ProductionInfo *)args;

	int socketid = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(a->warehouse_port);
	inet_aton(a->warehouse_ip, &client_addr.sin_addr);
	
	if(connect(socketid,(struct sockaddr *) &client_addr,sizeof(client_addr)) < 0)
		pthread_exit(0);

	write(socketid,"Producer",8);

	while(1)
	{
        sem_wait(&sem);
		for (int i = 0; i < count_types; i++)
		   for (int j = 0, limit = production_persecond[i]; j < limit; j++)	
    		{	
                struct product m;
                m.product_id = id;
                bzero(m.id,3);
                sprintf(m.id,"%s",producer_production[i].id);
                bzero(m.provider_id,3);
                sprintf(m.provider_id,"%s",provider_id);
                bzero(m.product_data,4084);
                
    			write(socketid,&m,4096);

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

                char t[2];
                bzero(t,sizeof(t));
                read(socketid,&t,2);

                if(strncmp(t,"NO",2)==0)
                    continue;

                printf("%s----------------%d\n",producer_production[i].id,m.product_id);
           
                id++;  
                
                production_persecond[i]--;

                if(production_persecond[i] == 0)
                    products_finished++;
    		}

        printf("--------------------------------------------------------\n");        
        share_persecond++;
        if(products_finished == count_types)
        {
            for (int i = 0; i < count_types; i++)
               production_persecond[i] = max_product[i];
            
            products_finished = 0;
            share_persecond = 0;
        }
        
        if(products_finished == 0)
            sleep(5);
        
        if(share_persecond == count_port)
        {
            for (int i = 0; i < count_types; i++)
               production_persecond[i] = max_product[i];
            share_persecond = 0;
            products_finished = 0;
        }
        
        sem_post(&sem);
        
        if(count_port!=1)
            sleep(1);                     
    }
}
#include <sys/socket.h> 
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <poll.h>

//definiciones
	#define Max_Size 1024
	int MountServer(int portno);
	int WarehouseManager(int socketfd);
	void* Producer(void * args);
	void* Consumer(void* args);
//end

sem_t mutex;
//struct's
	struct ServiceInfo
	{
		int clientfd;
	};

	struct product {
	    char provider_id[3];     // identificador del productor
	    int  product_id;         // numero secuencial del productor
	    char id[3];    // tipo de producto generado
	    char product_data[4084];  // esto es solo para que todos los productos tengan 4096 bytes de tamaño 
	};

	struct myprod{
		struct product * n;
		struct myprod* next;
	};
//end
//variables globales
	int error;
	int total_limit = 10000;
	int lenght_products=0;
	int count_types=0;
	struct product warehouse_product[Max_Size];
	int max_product[Max_Size];
	int actual_products[Max_Size];
	struct myprod* products;
//end

//metodos para linkedlist

	void print_product(struct myprod* element){
	printf("%s  %s  %s  %d\n",element->n->id,element->n->provider_id,element->n->product_data,element->n->product_id);
		return;
	}

	//agrega un producto a la linkedlist
	int add_element(struct product* element){
		
		struct myprod* current=(struct myprod*)malloc(sizeof(struct myprod));
		bzero(current,sizeof(struct myprod));
		if(lenght_products==0){	
			products =(struct myprod*)malloc(sizeof(struct myprod));
	   		bzero(products,sizeof(struct myprod));		
			current->n = element;
			current->next = NULL;
			products=current;
			lenght_products+=1;
			return 0;
		}

		current = products;
		struct myprod* tosave = (struct myprod*)malloc(sizeof(struct myprod));
        bzero(tosave,sizeof(struct myprod));
		tosave->n = element;

		while(current->next!=NULL){
			if(strncmp(current->n->id,element->id,3)==0 && strncmp(((current->next)->n)->id,element->id,3)!=0){
				tosave->next = current->next;
				current->next=tosave;
				lenght_products+=1;
				return 0;
			}
			
			current=current->next;
		}
		tosave->next=NULL;
		current->next = tosave;
		lenght_products+=1;
		return 0;
	}

	//extrae un producto de la linkedlist
	struct product* extract_element(char* element){
		struct myprod* first =(struct myprod*)malloc(sizeof(struct myprod));
		struct myprod* current =(struct myprod*)malloc(sizeof(struct myprod));
		struct myprod* last =(struct myprod*)malloc(sizeof(struct myprod));
		struct product* result;
		first=products;
		current=products;
		if(lenght_products<=0)
			return NULL;
		if(strcmp(products->n->id,element)==0){
			result=products->n;
			first=products->next;
			lenght_products-=1;
			products=first;
			return result;
		}

		while(current->next!=NULL){
			if(strcmp((current->next)->n->id,element)==0){
				products=(current->next)->next;
				result=current->next->n;
				current->next=products;
				lenght_products-=1;
				products=first;
				return result;
			}
			last=current;
			current=current->next;
		}

		if(strcmp(current->n->id,element)==0){
			result=current->n;
			last->next=NULL;
			lenght_products-=1;
			products=first;			
			return result;
		}
		

		return NULL;
	}
//end

int print_warehouse(){

	if(lenght_products==0){
		printf("%s\n","Empty");
		return 0;
	}
	struct myprod* temp=products;

	while(temp!=NULL){
		print_product(temp);
		temp=temp->next;
	}

	printf("--------------------------%d\n",total_limit);
	return 0;
}
int print_status()
{
    struct myprod* current =(struct myprod*)malloc(sizeof(struct myprod));
    int cant_perprod = 1;
    current = products;
    
    printf("Prod   Cant\n");
    while(current->next != NULL)
    {
    	if(strncmp(current->n->id,current->next->n->id,3)==0)
    		cant_perprod++;
    	else
    	{
    		printf("%s : %d\n",current->n->id,cant_perprod);
    		cant_perprod = 1;    		
    	}
    	current = current->next;
    }
    printf("%s : %d\n\n",current->n->id,cant_perprod);

    printf("Capacidad: %d\n",total_limit);
    printf("Ocupado : %d\n\n",lenght_products);
    printf("------------------------------------\n");
}

void take_entry(int argc, char const *argv[])
{
	// port, capacity, type: capacity
	if(argc < 3)
	{
		printf("Argument NULL Exception \n");
		exit(-1);
	}
	int products_total;
	char * name_type;
	if(argc==3)
	{
		if(( products_total = atoi(argv[2])) == 0)
			{
				count_types++;
				char * aux = (char *)argv[2];
				name_type = strtok(aux,":");
				products_total= atoi(strtok(NULL, ":"));
				max_product[0] = products_total;
				sprintf( warehouse_product[0].id,"%s",name_type);
			}
		else
		{
			total_limit= atoi(argv[2]);
		}
	}
	else if(argc>3)
	{		
		for (int i = 0; i < argc-2; ++i)
		{
			if(( products_total= atoi(argv[2+i]))==0)
			{
				char * aux = (char *)argv[2+i];
				name_type = strtok(aux,":");
				products_total = atoi(strtok(NULL, ":"));
		        max_product[count_types] = products_total;
				sprintf( warehouse_product[count_types].id,"%s",name_type);
				count_types++;
			}
			else
			{
				total_limit= atoi(argv[2+i]);
			}
		}
	}
}

int main(int argc, char const *argv[])
{
   	take_entry(argc,argv);

   	sem_init(&mutex,0,1);

    MountServer(atoi(argv[1]));
	return 0;
}

int MountServer(int portno)
{	
	printf("Loading...\n");
	int acceptfdtemp,error;
	//Open the socket for conections
	int listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd<0){
		printf("Error open the socket\n");
		exit(-1);
	}
	/*Set on the socket the options for mount the server*/
	int optval=1;
	error=setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void*)&optval,sizeof(optval));
	if(error<0){
		printf("Error mount_server\n");
		exit(-1);
	}
	//declare the necessary structs
	struct sockaddr_in serveraddr;
	struct sockaddr_in client;
	//fill the server struct
	bzero((char*)&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);
	//doing bind
	error=bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(error<0){
		printf("Error doing bind\n");
		exit(-1);
	}

	error=listen(listenfd,1);
	if(error<0){
		printf("Error on listen\n");
		exit(-1);
	}
	//attending clients
    printf("Waiting For Connections...\n");

	WarehouseManager(listenfd);
}

int WarehouseManager(int socket)
{
	struct sockaddr_in client_addr;
    int size;

    while(1)
    {
       int fd = accept(socket,(struct sockaddr *)&client_addr,&size);
         
        if(fd < 0)
           printf("UNABLE TO ESTABLISH AN ENTERING CONNECTION\n");
         else
            {
            	char buf[10];
            	struct ServiceInfo a;
            	a.clientfd = fd;
            	pthread_t thread;
		    	bzero(&buf,sizeof(buf));
		    	int prod = 0;
		    	
		    	if(read(fd,&buf,8) > 0)
		    	{
		          if(strncmp(buf,"Producer",8) == 0)
				    pthread_create(&thread,NULL,Producer,&a);  
		          else if(strncmp(buf,"Consumer",8) == 0)
				    pthread_create(&thread,NULL,Consumer,&a);    
		          else
		          	close(fd);	            	
	            }
                else
            	  close(fd);
            }
    }
}

void * Producer(void* args)
{
   struct ServiceInfo* a = (struct ServiceInfo *)args;
   int clientfd = a->clientfd;
   
   while(1)
   {
   	 int error = 0;
     struct product * m =(struct product*)malloc(sizeof(struct product));
   	 bzero(m,sizeof(struct product));
     struct pollfd set[1];
     set[0].fd = clientfd;
     set[0].events = POLLIN;
     set[0].revents = 0;

     poll(set,1,-1);

     if((set[0].revents & POLLNVAL) || (set[0].revents & POLLHUP) || (set[0].revents & POLLERR))
     	pthread_exit(0);

     sem_wait(&mutex);
     
     read(clientfd,m,sizeof(struct product));

     int pos = -1;
     
     for (int i = 0; i < count_types; i++)
        if(strncmp(m->id,warehouse_product[i].id,3) == 0)
       	  pos = i;

     if(lenght_products == total_limit)
     {
        write(clientfd,"NO",2);
        error = 1;
     } 

     if(pos!=-1 && error == 0)
     {
        if(actual_products[pos] >= max_product[pos])
        {
        	write(clientfd,"NO",2);
            error = 1;
        }
        else
          actual_products[pos]++;
     }

     if(error == 0)
     {
	    add_element(m);        
		print_status();/////////////////////print_warehouse();   DEPENDE

	    write(clientfd,"OK",2);
     }

     sem_post(&mutex);
   }
}

void* Consumer(void* args)
{
   struct ServiceInfo* a = (struct ServiceInfo *)args;
   int clientfd = a->clientfd;

   while(1)
   {
   	 struct product* result =(struct product*)malloc(sizeof(struct product));
   	 int error = 0;
	 bzero(result,sizeof(struct product));

     char m[3];
   	 bzero(&m,sizeof(m));
     struct pollfd set[1];
     set[0].fd = clientfd;
     set[0].events = POLLIN;
     set[0].revents = 0;

     poll(set,1,-1);

     if((set[0].revents & POLLNVAL) || (set[0].revents & POLLHUP) || (set[0].revents & POLLERR))
     	pthread_exit(0);

	 sem_wait(&mutex);
     
     read(clientfd,&m,sizeof(m));
     int pos = -1;
    
     for (int i = 0; i < count_types; i++)
        if(strncmp(m,warehouse_product[i].id,3) == 0)
        	pos = i;

	 result=extract_element((char*)m);
	 if(result == NULL)
		error = 1;
	
     if(error == 0)
     {
	    if(pos!=-1 && actual_products[pos] > 0)
          actual_products[pos]--;

		print_status();/////////////////////print_warehouse();   DEPENDE
		printf("%s------%s--------%d \n",result->id, result->provider_id, result->product_id ); 
	 }
     
     if(error == 0)
       write(clientfd,result,sizeof(struct product));
     else
       write(clientfd,"UNAVAILABLE",11);	

     free(result);

	 sem_post(&mutex);
    }
}
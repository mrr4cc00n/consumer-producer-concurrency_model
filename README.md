
Project Orientation (2017)
========================

The objective of this project is to simulate the problem of the "producer-consumer". The task at hand is to create a set of warehouses, suppliers and customers and run a simulation with it. For this, the following considerations will be taken into account:

Each team will implement 3 programs: a * warehouse *, a * producer *, and a * consumer *.

About the warehouse
----------------

* The store is a program (server) that listens on a port and allows connections from several clients. (3pt)

* The * warehouse * has a limited capacity of items that you may have in your inventory at any given time. (3pt)

~~~ bash
$ gcc warehouse.c -o warehouse
$ ./warehouse 3000 50 # port = 3000 capacity = 50
~~~

* When the server receives a new connection, the first thing it will receive will be a "producer" or a "consumer" string. (3pt)

* The warehouse must be able to connect several "producers" and also several "consumers". (3pt)

* The warehouse every time it receives a new product, or every time a consumer consumes a product, must display on the screen a current status of the warehouse (ie Total quantity of products, how many products of each type, available capacity. Etc.) (3pt)

* The warehouse may also have a maximum limit for each type of product that can be generated. This will be specified when creating the warehouse through the parameters (4pt)

~~~ bash
$ gcc warehouse.c -o warehouse
$ ./warehouse 3000 50 # port = 3000 capacity = 50
~~~

* When the server receives a new connection, the first thing it will receive will be a "producer" or a "consumer" string. (3pt)

* The warehouse must be able to connect several "producers" and also several "consumers". (3pt)

* The warehouse every time it receives a new product, or every time a consumer consumes a product, must display on the screen a current status of the warehouse (ie Total quantity of products, how many products of each type, available capacity. Etc.) (3pt)

* The warehouse may also have a maximum limit for each type of product that can be generated. This will be specified when creating the warehouse through the parameters (4pt)

~~~ bash
$ gcc warehouse.c -o warehouse
$ ./warehouse 3000 50 # this means you can store a maximum of 50 items (of any type)
$ ./warehouse 3000 A: 20 B: 30 # a maximum of 20 products of type "A" and a maximum of 30 of type "B"
$ ./warehouse 3000 50 A: 20 # maximum total is 50, but only 20 of type "A" (of type B can be 50)
~~~

* The entire system can be programmed, at least the part related to synchronization when writing to the data structure implemented in the warehouse, with threads. And also the part of waiting when the buffer is full (at the time of writing) or when it is empty (at the time of reading) (5pt)

* There is a way to implement the task without using threads, but using them gives 1 additional point (the 5th point)

* You can have more than one warehouse running simultaneously (6pt)

About the provider
-------------------

* The * producer * is a program that generates new products and that connects to the warehouse so that it can save them. (3pt)

* The producer program at the moment the connection begins sends the string "producer" to the warehouse to identify itself (3pt)

* Each team must implement a simple producer that generates a product * every 1 second * and sends it to the warehouse (3pt)

* If the warehouse cannot accept new products (because it is full) the producer must stop production. (3pt)

* The producer must print the products that he is generating through the console (at the time he generates them) (3pt)

* Each producer will have an identifier (3 characters) that will be part of all the products they generate. (3pt)

* Each generated product will also have an identifier that will be a sequential integer (i.e. the first product is 1, the second is 2 and so on) (3pt)

* At the time of execution, the producer receives as a parameter the IP and port where the warehouse is located in the form of ip: port (3pt)

~~~ bash
$ gcc producer.c -o producer
$ ./producer 10.6.122.44:3000
~~~

* The producer can produce more than one type of product. This information will be specified when creating a producer as part of the parameters. (4pt)

* In addition to the types of products that can be generated, you will also receive as a parameter an integer number that means the number of products of that type that are generated per time interval. (every 1 second) (4pt)

~~~ bash
$ gcc producer.c -o producer
$ ./producer A: 1 B: 2 10.6.122.44:3000 # one product of type "A" every 1 second and two products of type "B"
~~~

* The producer can connect to more than one warehouse and produce to all warehouses simultaneously (6pt)

* The previous requirements are maintained, so if a producer generates a product per second, and is connected to 2 warehouses, then at most, each warehouse will receive a product every 2 seconds. (6pt)

* The producer will receive as parameters all the warehouses to which he is going to connect. (6pt)

~~~ bash
$ gcc producer.c -o producer
$ ./producer A: 1 B: 2 10.6.122.44:3000 10.6.122.44:4000 10.6.122.45:3500 # 3 stores
~~~

* Each team of students will decide the strategy to distribute the products among the different warehouses (6pt)

About the products
-------------------

* the product will be an instance of the following c structure

~~~ c
struct product {
    char provider_id [3]; // producer identifier
    int product_id; // sequential number of the producer
    char product_type [3]; // type of generated product
    char product_data [4084] // this is just so that all products are 4096 bytes in size
}
~~~

========================


Orientacion del Proyecto
========================

El objetivo de este proyecto es simular el problema del "productor-consumidor". La tarea en cuestión consite en crear un conjunto de almacenes, proveedores y clientes y realizar una simulación con ello. Para eso se tendrán en cuenta las siguentes consideraciones:

Cada equipo implementará 3 programas: un *almacén*, un *productor*, y un *consumidor*.

Sobre el almacén
----------------

* El almacén es un programa (servidor) que escucha por un puerto y permite conexiones de varios clientes. (3pt)

* El *almacén* tiene una capacidad limitada de objetos que puede tener en un momento dado en su inventario. (3pt)

~~~bash
$ gcc warehouse.c -o warehouse
$ ./warehouse 3000 50 # port=3000 capacity=50
~~~

* Cuando el servidor recibe una nueva conexión lo primero que recibirá será un string "producer" o un "consumer". (3pt)

* El almacén debe poder que varios "productores" se conecten y también varios "consumidores". (3pt)

* El almacén cada vez que reciba un producto nuevo, o cada vez que un consumidor consuma un producto, debe visualizar en la pantalla un estado actual del almacén (i.e. Cantidad total de productos, cuantos productos de cada tipo, capacidad disponible. etc.) (3pt)

* El almacén puede además tener un limite máximo para cada tipo de producto que se puede generar. Esto será especificado en el momento de la creación del almacén a través de los parámetros (4pt)

~~~bash
$ gcc warehouse.c -o warehouse
$ ./warehouse 3000 50 # esto significa que puede almacenar un maximo de 50 elementos (de cualquier tipo)
$ ./warehouse 3000 A:20 B:30 # un máximo de 20 productos de tipo "A" y un máximo de 30 de tipo "B"
$ ./warehouse 3000 50 A:20 # máximo total es 50, pero solo 20 de tipo "A" (de tipo B pueden ser 50)
~~~

* Se puede programar todo el sistema, al menos la parte relacionada con la sincronización a la hora de escribir en la estructura de datos que se implemente en el almacén, con hilos. Y también la parte de la espera cuando el buffer esté lleno (a la hora de escribir) o cuando esté vacío (a la hora de leer) (5pt)

* Hay manera de implementar la tarea sin usar hilos, pero usarlos da 1 punto adicional (el 5to punto)

* Se pueden tener más de un almacén ejecutandose simultáneamente (6pt)

Sobre el proveedor
-------------------

* El *productor* es un programa que genera nuevos productos y que se conecta al almacén para que este los guarde. (3pt)

* El programa productor en el momento que comienza la conexión envia el string "producer" hacia el almacén para identificarse (3pt)

* Cada equipo debe implementar un productor simple que genera un producto *cada 1 segundo* y lo envia hacia el almacén (3pt)

* Si el almacén no puede aceptar nuevos productos (porque está lleno) el productor debe parar la producción. (3pt) 

* El productor debe imprimir por la consola los productos que va generando (en el momento que los genera) (3pt)

* Cada productor tendrá un identificador (de 3 caracteres) que formará parte de todos los productos que genera. (3pt) 

* Cada producto generado tambien tendra un identificador que será un número entero secuencial (i.e. el primero producto es el 1, el segundo es el 2 y asi sucesivamente) (3pt)

* En el momento de la ejecución el productor recibe como parámetro el IP y puerto donde está el almacén de forma ip:puerto (3pt)

~~~bash
$ gcc producer.c -o producer
$ ./producer 10.6.122.44:3000
~~~

* El productor puede producir más de un tipo de producto. Esta información se especificará a la hora de la creación de un productor como parte de los parámetros. (4pt)

* Además de los tipos de productos que se pueden generar, también recibirá como parámetro un número entero que significa la cantidad de productos de ese tipo que se generan por intervalo de tiempo. (cada 1 segundo) (4pt)

~~~bash
$ gcc producer.c -o producer
$ ./producer A:1 B:2 10.6.122.44:3000 # un producto de tipo "A" cada 1 segundo y dos producto de tipo "B"
~~~

* El productor puede conectarse a más de un almacén y producir hacia todos los almacenes simultáneamente (6pt)

* Los requerimientos anteriores se mantienen, asi que si un productor genera un producto por segundo, y está conectado a 2 almacenes, entonces como máximo, cada almacén recibirá un producto cada 2 segundos. (6pt)

* El productor recibirá como parámetros todos los almacenes a los que el se va a conectar. (6pt)

~~~bash
$ gcc producer.c -o producer
$ ./producer A:1 B:2 10.6.122.44:3000 10.6.122.44:4000 10.6.122.45:3500 # 3 almacenes
~~~

* Cada equipo de estudiantes decidirá la estrategia para repartir los productos entre los diferentes almacenes (6pt) 

Sobre el consumidor
-------------------

* El *consumidor* es un programa que consume productos y que se conecta al almacén para que este se los provea. (3pt)

* El programa consumidor en el momento que comienza la conexión envia el string "consumer" hacia el almacén para identificarse (3pt)

* Cada equipo debe implementar un consumidor simple que consume un producto *cada 1 segundo* desde el almacén (3pt)

* Si el almacén no puede generar productos (porque está vacío) el consumidor debe quedarse esperando a que haya disponibilidad. (3pt) 

* El consumidor debe imprimir por la consola los productos que va consumiento (en el momento que los consume) (3pt)

* En el momento de la ejecución el consumidor recibe como parámetro el IP y puerto donde está el almacén de forma ip:puerto (3pt)

~~~bash
$ gcc consumer.c -o consumer
$ ./consumer 10.6.122.44:3000
~~~

* El consumidor puede especificar la cantidad de productos que necesita en cada intervalo de tiempo y además debe poder especificar los tipos de productos que necesita (4pt)

~~~bash
$ gcc consumer.c -o consumer
$ ./consumer A:2 B:3 10.6.122.44:3000 # cada 1 segundo este consumidos necesita 2 productos de tipo "A" y 3 productos de tipo "B"
~~~

* Cada programa consumidor puede conectarse a más de un almacén (6pt)

* En la terminal, en el momento de la creación, se le pasará como parámetro todos los ip:puerto de todos los almacenes a los que el se conectará. (6pt)

~~~bash
$ gcc consumer.c -o consumer
$ ./consumer A:2 B:3 10.6.122.44:3000 10.6.122.45:4000 # 2 almacenes
~~~

* Cada equipo decidirá la mejor estrategia para consumir productos desde diferentes almacenes (6pt)

Sobre los productos
-------------------

* el producto será una instancia de la siguiente estructura de c

~~~c
struct product {
    char provider_id[3];     // identificador del productor
    int  product_id;         // numero secuencial del productor
    char product_type[3];    // tipo de producto generado
    char product_data[4084]  // esto es solo para que todos los productos tengan 4096 bytes de tamaño 
}
~~~ 


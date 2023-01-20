#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>

/*Escribir un programa que obtenga todas las posibles direcciones con las que se podría crear un socket asociado a un host dado como primer argumento del programa. Para cada dirección, mostrar la IP numérica, la familia de protocolos y tipo de socket. Comprobar el resultado para:
Una dirección IPv4 válida (ej. “147.96.1.9”).
Una dirección IPv6 válida (ej. “fd00::a:0:0:0:1”).
Un nombre de dominio válido (ej. “www.google.com”).
Un nombre en /etc/hosts válido (ej. “localhost”).
Una dirección o nombre incorrectos en cualquiera de los casos anteriores.

*/
int main(int argc, char**argv){

    if(argc<2){
        printf("Error, faltan argumentos");
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo* aux;
    struct sockaddr *socket;

    memset(&hints,0,sizeof(struct addrinfo));//inicializa a 0 el struct
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=0;
    hints.ai_flags=AI_PASSIVE;

    int erroradd;

    if((erroradd=getaddrinfo(argv[1],NULL,&hints,&res))!=0){
        printf("%s.\n",gai_strerror(erroradd));
    }
    else{
        int error;
        for(aux=res;aux!=NULL;aux=aux->ai_next){
            char buff[NI_MAXHOST]="";
            error=getnameinfo(aux->ai_addr,aux->ai_addrlen,buff,sizeof(buff),NULL,0,NI_NUMERICHOST);
            printf("%s     %i     %i\n",buff,aux->ai_protocol,aux->ai_socktype);
        }
    }
    freeaddrinfo(res);
    return 0;
}

/*Escribir un servidor UDP de hora de forma que:
La dirección y el puerto son el primer y segundo argumento del programa. Las direcciones pueden expresarse en cualquier formato (nombre de host, notación de punto…). Además, el servidor debe funcionar con direcciones IPv4 e IPv6 .
El servidor recibirá un comando (codificado en un carácter), de forma que ‘t’ devuelva la hora, ‘d’ devuelve la fecha y ‘q’ termina el proceso servidor.
En cada mensaje el servidor debe imprimir el nombre y puerto del cliente, usar getnameinfo(3).
Probar el funcionamiento del servidor con la herramienta Netcat (comando nc o ncat) como cliente.
*/

int main(int argc, char**argv){

    if(argc<3){
        printf("Error, faltan argumentos\n");
        return -1;
    }

    int sfd;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *aux;
    struct sockaddr_storage peer_addr;
    socklen_t peeraddr_len;

    memset(&hints,0,sizeof(hints));//inicializa a 0 el struct 
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;
    
    int erroradd;

    if((erroradd=getaddrinfo(argv[1],argv[2],&hints,&res))!=0){
        printf("%s.\n",gai_strerror(erroradd));
    }
    else{
        for(aux=res;aux!=NULL;aux=aux->ai_next){
            sfd=socket(aux->ai_family,aux->ai_socktype,aux->ai_protocol);
            if(sfd==-1)continue;
            if(bind(sfd,aux->ai_addr,aux->ai_addrlen)!=-1) break;
            close(sfd);
        }

        freeaddrinfo(res);

        //configuracion del server
        char comando[2];
        int error;
        ssize_t lectura;

        while(1){
            peeraddr_len=sizeof(peer_addr);
            lectura=recvfrom(sfd,comando,2,0,(struct sockaddr *) &peer_addr,&peeraddr_len);
            comando[1]='\0';
            if(lectura==-1)continue;
            char service[NI_MAXSERV]="";
            char host[NI_MAXHOST]="";
            error=getnameinfo((struct sockaddr *) &peer_addr,peeraddr_len,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICSERV|NI_NUMERICHOST);
            printf("%i bytes de %s:%s\n",lectura,host,service);

            time_t tiempo = time(NULL);
            struct tm *tmp=localtime(&tiempo);
            char h[150];

            if(comando[0]=='t'){
                size_t bt=strftime(h,sizeof(h,"%I:%M:%S %p",tmp);
                h[bt]='\0';
                sendto(sfd,h,bt,0,(struct sockaddr *) &peer_addr,peeraddr_len);
            }else if(comando[0]=='d'){
                size_t bt=strftime(h,sizeof(h),"%F",tmp);
                h[bt]='\0';
                sendto(sfd,h,bt,0,(struct sockaddr *) &peer_addr,peeraddr_len);
            }else if(comando[0]=='q'){
                printf("Saliendo\n");
                exit(0);
            }else{
                printf("Comando no soportado\n");
                
            }
        }
    }
    return 0;
}

/*Escribir el cliente para el servidor de hora. El cliente recibirá como argumentos la dirección del servidor, el puerto del servidor y el comando. Por ejemplo, para solicitar la hora, ./time_client 192.128.0.1 3000 t.*/

int main(int argc, char**argv){
    if(argc<3){
        printf("Error, faltan argumentos\n");
    return -1;
    }

    int sfd;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *aux;
    struct sockaddr_storage peer_addr;
    socklen_t peeraddr_len;

    memset(&hints,0,sizeof(hints));//inicializa a 0 el struct
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;
    int erroradd;

    if((erroradd=getaddrinfo(argv[1],argv[2],&hints,&res))!=0){
        printf("%s.\n",gai_strerror(erroradd));
    }
    else{
        for(aux=res;aux!=NULL;aux=aux->ai_next){
            sfd=socket(aux->ai_family,aux->ai_socktype,aux->ai_protocol);
            if(sfd==-1)continue;
            if(connect(sfd,aux->ai_addr,aux->ai_addrlen)!=-1) break;
            close(sfd);
        }
        
        freeaddrinfo(res);
        //configuracion del server
        char comando[150];
        int error;
        ssize_t lectura;
        peeraddr_len=sizeof(peer_addr);
        sendto(sfd,argv[3],2,0,res->ai_addr,res->ai_addrlen);
        printf("%s\n",argv[3]);
        time_t tiempo = time(NULL);
        struct tm *tmp=localtime(&tiempo);
        size_t max;
        char h[50];

       if(*argv[3]=='t'){
            size_t bt=recvfrom(sfd,h,sizeof(h),0,(struct sockaddr *) &peer_addr,&peeraddr_len);
            h[bt]='\0';
            printf("%s\n",h);
            bt=strftime(comando,sizeof(comando),"%I:%M:%S %p",tmp);
            comando[bt]='\0';
            printf("%s\n",comando);
        }else if(*argv[3]=='d'){
            size_t bt=recvfrom(sfd,h,sizeof(h),0,(struct sockaddr *) &peer_addr,&peeraddr_len);
            h[bt]='\0';
            printf("%s\n",h);
            bt=strftime(comando,sizeof(comando),"%F",tmp);
            comando[bt]='\0';
            printf("%s\n",comando);
        }
    }
    return 0;
}

/*Modificar el servidor para que, además de poder recibir comandos por red, los pueda recibir directamente por el terminal, leyendo dos caracteres (el comando y ‘\n’) de la entrada estándar. Multiplexar el uso de ambos canales usando select(2).
*/

int main(int argc, char**argv){
    if(argc<2){
        printf("Error, faltan argumentos\n");
    return -1;
    }

    int sfd;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *aux;
    struct sockaddr_storage peer_addr;
    socklen_t peeraddr_len;
    fd_set lec;
    int cambio = -1;

    memset(&hints,0,sizeof(hints));//inicializa a 0 el struct 
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;

    int erroradd;

    if((erroradd=getaddrinfo(argv[1],argv[2],&hints,&res))!=0){
        printf("%s.\n",gai_strerror(erroradd));
    }
    else{
        sfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
        bind(sfd,res->ai_addr,res->ai_addrlen);
            
        
        freeaddrinfo(res);
        //configuracion del server
        char comando[2];
        int error;
        ssize_t lectura;
        peeraddr_len=sizeof(peer_addr);
        char service[NI_MAXSERV];
        char host[NI_MAXHOST];

        for(;;){

            while(cambio == -1){
                FD_ZERO(&lec);
                FD_SET(sfd,&lec);
                FD_SET(0,&lec);
                cambio = select(sfd+1,&lec,NULL,NULL,NULL);
            }

            time_t tiempo = time(NULL);
            struct tm *tmp=localtime(&tiempo);
            size_t max;
            char h[150];
            
            if(FD_ISSET(sfd,&lec)){
                lectura=recvfrom(sfd,comando,2,0,(struct sockaddr *) &peer_addr,&peeraddr_len);
                comando[1]='\0';
                getnameinfo((struct sockaddr *) &peer_addr,peeraddr_len,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICSERV|NI_NUMERICHOST);
                printf("Red %i bytes de %s:%s\n",lectura,host,service);


                if(comando[0]=='t'){
                    size_t bt=strftime(h,sizeof(h),"%I:%M:%S %p",tmp);
                    h[bt]='\0';
                    sendto(sfd,h,bt,0,(struct sockaddr *) &peer_addr,peeraddr_len);
                }else if(comando[0]=='d'){
                    size_t bt=strftime(h,sizeof(h),"%F",tmp);
                    h[bt]='\0';
                    sendto(sfd,h,bt,0,(struct sockaddr *) &peer_addr,peeraddr_len);
                }else if(comando[0]=='q'){
                    printf("Saliendo\n");
                    exit(0);
                }else{
                    printf("Comando no soportado\n");
                    
                }
            }
            else{
                read(0,comando,2);
                comando[1]='\0';
                printf("Consola %i bytes y comando %s\n",2,comando);
                if(comando[0]=='t'){
                    size_t bt=strftime(h,sizeof(h),"%I:%M:%S %p",tmp);
                    h[bt]='\0';
                    printf("%s\n",h);
                }else if(comando[0]=='d'){
                    size_t bt=strftime(h,sizeof(h),"%F",tmp);
                    h[bt]='\0';
                    printf("%s\n",h);
                }else if(comando[0]=='q'){
                    printf("Saliendo\n");
                    exit(0);
                }else{
                    printf("Comando no soportado\n");
                    
                }
            }
        }
    }
    return 0;
}

/*Convertir el servidor UDP en multi-proceso siguiendo el patrón pre-fork. Una vez asociado el socket a la dirección local con bind(2), crear varios procesos que llamen a recvfrom(2) de forma que cada uno atenderá un mensaje de forma concurrente. Imprimir el PID del proceso servidor para comprobarlo. Para terminar el servidor, enviar la señal SIGTERM al grupo de procesos.*/

int main(int argc, char**argv){
    if(argc<2){
        printf("Error, faltan argumentos\n");
        return -1;
    }

    int sfd;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *aux;
    struct sockaddr_storage peer_addr;
    socklen_t peeraddr_len;
    fd_set lec;
    int cambio = -1;

    memset(&hints,0,sizeof(hints));//inicializa a 0 el struct 
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;

    int erroradd;

    if((erroradd=getaddrinfo(argv[1],argv[2],&hints,&res))!=0){
        printf("%s.\n",gai_strerror(erroradd));
    }
    else{
        sfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
        bind(sfd,res->ai_addr,res->ai_addrlen);
        freeaddrinfo(res);
        int status;
        int i=0;
        for(i;i<2;i++){
            pid_t pid;
            pid=fork();
            if(pid==0){
                char comando[2];
                int error;
                ssize_t lectura;
                peeraddr_len=sizeof(peer_addr);
                char service[NI_MAXSERV];
                char host[NI_MAXHOST];

                while(1){
                    time_t tiempo = time(NULL);
                    struct tm *tmp=localtime(&tiempo);
                    size_t max;
                    char h[150];
                    printf("PID del server: %i\n",getpid());
                    lectura=recvfrom(sfd,comando,2,0,(struct sockaddr *) &peer_addr,&peeraddr_len);
                    comando[1]='\0';
                    getnameinfo((struct sockaddr *) &peer_addr,peeraddr_len,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICSERV|NI_NUMERICHOST);
                    printf("Red %i bytes de %s:%s\n",lectura,host,service);
                    if(comando[0]=='t'){
                        size_t bt=strftime(h,sizeof(h),"%I:%M:%S %p",tmp);
                        h[bt]='\0';
                        sendto(sfd,h,bt,0,(struct sockaddr *) &peer_addr,peeraddr_len);
                    }
                    else if(comando[0]=='d'){
                        size_t bt=strftime(h,sizeof(h),"%F",tmp);
                        h[bt]='\0';
                        sendto(sfd,h,bt,0,(struct sockaddr *) &peer_addr,peeraddr_len);
                    }
                    else if(comando[0]=='q'){
                        printf("Saliendo\n");
                        exit(0);
                    }
                    else{
                        printf("Comando no soportado\n");
                        
                    }
                }
            }
        }
        wait(&status);
    }
    return 0;
}





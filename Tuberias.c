/*Escribir un programa que emule el comportamiento de la shell en la ejecución de una sentencia en la forma: comando1 argumento1 | comando2 argumento2. El programa creará una tubería sin nombre y creará un hijo:
El proceso padre redireccionará la salida estándar al extremo de escritura de la tubería y ejecutará comando1 argumento1.
El proceso hijo redireccionará la entrada estándar al extremo de lectura de la tubería y ejecutará comando2 argumento2.
Probar el funcionamiento con una sentencia similar a: ./ejercicio1 echo 12345 wc -c*/

int main(int agc, char *argv){
    int fd[2];
    int t = pipe(fd);
    pid_t hijo;
    hijo = fork();

    if(hijo==0){
        dup2(0,fd[0]);
        close(fd[1]);
        close(fd[0]);
        execlp(argv[3],argv[3],argv[4],(char)NULL);
    }
    else if(hijo==-1){
        printf("ERROR de hijo\n");
    }
    else{
        dup2(1,fd[1]);
        close(fd[1]);
        close(fd[0]);
        execlp(argv[1],argv[1],argv[2],(char *)NULL);
    }
    return 0;
}

/*Para la comunicación bi-direccional, es necesario crear dos tuberías, una para cada sentido: p_h y h_p. Escribir un programa que implemente el mecanismo de sincronización de parada y espera:
El padre leerá de la entrada estándar (terminal) y enviará el mensaje al proceso hijo, escribiéndolo en la tubería p_h. Entonces permanecerá bloqueado esperando la confirmación por parte del hijo en la otra tubería, h_p.
El hijo leerá de la tubería p_h, escribirá el mensaje por la salida estándar y esperará 1 segundo. Entonces, enviará el carácter ‘l’ al proceso padre, escribiéndolo en la tubería h_p, para indicar que está listo. Después de 10 mensajes enviará el carácter ‘q’ para indicar al padre que finalice.*/

int main(int agc, char **argv){
    int p_h[2],h_p[2];
    pipe(p_h);
    pipe(h_p);
    pid_t hijo;
    hijo = fork();
    char mP[256];
    char fl;
    char m[256];

    if(hijo==0){
        close(p_h[1]);
        close(h_p[0]);
        int i;
        for(i=0; i < 10;i++){
            ssize_t b=read(p_h[0],mP,255);
            mP[b]='\0';
            printf("HIJO ha recibido %s\n",mP);
            sleep(1);
            if(i==9){
                fl = 'q';
            }
            write(h_p[1],&fl,1);
        }

    }
    else if(hijo==-1){
        printf("ERROR de hijo\n");
    }
    else{
        close(p_h[0]);
        close(h_p[1]);
        while(fl != 'q'){
            printf("PADRE envia mensaje:\n");
            ssize_t b=read(0,m,255);
            m[b]='\0';
            write(p_h[1],m,b);
            read(h_p[0],&fl,1);
        }
        wait(NULL);
        printf("Mensajes enviados: 10\n");
    }
    return 0;
}

/*Escribir un programa que abra la tubería con el nombre anterior en modo sólo escritura, y escriba en ella el primer argumento del programa. En otro terminal, leer de la tubería usando un comando adecuado.*/

int main(int argc, char **argv){

    if(argc < 2){
        perror("Argumentos insuficintes\n");
        return -1;
    }
    mkfifo("tuberia",0777);
    int t = open("tuberia",O_WRONLY);
    write(t,argv[1],strlen(argv[1]));
    close(t);

    return 0;
}

/*Crear otra tubería con nombre. Escribir un programa que espere hasta que haya datos listos para leer en alguna de ellas. El programa debe mostrar la tubería desde la que leyó y los datos leídos. Consideraciones: 
Para optimizar las operaciones de lectura usar un buffer (ej. de 256 bytes).
Usar read(2) para leer de la tubería y gestionar adecuadamente la longitud de los datos leídos.
Normalmente, la apertura de la tubería para lectura se bloqueará hasta que se abra para escritura (ej. con echo 1 > tuberia). Para evitarlo, usar la opción O_NONBLOCK en open(2).
Cuando el escritor termina y cierra la tubería, read(2) devolverá 0, indicando el fin de fichero, por lo que hay que cerrar la tubería y volver a abrirla. Si no, select(2) considerará el descriptor siempre listo para lectura y no se bloqueará. 
*/

int main(){

    char *tuberia = "tuberia";
    char *tuberia2 = "tuberia2";
    mkfifo(tuberia,0644);
    mkfifo(tuberia2,0644);

    int t1 = open(tuberia, O_RDONLY | O_NONBLOCK);
    int t2 = open(tuberia2, O_RDONLY | O_NONBLOCK);

    char buffer[257]; 

    int sel,num;

    while(sel != -1){

        fd_set new;
        FD_ZERO(&new); 
        FD_SET(t1,&new); 
        FD_SET(t2,&new);

        int actual;

        int p1,p2;

        
        if(t1 > t2){
            p1 = t1+1;
            sel = select(p1,&new,NULL,NULL,NULL);
        }
        else if(t2 > t1){
            p2 = t2+1;
            sel = select(p2,&new,NULL,NULL,NULL);
        }

        if(sel > 0){

            
            if(FD_ISSET(t1,&new)){
                actual = t1;
                num=1;
            }
            else if(FD_ISSET(t2,&new)){
                actual = t2;
                num=2;
            }

            ssize_t b=256;
            while(b==256){
                b=read(actual,buffer,256);
                buffer[b]='\0';
                printf("Lectura de tuberia %i: %s\n",num,buffer);
            }

            if(b != 256 && num == 1){
                close(t1);
                int t1 = open(tuberia, O_RDONLY | O_NONBLOCK);
                if(t1 < 0){
                    perror("No se puede abrir la primera tuberia\n");
                    close(t1);
                    close(t2);
                    return -1;
                }
            }
            else if(b != 256 && num == 2){
                close(t2);
                int t2 = open(tuberia2, O_RDONLY | O_NONBLOCK);
                if(t2 < 0){
                    perror("No se puede abrir la segunda tuberia\n");
                    close(t1);
                    close(t2);
                    return -1;
                }
            }
        } 
    }
    close(t1);
    close(t2); 
}
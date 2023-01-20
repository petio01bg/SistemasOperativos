#include<sched.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/resource.h>


/*Escribir un programa que muestre la política de planificación (como cadena) y la prioridad del proceso actual, además de mostrar los valores máximo y mínimo de la prioridad para la política de planificación. */

int main(){
    pid_t pid;
    pid=getpid();
    int prio;
    prio=sched_getscheduler(pid);
    printf("Politica de planificacion: ");
    if(prio==SCHED_FIFO){
        printf("FIFO\n");
    }
    else if (prio==SCHED_RR){
        printf("Round Robin\n");
    }
    else if (prio==SCHED_OTHER){
        printf("Other\n");
    }
    else{
        printf("error\n");
    }
    struct sched_param st;
    sched_getparam(pid,&st);
    printf("Prioridad: %i\n",st.sched_priority);
    printf("Max: %i\n", sched_get_priority_max(prio));
    printf("Min: %i\n", sched_get_priority_min(prio));
    return 0;
}

/* Escribir un programa que muestre los identificadores del proceso (PID, PPID, PGID y SID), el número máximo de ficheros que puede abrir y su directorio de trabajo actual.*/

int main(){
    pid_t pid;
    pid=getpid();
    printf("PID: %i, PPID; %i, GPID: %i, SID: %i\n", pid, getppid(), getpgid(pid), getsid(pid));
    struct rlimit limit;
    getrlimit(RLIMIT_NOFILE,&limit);
    printf("Max ficheros abiertos: %li\n", limit.rlim_max);
    char * cwd;
    cwd=malloc(256);
    getcwd(cwd,256);
    printf("Directorio actual: %s\n", cwd);
    free(cwd);
    return 0;
}

/*Un demonio es un proceso que se ejecuta en segundo plano para proporcionar un servicio. Normalmente, un demonio está en su propia sesión y grupo. Para garantizar que es posible crear la sesión y el grupo, el demonio crea un nuevo proceso para crear la nueva sesión y ejecutar la lógica del servicio. Escribir una plantilla de demonio (creación del nuevo proceso y de la sesión) en el que únicamente se muestren los atributos del proceso (como en el ejercicio anterior). Además, fijar el directorio de trabajo del demonio a /tmp.
*/

int main(){
    pid_t dem;
    dem=fork();
    if(dem==0){
        setsid();
        pid_t pid;
        pid=getpid();
        printf("PID: %i, PPID; %i, GPID: %i, SID: %i\n", pid, getppid(), getpgid(pid), getsid(pid));
        chdir("/temp");
    }
    else{
        sleep(20);
    }
    return 0;
}

/*
Escribir dos versiones, una con system(3) y otra con execvp(3), de un programa que ejecute otro programa que se pasará como argumento por línea de comandos.*/

int main(int argc, char ** argv){
   if(execvp(argv[1],argv+1)==-1){
       printf("Error");
   }
   printf("El comando terminó de ejecutarse\n");
    return 0;
}

int main(int argc, char ** argv){
    int longitud =1;
    int i;
    for(i=1;i<argc;i++){
        longitud+=strlen(argv[i])+1;
    }
    char * argumento;
    argumento=malloc(sizeof(char) * longitud);
    strcpy(argumento,"");
    for(i=1;i<argc;i++){
        strcpy(argumento,argv[i]);
        strcpy(argumento,"");
    }
   if(system(argumento)==-1){
       printf("Error");
   }
   printf("El comando terminó de ejecutarse\n");
    return 0;
}

/*Usando la versión con execvp(3) del ejercicio 7 y la plantilla de demonio del ejercicio 6, escribir un programa que ejecute cualquier programa como si fuera un demonio. Además, redirigir los flujos estándar asociados al terminal usando dup2(2):
La salida estándar al fichero /tmp/daemon.out.
La salida de error estándar al fichero /tmp/daemon.err.
La entrada estándar a /dev/null.
Comprobar que el proceso sigue en ejecución tras cerrar la shell.*/

int main(int argc, char ** argv){

    pid_t dem;
    dem=fork();
    if(dem==0){
        setsid();
        pid_t pid;
        pid=getpid();
        printf("PID: %i, PPID; %i, GPID: %i, SID: %i\n", pid, getppid(), getpgid(pid), getsid(pid));
        int fd1,fd2,fd3;
        fd1=open("/tmp/daemon.out",O_CREAT|O_RDWR,0777);
        fd2=open("/tmp/daemon.err",O_CREAT|O_RDWR,0777);
        fd3=open("/dev/null",O_CREAT|O_RDWR,0777);
        int fd4 = dup2(fd1,1);
        int fd5 = dup2(fd2,2);
        int fd6 = dup2(fd3,0);
        if(execvp(argv[1],argv+1)==-1){
            printf("Error");
    
        }
    }
    return 0;
}

/*Escribir un programa que bloquee las señales SIGINT y SIGTSTP. Después de bloquearlas el programa debe suspender su ejecución con sleep(3) un número de segundos que se obtendrán de la variable de entorno SLEEP_SECS. Al despertar, el proceso debe informar de si recibió la señal SIGINT y/o SIGTSTP. En este último caso, debe desbloquearla con lo que el proceso se detendrá y podrá ser reanudado en la shell (imprimir una cadena antes de finalizar el programa para comprobar este comportamiento).
*/

int main(int argc, char ** argv){
    sigset_t senales;
    sigemptyset(&senales);
    sigaddset(&senales,SIGINT);
    sigaddset(&senales,SIGTSTP);
    sigprocmask(SIG_BLOCK,&senales,NULL);
    setenv("SLEEP_SECS","100",1);
    char *tiempo=getenv("SLEEP_SECS");
    printf("%s\n",tiempo);
    int t = atoi(tiempo);
    sleep(t);
    sigset_t despertar;
    sigpending(&despertar);
    if(sigismember(&despertar,SIGINT)==1){
        printf("Se ha recibido la senal SIGINT\n");
        sigdelset(&senales,SIGINT);
    }
     else{
        printf("No se ha recibido senal de tipo SIGINT\n");
    }
    if(sigismember(&despertar,SIGTSTP)==1){
        printf("Se ha recibido la senal SIGTSTP\n");
        sigdelset(&senales,SIGTSTP);
    }
    else{
        printf("No se ha recibido senal de tipo SIGTSTP\n");
    }
     sigprocmask(SIG_UNBLOCK,&senales,NULL);
    return 0;
}

/*Escribir un programa que instale un manejador para las señales SIGINT y SIGTSTP. El manejador debe contar las veces que ha recibido cada señal. El programa principal permanecerá en un bucle que se detendrá cuando se hayan recibido 10 señales. El número de señales de cada tipo se mostrará al finalizar el programa. */

volatile int contInt=0;
volatile int contTSTP=0;

void handler(int senal){
    if(senal==SIGINT) contInt++;
    if(senal==SIGTSTP) contTSTP++;
}

int main(int argc, char ** argv){

    struct sigaction sa;

    sigaction(SIGINT,NULL,&sa);
    sa.sa_handler=handler;
    sigaction(SIGINT,&sa,NULL);

    sigaction(SIGTSTP,NULL,&sa);
    sa.sa_handler=handler;
    sigaction(SIGTSTP,&sa,NULL);

    sigset_t senales;
    sigemptyset(&senales);

    while(contInt+contTSTP<10){
        sigsuspend(&senales);
    }

    printf("SIGINT: %i\n",contInt);
    printf("SIGTSTP: %i\n",contTSTP);

    return 0;
}

/*Escribir un programa que realice el borrado programado del propio ejecutable. El programa tendrá como argumento el número de segundos que esperará antes de borrar el fichero. El borrado del fichero se podrá detener si se recibe la señal SIGUSR1.
*/

volatile int stop=0;

void handler(int signal){
    if (signal == SIGUSR1){
        printf ("Señal SIGUSR1 recibida\n");
        stop = 1;
    }
    if (signal == SIGALRM){
        printf ("El tiempo ha terminado\n");
    }
}

int main(int arg, char xxargv){
    if (argc != 2){
        printf ("Argumentos insuficinetes\n");
        return;
    }

    struct sigaction action; 
    sigset_t sen;
    action.sa_handler = handler;
    sigaction (SIGUSRI,&action,NULL); 
    sigaction (SIGALRM,&action,NULL) ;

    int t = atoi (argv[1]);
    alarm(t);

    printf ("Temporizador en curso\ n"); 
    sigsuspend(&sen);

    if (stop==0){
        printf ("El programa se borra\n"); 
        unlink(argv[0]);
    }
    else if(stop == 1){
        printf ("El programa se salva\n");
    }

    return 1;
}







/*Añadir el código necesario para gestionar correctamente los errores generados por setuid(2). Consultar en el manual el propósito de la llamada y su prototipo.*/

int main(){
    char* c;
    if(setuid(0)==-1){
        perror(c);
    }
    return 1;
}

/*Imprimir el código numérico de error generado por la llamada del código anterior y el mensaje asociado.
*/

int main(){
    char* c;
    if(setuid(0)==-1){
        printf("ERROR(%d): %s\n", errno, strerror(errno));
    }
    return 1;
}

/*Escribir un programa que imprima todos los mensajes de error disponibles en el sistema. Considerar inicialmente que el límite de errores posibles es 255.*/

#define MAX_ERRORS 255
int main(){
    char* c;
    int i;
    for (i = 0; i < MAX_ERRORS; i++){
         printf("ERROR(%d): %s\n", i, strerror(i));
    }
    return 1;
}

/*Escribir un programa que muestre, con uname(2), cada aspecto del sistema y su valor. Comprobar la correcta ejecución de la llamada.*/

int main(){
    struct utsname info;

    if(uname(&info) == -1){
         printf("ERROR(%d): %s\n", errno, strerror(errno));
         return -1;
    }
    else{
        printf("Sistema operativo: %s\n", info.sysname);
        printf("Nodename: %s\n", info.nodename);
        printf("Operating system release: %s\n", info.release);
        printf("Operating system version: %s\n", info.version);
        printf("Hardware identifier: %s\n", info.machine);
    }

    return 1;
}

/*Escribir un programa que obtenga, con sysconf(3), información de configuración del sistema e imprima, por ejemplo, la longitud máxima de los argumentos, el número máximo de hijos y el número máximo de ficheros abiertos.
*/

int main(){
    printf("La longitud máxima de enlaces: %li\n", sysconf(_SC_ARG_MAX));
    printf("Numero maximo de hijos: %li\n", sysconf(_SC_CHILD_MAX));
    printf("Numero maximo de ficheros abiertos: %li\n", sysconf(_SC_OPEN_MAX));

    return 1;
}

/*Escribir un programa que obtenga, con pathconf(3), información de configuración del sistema de ficheros e imprima, por ejemplo, el número máximo de enlaces, el tamaño máximo de una ruta y el de un nombre de fichero.
*/

int main(){
    printf("Numero maximo de enlaces: %li\n", pathconf("Practica2.1.c",_PC_LINK_MAX));
    printf("Tamaño maximo de nombre de un fichero: %li\n", pathconf("Practica2.1.c",_PC_NAME_MAX));
    printf("Tamaño maximo de una ruta: %li\n", pathconf("Practica2.1.c",_PC_PATH_MAX));

    return 1;
}

/*Escribir un programa que muestre, igual que id, el UID real y efectivo del usuario. ¿Cuándo podríamos asegurar que el fichero del programa tiene activado el bit setuid?*/

int main(){
    printf("UID real: %d\n", getuid());
    printf("UID efectivo: %d\n", geteuid());
    return 1;
}

/*Modificar el programa anterior para que muestre además el nombre de usuario, el directorio home y la descripción del usuario.
*/

int main(){
    printf("UID real: %d\n", getuid());
    printf("UID efectivo: %d\n", geteuid());

    uid_t uid = geteuid();

    struct passwd *info = getpwuid(uid);

    char *name = info->pw_name;
    char *gecos = info->pw_gecos;
    char *dir = info->pw_dir;

    printf("Nombre de usuario: %s\n",name);
    printf("Descripcion de usuario: %s\n",gecos);
    printf("Directorio home: %s\n",dir);

    return 1;
}

/*Escribir un programa que muestre la hora, en segundos desde el Epoch, usando time(2).*/

int main(){
    time_t t = time(NULL);

    printf("Tiempo en segundos desde epoch: %li\n", t);

    return 1;
}

/*Escribir un programa que mida, en microsegundos, lo que tarda un bucle que incrementa una variable un millón de veces usando gettimeofday(2).
*/

int main(){

    struct timeval tv;

    gettimeofday(&tv, NULL);

    long int micro1 = tv.tv_usec;

    int i = 0;
    for(i; i<1000000;i++){
    }
    gettimeofday(&tv, NULL);

    long int micro2 = tv.tv_usec;

    long int final = micro2-micro1;

    printf("Tiempo en microsegundos: %li\n", final);

    return 1;
}

/*Escribir un programa que muestre el año usando localtime(3).*/

int main(){
    time_t t = time(NULL);
    struct tm *info = localtime(&t);
    int *year = info->tm_year + 1900;
    printf("Año: %i\n", year);
    return 1;
}

/*Modificar el programa anterior para que imprima la hora de forma legible, como "lunes, 29 de octubre de 2018, 10:34", usando strftime(3).*/

int main(){
    setlocale(LC_ALL,"es_ES");
    time_t t = time(NULL);
    struct tm *info = localtime(&t);
    int *year = info->tm_year + 1900;
    printf("Año: %i\n", year);

    char buf[100];
    strftime(buf,100, "%A, %d de %B de %Y, %H:%M", info);
    printf("Hoy es: %s\n", buf);
    return 1;
}

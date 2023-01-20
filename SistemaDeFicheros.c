/*Escribir un programa que, usando open(2), cree un fichero con los permisos rw-r--r-x. Comprobar el resultado y las características del fichero con ls(1).
*/

int main(int argc, char ** argv){
    int fd= open(argv[1],O_CREAT | O_RDWR, 0645);
    if(fd==-1){
        printf("error");
    }
    return 0;
}

/*Modificar el ejercicio 5 para que, antes de crear el fichero, se fije la máscara igual que en el ejercicio 6. Comprobar el resultado con ls(1). Comprobar que la máscara del proceso padre (la shell) no cambia.*/

int main(int argc, char ** argv){
    mode_t mode = umask(027);
    int fd= open("ejercicio7.txt",O_CREAT | O_RDONLY, 0777);
    if(fd==-1){
        printf("error");
    }

    mode_t cd =umask(mode);
    printf("umask al principio:%i\n",mode);
    printf("umask después:%i\n",cd);
    return 0;
}

/* Escribir un programa que emule el comportamiento de stat(1) y muestre:
El número major y minor asociado al dispositivo.
El número de inodo del fichero.
El tipo de fichero (directorio, enlace simbólico o fichero ordinario).
La hora en la que se accedió el fichero por última vez. ¿Qué diferencia hay entre st_mtime y st_ctime? Mtime es ultima modificacion y ctime es ultimo cambio de estado*/

int main(int argc, char ** argv){
    struct stat buff;
    int st = stat(argv[1],&buff);
    printf("Major: %li\n",(long)major(buff.st_dev));
    printf("Minor: %li\n",(long)minor(buff.st_dev));
    printf("Inode: %li\n",buff.st_ino);
    printf("Mode: %i\n",buff.st_mode);
    if(S_ISLNK(buff.st_mode)){
        printf("Es enlace simbolico\n");
    }else if(S_ISREG(buff.st_mode)){
        printf("Es archivo ordinario\n");
    }else if(S_ISDIR(buff.st_mode)){
        printf("Es directorio\n");
    }
    time_t  t = buff.st_atime;
    struct tm * t1 = localtime(&t);
    printf("Hora de ultimo acceso: %d:%d",t1->tm_hour,t1->tm_min);

    return 0;
}

/*link(2) y symlink(2) crean enlaces rígidos y simbólicos, respectivamente. Escribir un programa que reciba una ruta a un fichero como argumento. Si la ruta es un fichero regular, creará un enlace simbólico y rígido con el mismo nombre terminado en .sym y .hard, respectivamente. Comprobar el resultado con ls(1).
*/

int main(int argc, char ** argv){
    struct stat buff;
    int st = stat(argv[1],&buff);
    char * hard;
    char * symb;
    hard=malloc(sizeof(char)(5+strlen(argv[1])));
    symb=malloc(sizeof(char)(5+strlen(argv[1])));
    strcpy(hard,argv[1]);
    strcpy(symb,argv[1]);
    hard=strcat(hard,".hard");
    symb=strcat(symb,".sym");
    if(S_ISREG(buff.st_mode)){
        if(link(argv[1],hard)!=-1){
            printf("Se ha creado un enlace rigido: %s\n",hard);
        }
        if(symlink(argv[1],symb)!=-1){
            printf("Se ha creado un enlace simbolico: %s\n",symb);
        }
    }

    return 0;
}

/*Escribir un programa que redirija la salida estándar a un fichero cuya ruta se pasa como primer argumento. Probar haciendo que el programa escriba varias cadenas en la salida estándar.*/

int main(int argc, char** argv){
    int fichero = open(argv[1], O_CREAT|O_RDWR, 0777);
    int fichero2 = dup2(fichero,1);
    printf("Se ha redirigido a d\n", argv[1]);
    dup2(fichero2,fichero);
    return 1;
}

/*Modificar el programa anterior para que también redirija la salida estándar de error al fichero. Comprobar el funcionamiento incluyendo varias sentencias que impriman en ambos flujos. ¿Hay diferencia si las redirecciones se hacen en diferente orden? ¿Por qué ls > dirlist 2>&1 es diferente a ls 2>&1 > dirlist?
*/

/*ls > dirlist 2>&1: La salida estándar se redirecciona a disrlist y luego los errores se redireccionan a 1 que pertenece a dirlist con lo cual todo se podr visualizar en dirlist.
ls 2>&1 > dirlist: la lista de errores se redirecciona a la salida estándar y la salida estándar se redirige a dirlist luego los errores se van a ver por pantalla y la salida estándar se visualizará en el fichero dirlist.
*/

int main(int argc, char** argv){
    int fichero = open(argv[1], O_CREAT|O_RDWR, 00777);
    int fichero2 = dup2(fichero,1);
    int fichero3 = dup2(fichero,2);
    printf("Se ha redirigido a d\n", argv[1]);
    char* s;
    if(setuid(0)==-1){
        perror(s);
    }
    dup2(fichero2,fichero);
    dup2(fichero3,fichero);
    return 1;
}

/*Escribir un programa que intente bloquear un fichero usando lockf(3):
Si lo consigue, mostrará la hora actual y suspenderá su ejecución durante 10 segundos con sleep(3). A continuación, desbloqueará el fichero, suspenderá su ejecución durante otros 10 segundos y terminará.
Si no lo consigue, el programa mostrará el error con perror(3) y terminará.*/

int main(int argc, char** argv){
    int fichero = open(argv[1], O_CREAT|O_RDWR, 0777);
    if(lockf(fichero,F_TLOCK, (off_t)10000) != -1){
        time_t  t = time(NULL);
        struct tm* t1 = localtime(&t);
        printf("Hora actual: %d:%d\n",t1->tm_hour,t1->tm_min);
        sleep(10);
        lockf(fichero,F_ULOCK, (off_t)10000);
        sleep(10);
    }
    else{
        perror("No bloqueado");
    }

    return 0;
}

/*Escribir un programa que muestre el contenido de un directorio:
El programa tiene un único argumento que es la ruta a un directorio. El programa debe comprobar la corrección del argumento.
El programa recorrerá las entradas del directorio y escribirá su nombre de fichero. Además:
Si es un fichero regular y tiene permiso de ejecución para usuario, grupo u otros, escribirá el carácter ‘*’ después del nombre.
Si es un directorio, escribirá el carácter ‘/’ después del nombre
Si es un enlace simbólico, escribirá “->” y el nombre del fichero enlazado después del nombre. Usar readlink(2).
Al final de la lista, el programa escribirá el tamaño total que ocupan los ficheros (no directorios) en kilobytes.
*/

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Problema con los argumentos\n");
        return 0;
    }

    struct dirent *pDir;
    DIR *dir;
    int suma = 0;

    dir = opendir(argv[1]);
    if(dir == NULL){
        printf("No se puede acceder al directorio\n");
        return 0;
    }

     while((pDir=readdir(dir))!=NULL){
         struct stat buff;
         lstat(pDir->d_name, &buff);

        if(pDir->d_type == DT_DIR){
            printf("Directorio: %s/\n",pDir->d_name);
        }
        else if((pDir->d_type == DT_REG)){
            struct stat fbuff;
            char fi[256];
            strcpy(fi,argv[1]);
            strcat(fi,"/");
            strcat(fi,pDir->d_name);
            lstat(fi, &fbuff);
            if(fbuff.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)){
                printf("Fichero: %s*\n",pDir->d_name);
            }
            else{
                printf("Nombre: %s\n",pDir->d_name);
            }
            
            suma += (buff.st_size/1024);
        }
        else if(pDir->d_type == DT_LNK){

             printf("Fichero con enlace simbolico: %s\n",pDir->d_name);
            char *linkname=pDir->d_name;
            char target[1024];
            ssize_t len;
            char path[1024];
            strcpy(path,argv[1]);
            strcat(path, "/");
            strcat(path,linkname);
            len = readlink(path,target,sizeof(target)-1);
            
            if(len < 0){
                perror("symlink error");
            }
            target[len] = '\0';
            printf("'%s' -> %s\n", linkname, target);
        }
        else if(pDir->d_type == DT_REG){
            lstat(pDir->d_name, &buff);
            printf("Nombre: %s\n",pDir->d_name);
            suma += (buff.st_size/1024);
        }
        
    }
    printf("Tamaño total de los fichero: %i\n", suma);
    closedir(dir);
    return 0;
}

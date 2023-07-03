#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

//
typedef struct{
    regex_t reg;
    struct regular *siguiente;
} regular;
regular primero;

int buscarArchivo(char *archivo, FILE *fout);
int buscarLinea(char* lin);
int procesaDir(char *ruta, FILE *fout);

int main()
{
    primero.siguiente = NULL;
    double total = 0;
    FILE *fp;
    if( (fp = fopen("prueba.dat", "w")) == NULL ){
        printf("No se puede abrir el archivo\n");
        return 1;
    }
    regcomp(&(primero.reg),"include",0);
    total = procesaDir("/home/edwin/Development/Libraries/poco-1.6.1-all", fp);
    fclose(fp);
    printf("Total de archivos:%f\n",total);
    return 0;
}


int buscarArchivo(char *archivo, FILE *fout){
    FILE *fp;
    double lineas;
    char linea[1000];
    int retorno = 0;

    if( (fp = fopen(archivo,"r")) == NULL ){
        printf("No se puede abrir archivo:%s - %s\n",archivo, strerror(errno));
        return -1;
    }
    while(!feof(fp)){
        if(fgets(linea,999,fp) == NULL)
            break;
        lineas++;
        if(buscarLinea(linea) != 0){
            fprintf(fout,"%s:%g:%s\n",archivo,lineas,linea);
        }
    }
    fclose(fp);
    return retorno;
}

int buscarLinea(char* lin){
    int retorno = 0;
    int itmp;
    regular *rtmp = &primero;
    do{
        itmp = regexec(&(rtmp->reg), lin, 0, NULL, 0);
        if(itmp == 0) {
            retorno = 1;
            break;
        }
        rtmp = rtmp->siguiente;
    }while(rtmp != NULL);
    return retorno;
}

int procesaDir(char *ruta, FILE *fout){
    DIR *dir;
    struct dirent *ent;
    struct stat info;
    int facumulados = 0;
    int ok = 0;
    int nook = 0;
    int np = 0;
    int frar = 0;
    int itmp;
    char direct[1000];
    chdir(ruta);
    if( (dir = opendir(ruta)) != NULL ){
        fprintf(fout,"Directorio:%s\n",ruta);
        while( (ent = readdir(dir)) != NULL ){
            if(strcmp(ent->d_name,".") == 0)
                continue;
            if(strcmp(ent->d_name,"..") == 0)
                continue;
            if( stat(ent->d_name, &info) != 0 ){
                printf("No se tiene acceso a:%s - %s\n", ent->d_name, strerror(errno));
            }
            else{
                if( info.st_mode & S_IFDIR ){ // directorio
                    printf("Procesando directorio:%s\n",ent->d_name);
                    strcpy(direct,ruta);
                    strcat(direct,"/");
                    strcat(direct,ent->d_name);
                    facumulados += procesaDir(direct, fout);
                    chdir(ruta);
                }
                else if(info.st_mode & S_IFREG){
                    facumulados++;
                    itmp = buscarArchivo(ent->d_name,fout);
                    if(itmp < 0){
                        np++;
                    }
                    else if(itmp == 0){
                        nook++;
                    }
                    else{
                        ok++;
                    }
                }
                else{
                    printf("Archivo %s de tipo iregular, no se procesa\n", ent->d_name);
                    frar++;
                }
            }
        }
    }
    printf("Directorio:%s\nProcesados:%d\nCon Match:%d\nSin Match:%d\nError de Acceso:%d\n",ruta,facumulados,ok,nook,np);
    return facumulados;
}

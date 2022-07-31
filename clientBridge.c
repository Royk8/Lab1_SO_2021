#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "clientBridge.h"

void write_struct(int fd, unsigned long command, struct complex_struct * struct_address){
    if (ioctl(fd, command, struct_address) == -1){
        perror("Write message error at ioctl");
    }
}

void write_message(int fd, unsigned long command, char * message){
    if (ioctl(fd, command, message) == -1){
        perror("Write message error at ioctl");
    }
}

void read_message(int fd, unsigned long command, char * message){
    if(ioctl(fd, command, message) == -1){
	perror("Read message error at ioctl");
    }
}

void read_message_param(int fd, unsigned long command, int * value){
    if(ioctl(fd, command, value) == -1){
        perror("Read message param error at ioctl");
    }else{
        printf("Copy the messsage from the kernel\n");
    }
}

void write_int(int fd, unsigned long command, int * value){
    if (ioctl(fd, command, value) == -1){
        perror("Write int error at ioctl");
    }
}

void read_int(int fd, unsigned long command, int * value){
    if(ioctl(fd, command, value) == -1){
        perror("Read int error at ioctl");
    }else{
        printf("Copy the int from the kernel\n");
    }
}

int send_empty_command(int fd, unsigned long command){
    int returned_value = ioctl(fd, command);
    if(returned_value == -1){
       perror("Send command error at ioctl");
    }else{
	return returned_value;
        printf("Command OK to the kernel\n");
    }
}

void write_several_messages(int fd){
    write_message(fd, BRIDGE_W_S, "Message 2");
    write_message(fd, BRIDGE_W_S, "Message 3");
}

void read_all_messages_stack(int fd){
	char message[100];
	while(send_empty_command(fd, BRIDGE_STATE_S) > 0){
	    read_message(fd, BRIDGE_R_S, message);
	    printf("%s", message);
	}
}

void read_all_list_messages(int fd){
	char message[100];
	while(send_empty_command(fd, BRIDGE_STATE_L) > 0){
	    read_message(fd, BRIDGE_R_L, message);
	    printf("%s", message);
	}
}

void read_all_queue_messages(int fd){
    char message[100];
    int response = send_empty_command(fd, BRIDGE_STATE_Q);
    while(response > 0){
        if(response == 1){
            read_message(fd, BRIDGE_R_HIGH_PRIOR_Q, message);
            printf("%s\n", message);
        }else if(response==2){
            read_message(fd, BRIDGE_R_MIDDLE_PRIOR_Q, message);
            printf("%s\n", message);
        }else if(response==3){
            read_message(fd, BRIDGE_R_LOW_PRIOR_Q, message);
            printf("%s\n", message);
        }
        response = send_empty_command(fd, BRIDGE_STATE_Q);
    }
}

void first(int fd) {
    char filePath[100];
    printf("\n1. Invertir el orden de los datos.");
    printf("\n   Ingresa el nombre del archivo, con extension: ");
    scanf("%s", filePath);

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filePath, "r");
    if (fp == NULL) {
        perror("Error al abrir archivo.");
    }

    printf("\n Contenido del archivo:\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        printf("%s", line);
        write_message(fd, BRIDGE_W_S, line);
    }
    printf("\n Archivo invertido:\n");
    read_all_messages_stack(fd);

    fclose(fp);
    if (line)
        free(line);
}

void second(int fd){
    char filePath[100];
    printf("\n   Ingresa el nombre del archivo, con extension: ");
    scanf("%s", filePath);

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filePath, "r");
    if (fp == NULL) {
        perror("Error al abrir archivo.");
    }

    printf("\n Contenido del archivo:\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        int num = rand() % 3;
        switch (num)
        {
        case 0:
            write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, line);
            break;
        case 1:
            write_message(fd, BRIDGE_W_MIDDLE_PRIOR_Q, line);
            break;
        case 2:
            write_message(fd, BRIDGE_W_LOW_PRIOR_Q, line);
            break;
        }
        printf("%s", line);
    }
    printf("\nEl archivo con las lineas barajadas es:\n");
    read_all_queue_messages(fd);

}

int realThird(int fd){
    char filePath[100];
    printf("\n3. Verifica si los parentesis y las llaves estan balanceadas.");
    printf("\n   Ingresa el nombre del archivo, con extension: \n");
    scanf("%s", filePath);
    printf("\n file path: %s\n", filePath);

    FILE *filePointer;
    char ch;
    filePointer = fopen(filePath, "r");
    if (filePointer == NULL){
        printf("File is not available \n");
        return -1;
    } else {
        read_all_messages_stack(fd);
        while ((ch = fgetc(filePointer)) != EOF){
            if(ch == '(' || ch == '{'){
                char msgIn[2] = {ch, '\0'};
                write_message(fd, BRIDGE_W_S, msgIn);
            }
            else if(ch == ')'){
                if(send_empty_command(fd, BRIDGE_STATE_S) > 0){
                    char msgOut[2];
                    read_message(fd, BRIDGE_R_S, msgOut);
                    if(msgOut[0] != '('){
                        return 0;
                    }
                } else {
                    return 0;
                }
            } else if(ch == '}'){
                if(send_empty_command(fd, BRIDGE_STATE_S) > 0){
                    char msgOut[2];
                    read_message(fd, BRIDGE_R_S, msgOut);
                    if(msgOut[0] != '{'){
                        return 0;
                    }
                } else {
                    return 0;
                }
            }
        }
        if(send_empty_command(fd, BRIDGE_STATE_S) > 0){
            return 0;
        } else {
            return 1;
        }
    }
    fclose(filePointer);
}

void third(int fd){
    int result = realThird(fd);
    if(result == 1){
        printf("\nESTA BALANCEADO\n");
    }else if(result == 0){
        printf("\nESTA DESBALANCEADO\n");
    } else if(result == -1){
        printf("\nERROR\n");
    }
}

void fourth(int fd){
    printf("\nLlena la cola.\n");
    char ans[100]; 
    int x = 0;
    while(strcmp(ans, "EXIT") != 0){
        printf("\nIntroduce la palabra que quieres añadir a la cola o EXIT para salir.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") == 0){
            break;
        }
        printf("\nQue prioridad tiene? Selecciona una ingresando un numero:\n");
        printf(" 1.Prioridad Alta\n 2.Prioridad Media\n 3.Prioridad Baja\n ");
        scanf("%d", &x);
        switch(x){
            case 1:
                write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, ans);
                printf("\n%s ha sido agregado a la cola con alta prioridad", &ans[0]);
                break;
            case 2:
                write_message(fd, BRIDGE_W_MIDDLE_PRIOR_Q, ans);
                printf("\n%s ha sido agregado a la cola con media prioridad", &ans[0]);
                break;
            case 3:
                write_message(fd, BRIDGE_W_LOW_PRIOR_Q, ans);
                printf("\n%s ha sido agregado a la cola con baja prioridad", &ans[0]);
                break;
        }
    }
    printf("\nEl contenido de la cola de priroridad es:\n");
    read_all_queue_messages(fd);
}

void fifth(int fd){
    printf("\n5. Destruir lista.");

    printf("\nLlena la lista a destruir.\n");
    char ans[100];
    int x = 0;
    while(strcmp(ans, "EXIT") != 0){
        printf("\nIntroduce la palabra que quieres añadir a la lista o EXIT para salir.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") == 0){
            break;
        }
        write_message(fd, BRIDGE_W_L, ans);
        printf("\n%s ha sido agregado a la lista", &ans[0]);
    }
    printf("Ahora la lista sera borrada, dile adios.");    
    send_empty_command(fd, BRIDGE_DESTROY_L);
    read_all_list_messages(fd);
    send_empty_command(fd, BRIDGE_CREATE_L);

}

void sixth(int fd)
{
    printf("\n6. Invertir lista.");
    printf("\nArma la lista.\n");
    char ans[100];
    while(strcmp(ans, "EXIT") != 0){
        printf("\n Escribe una palabra o EXIT para salir.\n");
        scanf("%s", ans);
        printf("\nEntrada: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            
            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_L, ans);
        }
    }
    printf("\nInvertir lista:\n");
    send_empty_command(fd, BRIDGE_INVERT_L);
    read_all_list_messages(fd);
}

void seventh(int fd)
{
    printf("\nIngresa la lista A:.\n");
    char ans[100]; 
    while(strcmp(ans, "EXIT") != 0){
        printf("\nIntroduce la palabra que quieres añadir a la lista o EXIT para salir.\n");
        scanf("%s", ans);
        printf("\nEntrada: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){

            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_L, ans);
        }
    }
    printf("\nContinua ingresando la lista B.\n");
    ans[0] = 'n';
    while(strcmp(ans, "EXIT") != 0){
        printf("\nIntroduce la palabra que quieres añadir a la lista o EXIT para salir.\n");
        scanf("%s", ans);
        printf("\nEntrada: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){

            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_L, ans);
        }
    }
    printf("\nLISTA CONCATENADA:\n");
    send_empty_command(fd, BRIDGE_CONCAT_L);
    read_all_list_messages(fd);
}

void eighth(int fd){
    read_all_list_messages(fd);
    char mess1[] = "primero";
    char mess2[] = "segundo";
    char mess3[] = "tercero";
    char mess4[] = "cuarto";
    char mess5[] = "quinto";
    char mess6[] = "sexto";
    char mess7[] = "septimo";

    printf("1: %s\n", &mess1[0]);
    printf("2: %s\n", &mess2[0]);
    printf("3: %s\n", &mess3[0]);
    printf("4: %s\n", &mess4[0]);
    printf("5: %s\n", &mess5[0]);
    printf("6: %s\n", &mess6[0]);
    printf("7: %s\n", &mess7[0]);

    printf("--- Rotating --- \n");

    write_message(fd, BRIDGE_W_L, mess1);
    write_message(fd, BRIDGE_W_L, mess2);
    write_message(fd, BRIDGE_W_L, mess3);
    write_message(fd, BRIDGE_W_L, mess4);
    write_message(fd, BRIDGE_W_L, mess5);
    write_message(fd, BRIDGE_W_L, mess6);
    write_message(fd, BRIDGE_W_L, mess7);

    write_message(fd, BRIDGE_ROTATE_L, mess1);
    read_all_list_messages(fd);
}

void ninth(int fd){
    printf("\n9. Delete duplicates on the list.");
    printf("\nLet's add some items to the first list.\n");
    char ans[100]; // Just don't init with 0 plz
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the list or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_L, ans);
        }
    }
    printf("\nCleaned list:\n");
    send_empty_command(fd, BRIDGE_CLEAN_L);
    read_all_list_messages(fd);    
}

void tenth(int fd){
    printf("\nLlena tu lista.\n");
    char ans[100];
    while(strcmp(ans, "EXIT") != 0){
        printf("\nIntroduce la palabra que quieres añadir a la lista o EXIT para salir.\n");
        printf("\nEntrada: \n");
        scanf("%s", ans);
        if(strcmp(ans, "EXIT") == 0){
            break;
        }
        write_message(fd, BRIDGE_W_L, ans);
        printf("La palabra %s ha sido agregado a la lista\n", &ans[0]);
    }

    //printf("\nLectura de la lista creada:\n");
    //read_all_list_messages(fd);

    char  major[100];
    read_message(fd, BRIDGE_R_L, major);
   
    do{
        char aux[100];
        read_message(fd, BRIDGE_R_L, aux);
        
        if(strcmp(aux,major) > 0) {
            strcpy(major, aux);
        }
    } while( send_empty_command(fd, BRIDGE_STATE_L)!=0 );
    
    printf("\nEl valor mayor es: %s\n", major);

    //Limpiar lista
    read_all_list_messages(fd);
    //Despues de la segunda vez que se corra, da segmentation fault
}

void menu(int fd) {
    int x = 99;
    while(x != 0){
        printf("\n ----------------- Programa ----------------------------------\n");
        printf("\nCopia el número de la función: ");
        printf("\n\n1. Invertir lineas de archivo.");
        printf("\n2. Barajar aleatoriamente lineas de un archivo.");
        printf("\n3. ¿Parentesis balanceados?");
        printf("\n4. Implementación de cola con prioridades.");
        printf("\n5. Destruir lista completamente.");
        printf("\n6. Invertir nodos de una lista.");
        printf("\n7. Concantenar dos listas.");
        printf("\n8. Rotar n veces lista hacia la derecha.");
        printf("\n9. Limpiar lista.");
        printf("\n10. Valor máximo de una lista.");
        printf("\n\n0. Salir del programa ---------------------------------------\n\n");

        scanf("%d", &x);
        switch (x)
        {
            case 1:
                first(fd);
                break;
            case 2:
                second(fd);
                break;
            case 3:
                third(fd);
                break;
            case 4:
                fourth(fd);
                break;
            case 5:
                fifth(fd);
                break;
            case 6:
                sixth(fd);
                break;
            case 7:
                seventh(fd);
                break;
            case 8:
                eighth(fd);
                break;
            case 9:
                ninth(fd);
                break;
            case 10:
                tenth(fd);
                break;
            default:
                printf("Opción seleccionada, INCORRECTA.");
                break;
        }
    }
}

int main(int argc, char *argv[]){

    const char *file_name = "/dev/bridge"; //used by ioctl
    int fd;

    fd = open(file_name, O_RDWR);
    if (fd == -1){
        perror("Bridge ioctl file open");
        return 2;
    }

    char message[] = "mensajeEspecifico";
    int value = 10;

    //send_empty_command(fd, BRIDGE_CREATE_Q);                    //Create a queue
    //write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, message);		//Send message with high priority
    //write_message(fd, BRIDGE_W_MIDDLE_PRIOR_Q, message);	//Send message with middle priority
    //write_message(fd, BRIDGE_W_LOW_PRIOR_Q, message);		//Send message with low priority
    //read_message(fd, BRIDGE_R_HIGH_PRIOR_Q, message);		//Read a message with high priority
    //read_message(fd, BRIDGE_R_MIDDLE_PRIOR_Q, message);		//Read a message with middle priority
    //read_message(fd, BRIDGE_R_LOW_PRIOR_Q, message);		//Read a message with low priority
    //send_empty_command(fd, BRIDGE_STATE_Q);			//Get an int indicating the state of the queue
    //send_empty_command(fd, BRIDGE_DESTROY_Q);			//Destroy a queue completely releasing memory (IMPORTANT!!)

    //send_empty_command(fd, BRIDGE_CREATE_S);                   	//Create a stack
    //write_message(fd, BRIDGE_W_S, "Message WS");		//Write a message in the stack
    //read_message(fd, BRIDGE_R_S, message);			//Read a message from the stack
    //send_empty_command(fd, BRIDGE_STATE_S);			//Get an int indicating the statte of the stack
    //send_empty_command(fd, BRIDGE_DESTROY_S);			//Destroy a stack completely releasing the memory (IMPORTANT!!)

    //send_empty_command(fd, BRIDGE_CREATE_L);                    //Create a list
    //write_message(fd, BRIDGE_W_L, message);			//Write a message in the last position of a list
    //read_message(fd, BRIDGE_R_L, message);			//Read a message of the last position of a list
    //send_empty_command(fd, BRIDGE_INVERT_L);			//Invert the order of the elements of a list
    //write_int(fd, BRIDGE_ROTATE_L, &value);			//Rotate a list value position to the right
    //send_empty_command(fd, BRIDGE_CLEAN_L);			//Eliminate repeated elements in a list
    //read_message(fd, BRIDGE_GREATER_VAL_L, message);		//Find the greater value of a list of strings
    //printf("Value obtained: %s\n", message);
    //send_empty_command(fd, BRIDGE_END_L);			//End a list (to be able to create another list) (Module limited to at most three list)
    //send_empty_command(fd, BRIDGE_CONCAT_L);			//Concatenate two previous list in a third new one
    //send_empty_command(fd, BRIDGE_STATE_L);			//Get an int indicating the state of a list
    //send_empty_command(fd, BRIDGE_DESTROY_L);			//Destroy all the list of the module releasing memory (IMPORTANT!!)

    // struct complex_struct tmp;
    // strcpy((tmp.messages)[0],"Complex struct message 1");
    // strcpy((tmp.messages)[1], "Complex struct message 2");
    // strcpy((tmp.messages)[2], "Complex struct message 3");
    // tmp.value = 3;
    // write_struct(fd, BRIDGE_W_CS, &tmp);
    // write_several_messages(fd);
    // read_all_messages(fd);
    menu(fd);
    close (fd);
    return 0;
}
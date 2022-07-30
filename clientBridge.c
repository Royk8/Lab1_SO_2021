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

void read_all_messages_list(int fd){
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

void firstPoint(int fd) {
    char filePath[100];
    printf("\n1. Reverse the lines of a text file.");
    printf("\n   Input the file path: ");
    scanf("%s", filePath);

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filePath, "r");
    if (fp == NULL) {
        perror("First point open file.");
    }

    printf("\n File content:\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        printf("%s", line);
        write_message(fd, BRIDGE_W_S, line);
    }
    printf("\n Reversed file:\n");
    read_all_messages_stack(fd);

    fclose(fp);
    if (line)
        free(line);
}

int thirdPoint(int fd){
    char filePath[100];
    printf("\n3. Checks if a code is balanced.");
    printf("\n   Input the file path: ");
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

void forthPoint(int fd){
    printf("\n4. Multi-level Queue.");
    printf("\nLet's add some items.\n");
    char ans[100]; // Just don't init with 0 plz
    int x = 0;
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the queue or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") == 0){
            break;
        }
        printf("\nWhat's the priority? Type the number to choose one:\n");
        printf(" 1. High priority.\n 2. Medium priority.\n 3. Low priority.\n ");
        scanf("%d", &x);
        switch(x){
            case 1:
                write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, ans);
                printf("\n%s Element succesfully added to the high priority queue.\n", &ans[0]);
                break;
            case 2:
                write_message(fd, BRIDGE_W_MIDDLE_PRIOR_Q, ans);
                printf("\n%s Element succesfully added to the medium priority queue", &ans[0]);
                break;
            case 3:
                write_message(fd, BRIDGE_W_LOW_PRIOR_Q, ans);
                printf("\n%s Element succesfully added to the low priority queue", &ans[0]);
                break;
        }
    }
    printf("\nThe items printed in according to their priority\n");
    read_all_queue_messages(fd);
}

void fifthPoint(int fd){
    printf("\n5. Destroy list or stack.");
    int ans;
    printf("\nType 1 to destroy the list or 2 to destroy the stack.\n");
    scanf("%d", &ans);
    if(ans == 1){
        send_empty_command(fd, BRIDGE_DESTROY_L);
        read_all_messages_list(fd);
        printf("\nDone\n");
    } else if (ans == 2){
        send_empty_command(fd, BRIDGE_DESTROY_S);
        read_all_messages_stack(fd);
        printf("\nDone\n");
    } else {
        printf("\nERROR\n");
    }
}

void sixthPoint(int fd)
{
    printf("\n6. Invert list.");
    printf("\nLet's add some items.\n");
    char ans[100]; // Just don't init with 0 plz
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the list or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            // CREATE LIST ITEMS LOGIC
            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_L, ans);
        }
    }
    printf("\nInverted list:\n");
    send_empty_command(fd, BRIDGE_INVERT_L);
    read_all_messages_list(fd);
}

void seventhPoint(int fd)
{
    printf("\n7. Concat list.");
    printf("\nLet's add some items to the first list.\n");
    char ans[100]; // Just don't init with 0 plz
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the list or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            // CREATE LIST ITEMS LOGIC
            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_L, ans);
        }
    }
    printf("\nLet's add some items to the second list.\n");
    ans[0] = 'n';
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the list or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            // CREATE LIST ITEMS LOGIC
            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, ans);
        }
    }
    printf("\nConcatenated lists:\n");
    send_empty_command(fd, BRIDGE_CONCAT_L);
    read_all_messages_list(fd);
}

void eighthPoint(int fd){
    printf("\n8. Rotate list to right.");
    printf("\nLet's add some items to the first list.\n");
    char ans[100]; // Just don't init with 0 plz
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the list or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            // CREATE LIST ITEMS LOGIC
            strcat(ans,"\n");
            write_message(fd, BRIDGE_W_L, ans);
        }
    }
    printf("Rotated list.\n");
    send_empty_command(fd, BRIDGE_ROTATE_L);
    read_all_messages_list(fd);
}

void ninethPoint(int fd){
    printf("\n9. Delete duplicates on the list.");
    printf("\nLet's add some items to the first list.\n");
    char ans[100]; // Just don't init with 0 plz
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the list or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            strcat(ans,"\n");
            write_message(fd, BRIDGE_CLEAN_L, ans);
        }
    }
    printf("\nCleaned list:\n");
    read_all_messages_list(fd);
}

void tenthPoint(int fd){
    printf("\n10. Return the greatest value on the list.");
    printf("\nLet's add some items to the first list.\n");
    char ans[100]; // Just don't init with 0 plz
    while(strcmp(ans, "EXIT") != 0){
        printf("\nType the word you want to add to the list or EXIT.\n");
        scanf("%s", ans);
        printf("\nInput: %s\n", ans);
        if(strcmp(ans, "EXIT") != 0){
            write_message(fd, BRIDGE_CLEAN_L, ans);
        }
    }
    printf("\nSu lista depurada:\n");
    read_all_messages_list(fd);
    // char  major[100];
    // read_message(fd, BRIDGE_R_L, major);
   
    // do{
    //     char input[100];
    //     read_message(fd, BRIDGE_R_L, input);
        
    //     if(strcmp(input,major) > 0)
    //     {

    //         memcpy(major, input,strlen(input)+1);

    //     }
    // }while(send_empty_command(fd, BRIDGE_STATE_L)!=0);
    // printf("\nThe greatest value is: %s\n", major);
}

void menu(int fd) {
    int x = 100; // Just don't init with 0 plz
    while(x != 0)
    {
        printf("\n === First Practice Menu === ");
        printf("\n0. Exit.");
        printf("\nInput which point of this practice you wanna try [1, 10]: ");
        scanf("%d", &x);
        switch (x)
        {
            case 1:
                firstPoint(fd);
                break;
            case 2:
                printf("\nSorry, We didn't make it :/\n");
                break;
            case 3:
                int result = thirdPoint(fd);
                if(result == 1){
                    printf("\nThe file is balanced.\n");
                } else if(result == 0){
                    printf("\nThe file is unbalanced.\n");
                } else if(result == -1){
                    printf("\nERROR\n");
                }
                break;
            case 4:
                forthPoint(fd);
                break;
            case 5:
                fifthPoint(fd);
                break;
            case 6:
                sixthPoint(fd);
                break;
            case 7:
                seventhPoint(fd);
                break;
            case 8:
                eighthPoint(fd);
                break;
            case 9:
                ninethPoint(fd);
                break;
            case 10:
                tenthPoint(fd);
                break;
            default:
                if (x != 0)
                {
                    printf("\nError: Segmentation Fault! :/\n");
                } 
                else
                {
                    printf("\nBye.\n");
                }
                
                break;
        }
        printf("\nThe selected option was: ");
        printf("%d\n", x);
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
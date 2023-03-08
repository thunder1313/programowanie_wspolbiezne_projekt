#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>

#include "read_config.c"
#include "send_execute.c"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("[!] Error! Not enough arguments\n");
        return 1;
    }
    
    // pobieramy liczbę identyfikującą kolejkę
    int queue_num = read_config(argv[1]);
    printf("$ Creating message queue with number %d\n", queue_num);

    // tworzymy kolejkę o podanym kluczu
    int queue_id = msgget(queue_num, 0666 | IPC_CREAT);
    if(queue_id == -1) {
        printf("[!] Error! Could not create message queue\n");
    }
    printf("$ Queue created with ID %d\n", queue_id);

    int child = fork();
    // proces potomny
    if(child == 0) {
        execute_commands(queue_id);
    }
    // rodzic
    else {
        send_commands();
    }
    return 0;
}
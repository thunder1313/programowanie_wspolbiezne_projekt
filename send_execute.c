#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>

// struktura do wysylania komunikatu do procesu
typedef struct command {
	long mtype;
	char command[1024];
    int res_queue;
} COMMAND;

// struktura do zwrotu wykonanego komunikatu do procesu wywołującego
typedef struct response {
	long mtype;
	char mtext[2048];
} RESPONSE;

// funkcja odpowiedzialna za wysłanie komunikatu i nasłuchiwania odpowiedzi
void send_commands() {
    char input[2048];
    char process_name[64], cmd[1024], response_queue[16];

    while(1) {
        printf("> ");
        // pobierz komendę od uzytkownika
        fgets(input, sizeof(input), stdin);
        int input_length = strlen(input);
        // usuwanie znaku nowej linii na końcu inputu
        if (input[input_length-1] == '\n') {
            input[--input_length] = '\0';
        }
        // wyjscie z programu
        if(strcmp(input, "exit") == 0) {
            printf("$ Exiting program...\n");
            break;
        }

        // flaga steruje zapisywaniem nazwy procesu, tresci komendy i numeru kolejki
        int flag = 0, position = 0;
        for(int i = 0; i < input_length; i++) {
            // zapisywanie nazwy procesu ktoremu ma byc wyslane polecenie
            if(flag == 0) {
                // kiedy dojdziemy do konca nazwy procesu
                if(input[i] == ' ') {
                    process_name[position] = '\0';
                    i++;
                    // zmieniamy flage na zapisywanie tresci komendy
                    flag = 1;
                    position = 0;
                    continue;
                }
                process_name[position] = input[i];
                position++;
            }
            // zapisywanie tresci komendy do wykonania
            else if(flag == 1) {
                if(input[i] == '"') {
                    cmd[position] = '\0';
                    i++;
                    // zmieniamy flage na zapisywanie numeru kolejki komunikatow
                    flag = 2;
                    position = 0;
                    continue;
                }
                cmd[position] = input[i];
                position++;
            }
            else if(flag == 2) {
                response_queue[position] = input[i];
                position++;
            }
        }
        response_queue[position] = '\0';

        printf("$ Sending message to process: %s\n", process_name);
        printf("$ Command to execute: %s\n", cmd);
        printf("$ Return answer at: %s\n\n", response_queue);


        // odczytujemy numer kolejki zwiazany z nazwa procesu, ktory ma wykonac polecenie
        int executor_queue_number = read_config(process_name);
        
        // otwieramy jego kolejke
        int exec_queue_id = msgget(executor_queue_number, 0666 | IPC_CREAT);
        COMMAND exercise;
        exercise.mtype = 1;
        strcpy(exercise.command, cmd);
        exercise.res_queue = atoi(response_queue);
        msgsnd(exec_queue_id, &exercise, sizeof(exercise), 0);

        int response_queue_id = msgget(atoi(response_queue), 0666 | IPC_CREAT);
        RESPONSE resp;
        msgrcv(response_queue_id, &resp, sizeof(resp), 2, 0);
        printf("$ Response received!\n");
        printf("$ Response:\n%s\n", resp.mtext);
        msgctl(response_queue_id, IPC_RMID, NULL);
    }

}

// funkcja odpowiedzialna za nasłuchiwanie komunikatow, ich egzekwowanie i odsyłanie odpowiedzi
void execute_commands(int queue_id) {
    COMMAND received_exc;
    msgrcv(queue_id, &received_exc, sizeof(received_exc), 1, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    printf("$ Received command to execute: %s\n", received_exc.command);
    printf("$ Response will be sent to queue with ID: %d\n", received_exc.res_queue);

    int p[2];
    pipe(p);
    int f = fork();
    if(f == 0) {
        close(p[0]);
        close(1);
        dup(p[1]);
        execl("/bin/sh", "/bin/sh", "-c", received_exc.command, 0);
    }

    close(p[1]);
    char buffer[2048];
    int read_bytes = read(p[0], &buffer, sizeof(buffer));
    buffer[read_bytes] = '\0';
    close(p[0]);

    int response_queue_id = msgget(received_exc.res_queue, 0666 | IPC_CREAT);
    RESPONSE resp;
    resp.mtype = 2;
    strcpy(resp.mtext, buffer);
    printf("$ Sending execution of program:\n");
    printf("%s\n", resp.mtext);
    msgsnd(response_queue_id, &resp, sizeof(resp), 0);
}
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_PATH "config.txt"

// funkcja szuka podanej w parametrze nazwy procesu i zwraca jej numer kolejki komunikatow
int read_config(char message_queue[]) {
    // otworzenie pliku konfiguracyjnego w trybie odczytu
    int config = open(CONFIG_PATH, O_RDONLY);
    if(config == -1) {
        printf("[!] Error! Could not open config file\n");
        return -1;
    }
    // odczytanie wielkości pliku
    int file_length = lseek(config, 0, SEEK_END);
    // ustawienie wskaźnika na pozycję początkową
    lseek(config, 0, SEEK_SET);

    char name[128], value[128], buffer;
    int i = 0, read_bytes = 0;

    while(read_bytes != file_length) {
        read(config, &buffer, 1);
        read_bytes++;
        // jezeli doszlismy do pierwszej spacji to oznacza,
        // ze skonczylismy czytac nazwe procesu
        if(buffer == ' ') {
            // dodajemy na ostatnia pozycje w nazwie procesu znak \0
            name[i] = '\0';
            // czytamy kolejne 3 bity - dwukropek, spacje i pierwszy znak numeru kolejki
            read(config, &buffer, 1);
            read(config, &buffer, 1);
            read(config, &buffer, 1);
            read_bytes += 3;

            i = 0;
            // dopoki nie dojdziemy do konca linii lub pliku, zapisujemy numer kolejki komunikatow
            // do structury typu user
            while(buffer != '\n' && read_bytes != file_length+1) {
                value[i] = buffer;
                i++;
                read(config, &buffer, 1);
                read_bytes++;
            }
            value[i] = '\0';

            // kiedy mamy juz odczytane nazwe i numer z pliku, porownujemy czy nazwa procesu
            // zgadza sie z ta podana przy wywolaniu programu
            if(strcmp(message_queue, name) == 0) {
                return atoi(value);
            }
            i = 0;
        } else {
            name[i] = buffer;
            i++;
        }
    }
    printf("[!] Error! Could not find given process name in configuration file\n");
    return -1;
}
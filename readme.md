## Komunikacja dwóch procesów
# UWAGA
Program działa poprawnie tylko na systemie Linux, niestety na systemach MacOS się zawiesza.

### config.txt
Plik konfiguracyjny zawiera nazwy procesów oraz numery kolejek komunikatów do komunikacji z nimi w formacie

nazwa_procesu : numer_kolejki

### Włączenie programu
Na samym początku należy skompilować program. W tym celu będąc w folderze z projektem należy wpisać

`$ gcc projekt.c -o projekt`

Aby odpalić program należy wpisać w pierwszej konsoli

`$ ./projekt nazwa_procesu1`

Zostanie utworzona kolejka komunikatów o numerze zawartym w pliku konfiguracyjnym

Następnie należy otworzyć okno nowej konsoli i wpisać

`$ ./projekt nazwa_procesu2`

Od tego momentu oba procesy oczekują na wysłanie im polecenia do wykonania. Aby to zrobić można wpisać np. przykładową komendę poniżej

`$ nazwa_procesu "ls -la"`

### Przykładowe wywołanie programu
```
$ gcc projekt.c -o projekt
$ ./projekt usr1

# należy otworzyć nowe okno terminala
$ ./projekt usr2
$ usr1 "ls -la"
```

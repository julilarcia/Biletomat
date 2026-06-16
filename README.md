# **Biletomat**

## **Autorka**
Julia Dorobis - jdorobis@student.agh.edu.pl

## **Opis projektu**

Projekt przedstawia symulator biletomatu komunikacji miejskiej napisany w języku C. Program umożliwia użytkownikowi wybór biletu z dostępnego cennika, przeprowadzenie symulacji płatności gotówkowej, wydruk biletu oraz zapis historii transakcji do pliku.

Celem projektu jest praktyczne wykorzystanie mechanizmów omawianych podczas zajęć z systemów operacyjnych, takich jak procesy, potoki, pamięć współdzielona, semafory, wątki, mutexy, biblioteki statyczne i dynamiczne oraz obsługa plików.

---

## **Funkcjonalności programu**

Program umożliwia:

* wczytanie listy dostępnych biletów z pliku tekstowego,
* wyświetlenie menu biletomatu,
* wybór biletu przez użytkownika,
* obliczenie ceny biletu z wykorzystaniem biblioteki dynamicznej,
* symulację płatności gotówkowej,
* obliczenie i wyświetlenie reszty,
* uruchomienie osobnego procesu odpowiedzialnego za drukowanie biletu,
* przekazanie danych biletu do procesu drukarki przez potok,
* zapis wydrukowanego biletu do pliku,
* zapis historii transakcji do pliku logów,
* przechowywanie stanu biletomatu w pamięci współdzielonej,
* synchronizację dostępu do pamięci współdzielonej przy pomocy semafora,
* monitorowanie stanu biletomatu przez osobny wątek,
* obsługę prostego trybu serwisowego, umożliwiającego doładowanie papieru.

---

## **Struktura projektu**

```text
Biletomat/
├── Makefile
├── README.md
├── .gitignore
│
├── include/
│   ├── ticket.h
│   ├── tariff.h
│   ├── machine.h
│   ├── shared_state.h
│   ├── printer.h
│   ├── logger.h
│   ├── menu.h
│   └── payment.h
│
├── src/
│   ├── main.c
│   ├── menu.c
│   ├── payment.c
│   ├── printer.c
│   ├── shared_state.c
│   └── logger.c
│
├── libticket/
│   ├── ticket.c
│   └── ticket.h
│
├── libtariff/
│   ├── tariff.c
│   └── tariff.h
│
├── data/
│   └── cennik.txt
│
├── logs/
│   └── .gitkeep
│
├── tickets/
│   └── .gitkeep
│
└── build/
```

---

## **Opis najważniejszych katalogów i plików**

### **`src/`**

Katalog zawiera główne pliki źródłowe programu.

| Plik             | Opis                                                                                            |
| ---------------- | ----------------------------------------------------------------------------------------------- |
| `main.c`         | Główna logika programu, obsługa menu, inicjalizacja zasobów i sterowanie działaniem biletomatu. |
| `menu.c`         | Funkcje odpowiedzialne za wyświetlanie menu oraz stanu biletomatu.                              |
| `payment.c`      | Symulacja płatności gotówkowej.                                                                 |
| `printer.c`      | Obsługa procesu drukarki oraz komunikacji przez potok.                                          |
| `shared_state.c` | Obsługa pamięci współdzielonej i semafora.                                                      |
| `logger.c`       | Zapis historii transakcji do pliku logów.                                                       |

### **`include/`**

Katalog zawiera pliki nagłówkowe wykorzystywane przez główne moduły programu.

### **`libticket/`**

Katalog zawiera kod biblioteki statycznej `libticket.a`. Biblioteka odpowiada za obsługę struktur związanych z biletem, wczytywanie cennika, generowanie kodu biletu oraz formatowanie wydruku biletu.

### **`libtariff/`**

Katalog zawiera kod biblioteki dynamicznej `libtariff.so`. Biblioteka odpowiada za obliczanie ceny biletu, między innymi za obsługę biletów ulgowych.

### **`data/`**

Katalog zawiera plik `cennik.txt`, z którego program wczytuje dostępne bilety.

### **`logs/`**

Katalog przeznaczony na plik `transakcje.log`, w którym zapisywana jest historia transakcji.

### **`tickets/`**

Katalog przeznaczony na plik `wydrukowane_bilety.txt`, który symuluje wydrukowane bilety.

### **`build/`**

Katalog roboczy tworzony podczas kompilacji. Znajdują się w nim pliki obiektowe, biblioteki oraz program wynikowy.

---

## **Wykorzystane mechanizmy systemów operacyjnych**

### **1. Makefile**

Do kompilacji projektu wykorzystano plik `Makefile`. Automatyzuje on budowanie programu oraz bibliotek.

Makefile odpowiada za:

* kompilację plików źródłowych,
* utworzenie biblioteki statycznej,
* utworzenie biblioteki dynamicznej,
* zlinkowanie programu końcowego,
* czyszczenie plików wynikowych.

Przykładowe polecenia:

```bash
make
make clean
make run
```

---

### **2. Biblioteka statyczna**

W projekcie wykorzystano bibliotekę statyczną:

```text
libticket.a
```

Biblioteka znajduje się w katalogu `libticket/` i zawiera funkcje związane z obsługą biletu.

Odpowiada między innymi za:

* strukturę `Ticket`,
* strukturę `PrintedTicket`,
* wczytywanie biletów z pliku,
* wyszukiwanie biletu po identyfikatorze,
* generowanie kodu biletu,
* zapis sformatowanego biletu do pliku.

Biblioteka statyczna jest tworzona przy pomocy narzędzia `ar`.

---

### **3. Biblioteka dynamiczna**

W projekcie wykorzystano bibliotekę dynamiczną:

```text
libtariff.so
```

Biblioteka znajduje się w katalogu `libtariff/` i zawiera funkcję odpowiedzialną za obliczanie ceny biletu:

```c
float calculate_price(float base_price, int discount);
```

Program ładuje bibliotekę dynamiczną w czasie działania przy użyciu funkcji:

```c
dlopen()
dlsym()
dlclose()
```

Dzięki temu logika obliczania ceny biletu jest oddzielona od głównego programu.

---

### **4. Dynamiczna alokacja pamięci**

Program wykorzystuje dynamiczną alokację pamięci podczas wczytywania listy biletów z pliku `data/cennik.txt`.

Lista biletów jest przechowywana w dynamicznie alokowanej tablicy. W przypadku większej liczby biletów pamięć jest rozszerzana przy pomocy `realloc()`.

Wykorzystane funkcje:

```c
malloc()
realloc()
free()
```

---

### **5. Obsługa plików**

Program korzysta z plików do przechowywania danych wejściowych oraz wyników działania.

Wykorzystywane pliki:

| Plik                             | Opis                                |
| -------------------------------- | ----------------------------------- |
| `data/cennik.txt`                | Plik z listą dostępnych biletów.    |
| `logs/transakcje.log`            | Plik z historią transakcji.         |
| `tickets/wydrukowane_bilety.txt` | Plik symulujący wydrukowane bilety. |

Program wykorzystuje standardowe operacje plikowe języka C, takie jak:

```c
fopen()
fgets()
fprintf()
fclose()
```

---

### **6. Procesy**

W projekcie zastosowano proces potomny tworzony za pomocą funkcji:

```c
fork()
```

Osobny proces odpowiada za symulację drukarki. Dzięki temu główny program biletomatu oraz drukarka są logicznie oddzielone.

Proces główny:

* obsługuje menu,
* przyjmuje wybór użytkownika,
* obsługuje płatność,
* uruchamia proces drukarki.

Proces potomny:

* odbiera dane biletu,
* symuluje drukowanie,
* zapisuje bilet do pliku,
* zmniejsza liczbę dostępnego papieru.

---

### **7. Potoki**

Komunikacja między procesem głównym a procesem drukarki odbywa się z użyciem potoku:

```c
pipe()
```

Proces główny przesyła przez potok strukturę zawierającą dane biletu. Proces potomny odczytuje te dane i zapisuje wydrukowany bilet do pliku.

Wykorzystane funkcje:

```c
pipe()
read()
write()
close()
```

---

### **8. Pamięć współdzielona**

Stan biletomatu jest przechowywany w pamięci współdzielonej. Dzięki temu może być dostępny zarówno dla procesu głównego, jak i procesu drukarki.

W pamięci współdzielonej przechowywana jest struktura:

```c
typedef struct {
    int paper_count;
    float cash_inside;
    int sold_tickets;
    int machine_active;
} MachineState;
```

Zawiera ona:

* liczbę dostępnych blankietów papieru,
* ilość gotówki w automacie,
* liczbę sprzedanych biletów,
* informację o aktywności biletomatu.

Wykorzystane funkcje:

```c
shm_open()
ftruncate()
mmap()
munmap()
shm_unlink()
```

---

### **9. Semafory**

Dostęp do pamięci współdzielonej jest synchronizowany przy pomocy semafora.

Semafor zabezpiecza dane przed jednoczesną modyfikacją przez kilka procesów. Jest to istotne na przykład w sytuacji, gdy proces drukarki zmniejsza liczbę papieru, a proces główny odczytuje lub modyfikuje stan biletomatu.

Wykorzystane funkcje:

```c
sem_open()
sem_wait()
sem_post()
sem_close()
sem_unlink()
```

---

### **10. Wątki**

W programie działa osobny wątek monitorujący stan biletomatu. Wątek cyklicznie sprawdza ilość papieru i wypisuje ostrzeżenie, gdy liczba blankietów jest niska.

Wykorzystane funkcje:

```c
pthread_create()
pthread_join()
```

---

### **11. Mutex**

Do synchronizacji danych wykorzystywanych przez wątki zastosowano mutex.

Mutex chroni zmienną sterującą działaniem wątku monitorującego, dzięki czemu główny wątek programu może bezpiecznie zakończyć działanie monitora.

Wykorzystane funkcje:

```c
pthread_mutex_init()
pthread_mutex_lock()
pthread_mutex_unlock()
pthread_mutex_destroy()
```

---

## **Format pliku cennika**

Plik `data/cennik.txt` zawiera dane biletów w formacie:

```text
id;nazwa;cena_bazowa;czas_waznosci;czy_ulgowy
```

Przykład:

```text
1;Normalny 20 minut;4.00;20;0
2;Ulgowy 20 minut;4.00;20;1
3;Normalny 60 minut;6.00;60;0
4;Ulgowy 60 minut;6.00;60;1
5;Dobowy normalny;15.00;1440;0
6;Dobowy ulgowy;15.00;1440;1
```

Pole `czy_ulgowy` przyjmuje wartości:

| Wartość | Znaczenie      |
| ------- | -------------- |
| `0`     | Bilet normalny |
| `1`     | Bilet ulgowy   |

Cena biletu ulgowego jest obliczana w bibliotece dynamicznej jako połowa ceny bazowej.

---

## **Kompilacja**

Aby skompilować projekt, należy uruchomić polecenie:

```bash
make
```

Po poprawnej kompilacji w katalogu `build/` zostaną utworzone między innymi:

```text
build/biletomat
build/libticket.a
build/libtariff.so
```

---

## **Uruchomienie programu**

Program należy uruchamiać z katalogu głównego projektu:

```bash
./build/biletomat
```

Można również użyć:

```bash
make run
```

---

## **Przykładowe działanie programu**

```text
====================================
        BILETOMAT MIEJSKI
====================================
1. Normalny 20 minut - 4.00 zl
2. Ulgowy 20 minut - 2.00 zl
3. Normalny 60 minut - 6.00 zl
4. Ulgowy 60 minut - 3.00 zl
5. Dobowy normalny - 15.00 zl
6. Dobowy ulgowy - 7.50 zl
------------------------------------
90. Pokaz stan biletomatu
91. Doladuj papier - tryb serwisowy
0. Wyjscie
====================================
Wybierz opcje:
```

Po zakupie biletu program tworzy wpis w pliku `logs/transakcje.log` oraz zapisuje wydrukowany bilet w pliku `tickets/wydrukowane_bilety.txt`.

---

## **Pliki generowane przez program**

### **`logs/transakcje.log`**

Przykładowy wpis:

```text
2026-06-16 15:42:10 | Normalny 20 minut | 4.00 zl | OK
```

### **`tickets/wydrukowane_bilety.txt`**

Przykładowy wydruk:

```text
Data wydruku: 2026-06-16 15:42:10
==============================
BILET KOMUNIKACJI MIEJSKIEJ
Kod: TCK-12345-6789
Nazwa: Normalny 20 minut
Cena: 4.00 zl
Wazny: 20 minut
==============================
```

---

## **Czyszczenie projektu**

Aby usunąć pliki utworzone podczas kompilacji, należy użyć:

```bash
make clean
```

Polecenie usuwa katalog `build/` wraz z plikami wynikowymi.

---

## **Podsumowanie**

Projekt przedstawia prosty, ale kompletny symulator biletomatu miejskiego. Program wykorzystuje wiele mechanizmów systemów operacyjnych omawianych podczas zajęć, między innymi procesy, potoki, pamięć współdzieloną, semafory, wątki, mutexy oraz biblioteki statyczne i dynamiczne.

Dzięki podziałowi programu na moduły kod jest czytelniejszy, a poszczególne elementy systemu, takie jak obsługa biletu, płatność, drukowanie i zarządzanie stanem biletomatu, są od siebie logicznie oddzielone.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COMMAND 18
#define HASH_SIZE 5
#define SENSIBILITY 50

typedef struct car{
    int range;
    struct car* other;
} Car;

typedef struct station{
    int distance;
    struct station* successive;
    Car* available_cars;
} Station;

typedef struct data{
    struct data* next;
    Station* data;
} HashData;

void add_car();
void new_car(Car**, int);
int elaborate_cmd(char*);
void add_station(HashData**, FILE*);
int hash_function(int);
void insert(HashData**, Station*);
void remove_station(HashData**, FILE*);
void remove_car();
void route_calculation();
void initialization(HashData**);


int main() {
    char cmd[COMMAND];
    HashData* hash_map[HASH_SIZE];
    initialization(hash_map);

    while(fscanf(stdin, "%s", cmd) != EOF) {
        switch (elaborate_cmd(cmd)) {
            case 1:
                add_station(hash_map, stdin);
                break;
            case 2:
                remove_station(hash_map, stdin);
                break;
            case 3:
                add_car();
                break;
            case 4:
                remove_car();
                break;
            case 5:
                route_calculation();
                break;
        }
    }

    printf("\nThe program is terminated");

    return 1;
}


/**
 * Inizializza l'hash map inserendo la stazione "0" nella prima riga dell'hash map
 * @param map doppio puntatore poiché è un array di puntatori al tipo HashData, quindi prima deferenziazione è per accedere alla posizione dell'array (e.g.
 * array[2]) dopodiché è un puntatore alla prima struct HashData che contiene:
 *      - dato: stazione
 *      - next: puntatore al dato successivo
 */
void initialization(HashData** map) {
    Station *initial_station;

    initial_station = malloc(sizeof(Station));

    if( initial_station != NULL ){
        initial_station->distance = 0;
        initial_station->successive = NULL;
        initial_station->available_cars = NULL;
        insert(map, initial_station);
    }else{
        printf("Error: initial station not created;\n");
    }
}


void route_calculation() {

}


void remove_car() {

}


void remove_station(HashData** map, FILE* input) {
    int found = 0, distance, key;
    HashData* backup = NULL;
    HashData* destroy = NULL;
    Car* tmp;

    fscanf(input, "%d", &distance);
    key = hash_function(distance);
    backup = map[key];

    if(map[key]->data->distance == distance){
        destroy = map[key];
        map[key] = map[key]->next;
        found = 1;
    }else{
        while(map[key] && !found){
            if(map[key]->next->data->distance == distance) {
                destroy = map[key]->next;
                map[key]->next = map[key]->next->next;
                found = 1;
            }else {
                map[key] = map[key]->next;
            }
        }

        map[key] = backup;
    }

    while(destroy->data->available_cars){
        tmp = destroy->data->available_cars;
        destroy->data->available_cars = destroy->data->available_cars->other;
        free(tmp);
    }

    free(destroy->data);
    free(destroy);

    if(found) {
        printf("demolita\n");
    }else {
        printf("non demolita\n");
    }
}


/**
 * @param dist della stazione dal chilometro zero
 * @return resto della divisione intera con la sensibilità scelta
 */
int hash_function(int dist){
    return dist/SENSIBILITY;
}


/**
 * Aggiunge una stazione con l'arrivo del comando "aggiungi-stazione"
 * @param map doppio puntatore poiché è un array di puntatori al tipo HashData, quindi prima deferenziazione è per accedere alla posizione dell'array (e.g.
 * array[2]) dopodiché è un puntatore alla prima struct HashData che contiene:
 *      - dato: stazione
 *      - next: puntatore al dato successivo
 * @param input file da cui leggere i dati successivi riguardanti la distanza della stazione e le macchine
 */
void add_station(HashData** map, FILE* input) {
    Station *new_station;
    int quantity, range;

    new_station = malloc(sizeof(Station));

    if( new_station != NULL ){

        fscanf(input, "%d %d", &new_station->distance, &quantity);
        new_station->successive = NULL;

        for( int i=0; i<quantity; i++ ){
            fscanf(input, "%d", &range);
            new_car(&new_station->available_cars, range);
        }

        insert(map, new_station);
    }else{
        printf("Error: initial station not created;\n");
    }
}


/**
 * Inserire una determinata stazione come nuovo dato nella riga dell'hash map corretta
 * @param map
 * @param station
 */
void insert(HashData** map, Station* station){
    HashData *new_data;
    int key = hash_function(station->distance);

    new_data = malloc(sizeof(HashData));

    if( new_data != NULL ){
        new_data->data = station;
        new_data->next = map[key];
        map[key] = new_data;
    }
}


/**
 * Creazione di una nuova macchina ed aggiunta nella sezione "available_cars" di una stazione di servizio
 * @param available puntatore alla sezione delle macchine disponibili
 * @param range autonomia massima della macchina inserita
 */
void new_car(Car** available, int range){
    Car *new_car;

    new_car = malloc(sizeof(Car));

    if( new_car != NULL ){
        new_car->range = range;
        new_car->other = *available;
        *available = new_car;
    }else{
        printf("Error: new car not created;\n");
    }
}


/**
 * @param cmd letto dall'input
 * @return 1 -> "aggiungi-stazione"
 * 2 -> "demolisci-stazione"
 * 3 -> "aggiungi-auto"
 * 4 -> "rottama-auto"
 * 5 -> "pianifica-percorso"
 */
int elaborate_cmd(char* cmd){
    if(cmd[0] == 'd'){
        return 2;
    }else if(cmd[0] == 'r'){
        return 4;
    }else if(cmd[0] == 'p'){
        return 5;
    }else{
        if(strncmp(cmd, "aggiungi-auto", 10) == 0){
            return 3;
        }else{
            return 1;
        }
    }
}


void add_car() {

}

























//TEST
//Stampare per tutta la dimensione della hash map le stazioni presenti e le macchine nella stazione
/*
    for(int i=0; i<HASH_SIZE; i++){
        printf("\n\n\nGRUPPO: %d\n\n\n", i);
        while(hash_map[i]) {
            printf("Stazione: %d\n", hash_map[i]->data->distance);
            while (hash_map[i]->data->available_cars) {
                printf("Macchina: %d\n", hash_map[i]->data->available_cars->range);
                hash_map[i]->data->available_cars = hash_map[i]->data->available_cars->other;
            }
            hash_map[i] = hash_map[i]->next;
            printf("\n");
        }
    }
*/
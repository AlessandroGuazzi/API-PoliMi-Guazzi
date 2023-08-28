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

void add_car(HashData**, FILE*);
void new_car(Car**, int);
int elaborate_cmd(char*);
void add_station(HashData**, FILE*);
int hash_function(int);
void insert(HashData**, Station*);
void remove_station(HashData**, FILE*);
void remove_car(HashData**, FILE*);
void route_calculation();
Station* search_station(HashData**, int);
void print_hash(HashData**);


int main() {
    char cmd[COMMAND];
    HashData* hash_map[HASH_SIZE];

    while(fscanf(stdin, "%s", cmd) != EOF) {
        switch (elaborate_cmd(cmd)) {
            case 1:
                add_station(hash_map, stdin);
                break;
            case 2:
                remove_station(hash_map, stdin);
                break;
            case 3:
                add_car(hash_map, stdin);
                break;
            case 4:
                remove_car(hash_map, stdin);
                break;
            case 5:
                route_calculation();
                break;
        }
    }

    //print_hash(hash_map);

    return 1;
}


void route_calculation() {

}


/**
 * Rimuove una macchina da una determinata stazione, se non è presente scrive "non rottamata"
 * @param map Hash Map delle stazioni
 * @param input da cui legge lo stdin
 */
void remove_car(HashData** map, FILE* input) {
    int distance, range, found = 0;
    Station* target;
    Car* destroy;
    Car* backup;

    fscanf(input, "%d %d", &distance, &range);

    target = search_station(map, distance);

    if(target){
        backup = target->available_cars;

        if(target->available_cars == NULL){
            found = 0;
        }else if(target->available_cars->range == range){
            found = 1;
            destroy = target->available_cars;
            target->available_cars = target->available_cars->other;
        }else{
            while (target->available_cars->other && !found) {
                if (target->available_cars->other->range == range) {
                    destroy = target->available_cars->other;
                    target->available_cars->other = target->available_cars->other->other;
                    found = 1;
                } else {
                    target->available_cars = target->available_cars->other;
                }
            }
            target->available_cars = backup;
        }
    }

    if(found){
        free(destroy);
        printf("rottamata\n");
    }else{
        printf("non rottamata\n");
    }
}


/**
 * @param map Hash Map delle stazioni
 * @param distance della stazione che voglio cercare
 * @return la stazione se è presente a quella distanza, NULL se la stazione non c'è
 */
Station* search_station(HashData** map, int distance){
    int key = hash_function(distance), found=0;
    HashData* tmp = NULL;
    tmp = map[key];

    while(tmp && !found){
        if(tmp->data->distance == distance)
            found = 1;
        else
            tmp = tmp->next;
    }

    if(found)
        return tmp->data;
    else
        return NULL;
}


/**
 * Rimuovere una stazione stampando demolita se c'è, non demolita se non c'è
 * @param map Hash Map delle stazioni
 * @param input file da cui leggere lo stdin
 */
void remove_station(HashData** map, FILE* input) {
    int found = 0, distance, key;
    HashData* backup = NULL;
    HashData* destroy = NULL;
    Car* tmp;

    fscanf(input, "%d", &distance);
    key = hash_function(distance);
    backup = map[key];

    if(map[key] == NULL){
        found = 0;
    }else if(map[key]->data->distance == distance){
        destroy = map[key];
        map[key] = map[key]->next;
        found = 1;
    }else {
        while (map[key]->next && !found) {
            if (map[key]->next->data->distance == distance) {
                destroy = map[key]->next;
                map[key]->next = map[key]->next->next;
                found = 1;
            } else {
                map[key] = map[key]->next;
            }
        }
        map[key] = backup;
    }

    if(found){
        while(destroy->data->available_cars){
            tmp = destroy->data->available_cars;
            destroy->data->available_cars = destroy->data->available_cars->other;
            free(tmp);
        }

        free(destroy->data);
        free(destroy);
        printf("demolita\n");
    }else{
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
 * Aggiunge una stazione con l'arrivo del comando "aggiungi-stazione", le stazioni saranno salvate in ordine crescente di distanza dallo 0 nel corretto spazio della hash map
 * @param map doppio puntatore poiché è un array di puntatori al tipo HashData, quindi prima deferenziazione è per accedere alla posizione dell'array (e.g.
 * array[2]) dopodiché è un puntatore alla prima struct HashData che contiene:
 *      - dato: stazione
 *      - next: puntatore al dato successivo
 * @param input file da cui leggere i dati successivi riguardanti la distanza della stazione e le macchine
 */
void add_station(HashData** map, FILE* input) {
    Station *new_station;
    int quantity, range, distance;
    fscanf(input, "%d", &distance);

    if(search_station(map, distance) == NULL){
        new_station = malloc(sizeof(Station));

        if( new_station != NULL ){
            new_station->distance = distance;
            fscanf(input, "%d",&quantity);
            new_station->successive = NULL;

            for( int i=0; i<quantity; i++ ){
                fscanf(input, "%d", &range);
                new_car(&new_station->available_cars, range);
            }

            insert(map, new_station);
        }else{
            printf("Error: initial station not created;\n");
        }
    }else{
        fscanf(input, "%d",&quantity);
        for( int i=0; i<quantity; i++ ){
            fscanf(input, "%d", &range);
        }
        printf("non aggiunta\n");
    }
}


/**
 * Inserire una determinata stazione come nuovo dato nella riga dell'hash map corretta, tenendole in ordine di distanza crescentre dalla stazione 0
 * @param map has map delle stazioni
 * @param station da aggiungere
 */
void insert(HashData** map, Station* station){
    HashData *new_data, *tmp;
    int key = hash_function(station->distance);

    new_data = malloc(sizeof(HashData));

    if( new_data != NULL ){
        new_data->data = station;

        tmp = map[key];

        if(tmp == NULL){
            map[key] = new_data;
        }else if(tmp->data->distance > new_data->data->distance){
            new_data->next = map[key];
            map[key] = new_data;
        }else{
            while(tmp->next){
                if(tmp->next->data->distance > new_data->data->distance){
                    new_data->next = tmp->next;
                    tmp->next = new_data;
                    break;
                }
                tmp = tmp->next;
            }
            if(tmp->next == NULL){
                tmp->next = new_data;
            }
        }

    }

    printf("aggiunta\n");
}


/**
 * Creazione di una nuova macchina ed aggiunta nella sezione "available_cars" di una stazione di servizio, le macchine sono salvate in ordine decrescente di autonomia
 * @param available puntatore alla sezione delle macchine disponibili
 * @param range autonomia massima della macchina inserita
 */
void new_car(Car** available, int range){
    Car *new_car, *tmp;

    new_car = malloc(sizeof(Car));

    if( new_car != NULL ){
        tmp = *available;
        new_car->range = range;

        if(tmp == NULL){
            *available = new_car;
        }else if(tmp->range < new_car->range){
            new_car->other = *available;
            *available = new_car;
        }else{
            while(tmp->other){
                if(tmp->other->range < new_car->range){
                    new_car->other = tmp->other;
                    tmp->other = new_car;
                    break;
                }
                tmp = tmp->other;
            }
            if(tmp->other == NULL){
                tmp->other = new_car;
            }
        }

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


/**
 * Aggiunge una macchina alla giusta stazione e stampa "aggiunta", stampa "non aggiunta" se fallisce
 * @param map Hash Map delle stazioni
 * @param input file da cui leggere stdin
 */
void add_car(HashData** map, FILE* input) {
    int distance, range;
    Station* refilled;

    fscanf(input, "%d %d", &distance, &range);

    refilled = search_station(map, distance);

    if(refilled){
        new_car(&refilled->available_cars, range);
        printf("aggiunta\n");
    }else{
        printf("non aggiunta\n");
    }
}






















//TEST

void print_hash(HashData** map){
    for(int i=0; i<HASH_SIZE; i++){
        printf("\n\n\nGRUPPO: %d\n\n\n", i);
        while(map[i]) {
            printf("Stazione: %d\n", map[i]->data->distance);
            while (map[i]->data->available_cars) {
                printf("Macchina: %d\n", map[i]->data->available_cars->range);
                map[i]->data->available_cars = map[i]->data->available_cars->other;
            }
            map[i] = map[i]->next;
            printf("\n");
        }
    }
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COMMAND 18
#define HASH_SIZE 100000
#define SENSIBILITY 10000

typedef struct car{
    int range;
    struct car* other;
} Car;

typedef struct data HashData;

typedef struct reached{
    int distance;
} Reached;

typedef struct station{
    int distance;
    int id;
    int max_range;
    Reached* reachable;
    Car* available_cars;
} Station;

typedef struct data{
    struct data* previous;
    struct data* next;
    Station* data;
} HashData;

typedef struct node{
    HashData* reaching;
    struct node* next;
} Node;


void add_car(HashData**, FILE*);
void new_car(Car**, int);
int elaborate_cmd(char*);
void add_station(HashData**, FILE*);
int hash_function(int);
void insert(HashData**, Station*);
void remove_station(HashData**, FILE*);
void remove_car(HashData**, FILE*);
void route_calculation(HashData**, FILE*);
void dijkstra(HashData**, HashData*, HashData*, int, int, int, int*, Station**, int*);
void dijkstra_reverse(HashData**, HashData*, HashData*, int, int, int, int*, Station**, int*);
Node* dijkstra_setup(HashData**, HashData*, int, int);
Node* dijkstra_setup_reverse(HashData**, HashData*, int, int);
Station* search_station(HashData**, int);
Node* create_qnode(HashData *);
void delete_qnode(Node**);
void add_reachable(Reached **, Station*);
void add_reachable_reverse(Reached **, Station*);
void reset(HashData**, HashData*, HashData*);
void create_graph(HashData**, HashData*, int*, int);
void create_graph_reverse(HashData**, HashData*, int*, int);
void reorder_q(Node**, HashData*, const int*);
void find_new_max(Station*);


int main() {
    char cmd[COMMAND];
    HashData* hash_map[HASH_SIZE];

    for(int i=0; i<HASH_SIZE; i++){
        hash_map[i] = NULL;
    }

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
                route_calculation(hash_map, stdin);
                break;
        }
    }

    return 0;
}


void route_calculation(HashData** map, FILE* input) {

    int start, finish, key, dimension = 0, flag = 0, i;
    HashData *data, *source, *arrival;

    if(fscanf(input, "%d %d", &start, &finish) != EOF){}

    key = hash_function(finish);
    data = map[key];

    while(data){
        if(data->data->distance == finish){
            flag++;
            arrival = data;
            break;
        }
        data = data->next;
    }

    key = hash_function(start);
    data = map[key];

    while(data){
        if(data->data->distance == start){
            flag++;
            source = data;
            break;
        }
        data = data->next;
    }

    if(flag < 2){
        printf("nessun percorso\n");
        return;
    }else{
        if(start <= finish) {
            create_graph(map, data, &dimension, finish);
        }else{
            create_graph_reverse(map, data, &dimension, finish);
        }

        int distance[dimension], print[dimension];
        Station* precedent[dimension];

        if(start<=finish) {
            dijkstra(map, source, arrival, start, finish, dimension, distance, precedent, print);
        }else{
            dijkstra_reverse(map, source, arrival, start, finish, dimension, distance, precedent, print);
        }

        i=dimension-1;

        if(precedent[i] == NULL){
            printf("nessun percorso\n");
        }else{
            for(i = 0; i<dimension-1; i++){
                if(print[i] != -1){
                    printf("%d ", print[i]);
                }
            }
            printf("%d", print[i]);
            printf("\n");
        }

        if(start <= finish) {
            reset(map, source, arrival);
        }else{
            reset(map, arrival, source);
        }
    }
}


void create_graph_reverse(HashData **map, HashData *data, int *dimension, int finish){
    HashData *tmp = NULL;
    int key = hash_function(data->data->distance), coverage = 0;

    while(data->data->distance > finish){
        *dimension = *dimension + 1;
        data->data->id = *dimension;
        tmp = data;
        if(data->data->available_cars != NULL) {
            while ((data->data->distance - tmp->data->distance) <= data->data->max_range && tmp->data->distance >= finish) {
                if (tmp->data->distance != data->data->distance) {
                    add_reachable_reverse(&data->data->reachable, tmp->data);
                    break;
                }
                tmp = tmp->previous;
                if (tmp == NULL) {
                    if (key > 0) {
                        key--;
                        coverage = SENSIBILITY*key + SENSIBILITY-1;
                        if(data->data->distance - data->data->max_range > coverage){
                            goto jump;
                        }
                        while (map[key] == NULL && key > 0) {
                            key--;
                            coverage -= SENSIBILITY;
                            if(data->data->distance - data->data->max_range > coverage){
                                goto jump;
                            }
                        }
                        tmp = map[key];
                        if (tmp == NULL) {
                            break;
                        }else{
                            if(tmp->next){
                                while(tmp->next){
                                    tmp = tmp->next;
                                }
                            }
                        }
                    } else {
                        break;
                    }
                }
            }
        }
        jump:
        key = hash_function(data->data->distance);
        data = data->previous;
        if(data == NULL){
            if(key > 0){
                key--;
                coverage = SENSIBILITY*key + SENSIBILITY-1;
                if(coverage < finish){
                    rompi:
                    break;
                }
                while(map[key] == NULL && key > 0){
                    key--;
                    coverage -= SENSIBILITY;
                    if(coverage < finish){
                        goto rompi;
                    }
                }
                data = map[key];
                if(data == NULL){
                    break;
                }else{
                    if(data->next){
                        while(data->next){
                            data = data->next;
                        }
                    }
                }
            }else{
                break;
            }
        }
    }
    *dimension = *dimension + 1;
    data->data->id = *dimension;
}


void create_graph(HashData **map, HashData *data, int *dimension, int finish){
    HashData *tmp = NULL;
    int key = hash_function(data->data->distance), coverage = 0;

    while(data->data->distance<finish){
        *dimension = *dimension + 1;
        data->data->id = *dimension;
        tmp = data;
        if(data->data->available_cars != NULL) {
            while ((tmp->data->distance - data->data->distance) <= data->data->max_range &&
                   tmp->data->distance <= finish) {
                if (tmp->data->distance != data->data->distance) {
                    add_reachable(&data->data->reachable, tmp->data);
                    break;
                }
                tmp = tmp->next;
                if (tmp == NULL) {
                    if (key < HASH_SIZE - 1) {
                        key++;
                        coverage = SENSIBILITY*key;
                        if(data->data->distance + data->data->max_range < coverage){
                            goto jump;
                        }
                        while (map[key] == NULL && key < HASH_SIZE - 1) {
                            key++;
                            coverage += SENSIBILITY;
                            if(data->data->distance + data->data->max_range < coverage){
                                goto jump;
                            }
                        }
                        tmp = map[key];
                        if (tmp == NULL) {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            }
        }
        jump:
        key = hash_function(data->data->distance);
        data = data->next;
        if(data == NULL){
            if(key < HASH_SIZE-1){
                key++;
                coverage = SENSIBILITY*key;
                if(coverage > finish){
                    rompi:
                    break;
                }
                while(map[key] == NULL && key<HASH_SIZE-1){
                    key++;
                    coverage += SENSIBILITY;
                    if(coverage > finish){
                        goto rompi;
                    }
                }
                data = map[key];
                if(data == NULL){
                    break;
                }
            }else{
                break;
            }
        }
    }
    *dimension = *dimension + 1;
    data->data->id = *dimension;
}


void reset(HashData **map, HashData *source, HashData *arrival){
    int key = hash_function(source->data->distance);

    while(source->data->distance != arrival->data->distance){
        source->data->id = 0;
        free(source->data->reachable);
        source->data->reachable = NULL;
        source = source->next;
        if(source == NULL){
            if(key < HASH_SIZE-1){
                key++;
                while(map[key] == NULL && key<HASH_SIZE-1){
                    key++;
                }
                source = map[key];
            }
        }
    }
    source->data->id = 0;
    if(source->data->reachable) {
        free(source->data->reachable);
        source->data->reachable = NULL;
    }
}


void dijkstra(HashData **map, HashData *source, HashData *arrival, int start, int finish, int dimension, int *distance, Station **precedent, int *print){
    Node *u = NULL, *q = NULL;
    HashData *v = NULL;
    int alt, i, key;

    for(i=0; i<dimension; i++){
        distance[i] = -1;
        precedent[i] = NULL;
        print[i] = -1;
    }

    distance[0] = 0;

    if(start <= finish){
        q = dijkstra_setup(map, source, start, finish);
    }else{
        q = dijkstra_setup_reverse(map, source, start, finish);
    }

    while(q){
        u = q;
        key = hash_function(u->reaching->data->distance);

        if(distance[u->reaching->data->id-1] == -1 || u->reaching->data->distance == finish){
            break;
        }

        v = u->reaching;
        if(u->reaching->data->reachable != NULL) {
            while (v) {
                if ((v->data->distance - u->reaching->data->distance) <= u->reaching->data->max_range) {
                    alt = distance[u->reaching->data->id - 1] + 1;
                    if (distance[v->data->id - 1] == -1 || alt < distance[v->data->id - 1]) {
                        distance[v->data->id - 1] = alt;
                        precedent[v->data->id - 1] = u->reaching->data;
                        if(v->data->id == dimension){
                            goto jump;
                        }
                    }

                    /*if(v->data->distance == u->reaching->data->reachable->distance){
                        break;
                    }*/

                    v = v->next;
                    if (v == NULL) {
                        if (key < HASH_SIZE - 1) {
                            key++;
                            while (map[key] == NULL && key < HASH_SIZE - 1) {
                                key++;
                            }
                            v = map[key];
                        }
                    }
                } else {
                    break;
                }
            }
        }
        delete_qnode(&q);
    }
    jump:
    while(q){
        delete_qnode(&q);
    }

    i = i-1;

    if(precedent[i] != NULL){
        int j = i;
        print[i] = arrival->data->distance;
        i--;
        while(precedent[j]->distance != start){
            print[i] = precedent[j]->distance;
            j = precedent[j]->id-1;
            i--;
        }
        print[i] = source->data->distance;
    }

}


void dijkstra_reverse(HashData **map, HashData *source, HashData *arrival, int start, int finish, int dimension, int *distance, Station **precedent, int *print){
    Node *u = NULL, *q = NULL;
    HashData *v = NULL;
    int alt, i, key;

    for(i=0; i<dimension; i++){
        distance[i] = -1;
        precedent[i] = NULL;
        print[i] = -1;
    }

    distance[0] = 0;

    if(start <= finish){
        q = dijkstra_setup(map, source, start, finish);
    }else{
        q = dijkstra_setup_reverse(map, source, start, finish);
    }

    while(q){
        u = q;
        key = hash_function(u->reaching->data->distance);

        if(distance[u->reaching->data->id-1] == -1 || u->reaching->data->distance == finish){
            break;
        }

        v = u->reaching;

        if(u->reaching->data->reachable != NULL) {
            while (v) {
                if ((u->reaching->data->distance - v->data->distance) <= u->reaching->data->max_range) {
                    alt = distance[u->reaching->data->id - 1] + 1;
                    if (distance[v->data->id - 1] == -1 || alt < distance[v->data->id - 1]) {
                        distance[v->data->id - 1] = alt;
                        precedent[v->data->id - 1] = u->reaching->data;
                        if(v->data->id == dimension){
                            goto jump;
                        }
                        reorder_q(&q, v, distance);
                    }

                    /*if(v->data->distance == u->reaching->data->reachable->distance){
                        break;
                    }*/

                    v = v->previous;
                    if (v == NULL) {
                        if (key > 0) {
                            key--;
                            while (map[key] == NULL && key > 0) {
                                key--;
                            }
                            v = map[key];
                            if (v->next) {
                                while (v->next) {
                                    v = v->next;
                                }
                            }
                        }
                    }
                }else{
                    break;
                }
            }
        }

        delete_qnode(&q);
    }

    jump:
    while(q){
        delete_qnode(&q);
    }

    i = i-1;

    if(precedent[i] != NULL){
        int j = i;
        print[i] = arrival->data->distance;
        i--;
        while(precedent[j]->distance != start){
            print[i] = precedent[j]->distance;
            j = precedent[j]->id-1;
            i--;
        }
        print[i] = source->data->distance;
    }

}


void reorder_q(Node **q, HashData *near, const int *distance){
    Node *tmp = NULL, *v = NULL, *tmp2 = NULL;

    tmp = *q;
    v = *q;

    while(v){
        if(v->reaching->data->distance == near->data->distance){
            break;
        }
        v = v->next;
    }

    if(distance[tmp->reaching->data->id-1] > distance[v->reaching->data->id-1] && distance[tmp->reaching->data->id-1] != -1){
        v->next = tmp;
        *q = v;
    }else if(distance[tmp->reaching->data->id-1] == distance[v->reaching->data->id-1]){
        if(tmp->reaching->data->distance > v->reaching->data->distance){
            v->next = tmp;
            *q = v;
        }else{
            while(tmp->next && tmp->next->reaching->data->distance < v->reaching->data->distance && distance[tmp->next->reaching->data->id-1] == distance[v->reaching->data->id-1]){
                tmp = tmp->next;
            }
            tmp->next->next = v->next;
            v->next = tmp->next;
            tmp->next = v;
        }
    }else{
        while(tmp->next && distance[tmp->next->reaching->data->id-1] < distance[v->reaching->data->id-1] && distance[tmp->next->reaching->data->id-1] != -1){
            tmp = tmp->next;
        }
        if(tmp != v){
            if(tmp->next){
                if(distance[tmp->next->reaching->data->id-1] == -1 || distance[tmp->next->reaching->data->id-1] > distance[v->reaching->data->id-1]){
                    v->next = tmp->next;
                    tmp->next = v;
                }else{
                    while(tmp->next && tmp->next->reaching->data->distance < v->reaching->data->distance && distance[tmp->next->reaching->data->id-1] == distance[v->reaching->data->id-1]){
                        tmp = tmp->next;
                    }
                    if(tmp->next == v){
                        tmp = tmp->next;
                    }
                    if(tmp != v) {
                        tmp2 = tmp->next;
                        while(tmp2->next){
                            if(distance[tmp2->next->reaching->data->id-1] != distance[tmp->next->reaching->data->id-1]){
                                break;
                            }
                            tmp2 = tmp2->next;
                        }
                        if(tmp2 == v){
                            tmp2 = tmp->next;
                            while(tmp2->next){
                                if(tmp2->next == v){
                                    break;
                                }
                                tmp2 = tmp2->next;
                            }
                            tmp2->next = v->next;
                            v->next = tmp->next;
                            tmp->next = v;
                        }else{
                            tmp2->next = v->next;
                            v->next = tmp->next;
                            tmp->next = v;
                        }
                    }
                }
            }else{
                v->next = tmp->next;
                tmp->next = v;
            }
        }
    }
}


void delete_qnode(Node **q){
    Node *tmp;
    tmp = *q;
    *q = tmp->next;
    free(tmp);
}


Node* create_qnode(HashData *s){
    Node *result;

    result = malloc(sizeof(Node));

    if(result){
        result->reaching = s;
        result->next = NULL;
    }else{
        printf("Error: new q node not created");
    }

    return result;
}


Node* dijkstra_setup(HashData **map, HashData *source, int start, int finish){
    Node *result = NULL, *advancement = NULL;
    HashData *tmp;
    int key = hash_function(source->data->distance);

    result = create_qnode(source);

    if(source->data->distance == start && source->data->distance == finish){
        return result;
    }

    advancement = result;
    tmp = source->next;

    if(tmp == NULL){
        if(key < HASH_SIZE-1){
            key++;
            while(map[key] == NULL && key<HASH_SIZE-1){
                key++;
            }
            tmp = map[key];
        }
    }

    while(tmp->data->distance != finish){
        advancement->next = create_qnode(tmp);
        advancement = advancement->next;
        tmp = tmp->next;
        if(tmp == NULL){
            if(key < HASH_SIZE-1){
                key++;
                while(map[key] == NULL && key<HASH_SIZE-1){
                    key++;
                }
                tmp = map[key];
            }
        }
    }

    advancement->next = create_qnode(tmp);

    return result;
}


Node* dijkstra_setup_reverse(HashData **map, HashData *source, int start, int finish){
    Node *result = NULL, *advancement = NULL;
    HashData *tmp;
    int key = hash_function(source->data->distance);

    result = create_qnode(source);

    if(source->data->distance == start && source->data->distance == finish){
        return result;
    }

    advancement = result;
    tmp = source->previous;

    if (tmp == NULL) {
        if (key > 0) {
            key--;
            while (map[key] == NULL && key > 0) {
                key--;
            }
            tmp = map[key];
            if(tmp->next){
                while(tmp->next){
                    tmp = tmp->next;
                }
            }
        }
    }

    while(tmp->data->distance != finish){
        advancement->next = create_qnode(tmp);
        advancement = advancement->next;
        tmp = tmp->previous;
        if (tmp == NULL) {
            if (key > 0) {
                key--;
                while (map[key] == NULL && key > 0) {
                    key--;
                }
                tmp = map[key];
                if(tmp->next){
                    while(tmp->next){
                        tmp = tmp->next;
                    }
                }
            }
        }
    }

    advancement->next = create_qnode(tmp);

    return result;
}


void add_reachable(Reached ** n, Station* s) {
    if(*n == NULL){
        Reached *new_node = NULL;
        new_node = malloc(sizeof(Reached));
        if (new_node != NULL) {
            new_node->distance = s->distance;
            *n = new_node;
        } else {
            printf("Error: new reachable node was not created\n");
        }
    }else{
        if((*n)->distance < s->distance){
            (*n)->distance = s->distance;
        }
    }

}


void add_reachable_reverse(Reached ** n, Station* s) {
    if(*n == NULL){
        Reached *new_node = NULL;
        new_node = malloc(sizeof(Reached));
        if (new_node != NULL) {
            new_node->distance = s->distance;
            *n = new_node;
        } else {
            printf("Error: new reachable node was not created\n");
        }
    }else{
        if((*n)->distance > s->distance){
            (*n)->distance = s->distance;
        }
    }

}


void remove_car(HashData** map, FILE* input) {
    int distance, range, found = 0;
    Station* target;
    Car* destroy;
    Car* backup;

    if(fscanf(input, "%d %d", &distance, &range) != EOF){}

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
        if(target->max_range == destroy->range){
            free(destroy);
            find_new_max(target);
        }else{
            free(destroy);
        }
        printf("rottamata\n");
    }else{
        printf("non rottamata\n");
    }
}

void find_new_max(Station *target) {
    int max = 0;
    Car *tmp = NULL;

    tmp = target->available_cars;
    while(tmp){
        if(tmp->range > max){
            max = tmp->range;
        }
        tmp = tmp->other;
    }

    target->max_range = max;
}


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


void remove_station(HashData** map, FILE* input) {
    int found = 0, distance, key;
    HashData* backup = NULL;
    HashData* destroy = NULL;
    Car* tmp;

    if(fscanf(input, "%d", &distance) != EOF){}
    key = hash_function(distance);
    backup = map[key];

    if(map[key] == NULL){
        found = 0;
    }else if(map[key]->data->distance == distance){
        destroy = map[key];
        map[key] = map[key]->next;
        if(map[key] != NULL) {
            map[key]->previous = NULL;
        }
        found = 1;
    }else {
        while (map[key]->next && !found) {
            if (map[key]->next->data->distance == distance) {
                destroy = map[key]->next;
                if(map[key]->next->next != NULL) {
                    map[key]->next->next->previous = map[key];
                }
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


int hash_function(int dist){
    return dist/SENSIBILITY;
}


void add_station(HashData** map, FILE* input) {
    Station *new_station;
    int quantity, range, distance;
    if(fscanf(input, "%d", &distance) != EOF){}

    if(search_station(map, distance) == NULL){
        new_station = malloc(sizeof(Station));

        if( new_station != NULL ){
            new_station->distance = distance;
            new_station->id = 0;
            new_station->max_range = 0;
            new_station->reachable = NULL;
            new_station->available_cars = NULL;
            if(fscanf(input, "%d", &quantity) != EOF){}

            for( int i=0; i<quantity; i++ ){
                if(fscanf(input, "%d", &range) != EOF){}
                if(range > new_station->max_range){
                    new_station->max_range = range;
                }
                new_car(&new_station->available_cars, range);
            }

            insert(map, new_station);
        }else{
            printf("Error: initial station not created;\n");
        }
    }else{
        if(fscanf(input, "%d", &quantity) != EOF){}
        for( int i=0; i<quantity; i++ ){
            if(fscanf(input, "%d", &range) != EOF){}
        }
        printf("non aggiunta\n");
    }
}


void insert(HashData** map, Station* station){
    HashData *new_data, *tmp;
    int key = hash_function(station->distance);

    new_data = malloc(sizeof(HashData));

    if( new_data != NULL ){
        new_data->data = station;
        new_data->next = NULL;
        new_data->previous = NULL;

        tmp = map[key];

        if(tmp == NULL){
            map[key] = new_data;
        }else if(tmp->data->distance > new_data->data->distance){
            tmp->previous = new_data;
            new_data->next = map[key];
            map[key] = new_data;
        }else{
            while(tmp->next){
                if(tmp->next->data->distance > new_data->data->distance){
                    new_data->next = tmp->next;
                    new_data->previous = tmp;
                    tmp->next->previous = new_data;
                    tmp->next = new_data;
                    break;
                }
                tmp = tmp->next;
            }
            if(tmp->next == NULL){
                tmp->next = new_data;
                new_data->previous = tmp;
            }
        }

    }

    printf("aggiunta\n");
}


void new_car(Car** available, int range){
    Car *new_car = NULL;

    new_car = malloc(sizeof(Car));

    if( new_car != NULL ){
        new_car->range = range;
        new_car->other = *available;
        *available = new_car;
    }else{
        printf("Error: new car not created;\n");
    }
}


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


void add_car(HashData** map, FILE* input) {
    int distance, range;
    Station* refilled;

    if(fscanf(input, "%d %d", &distance, &range) != EOF){}

    refilled = search_station(map, distance);

    if(refilled){
        if(range > refilled->max_range){
            refilled->max_range = range;
        }
        new_car(&refilled->available_cars, range);
        printf("aggiunta\n");
    }else{
        printf("non aggiunta\n");
    }
}
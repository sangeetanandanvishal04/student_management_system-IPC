#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CACHE_SIZE 5
struct Member* cache_head = NULL;
#define MAX_NAME_LENGTH 30
#define MAX_HOSTEL_LENGTH 20

struct Member {
    int id;
    char firstname[20];
    char lastname[20];
    char hostel[20];
    char course[10];
    int room_num;
    char dob[15];
    int year_of_study;

    struct Member* next;

}*head;

int countCache(){
    struct Member* current = cache_head;
    int count=0;
    while (current != NULL) {
        current = current->next;
        count++;
    }
    return count;
}

char hostels[5][5] = {"BH-1", "BH-2", "BH-3", "BH-4", "BH-5"};
char courses[5][10] = {"BTech", "MTech", "MS", "PhD", "MBA"};

void First_names(char firstNames[][MAX_NAME_LENGTH], int count) {
    FILE *file = fopen("first_names.txt", "r");
    if (file == NULL) {
        printf("Error opening file: first_names.txt\n");
      
    }

    for (int i = 0; i < count; i++) {
        fscanf(file, "%s", firstNames[i]);
    }

    fclose(file);
}

void Last_names(char lastNames[][MAX_NAME_LENGTH], int count) {
    FILE *file = fopen("last_names.txt", "r");
    if (file == NULL) {
        printf("Error opening file: last_names.txt\n");
        
    }

    for (int i = 0; i < count; i++) {
        fscanf(file, "%s", lastNames[i]);
    }

    fclose(file);
}

struct Member* generate_random_student() {
    struct Member* member = (struct Member*)malloc(sizeof(struct Member));
    if (member == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    int new_id;
    do {
        new_id = rand() % 100 + 1;
        // Check if the generated id is already in use.
        struct Member* temp = head;
        while (temp != NULL) {
            if (temp->id == new_id) {
                new_id = 0; // Set to 0 to indicate the id is not unique.
                break;
            }
            temp = temp->next;
        }
    } while (new_id == 0);

    char first_names[20][MAX_NAME_LENGTH];
    char last_names[20][MAX_NAME_LENGTH];

    First_names(first_names,20);
    Last_names(last_names,20);

    member->id = new_id;
    strcpy(member->firstname, first_names[rand() % 20]);
    strcpy(member->lastname, last_names[rand() % 20]);
    strcpy(member->hostel, hostels[rand() % 5]);
    strcpy(member->course, courses[rand() % 5]);
    member->room_num = rand() % 100;
    sprintf(member->dob, "%02d-%02d-%04d", rand() % 29 + 1, rand() % 11 + 1, rand() % 10 + 1990);
    member->year_of_study = rand() % 10 + 1988;

    return member;
}

void insertInCache(struct Member* newMember) {

    if (cache_head == NULL) {
        cache_head = newMember;
    } 
    else {
        newMember->next = cache_head;
        cache_head = newMember;
    }
}

struct Member* searchInCache(int id) {

    struct Member* current = cache_head;
    while (current != NULL) {
        if (current->id == id)
            return current;
        current = current->next;
    }
    return NULL;
}

void removeEndCache(){
    struct Member* temp = cache_head;
    struct Member* prev;
    while(temp->next!=NULL){
        prev = temp;
        temp = temp->next;
    }
    if(temp == head){
        head=NULL;
    }
    else{
        prev->next=NULL;
    }
    free(temp);
}

void removeFromCache(int id) {

    struct Member* current = cache_head;
    struct Member* prev = NULL;

    while (current != NULL) {
        if (current->id == id) {
            if (prev == NULL) {
                cache_head = current->next;
            } 
            else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void clearCache() {
    struct Member* current = cache_head;
    while (current != NULL) {
        struct Member* temp = current;
        current = current->next;
        free(temp);
    }
    cache_head = NULL;
}

void Record_Generator() {

    FILE* file = fopen("disk.txt", "w+");
    if (file == NULL) {
        printf("Error opening file");
        return;
    } 
    else {
        printf("File created by Record Generator successfully\n");
    }
    
    head = NULL;
    clearCache();

    for (int i = 1; i <= 100; i++) {
        struct Member* member = generate_random_student();

        member->next = head;
        head = member;
        if(i>=96 && i<=100){
            int numCache = countCache();
            if(numCache>5){
               removeEndCache();
            }
            else{
               insertInCache(member);
            }
        }

        fprintf(file, "%d\t%s\t%s\t%s\t%s\t%d\t%s\t%d\n", member->id, member->firstname, member->lastname,
                member->hostel, member->course, member->room_num, member->dob, member->year_of_study);
    }
    
    fclose(file);

    printf("Record generated by Record Generator and saved to disk.txt file successfully.\n");
}

void main(){
     Record_Generator();
}
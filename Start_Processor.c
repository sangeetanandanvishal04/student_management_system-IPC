#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_STUDENTS 100

int is_uniqueID(int new_id){

    FILE* queueFile= fopen("operation_queue.txt", "a+");
   
    char operation[10];
    int studentId, operationId,hostelRoom, hours, minutes, seconds;

    while (fscanf(queueFile, "%s %d %d %d %d:%d:%d", operation, &studentId, &operationId , &hostelRoom , &hours, &minutes, &seconds) == 7) {
            if(studentId == new_id){
               return 0;
            }
        } 
        return 1;
  }

void server(int pipe_read, int pipe_write) {
    int service_state[MAX_STUDENTS];   // 0: ready, 1: running, 2: blocked
    int service_priority[MAX_STUDENTS]; // 0: member, 1: manager, 2: admin
    int service_completion_time[MAX_STUDENTS];
    int service_waiting_time[MAX_STUDENTS];
    int num_services_ready = 0;

    while (1) {
        // Read incoming requests from pipe_read
        int request_sequence_no, student_id, operation_id, submission_time;
        read(pipe_read, &request_sequence_no, sizeof(int));
        read(pipe_read, &student_id, sizeof(int));
        read(pipe_read, &operation_id, sizeof(int));
        read(pipe_read, &submission_time, sizeof(int));

        int service_index = request_sequence_no - 1;
        service_state[service_index] = 0; 

            if (service_priority[service_index] == 2) {
                num_services_ready++;  
            } 
            else if (service_priority[service_index] == 1 && num_services_ready <= 3) {
                num_services_ready++;  
            } 
            else if (num_services_ready <= MAX_STUDENTS) {
                num_services_ready++;  
            }  

            service_state[service_index] = 1;

            sleep(1); 

            service_state[service_index] = 2; 
            service_completion_time[service_index] = time(NULL);
            service_waiting_time[service_index] = service_completion_time[service_index] - submission_time;

            num_services_ready--;

            int response[4] = {request_sequence_no, submission_time, service_completion_time[service_index], service_waiting_time[service_index]};
            write(pipe_write, response, sizeof(response));
    }
}

int main() {
    srand(time(NULL));

    int kernel_to_server[2];
    int server_to_kernel[2];
    pipe(kernel_to_server);
    pipe(server_to_kernel);     
    
    pid_t kernel_pid = fork();
    if (kernel_pid == 0) { 
    
    pid_t record_gen_pid = fork();
    if (record_gen_pid == 0) {
        execl("./Rec_Gen", "Rec_Gen", NULL);
        exit(1);
    }

    waitpid(record_gen_pid, NULL, 0);
           
        close(kernel_to_server[1]);
        close(server_to_kernel[0]);

        dup2(kernel_to_server[0], STDIN_FILENO);
        dup2(server_to_kernel[1], STDOUT_FILENO);
    }
    else{
    
        close(kernel_to_server[0]);
        close(server_to_kernel[1]);

        int pipe_read, pipe_write;

        int main_to_kernel[2];
        pipe(main_to_kernel);

        dup2(main_to_kernel[0], STDIN_FILENO);

        pid_t admin_gen_pid = fork();
        if (admin_gen_pid == 0) {
        
        close(main_to_kernel[1]);

        clock_t start_time_Admin = clock();
        FILE* queueFile;
        srand(time(NULL)+ getpid());
        
        int connection_request = 1; 
        write(kernel_to_server[1], &connection_request, sizeof(int));

        int connection_response;
        read(server_to_kernel[0], &connection_response, sizeof(int));

        if (connection_response == 0) {
            printf("Request Generator Admin: Connection to server could not be established.\n");
            exit(1);
        } 
        else {
            read(server_to_kernel[0], &pipe_read, sizeof(int));
            read(server_to_kernel[0], &pipe_write, sizeof(int));
            printf("Request Generator Admin: Established connection with server.\n");

        }

        for (int i = 0; i < 12; i++) {
        
            int studentId;
            while(1){
            studentId = rand() % 100 + 1;
            int check = is_uniqueID(studentId);
               if(check == 0){
                  continue;
               }
               else{
                break;
               }
            }
            
            int operationId = rand() % 4;

            time_t currentTime = time(NULL);
            struct tm* timeInfo = localtime(&currentTime);
            int hour = timeInfo->tm_hour;
            int minute = timeInfo->tm_min;
            int second = timeInfo->tm_sec;

            queueFile= fopen("operation_queue.txt", "a+");

            if (operationId == 2) {
                int hostelRoom = rand() % 100;
                
                printf("Generated: Admin %d %d (%d) %02d:%02d:%02d\n", studentId, operationId, hostelRoom,hour, minute, second);
                fprintf(queueFile, "Admin %d %d %d %02d:%02d:%02d\n", studentId, operationId , hostelRoom , hour, minute, second);
                fflush(queueFile);
            } 
            else {
                printf("Generated: Admin %d %d %02d:%02d:%02d\n", studentId, operationId, hour, minute, second);
                fprintf(queueFile, "Admin %d %d %d %02d:%02d:%02d\n", studentId, operationId , 0, hour, minute, second);
                fflush(queueFile);
            }

            int request_sequence_no = i + 1;  
            int student_id = studentId;       
            int operation_id = operationId; 
            int submission_time = time(NULL);  

            write(pipe_write, &request_sequence_no, sizeof(int));
            write(pipe_write, &student_id, sizeof(int));
            write(pipe_write, &operation_id, sizeof(int));
            write(pipe_write, &submission_time, sizeof(int));

            int waitTime = rand() % 4 + 2;
            sleep(waitTime);
        }

        clock_t end_time_Admin = clock();
        double time_taken_Admin = (double)(end_time_Admin - start_time_Admin) / CLOCKS_PER_SEC;
        printf("Time taken by Admin Request Generator: %f seconds\n", time_taken_Admin);

        exit(0);
    }

    pid_t manager_gen_pid = fork();
        if (manager_gen_pid == 0) {

        close(main_to_kernel[1]);

        clock_t start_time_Manager = clock();
        FILE* queueFile;
        srand(time(NULL)+ getpid());

        int connection_request = 1;
        write(kernel_to_server[1], &connection_request, sizeof(int));

        int connection_response;
        read(server_to_kernel[0], &connection_response, sizeof(int));

        if (connection_response == 0) {
            printf("Request Generator Manager: Connection to server could not be established.\n");
            exit(1);
        } 
        else {
            int pipe_read, pipe_write;
            read(server_to_kernel[0], &pipe_read, sizeof(int));
            read(server_to_kernel[0], &pipe_write, sizeof(int));
            printf("Request Generator Manager: Established connection with server.\n");

        }

        for (int i = 0; i < 15; i++) {
            int studentId;
            while(1){
            studentId = rand() % 100 + 1;
            int check = is_uniqueID(studentId);
               if(check == 0){
                  continue;
               }
               else{
                break;
               }
            }
            
            int operationId = rand() % 4;

            time_t currentTime = time(NULL);
            struct tm* timeInfo = localtime(&currentTime);
            int hour = timeInfo->tm_hour;
            int minute = timeInfo->tm_min;
            int second = timeInfo->tm_sec;

            queueFile= fopen("operation_queue.txt", "a+");

            if (operationId == 2) {
                int hostelRoom = rand() % 100;
                printf("Generated: Manager %d %d (%d) %02d:%02d:%02d\n", studentId, operationId, hostelRoom,hour, minute, second);
                fprintf(queueFile, "Manager %d %d %d %02d:%02d:%02d\n", studentId, operationId , hostelRoom , hour, minute, second);
                fflush(queueFile);
            } 
            else {
                printf("Generated: Manager %d %d %02d:%02d:%02d\n", studentId, operationId, hour, minute, second);
                fprintf(queueFile, "Manager %d %d %d %02d:%02d:%02d\n", studentId, operationId , 0, hour, minute, second);
                fflush(queueFile);
            }

            int request_sequence_no = i + 1;  
            int student_id = studentId;       
            int operation_id = operationId;   
            int submission_time = time(NULL);  

            write(pipe_write, &request_sequence_no, sizeof(int));
            write(pipe_write, &student_id, sizeof(int));
            write(pipe_write, &operation_id, sizeof(int));
            write(pipe_write, &submission_time, sizeof(int));

            int waitTime = rand() % 4 + 1;
            sleep(waitTime);
        }

        clock_t end_time_Manager = clock();
        double time_taken_Manager = (double)(end_time_Manager - start_time_Manager) / CLOCKS_PER_SEC;
        printf("Time taken by Manager Request Generator: %f seconds\n", time_taken_Manager);

        exit(0);   
    }

    pid_t member_gen_pid = fork();
        if (member_gen_pid == 0) {

        close(main_to_kernel[1]);

        clock_t start_time_Member = clock();
        FILE* queueFile;
        srand(time(NULL)+ getpid());

        int connection_request = 1;
        write(kernel_to_server[1], &connection_request, sizeof(int));

        int connection_response;
        read(server_to_kernel[0], &connection_response, sizeof(int));

        if (connection_response == 0) {
            printf("Request Generator Member: Connection to server could not be established.\n");
            exit(1);
        } 
        else {
            int pipe_read, pipe_write;
            read(server_to_kernel[0], &pipe_read, sizeof(int));
            read(server_to_kernel[0], &pipe_write, sizeof(int));
            printf("Request Generator Member: Established connection with server.\n");

        }

        for (int i = 0; i < 35; i++) {
            int studentId;
            while(1){
            studentId = rand() % 100 + 1;
            int check = is_uniqueID(studentId);
               if(check == 0){
                  continue;
               }
               else{
                break;
               }
            }
            
            int operationId = rand() % 4;

            time_t currentTime = time(NULL);
            struct tm* timeInfo = localtime(&currentTime);
            int hour = timeInfo->tm_hour;
            int minute = timeInfo->tm_min;
            int second = timeInfo->tm_sec;

            queueFile= fopen("operation_queue.txt", "a+");

            if (operationId == 2) {
                int hostelRoom = rand() % 100;
                printf("Generated: Member %d %d (%d) %02d:%02d:%02d\n", studentId, operationId, hostelRoom,hour, minute, second);
                fprintf(queueFile, "Member %d %d %d %02d:%02d:%02d\n", studentId, operationId , hostelRoom , hour, minute, second);
                fflush(queueFile);
            } 
            else {
                printf("Generated: Member %d %d %02d:%02d:%02d\n", studentId, operationId, hour, minute, second);
                fprintf(queueFile, "Member %d %d %d %02d:%02d:%02d\n", studentId, operationId , 0, hour, minute, second);
                fflush(queueFile);
            }

            int request_sequence_no = i + 1; 
            int student_id = studentId;       
            int operation_id = operationId;   
            int submission_time = time(NULL);  

            write(pipe_write, &request_sequence_no, sizeof(int));
            write(pipe_write, &student_id, sizeof(int));
            write(pipe_write, &operation_id, sizeof(int));
            write(pipe_write, &submission_time, sizeof(int));

            sleep(1);
        }

        clock_t end_time_Member = clock();
        double time_taken_Member = (double)(end_time_Member - start_time_Member) / CLOCKS_PER_SEC;
        printf("Time taken by Member Request Generator: %f seconds\n", time_taken_Member);
   
        exit(0);
    }

    waitpid(admin_gen_pid, NULL, 0);
    waitpid(manager_gen_pid, NULL, 0);
    waitpid(member_gen_pid, NULL, 0);
    
    }

    if (fork() == 0) {
        execl("./Server", "Server", NULL);
        exit(1);
    } 
    else {
        wait(NULL);
    }
    return 0;
}
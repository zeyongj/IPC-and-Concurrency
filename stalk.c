#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include "list.h"
#include "message.h"
#include <time.h>

#define DEFAULT_PORT 10030
#define DEFAULT_IP "127.0.0.1"

bool should_stop = false;
enum THREAD_TYPE
{
    INPUT = 0,
    SEND = 1,
    RECV = 2,
    OUTPUT = 3,
    MAX_THREAD,
};
pthread_t pid[MAX_THREAD];
int server_fd;
unsigned short udp_port = DEFAULT_PORT;
unsigned short opponent_port;

pthread_mutex_t list_lock, out_list_lock;
sem_t input_empty, input_full, output_empty, output_full;;
List *head, *output_head;

void free_fn(void *item);
void *input_func(void *args);
void *send_func(void *args);
void *recv_func(void *args);
void *output_func(void *args);

void help(const char *app)
{
    printf("help:\n"
           "\t%s [my listen port] [remote machine name] [remote machine port]\n"
           "Example:\n"
           "\t%s 6060 cpu3 6061\n\n",
           app, app);
}

int main(int argc, char *argv[])
{
    int ret;
    struct sockaddr_in ser_addr;

    const char *remote_user;

    if (argc != 4)
    {
        help(argv[0]);
        return 1;
    }
    udp_port = (unsigned short)atoi(argv[1]);
    remote_user = argv[2];
    opponent_port = (unsigned short)atoi(argv[3]);
    server_fd = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if (server_fd < 0)
    {
        printf(" - failed to create/bind UDP socket\n");
        return -1;
    }
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port = htons(udp_port);
    struct timeval tv;
	tv.tv_sec = 3;  //timeout 5s
	tv.tv_usec =  0;
	setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); 
    
    ret = bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)); //bind udp port
    if (ret < 0)
    {
        printf(" - failed to create/bind UDP socket\n");
        return -1;
    }
    head = List_create();
    output_head = List_create();

    pthread_mutex_init(&list_lock, NULL);
    pthread_mutex_init(&out_list_lock, NULL);
    sem_init(&input_empty, 0, 1);//initialize
    sem_init(&input_full, 0, 0);//
    sem_init(&output_empty, 0, 1);//initialize
    sem_init(&output_full, 0, 0);//

    if (pthread_create(&pid[INPUT], NULL, input_func, NULL) != 0)
    {
        printf("fail to create thread\n");
        return -1;
    }
    if (pthread_create(&pid[SEND], NULL, send_func, NULL) != 0)
    {
        printf("fail to create thread\n");
        return -1;
    }
    if (pthread_create(&pid[RECV], NULL, recv_func, NULL) != 0)
    {
        printf("fail to create thread\n");
        return -1;
    }
    if (pthread_create(&pid[OUTPUT], NULL, output_func, argv[2]) != 0)
    {
        printf("fail to create thread\n");
        return -1;
    }

    for (int i = 0; i < MAX_THREAD; ++i)
    {
        pthread_join(pid[i], NULL);
    }
    /* release all mutex  and semaphores */
    pthread_mutex_destroy(&list_lock);
    pthread_mutex_destroy(&out_list_lock);
    sem_destroy(&input_empty);
    sem_destroy(&input_full);
    sem_destroy(&output_empty);
    sem_destroy(&output_full);

    List_free(head, free_fn);
    List_free(output_head, free_fn);

    printf("totally terminate the session\n");
    return 0;
}

void free_fn(void *item)
{
    free(item);
    //free(((char *)item) - sizeof(msg_header));
    return;
}

void *input_func(void *args)
{
    char *input;
    while (!should_stop)
    {
        bool exit = false;
        sem_wait(&input_empty);  
        input = malloc(sizeof(char) * (DATA_BUF_LEN) + sizeof(msg_header));
        printf("Me:");
        //fflush(stdout);
        scanf("%s", input + sizeof(msg_header)); //sizeof(input));
        printf("\033[1A"); 
        printf("\033[K");  //clear line
         printf("Me:%s\n", input+ sizeof(msg_header));
        //add to list
        *((msg_header *)input) = CHAT_MSG;
        if (strcmp(input + sizeof(msg_header), "!")  == 0)
        {
            exit = true;
            //free(input);
            *((msg_header *)input) = SHUTDOWN;
            // List_append(head, input);
            // sem_post(&input_full);  
            // break;
        }
        //pthread_mutex_lock(&list_lock);
        List_append(head, input);
        //pthread_mutex_unlock(&list_lock);
        if(exit)
            should_stop = exit;
        sem_post(&input_full);  
        
    }
    return NULL;
}

void *send_func(void *args)
{
    struct sockaddr_in addr_serv;
    
    while (!should_stop)
    {
        sem_wait(&input_full); 
        //pthread_mutex_lock(&list_lock);
        const char *data = (const char *)List_last(head);
        // if(data == NULL)
        // {
        //     printf("data is NULL!\n");
        //     data = (const char *)List_first(head);
        // }
        // printf("send message success %s\n", data + 2);
        // fflush(stdout);
        
        if (data != NULL)
        {
            int len = sizeof(addr_serv);
            memset(&addr_serv, 0, sizeof(addr_serv));
            addr_serv.sin_family = AF_INET;
            addr_serv.sin_addr.s_addr = inet_addr(DEFAULT_IP);
            addr_serv.sin_port = htons(opponent_port);
            
            int send_num = sendto(server_fd, data, sizeof(msg_header) + strlen(data+sizeof(msg_header)) + 1, 0, (struct sockaddr *)&addr_serv, len);
            //printf("%d\n", send_num);
            // if(*((msg_header *)data) == SHUTDOWN)
            // {
            //     break;
            // }
        }
        else
        {
            printf("  - wrong happen....an NULL message\n");
        }
        sem_post(&input_empty);  

        //pthread_mutex_unlock(&list_lock);
    }

    //sem_post(&output_empty);   
    return NULL;
}

void *recv_func(void *args)
{
    char *output;
    struct sockaddr_in client_addr; 
	socklen_t len = sizeof(client_addr);
    int count = 100;
    while (!should_stop)
    {
        
        if(count > 0){
            sem_wait(&output_empty);   
            output = malloc(sizeof(char) * DATA_BUF_LEN + sizeof(msg_header));
        }
        // if(should_stop)
        // {
        //     sem_post(&output_full);  
        //     break;
        // }
        count = recvfrom(server_fd, output, sizeof(char) * DATA_BUF_LEN + sizeof(msg_header), 0, 
             (struct sockaddr *)&client_addr, &len);//
        // if(count > 0)
        // printf("count: %d\n", count);
        //printf("receive a message: %s\n", output + 2);
        if(count > 0){
            if(*((msg_header *)output) == SHUTDOWN)
            {
                should_stop = true;
            }
            //count = 0;
            List_append(output_head, output);
            sem_post(&output_full); 
        }
        else{
            if(should_stop){
                free(output);
                sem_post(&output_full);
            }
        }
    }

    return NULL;

}

void *output_func(void *args)
{
    
    while(!should_stop)
    {
        sem_wait(&output_full); 
        const char *data = (const char *)List_last(output_head);
        // if(data == NULL)
        // {
        //     data = (const char *)List_first(output_head);
        // }
        //List_next(output_head);
        if (data != NULL)
        {
            if(*((msg_header *)data) != SHUTDOWN)
                printf("\r%s>: %s\n", (const char*)args, data+sizeof(msg_header));
            else{
                should_stop = true;
                printf("USER[%s] is offline...Good Bye\n", (const char*)args);
                fflush(stdout);
            }
            
        }
        sem_post(&output_empty);  
    }

}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
 
#define BUF_SIZE 512
#define MAX_CLNT 100
#define NAME_SIZE 20
 
void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);
 
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
char clnt_name[NAME_SIZE] = {};
char clnt_names[MAX_CLNT][NAME_SIZE] = {};
char cLatLng[BUF_SIZE] = {};
char cSend_msg[BUF_SIZE] = {};
 
pthread_mutex_t mutx;
 
int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock, i;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
 
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
 
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
 
    if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
{
}
        //error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
{
}
        //error_handling("listen() error");
        printf("waiting Client...\n");
 
    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
 
        if (clnt_cnt >= MAX_CLNT) {
            printf("CONNECT FAIL : %d \n", clnt_sock);
            write(clnt_sock, "too many users. sorry", BUF_SIZE);
            continue;
        }
 
 
        pthread_mutex_lock(&mutx);
 
        clnt_socks[clnt_cnt] = clnt_sock;
        read(clnt_sock, clnt_name, NAME_SIZE);
        strcpy(clnt_names[clnt_cnt++], clnt_name);
        // ㄴ 클라이언트로부터 받은 접속자 이름입력
        pthread_mutex_unlock(&mutx);
 
 
 
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id); 
        printf("Connected client IP: %s, Name : %s \n", inet_ntoa(clnt_adr.sin_addr),clnt_name);
    }
    close(serv_sock);
    return 0;
}
 
void error_handling(char * msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
 
void * handle_clnt(void * arg)
{
    int n;
    printf("%s\n",cSend_msg); 
    while((n=read(*(int*)arg, cLatLng, BUF_SIZE))>0)
    {
        printf("arg: %d, LatLng: %s ,size:  %d, n: %d\n",*(int*)arg,cLatLng,(int)sizeof(cLatLng),n);
        if(write(*(int*)arg,cLatLng,n)==-1)
            printf("full error\n");
    }
}
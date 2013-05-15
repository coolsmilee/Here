#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include "../common/protocol.h"

int main(int argc, char** argv)
{
    int    sockfd, n;
    char    recvline[BUFSIZE], sendline[BUFSIZE];
    struct sockaddr_in    servaddr;
    int op_code, paras_num;
    parameter *paras_send;
    parameter paras_recv[MAX_PARA_NUM];
    
    int i, package_size;

    if( argc != 2){
        printf("usage: ./client <ipaddress>\n");
        exit(0);
    }

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    if( inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    //printf("send msg to server: \n");
    //fgets(sendline, BUFSIZE, stdin);
    memset(sendline, 0, sizeof(sendline));
    
    char op[20];
    char mail[20];
    char pswd[20];
    
    gets(op);
    gets(mail);
    gets(pswd);
    
    op_code = atoi(op);
    
    //op_code = 102;
    paras_num = 2;
    paras_send = (parameter *)malloc(paras_num * sizeof(parameter));
    
    paras_send[0].len = strlen(mail);
    memcpy(paras_send[0].data, mail, strlen(mail));
    
    paras_send[1].len = strlen(pswd);    
    memcpy(paras_send[1].data, pswd, strlen(pswd));
    
    package( op_code,
             paras_num,
             paras_send,
             &package_size,
             sendline );
             
    if( send(sockfd, sendline, package_size, 0) < 0)
    {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    
    int res = recv(sockfd, recvline, sizeof(recvline), 0);
    printf("recv %d bytes\n",res);
    
    parse( &op_code,
           &paras_num,
           paras_recv,
           recvline );
    
    printf("op_code = %d\n",op_code);
    printf("paras_num = %d\n",paras_num);
    
    for( i=0; i<paras_num; i++ )
    {
        printf("paras_recv[%d].len = %d\n",i,paras_recv[i].len);        
        printf("paras_recv[%d].data = %d\n",i,ntohl(*(int *)paras_recv[i].data));
    }
    
    close(sockfd);
    exit(0);
}

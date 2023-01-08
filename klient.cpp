#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dataDef.h"

int klient(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char buffer[256];

    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr)); //vynulovanie
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0); //vytvorenie socketa
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) //ziadost o pripojenie,
    {
        perror("Error connecting to socket");
        return 4;
    }
    /*
    printf("Please enter a message: ");
    bzero(buffer,256); //vynulovanie buffera
    fgets(buffer, 255, stdin); //zapisanie napisanej spravy pouzivatelom do buffera

    n = write(sockfd, buffer, strlen(buffer)); //zapiasnie spravy na server(odoslanie)
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }

    bzero(buffer,256); //vynulovanie klientovho buffera
    n = read(sockfd, buffer, 255); //precitanie spravy zo servera
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
     printf("%s\n",buffer);
     */
    //inicializacia dat zdielanych medzi vlaknami
    DATA data;
    data_init(&data, sockfd);

    //vytvorenie vlakna pre zapisovanie dat do socketu <pthread.h>
    pthread_t thread;
    pthread_create(&thread, NULL, data_writeData, (void *)&data);

    //v hlavnom vlakne sa bude vykonavat citanie dat zo socketu
    data_readData((void *)&data);

    //pockame na skoncenie zapisovacieho vlakna <pthread.h>
    pthread_join(thread, NULL);
    data_destroy(&data);


    close(sockfd); // rusenie spojenia

    return 0;
}
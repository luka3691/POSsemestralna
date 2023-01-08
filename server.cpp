#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "dataDef.h"

int server(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    //char buffer[256];

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //definuje rozsah ip adries
    serv_addr.sin_addr.s_addr = INADDR_ANY; //definuje ze vsetky adresy sa
    serv_addr.sin_port = htons(atoi(argv[1])); //hostnetwork (typ architektury sa definuje), je ako vstup definovany

    sockfd = socket(AF_INET, SOCK_STREAM, 0); //definovanie typu socketov, volba protokolu, Unix-domain socket so spoľahlivým, prúdovým prenosom.
    if (sockfd < 0) //ak je socket vytvoreny mensi ako nula tak nastala chyba
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) //priradenie socketu adresu
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5); //definovanie socketu ako pasivny, mozem mat naraz 5 cakajucich klientov, sluzi iba na prijmanie ziadosti o spojenie
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len); //vytvori novy socket na komunikaciu s klientom, vracia deskriptov socketu
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return 3;
    }
    //inicializacia dat zdielanych medzi vlaknami
    //bzero(buffer,256); //o 1 vacsie aby spravne to bolo ukoncene dakou nulov
    //n = read(newsockfd, buffer, 255); //citanie vstupu od klienta cez buffer
    DATA data;
    data_init(&data, newsockfd);

    //vytvorenie vlakna pre zapisovanie dat do socketu <pthread.h>
    pthread_t thread;
    pthread_create(&thread, NULL, data_writeData, (void *)&data);

    //v hlavnom vlakne sa bude vykonavat citanie dat zo socketu
    data_readData((void *)&data);

    //pockame na skoncenie zapisovacieho vlakna <pthread.h>
    pthread_join(thread, NULL);
    data_destroy(&data);

    /*
      if (n < 0)
    {
        perror("Error reading from socket");
        return 4;
    }
    printf("Here is the message: %s\n", buffer);

    const char* msg = "I got your message";
    n = write(newsockfd, msg, strlen(msg)+1); // odoslanie spravy msg klientovy
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }
    */
    close(newsockfd);
    close(sockfd);

    return 0;
}
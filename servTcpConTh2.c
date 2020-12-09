/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
	Intoarce corect identificatorul din program al thread-ului.
  
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

/* portul folosit */
#define PORT 2908
int clientinr=0;
/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData
{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl;       //descriptorul intors de accept
} thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int main()
{
    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    int nr; //mesajul primit de trimis la client
    int sd; //descriptorul de socket
    int pid;
    pthread_t th[100]; //Identificatorii thread-urilor care se vor crea
    int i = 0;

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 2) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }
    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData *td; //parametru functia executata de thread
        int length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        // client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        // int idThread; //id-ul threadului
        // int cl; //descriptorul intors de accept

        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = i++;
        td->cl = client;
        clientinr++;
        pthread_create(&th[i], NULL, &treat, td);


    } //while
};
static void *treat(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    printf("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
    fflush(stdout);
    pthread_detach(pthread_self());
    joc((struct thData *)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close((intptr_t)arg);
    clientinr--;
    return (NULL);
};

voi joc(void *arg)
{
    int nr, i = 0;
    struct thData tdL; // structura pt thread
    tdL = *((struct thData *)arg);
    int joc = 0;
    char raspuns[300];
    while (1)
    {

        if (read(tdL.cl, &nr, sizeof(int)) <= 0)
        {
            printf("[Thread %d]\n", tdL.idThread);
            perror("Eroare la read() de la client.\n");
        }
        int ok = 0;
        //printf("[Thread %d]Mesajul a fost receptionat...%d\n", tdL.idThread, nr);
if(nr==5)
{strcpy(raspuns,"am iesit \n");
break;}

        /*pregatim mesajul de raspuns */
        if (nr == 1)
        {
            if (joc == 1)
                strcpy(raspuns,"sunteti deja in joc, dati alta comanda \n");
            strcpy(raspuns,"Se incepe jocul \n");
            int matrice[7][6];
            ok = 1;
            joc = 1;
        }
        else if (nr == 2)
        {
            if (joc == 1)
            {
            strcpy(raspuns,"Se muta piesa. Dupa ce se muta piesa, se va apela functia <<afiseazamatrice>>. La sf se va verifica stadiul tablei, apeland functia <<verifica tabla>>(daca cineva a castigat,daca e remiza, daca inca nu s-a terminat jocul) \n");
                ok = 2;
            }
            else
            {
                strcpy(raspuns,"Trebuie sa incepeti un joc");
                ok = 0;
            }
        }
        else if (nr == 3)
        {
             strcpy(raspuns,"Se va apela functia de afisat scorul");
            ok = 3;
        }

        else if (nr == 4)
        {
            if (joc == 1)
            {
                strcpy(raspuns,"se va apela functia de resign \n");
                ok = 4;
                joc = 0;
            }
        }
        else
        {
            ok = -1;
            strcpy(raspuns,"nu ati dat o comanda valida \n");
        }

        //printf("[Thread %d]Trimitem mesajul inapoi...%d\n", tdL.idThread, nr);

        /* returnam mesajul clientului */
        if (write(tdL.cl, &raspuns, 300) <= 0)
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);
    }
}

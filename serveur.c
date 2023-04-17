#include <stdio.h>          
#include <stdlib.h>         
#include <unistd.h>         
#include <sys/types.h>      
#include <string.h>         
#include <sys/socket.h>     
#include <pthread.h>
#include <arpa/inet.h>      
#include <netinet/in.h>     
#include <netdb.h>          


#define MAX_CLIENTS 10

typedef struct {
    char nom[30];
    char message[100]; 
} User;

void *fonction(void *arg){
    int socket = *(int*) arg;
    char question[] = "Que voulez vous envoyer ?";
    User user;
    send(socket, question, strlen(question)+1,0);   
    recv(socket, &user, sizeof(user), 0);
    printf("le message du client %s est : %s\n", user.nom, user.message);   //%s ou autre chose ?
    close(socket);
    free(arg);      //pas sur de quoi a quoi ca sert
    pthread_exit(NULL);
}

int main(void){
    int socketServeur = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServeur  == -1){
		perror("Error creating socket.\n");
		exit(1);
	}
    struct sockaddr_in addrServeur  = {0};
    addrServeur.sin_addr.s_addr = inet_addr("127.0.0.1.");
    addrServeur.sin_family = AF_INET;
    addrServeur.sin_port = htons(2003);     //essayer un autre port aussi

    //Connection de la socket a l'addresse serveur
    bind(socketServeur, (struct sockaddr *)&addrServeur, sizeof(addrServeur));       
    printf("bind : %d \n", socketServeur);

    listen(socketServeur, 2);   //file d'attente au max de 2
    printf("Serveur ecoute \n");

    pthread_t threads[2];
    for (int i=0; i<2; i++){
        struct sockaddr_in addrClient;      //pas bien compris comment ces quelques lignes qui suivent marchent
        socklen_t tailleClient = sizeof(addrClient);
        int socketClient = accept(socketServeur, (struct sockaddr*)&addrClient, &tailleClient);
        printf("socketClient : %d \n", socketClient);
        printf("Connexion acceptee \n");

        printf("Client %d \n", socketClient);

        int *arg = malloc(sizeof(int));
        *arg = socketClient;
        pthread_create(&threads[i], NULL, fonction, arg);
    }

    for (int i=0; i<2; i++){
        pthread_join(threads[i], NULL);
    }

    close(socketServeur);
    printf("Fermeture");

    return 0;
}

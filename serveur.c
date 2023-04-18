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

/* A faire :
	Message envoyé fait max 8 charactères!!! problème
	Variable globale pour stocker les messages de tous les utilisateurs. Faire stucture qui est une matrice de messages.
	Condition d'arrêt du programme (et pourquoi si ctr C du client, message à l'infini du serveur)
	
	
   Resolu:
   	Pourquoi message vide au début
   	Pourquoi décalage affichage
   	transformer structure et mettre message en tant que string (il se peut que ça soit déjà le cas)
   	remplacer scanf() par fgets() car stock un string et donc plusieurs mots. Corriger taille des char envoyés.
 */


#define MAX_CLIENTS 10
#define BUFFER_SIZE 100

//Definir la structure qui permet d'envoyer le message
typedef struct {
    char nom[BUFFER_SIZE];
    char message[BUFFER_SIZE]; 
} User;


void *th_client(void *arg){
    int socket = *(int*) arg;
    User userb; 
    memset(userb.message, '\0', BUFFER_SIZE);
    recv(socket, &userb.nom, sizeof(userb.nom), 0);
    printf("le nom du client est : %s\n", userb.nom);
    while(1){
    	//char question[] = "Que voulez vous envoyer ?";
    	//send(socket, question, strlen(question)-2,0);  
    	char mes[BUFFER_SIZE];
    	recv(socket, &mes, sizeof(mes), 0);
    	printf("le message du client %s est : %s \n", userb.nom, mes);
    	if (strcmp(mes, "fin")==0){
    	    printf("finito bebe pour %s \n", userb.nom);
    	    break;
    	}
    }
    close(socket);
    //free(arg);      //pas sur de quoi a quoi ca sert
    pthread_exit(NULL);
}


int main(void){
    //Creer socket serveur
    int socketServeur = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServeur  == -1){
		perror("Error creating socket.\n");
		exit(1);
	}
    struct sockaddr_in addrServeur  = {0};
    addrServeur.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServeur.sin_family = AF_INET;
    addrServeur.sin_port = htons(2002);    

    //Connection de la socket a l'addresse serveur
    bind(socketServeur, (struct sockaddr *)&addrServeur, sizeof(addrServeur));       
    printf("bind : %d \n", socketServeur);

    //Serveur ecoute
    listen(socketServeur, 2);   //file d'attente au max de 2
    printf("Serveur ecoute \n");

    //Creer thread pour chaque client qui se connecte
    pthread_t clients[2];
    for (int i=0; i<2; i++){
        struct sockaddr_in addrClient;
        socklen_t tailleClient = sizeof(addrClient);
        
        //Accepter la connection
        int socketClient = accept(socketServeur, (struct sockaddr*)&addrClient, &tailleClient);
        printf("socketClient : %d \n", socketClient);
        printf("Connexion acceptee \n");
       
        pthread_create(&clients[i], NULL, th_client, (void*)&socketClient);
    }

    //Fermer les clients
    for (int i=0; i<2; i++){
        pthread_join(clients[i], NULL);
    }

    close(socketServeur);
    printf("Fermeture");

    return 0;
}

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
	
	
	
   Resolu:
   	Pourquoi message vide au début
   	Pourquoi décalage affichage
   	transformer structure et mettre message en tant que string (il se peut que ça soit déjà le cas)
   	remplacer scanf() par fgets() car stock un string et donc plusieurs mots. Corriger taille des char envoyés.
   	Message envoyé fait max 8 charactères!!! problème : solution : utiliser BUFFER_SIZE plutot que sizeof(mes). Pourquoi ? je sais pas, mais ça marche mieux comme ça
   	19/04
   	Variable globale pour stocker les messages de tous les utilisateurs. Faire stucture qui est une matrice de messages.
   	Creer la matrice des messages et la remplire à la création
   	Condition d'arrêt du programme (et pourquoi si ctr C du client, message à l'infini du serveur)
   	Pourquoi dans liste_messages le dernier message envoyé écrase les autres ? solution : char***, malloc et strcpy. Par ce que c'est des adresses qu'on modifie et ça fout le bordel vite
 */


#define MAX_CLIENTS 5
#define MAX_MESSAGES 10
#define BUFFER_SIZE 100

int id_client = 0;
//char *liste_messages[MAX_CLIENTS][MAX_MESSAGES];
char*** liste_messages;


//Definir la structure client
typedef struct {
    int id;
    char nom[BUFFER_SIZE];
} User;


void *th_client(void *arg){
    //créer client
    int socket = *(int*) arg;
    User userb; 
    userb.id = id_client;
    id_client = id_client + 1;
    
    //Récupérer nom client
    recv(socket, &userb.nom, BUFFER_SIZE, 0);
    printf("le nom du client est %s et son idenifiant %d \n", userb.nom, userb.id);
    
    //Récupérer les messages du clients, jusqu'à MAX_MESSAGES messages
    for (int num_mes=0; num_mes<MAX_MESSAGES; num_mes++){
    	//char question[] = "Que voulez vous envoyer ?";
    	//send(socket, question, strlen(question)-2,0);  
    	char mes[BUFFER_SIZE];
    	//Couper la communication si pas de reception (càd ctr+C du client) ou si le client dit "fin" 
    	if (recv(socket, &mes, BUFFER_SIZE, 0)==-1 || strcmp(mes, "fin")==0){
    	    break ;
    	}
    	
    	printf("le message du client %s est : %s \n", userb.nom, mes);
    	
    	//Sauvegarder le message dans la liste liste_message
    	//strcpy(liste_messages[userb.id][num_mes], mes);
    	
    	char** liste_ligne = liste_messages[userb.id];
    	strcpy(liste_ligne[num_mes], mes);
      	liste_messages[userb.id] = liste_ligne;
    	printf("liste = %s \n", liste_messages[0][0]);
    	printf("liste = %s \n", liste_messages[0][1]);
 
    	
    	for (int ligne=0; ligne<MAX_CLIENTS; ligne++){
    	    for (int colonne=0; colonne<MAX_MESSAGES; colonne++){
    		printf("liste[%d][%d] = %s \n", ligne, colonne, liste_messages[ligne][colonne]);}}
    }
    printf("finito bebe pour %s \n", userb.nom);
    close(socket);
    //free(arg);      //pas sur de quoi a quoi ca sert
    pthread_exit(NULL);
}


int main(void){
    //Remplir de "rien" le tableau des messages
    /*for (int i=0; i<MAX_CLIENTS; i++){
    	for (int j=0; j<MAX_MESSAGES; j++){
    	    liste_messages[i][j] = (char*) malloc(BUFFER_SIZE * sizeof(char));
	    memset(liste_messages[i][j], 0, BUFFER_SIZE);
    	}
    }*/
    
    liste_messages = malloc(MAX_CLIENTS * sizeof(char**));
    for (int i=0; i<MAX_CLIENTS; i++){
    	liste_messages[i] = malloc(MAX_MESSAGES * sizeof(char*));
    	    for (int j=0; j<MAX_MESSAGES; j++){
    	    	liste_messages[i][j] = malloc(BUFFER_SIZE * sizeof(char));
    	    	memset(liste_messages[i][j], 0, BUFFER_SIZE);}}
   
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
    listen(socketServeur, MAX_CLIENTS);   //file d'attente au max de MAX_CLIENTS
    printf("Serveur ecoute \n");

    //Creer thread pour chaque client qui se connecte
    pthread_t clients[MAX_CLIENTS];
    for (int i=0; i<MAX_CLIENTS; i++){
        struct sockaddr_in addrClient;
        socklen_t tailleClient = sizeof(addrClient);
        
        //Accepter la connection
        int socketClient = accept(socketServeur, (struct sockaddr*)&addrClient, &tailleClient);
        printf("socketClient : %d \n", socketClient);
        printf("Connexion acceptee \n");
       
        pthread_create(&clients[i], NULL, th_client, (void*)&socketClient);
    }

    //Fermer les clients
    for (int i=0; i<MAX_CLIENTS; i++){
        pthread_join(clients[i], NULL);
    }
    
    //Libérer mémoire de liste_messages
    for (int ligne=0; ligne<MAX_CLIENTS; ligne++){
    	    for (int colonne=0; colonne<MAX_MESSAGES; colonne++){
    		free(liste_messages[ligne][colonne]);}}

    close(socketServeur);
    printf("Fermeture\n");

    return 0;
}

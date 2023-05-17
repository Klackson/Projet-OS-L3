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
	mutex liste_messages
	meilleure affichage après la reception
	tenter le ctr+C 
	changer le finito bebe
	
	bonus : couper le serveur
			envoyer des fichiers?
	
	
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
   	Fonction envoie_message qui envoie depuis le serveur à tous les clients -> vérifier qu'elle fonctionne
	Client reçoit les messages -> boucle while(), regarder code Jajou
	Faire une foncction format_message() qui reformate la longueur du message (cf client avant send()) + met les infos de qui l'envoie et à quelle heure. -> faite, regarder où il faut la mettre
	Dans client.c, quand thread_client fermé (car 10 messages envoyés), faire que le thread_reception s'arrête aussi. Idée : créer variable globale et ||cond=!1 dans le if du while, mais pas sur, ou faire une condition et mutex, car en plus faut protéger accès à l'historique.
 */


#define MAX_CLIENTS 5
#define MAX_MESSAGES 10
#define BUFFER_SIZE 100

int id_client = 0;
//char *liste_messages[MAX_CLIENTS][MAX_MESSAGES];
char*** liste_messages;
int liste_sockets[MAX_CLIENTS];
int clients_connect[MAX_CLIENTS];


//Definir la structure client
typedef struct {
    int id;
    char nom[BUFFER_SIZE];
} User;


//Raccourcir les string obtenus avec fgets() pour enlever \0 qui casse tout
char* format_string(char* string){
    int len_string = strlen(string);
    char* string_court = (char*)malloc(len_string);
    strncpy(string_court, string, len_string-1);
    return string_court;
}


void envoyer_message(char* message, int id_source, char* nom_source){
	char str_id[3];
	sprintf(str_id, "%d", id_source);
		
	char* message_cat = malloc(strlen(message) + strlen(nom_source) + strlen("Le message de ") + strlen(str_id) + strlen(", id = , est : "));
    strcat(message_cat, "Le message de ");
    strcat(message_cat, nom_source);
    strcat(message_cat, ", id = ");
    strcat(message_cat, str_id);
    strcat(message_cat, ", est : ");
    strcat(message_cat, message);
    
    for (int i=0; i<id_client; i++){
    	if (i != id_source && clients_connect[i]){
    	    send(liste_sockets[i], message_cat, BUFFER_SIZE, 0);	
    	}
    }	   
    free(message_cat);
}


void *th_client(void *arg){

    //créer client
    int socket = *(int*) arg;
    User userb; 
    userb.id = id_client;
    clients_connect[id_client] = 1;
    
    //Envoyer son id au client
    send(socket, &id_client, 1,0);
    id_client = id_client + 1;
    
    //Récupérer nom client
    recv(socket, &userb.nom, BUFFER_SIZE, 0);
    printf("Le nom du client est %s et son identifiant est %d \n", userb.nom, userb.id);
    
    
    
    //Récupérer les messages du clients, jusqu'à MAX_MESSAGES messages
    for (int num_message=0; num_message<MAX_MESSAGES; num_message++){
    	char message[BUFFER_SIZE];
    	
    	//Couper la communication si pas de reception ou si le client dit "fin" 
    	if (recv(socket, &message, BUFFER_SIZE, 0)<0 || strcmp(message, "fin")==0){
    		clients_connect[userb.id] = 0;
    	    break ;
    	}
    	
    	printf("le message du client %s est : %s \n", userb.nom, message);
    	
    	//Sauvegarder le message dans la liste liste_message
    	char** liste_ligne = liste_messages[userb.id];
    	strcpy(liste_ligne[num_message], message);
      	liste_messages[userb.id] = liste_ligne;
      	      	
      	//Envoyer le message à tout le monde
      	envoyer_message(message, userb.id, userb.nom);
    	
    }
    //Lorsque le client a envoyé 10 messages ou demande la fin de la communication
    printf("finito bebe pour %s \n", userb.nom);          
    pthread_exit(NULL);
}



int main(void){

    //Créer le tableau des messages
    listes_messages = malloc(MAX_CLIENTS * sizeof(char**));
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
	
	//Liste des clients connectés
	for (int i=0; i<MAX_CLIENTS; i++){
		clients_connect[i] = 0;
	}
	
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
        if (socketClient < 0){
        	perror("Erreur connexion\n");
        	exit(1);
        }
        printf("Nouvelle connexion effectuée \n");
        liste_sockets[i] = socketClient;
       
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

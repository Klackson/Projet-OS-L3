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

#define BUFFER_SIZE 100
#define MAX_MESSAGES 10

//Definir la structure qui permet d'envoyer le message
typedef struct{
    char nom[BUFFER_SIZE];
    char message[BUFFER_SIZE]; 
} User;


int main(void){
    //Creer socket client
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient = {0};
    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(2002);
    
    //Connecter socket client au socket serveur
    int id_co = connect(socketClient, (const struct sockaddr*)&addrClient, sizeof(addrClient));
    if (id_co<0) {perror("erreur conection");}

    User userb;

    printf("Quel est votre nom d'utilisateur ? ");
    fgets(userb.nom, BUFFER_SIZE, stdin);
    
    int len_nom = strlen(userb.nom);
    char* nom_court = (char*)malloc(len_nom);
    strncpy(nom_court, userb.nom, len_nom-1);
    
    send(socketClient, nom_court, sizeof(nom_court), 0);
    
    for (int num_mes=0; num_mes<MAX_MESSAGES; num_mes++){
    	//reception d'un message venant du serveur
    	//char question[25];
    	//if (recv(socketClient, question, 25, 0) < 0){perror("erreur reception");}  	
    	//printf("%s\n", question);

    	//Envoie d'un message au serveur
    	printf("Que voulez vous envoyer ? ");
    	fgets(userb.message, BUFFER_SIZE, stdin);
    	
    	int len_message = strlen(userb.message);
    	char* message_court = (char*)malloc(len_message);
    	strncpy(message_court, userb.message, len_message-1);	
    	
    	send(socketClient, message_court, BUFFER_SIZE, 0);
    	if (strcmp(message_court, "fin")==0){
    	    break;
    	}
    	printf("message envoyé \n");
    }
    printf("Déconnexion effectuée \n");
    close(socketClient);
  
    return 0;
}

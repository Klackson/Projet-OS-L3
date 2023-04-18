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
    scanf("%s", userb.nom);
    send(socketClient, &userb.nom, sizeof(userb.nom), 0);
    while(1){
    	//reception d'un message venant du serveur
    	//char question[25];
    	//if (recv(socketClient, question, 25, 0) < 0){perror("erreur reception");}  	
    	//printf("%s\n", question);

    	//Envoie d'un message au serveur
    	printf("Que voulez vous envoyer ? ");
    	scanf("%s", userb.message);	
    	send(socketClient, &userb.message, sizeof(userb.message), 0);
    	if (strcmp(userb.message, "fin")==0){
    	    printf("Déconnexion");
    	    sleep(1);
    	    break;
    	}
    	printf("message envoyé \n");
    }
    close(socketClient);
  
    return 0;
}

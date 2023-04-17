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

#define BUFFER 200

//Definir la structure qui permet d'envoyer le message
typedef struct{
    char nom[30];
    char message[200]; 
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

    User user;
    printf("Quel est votre nom d'utilisateur ? ");
    scanf("%s\n", user.nom);
    send(socketClient, &user.nom, sizeof(user.nom), 0);
    while(1 && user.message!="fin"){
    	//reception d'un message venant du serveur
    	//char question[25];
    	//if (recv(socketClient, question, 25, 0) < 0){perror("erreur reception");}  	
    	//printf("%s\n", question);

    	//Envoie d'un message au serveur
    	printf("Que voulez vous envoyer ? ");
    	scanf("%s \n", user.message);	
    	if(send(socketClient, &user.message, sizeof(user.message), 0)<0){perror("message pas envoyé");}
    	printf("message envoyé \n");
    }
    close(socketClient);
  
    return 0;
}

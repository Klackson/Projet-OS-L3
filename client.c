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


//Definir la structure qui permet d'envoyer le message
typedef struct{
    char nom[30];
    char message[100]; 
} User;


int main(void){
    //Creer socket client
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    printf("socketClient : %d\n", socketClient);	//ca marche
    struct sockaddr_in addrClient = {0};
    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(2002);
    
    //Connecter socket client au socket serveur
    int id_co = connect(socketClient, (const struct sockaddr*)&addrClient, sizeof(addrClient));
    printf("id_co : %d\n", id_co);	//renvoie -1

    User user;
    char question[25];

    //recption d'un message venant du serveur
    int longueur = recv(socketClient, question, 25, 0);
    printf("recv renvoie : %d \n", longueur);	//renvoie -1
    printf("%s\n", question);

    //Envoie d'un message au serveur
    scanf("%29s %99s", user.nom, user.message);
    printf("c'est %s %s \n", user.nom, user.message);	//ca marche, faut avoir un seul espace (donc 2 arguments) quand on tape dans le prompt
    send(socketClient, &user, sizeof(user), 0);
    printf("test \n");

    close(socketClient);
  
    return 0;
}

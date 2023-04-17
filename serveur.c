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
	transformer structure et mettre message en tant que string (il se peut que ça soit déjà le cas)
	remplacer scanf() par fgets() car stock un string et donc plusieurs mots
	comprendre pourquoi serveur.c affiche une ligne inutile apres un rcv() (boucle une fois pour rien en gros)
	comprendre pourquoi si j'enlève la ligne " printf("mes : %s \n", user.message); " ça marche pas. Et pour resoudre ça, essayer de donner une valeur de défaut à user.message. Sauf que le = marche pas. Comprendre pourquoi
 */


#define MAX_CLIENTS 10

//Definir la structure qui permet d'envoyer le message
typedef struct {
    char nom[30];
    char message[100]; 
} User;


void *fonction(void *arg){
    int socket = *(int*) arg;
    User user; 
    recv(socket, &user.nom, sizeof(user.nom), 0);
    printf("le nom du client est : %s\n", user.nom);
    printf("mes : %s \n", user.message);
    while(1 && user.message!="fin"){
    	//char question[] = "Que voulez vous envoyer ?";
    	//send(socket, question, strlen(question)-2,0);  
    	recv(socket, &user.message, sizeof(user.message), 0);
    	printf("le message du client %s est : %s\n", user.nom, user.message);
    }
    close(socket);
    free(arg);      //pas sur de quoi a quoi ca sert
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
    pthread_t threads[2];
    for (int i=0; i<2; i++){
        struct sockaddr_in addrClient;
        socklen_t tailleClient = sizeof(addrClient);
        
        //Accepter la connection
        int socketClient = accept(socketServeur, (struct sockaddr*)&addrClient, &tailleClient);
        printf("socketClient : %d \n", socketClient);
        printf("Connexion acceptee \n");

        printf("Client %d \n", socketClient);

        int *arg = malloc(sizeof(int));
        *arg = socketClient;
        pthread_create(&threads[i], NULL, fonction, arg);
    }

    //Fermer les threads
    for (int i=0; i<2; i++){
        pthread_join(threads[i], NULL);
    }

    close(socketServeur);
    printf("Fermeture");

    return 0;
}

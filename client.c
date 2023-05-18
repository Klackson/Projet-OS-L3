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
#include <signal.h>    

int interruption = 0;
int nb_messages_stock= 0;
char** messages_stock;
#define BUFFER_SIZE 1000
#define MAX_MESSAGES 100
#define limite_msg_interrupt 100 //On peut recevoir jusqu'à 100 messages pendant l'interruption de réception

int interruption = 0;
int nb_messages_stock= 0;
char** messages_stock;
//Definir la structure qui permet d'envoyer le message
typedef struct{
    int id;
    char nom[BUFFER_SIZE];
} User;


char* format_string(char* string){
    int len_string = strlen(string);
    char* string_court = (char*)malloc(len_string);
    strncpy(string_court, string, len_string-1);
    return string_court;
    free(string_court);
}

void suspension(int sig_num){
	*&interruption = 1;
	printf("\nVous avez interrompu la réception de messages jusqu'à ce que vous en envoyiez un nouveau.\nTapez tranquillement votre message ici : ");
	fflush(stdout); // Permet d'attendre la fin du print avant de saisir le message dans le main
	return;
}

void* reception(void* arg){
	int socket_client = *(int*)arg;
	char message_serveur[BUFFER_SIZE];
	while(1){
		if (recv(socket_client, &message_serveur, BUFFER_SIZE, 0)<0){
			perror("erreur reception message du serveur");
			exit(1);
    	}
    	else {
    		if (interruption){
    			strcpy(messages_stock[nb_messages_stock], message_serveur);
    			nb_messages_stock++;
    		}
    		else{
				printf("\n%s \n\nQue voulez vous envoyer ? ", message_serveur);
				fflush(stdout);
			}
    	}
    }
    pthread_exit(NULL);
}

int main(void){

    //Creer socket client
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient = {0};
    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(2002);
    
    //Connecter socket client au socket serveur
    int id_co = connect(socketClient, (const struct sockaddr*)&addrClient, sizeof(addrClient));
    if (id_co<0) {
		perror("erreur connection");
		exit(1);
	}

    User userb;
    //Recoit son id
    recv(socketClient, &userb.id, 1, 0);
    printf("id client : %d \n", userb.id);
	
    //Demander le nom
    printf("Quel est votre nom d'utilisateur ? ");
    fgets(userb.nom, BUFFER_SIZE, stdin);
    
    //Envoie son nom
    char* nom_court = format_string(userb.nom);    
    send(socketClient, nom_court, sizeof(nom_court), 0);
    
    //Créer thread de reception des messages du serveur
    pthread_t th_reception;
    pthread_create(&th_reception, NULL, reception, (void*) &socketClient);
    
    //Création du stock de message arrivant pendant l'interruption
    messages_stock = malloc(limite_msg_interrupt * sizeof(char*));
    for (int i=0; i<limite_msg_interrupt; i++){
    	messages_stock[i] = malloc(BUFFER_SIZE * sizeof(char));
    	memset(messages_stock[i], 0, BUFFER_SIZE);
    }
    
	signal(SIGINT, suspension);
	char message[BUFFER_SIZE];
    
    int num_message=0;
	printf("\nVos messages seront transmis à tous les utilisateurs. \nTapez 'fin' pour vous déconnecter,\nCtrl+c pour mettre la réception de message en pause\nEt 'help' pour un rappel des commandes.\n\n");
    while(1){
  		//Envoi d'un message au serveur
		fflush(stdout);
    	printf("Que voulez vous envoyer ? ");
    	fgets(message, BUFFER_SIZE, stdin);
    	char* message_court = format_string(message);
    	
		//Si on tape "interruption", la réception de message se met en pause
		/*
    	if (strcmp(message_court, "interruption")==0){
			printf("Vous avez interrompu la réception de message jusqu'à ce que vous en envoyiez un nouveau\n");
    		interruption = 1;
    		continue;
    	}*/

		if (strcmp(message_court, "help")==0){
    	    printf("\nTapez 'fin' pour vous déconnecter.\nCTRL + C pour suspendre la réception de messages\n\n");
			continue;
    	}
    	
    	send(socketClient, message_court, BUFFER_SIZE, 0);
    	num_message++;
    	    	
    	if (strcmp(message_court, "fin")==0){
    	    break;
    	}

		printf("message envoyé \n\n");
    	
    	if (interruption){ // Si on a envoyé un message après avoir suspendu la réception de messages, on affiche les messages reçus pendant cette période
			interruption = 0;
			if(nb_messages_stock==0){
				printf("Vous n'avez reçu aucun message pendant l'interruption.\n"); 
				continue;
			}

    		printf("Vous avez reçu %d messages pendant l'interruption :\n",nb_messages_stock);
    		for(int i=0; i<nb_messages_stock; i++){
    			printf("Message %d : %s\n",i+1, messages_stock[i]);
    		}
    		
    		nb_messages_stock=0;
    	}
    }
    
    printf("Déconnexion effectuée \n");
    
    //Libérer mémoire de messages_stock
    for (int i=0; i<limite_msg_interrupt; i++){
    	free(messages_stock[i]);
    }
    
    close(socketClient);
    return 0;
}

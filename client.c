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
#define limite_msg_interrupt 100

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

void* reception(void* arg){
	int socket_client = *(int*)arg;
	char message_serveur[BUFFER_SIZE];
	while(1){
		if (recv(socket_client, &message_serveur, BUFFER_SIZE, 0)<0){
			perror("erreur reception message du serveur");
			break;
    	}
    	else {
    		if (interruption){
    			strcpy(messages_stock[nb_messages_stock], message_serveur);
    			nb_messages_stock++;
    		}
    		else{
				printf("\n%s \n", message_serveur);
				sleep(1);
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
    if (id_co<0) {perror("erreur conection");}

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
    
    //Creer stock de messages
    messages_stock = malloc(limite_msg_interrupt * sizeof(char*));
    for (int i=0; i<limite_msg_interrupt; i++){
    	messages_stock[i] = malloc(BUFFER_SIZE * sizeof(char));
    	memset(messages_stock[i], 0, BUFFER_SIZE);
    }
    
    int num_message=0;
    while(1){
  		//Envoie d'un message au serveur
    	char message[BUFFER_SIZE];
    	printf("Que voulez vous envoyer ? ");
    	fgets(message, BUFFER_SIZE, stdin);
    	char* message_court = format_string(message);
    	
    	if (strcmp(message_court, "interruption")==0){
    		interruption = 1;
    		continue;
    	}
    	
    	send(socketClient, message_court, BUFFER_SIZE, 0);
    	num_message++;
    	    	
    	if (strcmp(message_court, "fin")==0){
    	    break;
    	}
    	
    	if (interruption){
    		printf("Vous avez reçu %d messages pendant l'interruption :\n",nb_messages_stock);
    		for(int i=0; i<nb_messages_stock; i++){
    			printf("Message %d : %s\n",i+1, messages_stock[i]);
    		}
    		interruption = 0;
    		nb_messages_stock=0;
    		for(int i=0; i<limite_msg_interrupt; i++){
				strcpy(messages_stock[i], "");
			}
    	}
    		
    	printf("message envoyé \n");
    }
    printf("Déconnexion effectuée \n");
    
    //Libérer mémoire de messages_stock
    for (int i=0; i<limite_msg_interrupt; i++){
    	free(messages_stock[i]);
    }
    
    close(socketClient);
    return 0;
}

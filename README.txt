# Projet Système : Messagerie instantanée

Ce projet est une application de chat client-serveur en C. Il permet à plusieurs clients de se connecter à un serveur et d'échanger des messages en temps réel.

## Fonctionnalités

- Les clients peuvent se connecter au serveur en spécifiant leur nom d'utilisateur.
- Les messages envoyés par un client sont transmis à tous les autres clients connectés.
- Lorsqu'un client envoie un message, il est affiché à tous les clients, sauf à l'expéditeur lui-même.
- Les clients peuvent se déconnecter en envoyant la commande "fin".
- Les clients peuvent suspendre la réception de messages en appuyant sur Ctrl+C. Les messages reçus pendant la suspension sont stockés et affichés lorsque la réception est reprise.
- Les clients peuvent demander un rappel des commandes disponibles en envoyant la commande "help".

## Instructions d'utilisation

1. Compilez le code serveur en exécutant la commande suivante : gcc serveur.c -lpthread -o serveur

2. Compilez le code client en exécutant la commande suivante : gcc client.c -lpthread -o client
  
3. Exécutez le fichier serveur généré en utilisant la commande suivante : ./serveur
Le serveur se mettra en attente de connexions de clients.

2. Exécutez le fichier client généré en utilisant la commande suivante : ./client   
Le client se connectera automatiquement au serveur et vous serez invité à entrer votre nom d'utilisateur.

3. Une fois connecté, vous pouvez commencer à envoyer et à recevoir des messages.
   - Pour envoyer un message, tapez le message souhaité et appuyez sur Entrée.
   - Pour suspendre la réception de messages, appuyez sur Ctrl+C. Vous pouvez toujours envoyer des messages pendant la suspension.
   - Pour reprendre la réception de messages après la suspension, envoyez simplement un nouveau message.
   - Pour vous déconnecter, envoyez la commande "fin".

## Auteurs
Melvin Gode et Paul Malet


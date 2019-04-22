#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

//~ local
#include "gestiondujeu.h"
#include "navalmap.h"
#include "liste_action.h"

void Genere_fifo (int shipID) {	
	// Création des noms
	char* nom_fifo_pere;
	char* nom_fifo_fils;
	nom_fifo_pere = Genere_nom (shipID, 'P');
	nom_fifo_fils = Genere_nom (shipID, 'F');
	
	// Création des fichiers fifo
	mkfifo (nom_fifo_pere, 0600);
	mkfifo (nom_fifo_fils, 0600);	
	
	// Libère mémoire chaine de caractères
	free (nom_fifo_pere);
	free (nom_fifo_fils);
}

void ia_decision (int shipID, message_tableau_pere* message_in, message_tableau_fils* message_out)
{	
	//Si le bateau a déjà coulé, il ne fait rien
	if (!(message_in[shipID].is_couler ^ 1)) {
		message_out[shipID].action = -1;
		return;
	}
	
	//radar
	if (!(message_in[shipID].is_radar)){ //si le radar est trop vieux ou n'a jamais été réalisé (tour 1)
		message_out[shipID].action = 6;
	}
	
	//attaque sur une position
	else if (distance (message_in[shipID].coord, message_in[shipID].radar) >=2 && distance (message_in[shipID].coord, message_in[shipID].radar) <= 4) {
		message_out[shipID].action = 1;
		message_out[shipID].coord = message_in[shipID].radar;
	}
		
	//déplacement vers position adverse
	else {
		message_out[shipID].action = 4;
		//en haut à droite
		if (message_in[shipID].radar.x >= message_in[shipID].coord.x && message_in[shipID].radar.y >= message_in[shipID].coord.y) {
			message_out[shipID].coord.x = rand()%2 + 1; 
			message_out[shipID].coord.y = rand()%2 + 1;
		}
		//en bas à droite
		if (message_in[shipID].radar.x >= message_in[shipID].coord.x && message_in[shipID].radar.y < message_in[shipID].coord.y) {
			message_out[shipID].coord.x = rand()%2 + 1; 
			message_out[shipID].coord.y = -(rand()%2 + 1);
		}
		//en bas à gauche
		if (message_in[shipID].radar.x < message_in[shipID].coord.x && message_in[shipID].radar.y < message_in[shipID].coord.y) {
			message_out[shipID].coord.x = -(rand()%2 + 1); 
			message_out[shipID].coord.y = -(rand()%2 + 1);
		}
		//en haut à gauche
		if (message_in[shipID].radar.x < message_in[shipID].coord.x && message_in[shipID].radar.y >= message_in[shipID].coord.y) {
			message_out[shipID].coord.x = -(rand()%2 + 1); 
			message_out[shipID].coord.y = rand()%2 + 1;
		}
	}
}

void Fils_Process (int shipID, message_tableau_pere* message_in, message_tableau_fils* message_out) {
	if (fork()) {		
		// Création des noms
		char* nom_fifo_pere;
		char* nom_fifo_fils;
		nom_fifo_pere = Genere_nom (shipID, 'P');
		nom_fifo_fils = Genere_nom (shipID, 'F');
		
		// Lire ce que le père a envoyé
		int fd1 = open (nom_fifo_pere, O_RDONLY);
		read (fd1, &message_in[shipID], sizeof(message_tableau_pere));
		
		// Ecrire les actions à faire au père
		int fd2 = open (nom_fifo_fils, O_WRONLY);
		ia_decision (shipID, message_in, message_out);
		write (fd2, &message_out[shipID], sizeof(message_tableau_fils));
		
		// Fermeture des fichiers descripteurs
		close (fd1);
		close (fd2);
		
		// Libère mémoire de chaine de caractères
		free (nom_fifo_pere);
		free (nom_fifo_fils);
		
		// Fin de traitement du fils
		exit (0);
	}
}

void Pere_Process (int shipID, message_tableau_pere* message_in, message_tableau_fils* message_out,navalmap_t* MAP)
{
	// Création des noms
	char * nom_fifo_pere;
	char * nom_fifo_fils;
	nom_fifo_pere = Genere_nom (shipID, 'P');
	nom_fifo_fils = Genere_nom (shipID, 'F');
	
	// Ecrire message pour le fils
	int fd1 = open (nom_fifo_pere, O_WRONLY);
	Mise_a_jour_data (message_in, MAP, shipID);
	write (fd1, &message_in[shipID], sizeof(message_tableau_pere));
	
	// Lire message du fils
	int fd2 = open (nom_fifo_fils, O_RDONLY);
	read (fd2, &message_out[shipID], sizeof(message_tableau_fils));
	
	// Fermeture des fichiers descripteurs
	close (fd1);
	close (fd2);
	
	// Libère mémoire chaines de caractères
	free (nom_fifo_pere);
	free (nom_fifo_fils);
	
	// Attente du fils
	wait (NULL);
}

void FreeFifo (int shipID)
{
	// Création des noms
	char* nom_fifo_pere;
	char* nom_fifo_fils;
	nom_fifo_pere = Genere_nom (shipID, 'P');
	nom_fifo_fils = Genere_nom (shipID, 'F');
	
	// Supression des fichiers fifo
	remove (nom_fifo_pere);
	remove (nom_fifo_fils);
	
	// Libère mémoire chaines de caractères
	free (nom_fifo_pere);
	free (nom_fifo_fils);
}

char* Genere_nom (int shipID, char fifo_type)
{
	char* name;
	name = malloc (3 * sizeof(char));
	name[0] = fifo_type;
	name[1] = shipID + '!';
	name[2] = '\0';
	return name;
}

void gestion_du_jeu (navalmap_t* MAP)
{
	int i;
	int maxTours = MAP->nbTours;
	message_tableau_pere* message_in;
	message_tableau_fils* message_out;
	
	message_in = calloc (MAP->nbShips, sizeof(message_tableau_pere));
	message_out = calloc (MAP->nbShips, sizeof(message_tableau_fils));
	
	for(i = 0; i < MAP->nbShips; i++) {
		Genere_fifo (i);
	}

	while (MAP->nbTours) { //tant que le nombre de tours n'est pas écoulé, le jeu continue
		printf ("\n\nTOUR : %d\n", (maxTours - MAP->nbTours) + 1);
		MAP->nbTours--;

		//Utilisation du parallélisme
		for (i = 0; i < MAP->nbShips; i++) { 
			Fils_Process (i, message_in, message_out);     
		}
		for (i = 0; i < MAP->nbShips; i++) { 
			Pere_Process (i, message_in, message_out, MAP); 
		} 
		Gestion_des_actions (MAP, message_out, message_in);
		if (fin_du_jeu (message_in, MAP->nbShips) == 1) { //s'il reste qu'un seul bateau, le jeu est fini
			break;
		}
	}
	
	printf ("\n\n** FIN DU JEU **\n");
	for(i = 0; i < MAP->nbShips; i++) {
		FreeFifo (i);
	}
	
	free (message_in);
	free (message_out);
}

void Mise_a_jour_data (message_tableau_pere* P, navalmap_t* MAP, int shipID) {
	P[shipID].coord = MAP->shipPosition[shipID];
	P[shipID].coque = MAP->shipCoque[shipID];
	P[shipID].kerosen = MAP->shipKerosen[shipID];
}

int fin_du_jeu (message_tableau_pere* P, int nbJoueurs) {
	int i;
	int cmp = 0;
	for (i = 0; i < nbJoueurs; i++) { 
		if (!(P[i].is_couler ^ 1)) {
			cmp++; 
		}
	}
	printf ("Nombre de navires coulés : %d\n", cmp);
	if((cmp == (nbJoueurs - 1))) { // s'il reste un survivant
		i = 0;
		while (P[i].is_couler & 1) {
			i++;
		}
		printf ("\n\nEt le gagnant est J%d !\n", i);
		return 1;
	}
	return cmp == nbJoueurs; // s'il ne reste aucun survivant
}

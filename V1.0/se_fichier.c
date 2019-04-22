#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//~ locale
#include "se_fichier.h"
#include "navalmap.h"

fichier ouverture_du_fichier (const char* chemin) {
	fichier f;
	f.acces = O_RDONLY;
	if ((f.descripteur = open (chemin, f.acces)) == -1) {
		fprintf (stderr, "ERREUR : fichier non lisible\n");
		exit (EXIT_FAILURE);
	}
	return f;
}

void lire_fichier (fichier f, char* caractere) {
	if (read (f.descripteur, caractere, 256) == -1) {
		fprintf (stderr, "ERREUR : lecture impossible");
		exit (EXIT_FAILURE);
	}
}

void lire_caractere (char* caractere, char* charType, int* tete_lecture) {
	int j = 0;
	while (caractere[*tete_lecture] != ';') {
		charType[j] = caractere[*tete_lecture];
		j++;
		*tete_lecture = *tete_lecture + 1;
	}
}

int lire_entier (char* caractere, int* tete_lecture) {
	int j = 0;
	char* entier;
	entier = calloc (10, sizeof(char));
	*tete_lecture = *tete_lecture + 1;
	while (caractere[*tete_lecture] != ';' && caractere[*tete_lecture] != '\n') {
		entier[j] = caractere[*tete_lecture];
		j++;
		*tete_lecture = *tete_lecture + 1;
		
	}
	int res = atoi (entier);
	free (entier);
	return res;
}


void fermeture (fichier f) {
	if (close (f.descripteur) == -1) {
		fprintf (stderr, "ERREUR : échec de fermeture :/\n");
		exit (EXIT_FAILURE); 
	}
}


navalmap_t* load_data (navalmap_t* MAP, char* path) {
	coord_t size;
	fichier f;
	map_t mapType;
	int nbJoueur, coque, kerosen, nbTours, tete_lecture; 
	char charType[9];
	char caractere[256];
	
	tete_lecture = 27;
	
	f = ouverture_du_fichier (path);
	lire_fichier (f, caractere);
	fermeture (f);
	lire_caractere (caractere, charType, &tete_lecture);
	if (strcmp (charType, "rectangle")) {
		mapType = MAP_RECT;
	}
	else {
		fprintf (stderr, "ERREUR:carte non reconnue");
		exit (EXIT_FAILURE);
	}
	
	size.x = lire_entier (caractere, &tete_lecture);
	size.y = lire_entier (caractere, &tete_lecture);
	
	tete_lecture = 70;
	
	nbJoueur = lire_entier (caractere, &tete_lecture);
	coque = lire_entier (caractere, &tete_lecture);
	kerosen = lire_entier (caractere, &tete_lecture);
	nbTours = lire_entier (caractere, &tete_lecture);
	
	MAP = init_navalmap (MAP_RECT, size, nbJoueur, coque, kerosen, nbTours);
	
	return MAP;
}

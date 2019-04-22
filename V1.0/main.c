#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

//local
#include "navalmap.c"
#include "nm_rect.c"
#include "se_fichier.c"
#include "gestiondujeu.c"

navalmap_t* initialisation_de_la_carte (navalmap_t * MAP,char * path) {
	initNavalMapLib ();
	MAP = load_data (MAP, path);
	rect_initEntityMap(MAP);
	return MAP;
}

int main (int argc, char ** argv)
{
	//vérifie qu'on a 2 arguments
	if (argc != 2) {
		fprintf (stderr, "ERREUR - ARGUMENTS NON VALIDE : essayez ./Sod fichier\n");
		exit (EXIT_FAILURE);
	}
	
	//initialisation de la carte
	navalmap_t* MAP;
	MAP = initialisation_de_la_carte (MAP, argv[1]);
	
	//place les bateaux au hasard sur la carte
	placeRemainingShipsAtRandom (MAP); 
	/* Si on lit le nom de la fonction, il devrait deja y avoir un bateau : celui du joueur humain. 
	Mais comme nous ne l'avons pas implémenté, on peut se poser des questions sur la présence du mot "Remaining". */
	
	//le jeu
	gestion_du_jeu (MAP); 
	
	//libération de la mémoire de la carte
	free (MAP);
	
	//fin
	exit (EXIT_SUCCESS);
}

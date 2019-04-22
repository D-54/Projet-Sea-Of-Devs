#include "navalmap.h"
#include "gestiondujeu.h"
#include "liste_action.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h> 

#define inMap(x) (x>=0 && x<=10) ? 1 : 0
#define square(x) (x*x)
#define max(x,y) (x>y) ? x : y

typedef struct {
	int* i;
	int id;								// > Les bateaux identifiants du thread
	int Ship_total;
	coord_t coord_in;
	coord_t coord_out;
	navalmap_t* MAP;
	pthread_mutex_t* mutex;
	pthread_cond_t* signal;
} action_t;

void barrier (action_t* action)
{
	int i;
	pthread_mutex_lock (action->mutex);
	(*action->i)++;
	if ((*action->i) < action->Ship_total)	{					// Si tous le monde n'est pas présent, on attend
		pthread_cond_wait (action->signal, action->mutex);		// On attend le signal
	}
	else {
		for (i = 0; i < (*action->i) - 1; ++i)	{ // Tous les threads se mettent en route
			pthread_cond_signal (action->signal);
		}	
		(*action->i) = 0; 					// Remise à zero de la barière
	}
	pthread_mutex_unlock (action->mutex);
}

void* Deplace_navire (void* arg)
{
	action_t* action = (action_t*) arg;
	
	MOV (action->MAP, action->id, action->coord_in);
	
	barrier (action);
	
	return NULL;
}

void* Attaque_navire (void* arg)
{
	action_t* action = (action_t*) arg;
	ATK (action->MAP, action->id, action->coord_in);
	
	//barrier(action); //avec ça, le programme bloque
	
	return NULL;
}

void* Support_navire (void* arg)
{
	action_t* action = (action_t*) arg;
	
	int detect = SCN (action->MAP, action->id);
	
	action->coord_out = action->MAP->shipPosition[detect];
	
	printf("J%d à détecter J%d en (%d,%d)\n", action->id, detect, action->coord_out.x, action->coord_out.y);
	arg = action;
	//barrier(action); //avec ça, le programme bloque
	
	return NULL;
}

void Gestion_des_actions (navalmap_t* MAP, message_tableau_fils* F, message_tableau_pere* P)
{
	// # 0. Déclaration des variables:
	action_t* action;
	pthread_t* tab_thread;
	pthread_mutex_t mutex;
	pthread_cond_t signal;
	int i;
	int s = 0;
	
	// # 1. Allocation de mémoire
	tab_thread = malloc (MAP->nbShips * sizeof(pthread_t));	// > tableau de threads
	action = malloc (MAP->nbShips * sizeof(action_t));		// > structure de donnée permettant de gérer les actions
	for (i = 0; i < MAP->nbShips; i++) {		
		action[i].MAP = malloc (sizeof(navalmap_t));			// > Allocation de mémoire pour sauvegarder les MAP pour chaque threads
		memcpy (action[i].MAP, MAP,sizeof(navalmap_t));		// > Copie de la MAP pour chaque threads
	}
	
	// # 2. Initialisation du mutex et des variables
	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&signal, NULL);
	
	F[0].action = 6;
	int indice_tableau = 0;
	
	for (i = 0; i < MAP->nbShips; ++i) {
		if (F[i].action == 4) {
			action[indice_tableau].coord_in = F[i].coord;
			action[indice_tableau].id = i;
			P[i].is_radar = 0;
			indice_tableau++;
		}
	}
	
	// #3. Déplacement
	for (i = 0; i < indice_tableau; i++) {
		action[i].Ship_total = indice_tableau;
	}
	for (i = 0; i < indice_tableau; i++) {
		action[i].i = &s;
		action[i].MAP = MAP;
		action[i].signal = &signal;
		action[i].mutex = &mutex;
		pthread_create(tab_thread + i, NULL, Deplace_navire, action + i);
	}
	
	for (i = 0; i < indice_tableau; i++) {
		pthread_join (tab_thread[i], NULL); 
		//printf ("D ferme\n"); //affiche que les threads effectuant le déplacement ont bien tous join
	}
	
	int min = indice_tableau;
	
	for (i = 0; i < MAP->nbShips; i++) {
		if(F[i].action == 1) {
			action[indice_tableau].coord_in = F[i].coord;
			action[indice_tableau].id = i;
			P[i].is_radar = 0;
			indice_tableau++;
		}	
	}
	
	// #4. Attaque
	for (i = min ; i < indice_tableau; i++) {
		action[i].Ship_total = indice_tableau;
	}
	s = 0;
	for (i = min; i < indice_tableau; i++) {
		action[i].i = &s;
		action[i].MAP = MAP;
		action[i].signal = &signal;
		action[i].mutex = &mutex;
		pthread_create (tab_thread + i, NULL, Attaque_navire, action + i);
	}
	
	for (i = min; i < indice_tableau; i++) {
		pthread_join( tab_thread[i], NULL);
		// printf("A ferme\n"); //affiche que les threads effectuant l'attaque ont bien tous join
	}
	
	min = indice_tableau;
	
	for (i = 0; i < MAP->nbShips; i++) {
		if (F[i].action == 6) {
			action[indice_tableau].coord_in = F[i].coord;
			action[indice_tableau].id = i;
			P[i].is_radar = 1;
			indice_tableau++;
		}	
	}
	
	for (i = min; i < indice_tableau; i++) {		
		action[i].Ship_total = indice_tableau;
	}
	
	// #5. Radar
	s = 0;	
	for (i = min; i < indice_tableau; i++) {
		action[i].i = &s;
		action[i].MAP = MAP;
		action[i].signal = &signal;
		action[i].mutex = &mutex;
		pthread_create (tab_thread + i, NULL, Support_navire, action + i);
	}
	for (i = min; i < indice_tableau; i++) {
		pthread_join (tab_thread[i], NULL);
		//printf ("S ferme\n"); //affiche que les threads effectuant le radar (scanner) ont bien tous join
	}
	for (i = min; i < indice_tableau; i++) {
		P[action[i].id].radar = action[i].coord_out;
	}

	printf ("#####Statistiques des navires#####\n");
	for (i = 0; i < MAP->nbShips; i++) {
		printf ("J%d : %dC, %dK\n", i, MAP->shipCoque[i], MAP->shipKerosen[i]);
		if (MAP->shipCoque[i] < 0 || MAP->shipKerosen[i] < 0) {
			P[i].is_couler = 1;
		}
	}
	
	// #6. Désallocation de mémoire et cloture des threads
	pthread_mutex_destroy (&mutex);
	pthread_cond_destroy (&signal);

	free (action);
	free (tab_thread);
}

//Radar (Sacanner) 
int SCN (navalmap_t* MAP, const int shipID)
{
	MAP->shipKerosen[shipID] -= 3;
	int k, i = 0;
	int nbShipsTrouver, id = -1;
	int* list = NULL;
	int dist = MAP->size.x + MAP->size.y + 1;
	
	for(k = 1; k < dist; k++) {
		
		list = rect_getTargets (MAP, MAP->shipPosition[shipID], k, &nbShipsTrouver);
		
		if(nbShipsTrouver > 0) {
			k = dist;
			id = list[i];
		}
	}
	free (list);
	return id;
}

//Attaque
void ATK (navalmap_t* nmap, const int shipID, const coord_t target) {
	//si le bateau a assez de kerosen pour attaquer (au-dessus de 5K)
	if (nmap->shipKerosen[shipID] >= 5) {
		nmap->shipKerosen[shipID] -= 5;
		//si la cible est en dehors des limites de la carte
		if (!(inMap(target.x) && inMap(target.y))) {
			printf ("Cible en dehors de la map\n");
			NON (nmap, shipID);
			return;
		}
		int dist = distance (nmap->shipPosition[shipID], target);
		printf ("J%d attaque en (%d, %d)\n", shipID, target.x, target.y);
		//si la cible est à portée de tir
		if ((nmap->map[target.y][target.x].type == ENT_SHIP) && ((dist>=2) && dist<=4)) {
			//si le bateau cible a encore de la ressource (au dessus de 20C)
			if (nmap->shipCoque[nmap->map[target.y][target.x].id] >= 20) {
				nmap->shipCoque[nmap->map[target.y][target.x].id] -= 20;
				printf ("Cible touchée ! J%d prend 20 dégats\n", nmap->map[target.y][target.x].id);
			}
			//si le prochain coup fait couler le bateau cible (en dessous de 20C)
			else if (nmap->shipCoque[nmap->map[target.y][target.x].id] < 20 && nmap->shipCoque[nmap->map[target.y][target.x].id] >= 0) {
				nmap->shipCoque[nmap->map[target.y][target.x].id] = -1;
				nmap->shipKerosen[nmap->map[target.y][target.x].id] = -1;
				printf ("Cible touchée ! J%d coule !\n", nmap->map[target.y][target.x].id);
			}
		}
		//si aucun bateau est présent à l'emplacement indiqué par le radar
		else {
			printf ("Aucun bateau n'est présent à ces coordonées !\n");
		}
	}
	//si le bateau n'a pas assez de kerosen pour attaquer (en dessous de 5K)
	else {
		printf ("J%d n'a pas assez de kerosen pour attaquer\n", shipID);
		NON (nmap, shipID);
	}
}

//Déplacement
void MOV (navalmap_t* nmap, const int shipID, const coord_t moveVec) {
	//si le bateau a assez de kerosen pour bouger (au-dessus de 2K)
	if (nmap->shipKerosen[shipID] >= 2) {
		nmap->shipKerosen[shipID] -= 2;
		moveShip (nmap, shipID, moveVec);
		printf ("J%d se déplace en (%d, %d)\n", shipID, nmap->shipPosition[shipID].x, nmap->shipPosition[shipID].y);
	}
	//si le bateau n'a pas assez de kerosen pour bouger (en-dessous de 2K)
	else {
		printf ("J%d n'a pas assez de kerosen pour se déplacer\n", shipID);
		NON (nmap, shipID);
	}
}

//Charge (non-utilisée)
void BST (navalmap_t* nmap, const int shipID, const coord_t moveVec)
{	//si le bateau a assez de kerosen pour faire une charge (au-dessus de 3K)
	if (nmap->shipKerosen[shipID] >= 3) {
		nmap->shipKerosen[shipID] -= 3;
		if ((nmap->shipPosition[shipID].x == moveVec.x) ||  (nmap->shipPosition[shipID].y == moveVec.y)) {
			if (distance (nmap->shipPosition[shipID], moveVec) >= 4 && distance (nmap->shipPosition[shipID], moveVec) <=5) {
				moveShip (nmap, shipID, moveVec);		
			}
		}	
	}
	//si le bateau n'a pas assez de kerosen pour faire une charge (en-dessous de 3K)
	else {
		printf ("J%d n'a pas assez de kerosen pour réaliser une charge\n", shipID);
		NON (nmap, shipID);
	}
}

//Réparation (non-utilisée)
void RPR (navalmap_t* nmap, const int shipID) {
	//si le bateau a assez de kerosen pour se réparer (au-dessus de 20K)
	if (nmap->shipKerosen[shipID] >= 20) {
		nmap->shipKerosen[shipID] -= 20;
		nmap->shipCoque[shipID] += 25;
	}
	//si le bateau n'a pas assez de kerosen pour se réparer (en-dessous de 20K)
	else {
		printf ("J%d n'a pas assez de kerosen pour récuperer\n", shipID);
		NON (nmap, shipID);
	}
}

// Aucune action (-1K)
void NON (navalmap_t* nmap, const int shipID) {
	printf ("J%d ne fais rien. \n", shipID);
	nmap->shipKerosen[shipID]--;	
}

int distance (const coord_t coord1, const coord_t coord2) { 
	return sqrt(square(abs(coord1.x - coord2.x)) + square(abs(coord1.y - coord2.y))); 
} 

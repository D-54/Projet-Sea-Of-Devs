#ifndef _H_NAVALMAP_
#define _H_NAVALMAP_

// Paire d'entiers représentant entre autre des coordonnées
typedef struct {
	int									x;					//< Premier entier
	int									y;					//< Second entier
} coord_t;

// Type de carte navale
typedef enum {
	MAP_RECT,												//< Carte rectangulaire avec bords
	MAP_TOTAL
} map_t;

// Type d'entité présente sur la carte navale
typedef enum {
	ENT_SEA,												//< Case maritime
	ENT_SHIP,												//< Case présentant un navire
	ENT_TOTAL
} entity_t;

// Entité
typedef struct {
	entity_t							type;				//< Type de l'entité
	int									id;					//< Identifiant de l'entité
} entityid_t;

// Carte navale
typedef struct navalmap {
	map_t								type;				//< Type de la carte
	coord_t								size;				//< Taille de la carte
	int									nbShips;			//< Nombre de navires (joueurs) sur la carte
	int 								nbTours; 			//< Nombre de tours
	coord_t								* shipPosition;		//< Position des navires
	int 								* shipKerosen;		//< Nombre de quantité de Kerosen
	int 								* shipCoque;		//< Nombre de point de coque 
	entityid_t							** map;				//< Carte des entités
	
	// Les pointeurs de fonctions peuvent être utilisés de la manière suivante :
	//	navalmap_t * nm;
	//	nm->initEntityMap (nm);
	//	nm->isMovePossible (nm, 4, pos);
	// Pour plus d'informations, reportez-vous aux définitions de ces fonctions dans les fichiers nm_*.h
	void (* initEntityMap) (struct navalmap *);				//< Fonction initialisant les entités présentes sur la carte
	int (* isMovePossible) (struct navalmap *, const int, const coord_t);
															//< Fonction indiquant si un mouvement est possible
	int * (* getTargets) (struct navalmap *, const coord_t, const int, int *);
															//< Fonction indiquant les entités situées à distance donnée d'une cible
} navalmap_t;

// Initialisation de la bibliothèque
// Génération de la graine aléatoire à partir de time()
void initNavalMapLib ();

// Initialisation d'une carte navale
// Création de la structure de la carte navale, les navires ne sont pas placés sur la carte
// \param								mapType				Type de la carte
// \param								mapSize				Taille de la carte
// \param								nbShips				Nombre de navires
// \return													Carte navale générée	
navalmap_t * init_navalmap (
	const map_t							mapType,
	const coord_t						mapSize,
	const int							nbShips,
	const int 							coque,
	const int 							kerosen,
	const int 							tours);

// Suppression de la carte navale
// Désallocation de la mémoire utilisée par la structure de la carte navale
// \param								nmap				Carte à désallouer
void free_navalmap (
	navalmap_t							* nmap);

// Déplacement d'un navire sur la carte
// Le navire donné se déplace en fonction d'un vecteur (a, b), la nouvelle position devient donc (pos.x + a, pos.y + b)
// \param								nmap				Carte navale
// \param								shipID				Identifiant de navire
// \param								moveVec				Vecteur de déplacement
void moveShip (
	navalmap_t							* nmap,
	const int							shipID,
	const coord_t						moveVec);

// Place aléatoirement les navires non placés
// L'algorithme choisit aléatoirement une position, et vérifie qu'elle n'est pas déjà prise avant de l'affecter à un navire
// \param								nmap				Carte navale
void placeRemainingShipsAtRandom (
	navalmap_t							* nmap);

// Place un navire donné sur une position donnée
// Si le navire ne peut pas être placé à la position donnée, alors rien ne se passe
// \param								nmap				Carte navale
// \param								shipID				Identifiant de navire
// \param								pos					Position souhaitée du navire
void placeShip (
	navalmap_t							* nmap,
	const int							shipID,
	const coord_t						pos);

#include "nm_rect.h"
#endif

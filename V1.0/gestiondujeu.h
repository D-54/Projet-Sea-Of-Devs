#include "navalmap.h"

/* Voici l'ensemble des structures permettant de gérer les communications entre le père et le fils
 * 
 * Les structures de données sont envoyées à travers des tubes nommées:
 * -> Un fichier fifo permettant aux père d'écrire un message aux fils
 * -> Un fichier fifo permettant aux fils d'écrire un message pour le père
 */

/* 0. Structure permetant de sauvegarder les messages du père à envoyer aux fils */
typedef struct {
	int is_couler;				// > envoie au fils s'l n'a plus de vie
	int is_radar;				// > vérifie si le radar est trop vieux
	coord_t coord;				// > position du navire dans la map
	coord_t radar;				// > position du navire détectée avec le radar
	int coque;
	int kerosen;
} message_tableau_pere;

/* 1. Structure permettant de sauvegarder les messages du fils à envoyer au père */
typedef struct {
	int action;					// > action que doit effectuer le navire
	coord_t coord; 				// > coordonées d'attaque ou de déplacement
} message_tableau_fils;


/* Voici les fonctions utilisées dans la gestiondujeu.c */

/* Fonction de la gestion des traitements entre fils et père
 * @param la map pour les fonctions utilisées
 */
void gestion_du_jeu (navalmap_t* MAP);

/* 0. Générer les 2 * le nombre de bateaux de fichiers fifo */

/* Génère 2 fichiers fifo
 * @param l'identifiant du navire pour générer un fichier unique en fonction de l'identifiant (cette fonction peut générer jusqu'à 94 fichiers aux noms différents (voir la table ascii, on part de ' ' jusqu'à '~'))
 */
void Genere_fifo (int shipID);

/* 1. Traitement du fils */

/* Processus du fils
 * 
 * Le fils va :
 * -> Lire ce que le père lui à envoyer 
 * -> Determiner un action en fonction de ce que le père à envoyer
 * -> Renvoyer l'action à faire au père
 * 
 * Le processus fils va ouvrir un fichier fifo et va attendre ce que le père va lui envoyer. 
 * Il va ensuite ouvrir un autre fichier et écrire un message à transmettre au père.
 * @param l'identifiant du navire dont ses actions sont traitées 
 */
void Fils_Process (int shipID, message_tableau_pere* message_in, message_tableau_fils* message_out);

/* 2. Traitement du père */

/* Processus du père
 * 
 * Le père va :
 * -> Donner les informations nécessaire pour le fils
 * -> Va éxecuter les actions décidées par le fils
 * 
 * Le processus du père va ouvrir un fichier fifo et va écrire un message pour le fils. 
 * Il va ensuite ouvrir un autre fichier et va attendre le message des ses fils.
 * @param l'identifiant du navire 
 */
void Pere_Process (int shipID, message_tableau_pere* message_in, message_tableau_fils* message_out, navalmap_t* MAP);

/* 3. Supression des fichier */

/* Supprime les fichiers fifos
 * @param l'identifiant du navire qui représente l'identité unique du fichier à supprimer
 */
void FreeFifo (int shipID);

/* Fonction permettant de générer les noms des fichiers 
 * Génère un nom unique
 * @param le nom du fichier généré
 * @param l'identifiant du navire
 * @param une lettre permettant de générer un fichier père ('P') ou un fichier fils ('F')
 */
char* Genere_nom (int shipID, char fifo_type);

/* Mise à jour des données à envoyer aux fils
 * @param P le tableau de message
 * @param MAP où se trouve les informations de la carte à transmettre aux fils
 * @param shipID qui est l'identifiant du navire
 */
void Mise_a_jour_data (message_tableau_pere* P, navalmap_t* MAP, int shipID);

/* Gestion de la décision de l'ordinateur 
 * @param l'identifiant du navire
 * @param message du père
 * @param message du fils à éditer
 */
void ia_decision (int shipID, message_tableau_pere* message_in, message_tableau_fils* message_out);

/* Permet de déterminer la fin du jeu
 * @param P qui est le message du père afin de déterminer si les bateaux ont coulé
 * @param nbJoueurs est le nombre de joueurs pour vérifié s'ils ont coulé
 * @return Booleens si c'est la fin ou non
 */
int fin_du_jeu (message_tableau_pere* P, int nbJoueurs);

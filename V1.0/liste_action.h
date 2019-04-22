/* Fichier de gestion des actions envoyées par les navires */

/* 0. Gestion des actions à faire */

/* Fonction principal pour la gestion des actions */
/* Cette fonction permet la gestion des actions
 * @param MAP la map ou on va faire les actions
 * @param tableau de message envoyé par les fils
 */
void Gestion_des_actions (navalmap_t* MAP, message_tableau_fils* F, message_tableau_pere* P);

/* 1. Liste des actions possible */

/* Fonction de déplacement voir fonction moveship dans navalmap...
 * (voir glossaire pour plus de précision)
 */
void MOV (navalmap_t* nmap, const int shipID, const coord_t moveVec);

/* Fonction d'attaque (voir glossaire pour plus de précision)
 * @param nmap la map ou se fait les attaques
 * @param shipID est l'identifiant du navire qui veut attaquer
 * @param target qui est la cible à attaquer
 */
void ATK (navalmap_t* nmap, const int shipID, const coord_t target);

/* Fonction de radar (voir glossaire pour plus de précision)
 * @param MAP la carte
 * @param shipID identifiant du navire
 */
int SCN (navalmap_t* MAP, int shipID);


/* Cette fonction permet de connaitre l'emplacement d'un navire (elle utilise la bibliothèque math)
 * @param coordonnée de la position de départ
 * @param coordonnée de la position cible
 */
int distance (const coord_t coord1, const coord_t coord2);

/* Fonction "Aucune action" 
 * @param MAP la carte
 * @param shipID : l'identifiant du bateau
 */
void NON (navalmap_t* nmap, const int shipID);

/* Fonction "Réparation" 
 * @param MAP la carte
 * @param shipID : l'identifiant du bateau
 */
void RPR (navalmap_t* nmap, const int shipID);

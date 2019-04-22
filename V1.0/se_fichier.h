#include "navalmap.h"

typedef struct FICHIER {
	int descripteur;	//> contient le descripteur
	int acces;			//> contient l'accès au fichier 
} fichier;

// Ouvre le fichier fichier
// \param chemin chemin du fichier
// \return fichier
fichier ouverture_du_fichier (const char* chemin);

// Copie le fichier dans une chaine de caractères
// \param f fichier contenant l'accès et le descripteur
// \param caractère où sera copié le fichier
void lire_fichier (fichier f, char* caractere);

// Récupère une chaine de caractères (jusqu'à atteindre ';')
// \param caractère où se trouve le fichier copié
// \param charType où sera sauvegardé la chaine de caractères souhaité
// \param tete_lecture est la tête de lecture à partir duquel on lit le fichier 
void lire_caractere (char* caractere, char* charType, int* tete_lecture);

// Récupère un entier (jusqu'à atteindre ';' ou '/n')
// \param caractère où se trouve le fichier copié
// \param tete_lecture est la tête de lecture à partir duquel on lit le fichier
// \return entier res où se trouve l'entier extrait 
int lire_entier (char* caractere, int* tete_lecture);

// Ferme le fichier
// \param f où se trouve le descripteur du fichier à fermer
void fermeture (fichier f);

// Chargeur de données à partir du fichier fichier
// Cette fonction récupère les données dans le fichier 
// \param navalmap_t Carte à initialiser
// \param path chemin du fichier à charger
// \return navalmap_t Carte initialisé 
navalmap_t* load_data (navalmap_t* MAP, char* path);

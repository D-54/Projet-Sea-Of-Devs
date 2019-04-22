// Initialise la carte avec des entités maritimes (ENT_SEA)
// \param								nmap				Carte navale
void rect_initEntityMap (
	navalmap_t							* nmap);

// Test si le mouvement demandé est possible
// Le navire donné tente de se déplacer en fonction d'un vecteur (a, b), la nouvelle position devient donc (pos.x + a, pos.y + b)
// \param								nmap				Carte navale
// \param								shipID				Identifiant de navire
// \param								moveVec				Vecteur de déplacement
// \return													1 si le déplacement est possible, 0 sinon
int rect_isMovePossible (
	navalmap_t							* nmap,
	const int							shipID,
	const coord_t						moveVec);

// Obtenir les navires présents à une distance donnée (>= 0) d'une position cible
// Les navires sont recensés par leurs identifiants
// \param								nmap				Carte navale
// \param								pos					Position cible
// \param								dist				Distance à partir de la position cible
// \param 								nbShips				Nombre de navires trouvés
// \return													Identifiants des navires trouvés	
int * rect_getTargets (
	navalmap_t							* nmap,
	const coord_t						pos,
	const int							dist,
	int									* nbShips);

#include "navalmap.h"
#include <stdlib.h>
#include <stdio.h>

void rect_initEntityMap (
	navalmap_t							* nmap) {
	int									i, j;
	for (j = 0; j < nmap->size.y; ++j)
		for (i = 0; i < nmap->size.x; ++i) {
			nmap->map [j][i] .type = ENT_SEA;
			nmap->map [j][i] .id = 0;
		}
}

int rect_isMovePossible (
	navalmap_t							* nmap,
	const int							shipID,
	const coord_t						moveVec) {
	if (shipID >= nmap->nbShips) return 0;

	return (nmap->shipPosition [shipID] .x + moveVec.x >= 0
		 && nmap->shipPosition [shipID] .x + moveVec.x < nmap->size.x
		 && nmap->shipPosition [shipID] .y + moveVec.y >= 0
		 && nmap->shipPosition [shipID] .y + moveVec.y < nmap->size.y);
}

int * rect_getTargets (
	navalmap_t							* nmap,
	const coord_t						pos,
	const int							dist,
	int									* nbShips) {
	int									j = pos.y + dist, i = pos.x;
	int									* list = NULL;
	* nbShips = 0;

	if (dist == 0) {
		if (nmap->map [pos.y][pos.x] .type == ENT_SHIP) {
			list = malloc (sizeof (int) );
			* list = nmap->map [pos.y][pos.x] .id;
			++ * nbShips;
			return list;
		}

		return list;
	}

	int									* tmp = malloc (4 * sizeof (int) );
	int									k;

	for (k = 0; k < dist; ++k, ++i, --j) {
		if (i < 0 || i >= nmap->size.x || j < 0 || j >= nmap->size.y) continue;
		if (nmap->map [j][i] .type == ENT_SHIP) {
			tmp [* nbShips] = nmap->map [j][i] .id;
			++ * nbShips;
		}
	}
	for (k = 0; k < dist; ++k, --i, --j) {
		if (i < 0 || i >= nmap->size.x || j < 0 || j >= nmap->size.y) continue;
		if (nmap->map [j][i] .type == ENT_SHIP) {
			tmp [* nbShips] = nmap->map [j][i] .id;
			++ * nbShips;
		}
	}
	for (k = 0; k < dist; ++k, --i, ++j) {
		if (i < 0 || i >= nmap->size.x || j < 0 || j >= nmap->size.y) continue;
		if (nmap->map [j][i] .type == ENT_SHIP) {
			tmp [* nbShips] = nmap->map [j][i] .id;
			++ * nbShips;
		}
	}
	for (k = 0; k < dist; ++k, ++i, ++j) {
		if (i < 0 || i >= nmap->size.x || j < 0 || j >= nmap->size.y) continue;
		if (nmap->map [j][i] .type == ENT_SHIP) {
			tmp [* nbShips] = nmap->map [j][i] .id;
			++ * nbShips;
		}
	}
	
	if (* nbShips == 0) {
		free (tmp);
		return list;
	}

	list = malloc (* nbShips * sizeof (int) );
	for (k = 0; k < * nbShips; ++k) {
		list [k] = tmp [k];
	}

	free (tmp);

	return list;
}


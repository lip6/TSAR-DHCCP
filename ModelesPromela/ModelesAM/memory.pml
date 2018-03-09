// modifier , différent que le modèle dans test_plat1 et test_plat2...//


//////////////////////////////////////////////////
////////// minim _model.h ////////////////////////
//////////////////////////////////////////////////

// modele des composant minimaux représentant   //
// la mémoire et qui seront utile pour tester   //

#ifndef _MEMORY_PML_
#define _MEMORY_PML_

#include "defs.h"


//////////////////////////////////////////////////
// modèle à placer à la place de la mémoire     //
// voir rapport de spécification...             //
//////////////////////////////////////////////////

// utiliser pour les testes du controleur mémoire
// et comme composant mémoire dans les plates
// formes.


proctype minim_MEM(chan MCMEMDTREQ, MEMMCDTACK){


	msg m ;
	byte v_x = 0;
	byte v_y = 0;

// Mode IDLE du controleur mémoire.

MEM_IDLE :

	MCMEMDTREQ ? m -> if
			:: ((m.type == GET) && (m.addr == X) ) -> goto GET_X ;
			:: ((m.type == GET) && (m.addr == Y) ) -> goto GET_Y ;
			:: ((m.type == PUT) && (m.addr == X) ) -> goto PUT_X ;
			:: ((m.type == PUT) && (m.addr == Y) ) -> goto PUT_Y ;
			:: else -> printf("mimin_MEM : Requete invalide MCMEMDTREQ %d", m.type);
			fi;

// Lecture de X.
GET_X :

	m.type = ACK_GET ;
	m.addr = X ;
	
	// aquitement.

	MEMMCDTACK ! m ;
	goto MEM_IDLE ;

// lecture de Y

GET_Y :

	m.type = ACK_GET ;
	m.addr = Y ;
	
	// aquitement.

	MEMMCDTACK ! m ;
	goto MEM_IDLE ;

// PUT de X

PUT_X :

	m.type = ACK_PUT ;
	m.addr = X ;
	
	// aquitement.

	MEMMCDTACK ! m ;
	goto MEM_IDLE ;


// PUT de Y

PUT_Y :

	m.type = ACK_PUT ;
	m.addr = Y ;
	
	// aquitement.
*/
	MEMMCDTACK ! m ;
	goto MEM_IDLE ;
}

///////////////////////////////////////////////////
//       Processeur a commande aléatoire         //
///////////////////////////////////////////////////

// processeur utilisé dans les plates formes comme
// un processeur qui génère des requetes aléatoire


#endif

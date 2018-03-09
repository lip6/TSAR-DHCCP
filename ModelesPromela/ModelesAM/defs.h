#ifndef _LIB_PROJECT_H
#define _LIB_PROJECT_H

/************************************************************************/
/*************************** Structures *********************************/
/************************************************************************/

typedef msg {
	mtype type ;	// type du message.
	bit addr;	// adresse du case mémoire.
	byte cache_id;	// identifiant du processeur.
};

#define CACHE_TH 2
#define NB_CACHES 3

/******************** Definition des 2 valeurs X et Y ********************/

#define X 0
#define Y 1


/************* Definition des 2 valeurs INVALIDE et VALIDE ***************/

#define INVALIDE 0
#define VALIDE 1


/********** Type de message dans les canaux de communications ***********/

mtype = { DT_RD, DT_WR, ACK_DT_RD, ACK_DT_WR, RD, WR, ACK_RD, ACK_WR, CLNUP, ACK_CLNUP, B_INV, M_INV, M_UP, ACK_B_INV, ACK_M_INV, ACK_M_UP, GET, PUT, ACK_GET, ACK_PUT };

///////////////////////////////////////////////////////////
//       Message interne au controleur mémoire           //
// UP_REQ_MC : Un update était transmis pour le système  //
// ACK_UP_REQ_MC : Tous les caches ont recue l'update    //
// INV_REQ_WR_MC : Invalidation est transmis au système  //
// ACK_REQ_WR_MC : tous les caches ont recue l'INV       //
///////////////////////////////////////////////////////////



#endif

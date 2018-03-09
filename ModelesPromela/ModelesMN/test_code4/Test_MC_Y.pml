///////////////////////////////////////
////// Test du controleur mémoire /////
///////////////////////////////////////

// Automate pour l'adresse Y.

#include "minim_model.h"
#include "Memory_controler.h"

init {

	int cpt = 0;
	chan L1MCDTREQ  = [1] of {msg} ;
	chan MCL1DTACK  = [1] of {msg} ;
	chan L1MCCUREQ  = [1] of {msg} ;
	chan MCL1CUACK  = [1] of {msg} ;
	chan MCL1CPREQ  = [1] of {msg} ;
	chan L1MCCPACK  = [1] of {msg} ;
	chan MCMEMDTREQ = [1] of {msg} ;
	chan MEMMCDTACK = [1] of {msg} ;


	
	run MC_Y(L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK);
	run minim_MEM(MCMEMDTREQ, MEMMCDTACK);
	run minim_L1(L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK , Y);

	do
	:: if
		:: (cpt == 600 ) -> break ;
		:: else -> cpt = cpt+1 ;
	fi ;
	od;

}

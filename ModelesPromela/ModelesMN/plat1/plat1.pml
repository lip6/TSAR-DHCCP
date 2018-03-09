///////////////////////////////////////////////
////// Plat forme à 1 proc + 1 CM + 1 MEM /////
///////////////////////////////////////////////

// simulation + vérification des propriétés suivantes :
// p1. abscence de bloquage.
// p2. RD after WR sur X et Y sera éffectuer sur la dernière valeur écrite.
// p3. Tous les successions des RD sans WR entre eux , retourne la meme valeur.

#include "minim_model.h"
#include "Memory_controler.h"
#include "obs.h"
#include "CacheL1.h"

init {

	//int cpt = 0;

	chan PL1DTREQ   = [1] of {msg} ;
	chan L1PDTACK   = [1] of {msg} ;
	chan L1MCDTREQ  = [1] of {msg} ;
	chan MCL1DTACK  = [1] of {msg} ;
	chan L1MCCUREQ  = [1] of {msg} ;
	chan MCL1CUACK  = [1] of {msg} ;
	chan MCL1CPREQ  = [1] of {msg} ;
	chan L1MCCPACK  = [1] of {msg} ;
	chan MCMEMDTREQ = [1] of {msg} ;
	chan MEMMCDTACK = [1] of {msg} ;


	atomic {
	//processeur.
	run proc(PL1DTREQ, L1PDTACK) ;
	// Cache L1.
	run CacheL1 (PL1DTREQ, L1PDTACK, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK , 0) ;
	// CM pour X.
	run MC_X(L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK);
	// CM pour Y.
	run MC_Y(L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK);
	// Memoire.
	run minim_MEM(MCMEMDTREQ, MEMMCDTACK);
	// Observateurs p2 pour X.
	//run obs_rd_after_wr(PL1DTREQ, L1PDTACK, X) ;
	// Observateurs p2 pour Y.
	//run obs_rd_after_wr(PL1DTREQ, L1PDTACK, Y) ;
	// Observateurs p3 pour X.
	//run obs_rd_after_rd(PL1DTREQ, L1PDTACK, X) ;
	// Observateurs p3 pour Y.
	//run obs_rd_after_rd(PL1DTREQ, L1PDTACK, Y) ;

	skip 
	};

	// boucle infinie.
	do
	:: (1) -> skip ;
	od;

}

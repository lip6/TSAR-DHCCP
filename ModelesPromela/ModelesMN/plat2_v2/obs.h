#ifndef _OBS_H
#define _OBS_H

#include "lib_project.h"

//////////////////////////////////////////////////
// Observateurs pour plat-forme 1               //
// 1 proc + 1 Cache + 1 CM + MEM                //
//////////////////////////////////////////////////


//////////////////////////////////////////////////
// Observateur pour vérifier les RD after WR    //
// Chaque lecture après écriture sera éffectuer //
// sur la dernière valeur écrite .              //
//////////////////////////////////////////////////

chan chan_empty   = [1] of {bit} ;

// propriété non vérifier, l'observateur se bloque.

proctype obs_rd_after_wr(chan OBSREQ, OBSACK; bit addr) {

	msg m   ;
	byte val ;
	bit err_wr = 1 ;

////////////////////
// Etat de départ //
////////////////////

Start :
	
	OBSREQ ? m ->   if
			:: ((m.type == DT_WR) && (m.addr == addr)) -> goto OBS_ATT_WR ;
			:: ((m.type == DT_RD) && (m.addr == addr)) -> OBSACK ? m ; goto Start ;	
			:: else -> goto Start ;
			fi ;

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////

OBS_ATT_WR :

	OBSACK ?  m -> 
		if
		:: ((m.type == ACK_DT_WR) && (m.addr == addr)) -> val = m.val ; goto OBS_REQ ;
		:: else -> err_wr = 0 ; assert (err_wr == 1) ; goto OBS_ATT_WR ;
		fi;


////////////////////////////////
// Observation des requetes   //
////////////////////////////////

OBS_REQ :

	do
	:: OBSREQ ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> goto OBS_ATT_WR ;
		:: else -> err_wr = 0 ; assert (err_wr == 1) ; goto OBS_REQ ;
		fi ;
	od ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD :

	OBSACK ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr))->
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> err_wr = 0 ; assert (err_wr == 1) ; goto OBS_ERROR ;
			fi;
		fi;

//////////////////////////////////////////////
// Erreur : propriété echoue                //
//////////////////////////////////////////////

OBS_ERROR :

	err_wr = 0 ; assert (err_wr == 1) ;
	chan_empty ? 1 ;

}

//////////////////////////////////////////////////
// Observateur pour vérifier les RD succesif    //
// Tous les RD succesive sans un WR entre eux   //
// retourne la meme valeur                      //
//////////////////////////////////////////////////

// propriété non vérifier, l'observateur se bloque.

proctype obs_rd_after_rd(chan OBSREQ, OBSACK; bit addr) {

	msg m   ;
	byte val ;
	bit err_rd = 1 ;

////////////////////
// Etat de départ //
////////////////////

Start :
	
	OBSREQ ? m ->
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_RD_1 ;
		:: else -> OBSACK ? m ; goto Start ; // on consomme ACK.
		fi ;

////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_1 :

	OBSACK ? m -> 
		if
		:: ((m.type == ACK_DT_RD)&& (m.addr == addr)) -> val = m.val ; goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi ;


//////////////////////////////////////
// Observation des requetes         //
//////////////////////////////////////

OBS_REQ :

	OBSREQ ? m ->
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> OBSACK ? m ; goto Start ; // je consomme l'aquitement sur l'�criture.
		:: else -> goto OBS_ERROR ;
		fi ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD :

	OBSACK ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr))-> 
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> goto OBS_ERROR ;
			fi;
		:: else -> goto OBS_ERROR ;
		fi ;

//////////////////////////////////////////////
// Erreur : propriété echoue                //
//////////////////////////////////////////////

OBS_ERROR :
	
	err_rd = 0 ; assert (err_rd == 1) ;
	chan_empty ? 1 ;

}

//////////////////////////////////////////////////
// Observateurs pour plat-forme 2               //
// 3 proc + 3 Cache + 1 CM + MEM                //
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Observateur pour vérifier les RD after WR    //
// Chaque lecture après écriture sera éffectuer //
// sur la dernière valeur écrite .              //
//////////////////////////////////////////////////

// propriété non vérifier, l'observateur se bloque.

proctype obs_rd_after_wr_plat2(chan OBSREQP1, OBSACKP1, OBSREQP2, OBSACKP2, OBSREQP3, OBSACKP3; bit addr) {

	msg m   ;
	byte val ;
	bit err_wr_p2 = 1 ;

////////////////////
// Etat de départ //
////////////////////

Start :
	
	do
	:: OBSREQP1 ? m -> 
		if
		:: ((m.type == DT_WR) && (m.addr == addr))-> goto OBS_ATT_WR1 ;
		:: else -> OBSACKP1 ? m ; goto Start ;
		fi;
	:: OBSREQP2 ? m ->
		if
		:: ((m.type == DT_WR) && (m.addr == addr))-> goto OBS_ATT_WR2 ;
		:: else -> OBSACKP2 ? m ; goto Start ;
		fi;
	:: OBSREQP3 ? m ->
		if
		:: ((m.type == DT_WR) && (m.addr == addr))-> goto OBS_ATT_WR3 ;
		:: else -> OBSACKP3 ? m ; goto Start ;
		fi;
	od ;

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////


OBS_ATT_WR1 :

	OBSACKP1 ? m ->
		if 
		:: ((m.type == ACK_DT_WR ) && (m.addr == addr))-> val = m.val ; goto OBS_REQ ;
		:: else -> goto OBS_ATT_WR1 ;
		fi;

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////

OBS_ATT_WR2 :

	OBSACKP2 ? m ->
		if 
		:: ((m.type == ACK_DT_WR ) && (m.addr == addr))-> val = m.val ; goto OBS_REQ ;
		:: else -> goto OBS_ATT_WR2 ;
		fi;

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////

OBS_ATT_WR3 :

	OBSACKP3 ? m ->
		if 
		:: ((m.type == ACK_DT_WR ) && (m.addr == addr))-> val = m.val ; goto OBS_REQ ;
		:: else -> goto OBS_ATT_WR3 ;
		fi;

////////////////////////////////
// Observation des requetes   //
////////////////////////////////

OBS_REQ :

	do
	:: OBSREQP1 ? m ->
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD1 ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> goto OBS_ATT_WR1 ;
		:: else -> goto OBS_REQ ;
		fi ;
	:: OBSREQP2 ? m ->
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD2 ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> goto OBS_ATT_WR2 ;
		:: else -> goto OBS_REQ ;
		fi ;
	:: OBSREQP3 ? m ->
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD3 ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> goto OBS_ATT_WR3 ;
		:: else -> goto OBS_REQ ;
		fi ;
	od ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD1 :


	OBSACKP1 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) ->
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> goto OBS_ERROR ;
			fi;
		:: else -> goto OBS_ATT_RD1 ;
		fi ;


//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD2 :

	OBSACKP2 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) ->
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> goto OBS_ERROR ;
			fi;
		:: else -> goto OBS_ATT_RD2 ;
		fi ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD3 :

	OBSACKP3 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) ->
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> goto OBS_ERROR ;
			fi;
		:: else -> goto OBS_ATT_RD3 ;
		fi ;
	
//////////////////////////////////////////////
// Erreur : propriété echoue                //
//////////////////////////////////////////////

OBS_ERROR :

	err_wr_p2 = 0 ; assert (err_wr_p2 == 1) ;
	chan_empty ? 1 ;

}



//////////////////////////////////////////////////
// Observateur pour vérifier les RD succesif    //
// Tous les RD succesive sans un WR entre eux   //
// retourne la meme valeur                      //
//////////////////////////////////////////////////

// propriété non vérifier, l'observateur se bloque.

proctype obs_rd_after_rd_plat2(chan OBSREQ1, OBSACK1, OBSREQ2, OBSACK2, OBSREQ3, OBSACK3; bit addr) {

	msg m   ;
	byte val ;
	bit err_rd_p2 = 1 ;

////////////////////
// Etat de départ //
////////////////////

Start :

	do
	:: OBSREQ1 ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_RD_1 ;
		:: else -> OBSACK1 ? m ; goto Start ; // on acquite lecriture.
		fi; 
	:: OBSREQ2 ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_RD_2 ;
		:: else -> OBSACK2 ? m ; goto Start ; // on acquite lecriture.
		fi;
	:: OBSREQ3 ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_RD_3 ;
		:: else -> OBSACK3 ? m ; goto Start ; // on acquite lecriture.
		fi;
	od ;

	

////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_1 :

	OBSACK1 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) -> val = m.val ; goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi ;

////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_2 :

	OBSACK2 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) -> val = m.val ; goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi ;


////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_3 :

	OBSACK3 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) -> val = m.val ; goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi ;


//////////////////////////////////////
// Observation des requetes         //
//////////////////////////////////////

OBS_REQ :

	do
	:: OBSREQ1 ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD1 ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> OBSACK1 ? m ; goto Start ;
		:: else -> goto OBS_ERROR ;
		fi ;
	:: OBSREQ2 ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD2 ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> OBSACK2 ? m ; goto Start ;
		:: else -> goto OBS_ERROR ;
		fi ;
	:: OBSREQ3 ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == addr)) -> goto OBS_ATT_RD3 ;
		:: ((m.type == DT_WR) && (m.addr == addr)) -> OBSACK3 ? m ; goto Start ;
		:: else -> goto OBS_ERROR ;
		fi ;
	od ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD1 :

	OBSACK1 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) -> 
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> goto OBS_ERROR ;
			fi;
		:: else -> goto OBS_ERROR ;
		fi ;


//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD2 :

	OBSACK2 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) -> 
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> goto OBS_ERROR ;
			fi;
		:: else -> goto OBS_ERROR ;
		fi ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD3 :

	OBSACK3 ? m ->
		if
		:: ((m.type == ACK_DT_RD) && (m.addr == addr)) -> 
			if
			:: (m.val == val) -> goto OBS_REQ ;
			:: else -> goto OBS_ERROR ;
			fi;
		:: else -> goto OBS_ERROR ;
		fi ;

//////////////////////////////////////////////
// Erreur : propriété echoue                //
//////////////////////////////////////////////

OBS_ERROR :

	err_rd_p2 = 0 ; assert (err_rd_p2 == 1) ;
	chan_empty ? 1 ;

}


#endif

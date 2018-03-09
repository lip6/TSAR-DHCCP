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
		:: else -> goto Start ;
		fi;
	:: OBSREQP2 ? m ->
		if
		:: ((m.type == DT_WR) && (m.addr == addr))-> goto OBS_ATT_WR2 ;
		:: else -> goto Start ;
		fi;
	:: OBSREQP3 ? m ->
		if
		:: ((m.type == DT_WR) && (m.addr == addr))-> goto OBS_ATT_WR3 ;
		:: else -> goto Start ;
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

proctype obs_rd_after_rd_plat2(chan PL1DTREQ1, L1PDTACK1, PL1DTREQ2, L1PDTACK2, PL1DTREQ3, L1PDTACK3; bit addr) {

	msg m   ;
	byte val ;
	bit err_rd_p2 = 1 ;

////////////////////
// Etat de départ //
////////////////////

Start :
	do
	:: PL1DTREQ1 ? < DT_RD, eval(addr), m.val, m.cache_id > -> goto OBS_RD_1 ;
	:: PL1DTREQ2 ? < DT_RD, eval(addr), m.val, m.cache_id > -> goto OBS_RD_2 ;
	:: PL1DTREQ3 ? < DT_RD, eval(addr), m.val, m.cache_id > -> goto OBS_RD_3 ;
	od ;

////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_1 :

	L1PDTACK1 ? < ACK_DT_RD, eval(addr), m.val, m.cache_id > -> val = m.val ; goto OBS_REQ ;

////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_2 :

	L1PDTACK2 ? < ACK_DT_RD, eval(addr), m.val, m.cache_id > -> val = m.val ; goto OBS_REQ ;

////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_3 :

	L1PDTACK3 ? < ACK_DT_RD, eval(addr), m.val, m.cache_id > -> val = m.val ; goto OBS_REQ ;


//////////////////////////////////////
// Observation des requetes         //
//////////////////////////////////////

OBS_REQ :

	do
	:: PL1DTREQ1 ? < DT_RD, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_RD1 ;
	:: PL1DTREQ2 ? < DT_RD, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_RD2 ;
	:: PL1DTREQ3 ? < DT_RD, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_RD3 ;
	:: PL1DTREQ1 ? < DT_WR, eval(addr), m.val, m.cache_id> -> goto Start ;
	:: PL1DTREQ2 ? < DT_WR, eval(addr), m.val, m.cache_id> -> goto Start ;
	:: PL1DTREQ3 ? < DT_WR, eval(addr), m.val, m.cache_id> -> goto Start ;
	od ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD1 :

	L1PDTACK1 ? < ACK_DT_RD , eval (addr), m.val, m.cache_id> ->
		if
		:: (m.val == val) -> goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi;


//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD2 :

	L1PDTACK2 ? < ACK_DT_RD , eval (addr), m.val, m.cache_id> ->
		if
		:: (m.val == val) -> goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi;


//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD3 :

	L1PDTACK3 ? < ACK_DT_RD , eval (addr), m.val, m.cache_id> ->
		if
		:: (m.val == val) -> goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi;

//////////////////////////////////////////////
// Erreur : propriété echoue                //
//////////////////////////////////////////////

OBS_ERROR :

	err_rd_p2 = 0 ; assert (err_rd_p2 == 1) ;
	chan_empty ? 1 ;

}


#endif

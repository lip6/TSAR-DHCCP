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

proctype obs_rd_after_wr(chan PL1DTREQ, L1PDTACK; bit addr) {

	msg m   ;
	byte val ;
	bit err_wr = 1 ;
	byte cpt = 0 ;
	bit flag_pl1 = 0 ;	// 0 canaux empty , 1 canaux plein
	bit flag_l1p = 0 ;	// 0 canaux empty , 1 canaux plein

////////////////////
// Etat de départ //
////////////////////

Start :
	
	PL1DTREQ ? < DT_WR, eval(addr), m.val, m.cache_id > -> flag_pl1 = 1 ; goto OBS_ATT_WR ;
	

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////

OBS_ATT_WR :

	L1PDTACK ? < ACK_DT_WR, eval(addr), m.val, m.cache_id > -> 
		atomic {
			val = m.val ; 
			goto OBS_REQ ;
		};


////////////////////////////////
// Observation des requetes   //
////////////////////////////////

OBS_REQ :

	do
	:: PL1DTREQ ? < DT_RD, eval(addr), m.val, m.cache_id> -> flag_pl1 = 1 ; goto OBS_ATT_RD ;
	:: PL1DTREQ ? < DT_WR, eval(addr), m.val, m.cache_id> -> flag_pl1 = 1 ; goto OBS_ATT_WR ;
	od ;

	

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD :

	L1PDTACK ? < ACK_DT_RD , eval (addr), m.val, m.cache_id> ->
		if
		:: (m.val == val) -> goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi;

//////////////////////////////////////////////
// Erreur : propriété echoue                //
//////////////////////////////////////////////

OBS_ERROR :

	err_wr = 0 ; assert (err_wr == 1) ;
	chan_empty ? 1 ;
	//goto Start;

}

//////////////////////////////////////////////////
// Observateur pour vérifier les RD succesif    //
// Tous les RD succesive sans un WR entre eux   //
// retourne la meme valeur                      //
//////////////////////////////////////////////////

// propriété non vérifier, l'observateur se bloque.

proctype obs_rd_after_rd(chan PL1DTREQ, L1PDTACK; bit addr) {

	msg m   ;
	byte val ;
	bit err_rd = 1 ;

////////////////////
// Etat de départ //
////////////////////

Start :
	
	PL1DTREQ ? < DT_RD, eval(addr), m.val, m.cache_id > -> goto OBS_RD_1 ;

////////////////////////////////
// Attente l'aquitement de RD //
////////////////////////////////

OBS_RD_1 :

	L1PDTACK ? < ACK_DT_RD, eval(addr), m.val, m.cache_id > -> val = m.val ; goto OBS_REQ ;


//////////////////////////////////////
// Observation des requetes         //
//////////////////////////////////////

OBS_REQ :

	do
	:: PL1DTREQ ? < DT_RD, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_RD ;
	:: PL1DTREQ ? < DT_WR, eval(addr), m.val, m.cache_id> -> goto Start ;
	od ;

//////////////////////////////////////////////
// Observation des aquitements de lecture   //
//////////////////////////////////////////////

OBS_ATT_RD :

	L1PDTACK ? < ACK_DT_RD , eval (addr), m.val, m.cache_id> ->
		if
		:: (m.val == val) -> goto OBS_REQ ;
		:: else -> goto OBS_ERROR ;
		fi;

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

proctype obs_rd_after_wr_plat2(chan PL1DTREQ1, L1PDTACK1, PL1DTREQ2, L1PDTACK2, PL1DTREQ3, L1PDTACK3; bit addr) {

	msg m   ;
	byte val ;
	bit err_wr_p2 = 1 ;

////////////////////
// Etat de départ //
////////////////////

Start :
	
	do
	:: PL1DTREQ1 ? < DT_WR, eval(addr), m.val, m.cache_id > -> goto OBS_ATT_WR1 ;
	:: PL1DTREQ2 ? < DT_WR, eval(addr), m.val, m.cache_id > -> goto OBS_ATT_WR2 ;
	:: PL1DTREQ3 ? < DT_WR, eval(addr), m.val, m.cache_id > -> goto OBS_ATT_WR3 ;
	od ;

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////

OBS_ATT_WR1 :

	L1PDTACK1 ? < ACK_DT_WR, eval(addr), m.val, m.cache_id > -> val = m.val ; goto OBS_REQ ;

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////

OBS_ATT_WR2 :

	L1PDTACK2 ? < ACK_DT_WR, eval(addr), m.val, m.cache_id > -> val = m.val ; goto OBS_REQ ;

////////////////////////////////
// Attente l'aquitement de WR //
////////////////////////////////

OBS_ATT_WR3 :

	L1PDTACK3 ? < ACK_DT_WR, eval(addr), m.val, m.cache_id > -> val = m.val ; goto OBS_REQ ;

////////////////////////////////
// Observation des requetes   //
////////////////////////////////

OBS_REQ :

	do
	:: PL1DTREQ1 ? < DT_RD, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_RD1 ;
	:: PL1DTREQ2 ? < DT_RD, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_RD2 ;
	:: PL1DTREQ3 ? < DT_RD, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_RD3 ;
	:: PL1DTREQ1 ? < DT_WR, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_WR1 ;
	:: PL1DTREQ2 ? < DT_WR, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_WR2 ;
	:: PL1DTREQ3 ? < DT_WR, eval(addr), m.val, m.cache_id> -> goto OBS_ATT_WR3 ;
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

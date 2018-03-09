
#ifndef _PROC_PML_
#define _PROC_PML_



proctype proc (chan PL1DTREQ, L1PDTACK){

	msg m;
	bit err = 1 ;

//////////////////////////////////////////////////
// Etat initiale ou le processeur               //
// déscide aléatoirement l'opération à réaliser //
//////////////////////////////////////////////////

Idle :
	do
	:: goto DT_RD_X ;
	//:: goto DT_RD_Y ;
	:: goto DT_WR_X ;
	//:: goto DT_WR_Y ;
	od ;

//////////////////////////////////////////////////
// Etat demande lecture de l'adresse X          //
//////////////////////////////////////////////////

DT_RD_X :

	m.type = DT_RD  ;
	m.addr = X      ;
	PL1DTREQ ! m    ;
	goto ATT_RD_X   ;

//////////////////////////////////////////////////
// Etat demande lecture de l'adresse Y          //
//////////////////////////////////////////////////

DT_RD_Y :
	
	m.type = DT_RD  ;
	m.addr = Y      ;
	PL1DTREQ ! m    ;
	goto ATT_RD_Y   ;
	
//////////////////////////////////////////////////
// Etat demande l'écriture de l'adresse X       //
//////////////////////////////////////////////////

DT_WR_X :

	m.type = DT_WR  ;
	m.addr = X      ;
	PL1DTREQ ! m    ;
	goto ATT_WR_X   ;

//////////////////////////////////////////////////
// Etat demande l'écriture de l'adresse Y       //
//////////////////////////////////////////////////

DT_WR_Y :

	m.type = DT_WR  ;
	m.addr = Y      ;
	PL1DTREQ ! m    ;
	goto ATT_WR_Y   ;

//////////////////////////////////////////////////
// Attente l'aquitement de lecture de X         //
//////////////////////////////////////////////////

ATT_RD_X :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_RD) && (m.addr == X) ) -> 
			goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
// Attente l'aquitement de lecture de Y         //
//////////////////////////////////////////////////

ATT_RD_Y :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_RD) && (m.addr == Y) ) -> 
			goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
// Attente l'aquitement de l'écriture de X      //
//////////////////////////////////////////////////

ATT_WR_X :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_WR) && (m.addr == X) ) -> 
			goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
// Attente l'aquitement de l'écriture de Y      //
//////////////////////////////////////////////////

ATT_WR_Y :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_WR) && (m.addr == Y) ) -> 
			goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
//                Etat d'erreur                 //
//////////////////////////////////////////////////

ERROR : 

	do 
	:: (err == 1) -> skip ;
	od ;
}

/////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

proctype proc_rd(chan PL1DTREQ, L1PDTACK; bit addr) { //modifié


	msg m   ;
	bit err = 1 ;

//////////////////////////////////////////////////
// Etat initiale ou le processeur               //
// déscide aléatoirement l'opération à réaliser //
//////////////////////////////////////////////////

Idle :

	do
	:: goto Idle ;
	:: goto DT_RD_addr ;
	od ;



//////////////////////////////////////////////////
// Etat demande lecture de l'adresse ?          //
//////////////////////////////////////////////////

DT_RD_addr :

	m.type = DT_RD  ;
	m.addr = addr   ;
	PL1DTREQ ! m    ;
	goto ATT_RD     ;

//////////////////////////////////////////////////
// Attente l'aquitement de lecture de ?         //
//////////////////////////////////////////////////

ATT_RD :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_RD) && (m.addr == addr) ) -> 
			goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
//                Etat d'erreur                 //
//////////////////////////////////////////////////

ERROR : 

	do 
	:: (err == 1) -> assert (err == 1) ; skip ;
	od ;
}

/////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

proctype proc_rd_wr(chan PL1DTREQ, L1PDTACK; bit addr) { // modifié


	msg m   ;
	bit err = 1 ;

//////////////////////////////////////////////////
// Etat initiale ou le processeur               //
// déscide aléatoirement l'opération à réaliser //
//////////////////////////////////////////////////

Idle :

	do
	:: goto Idle ;
	:: goto DT_RD_addr;
	:: goto DT_WR_addr;
	od ;

//////////////////////////////////////////////////
// Etat demande lecture de l'adresse  ?         //
//////////////////////////////////////////////////

DT_RD_addr :

	m.type = DT_RD  ;
	m.addr = addr   ;
	PL1DTREQ ! m    ;
	goto ATT_RD     ;


DT_WR_addr :

	m.type = DT_WR  ;
	m.addr = addr   ;
	PL1DTREQ ! m    ;
	goto ATT_WR     ;



//////////////////////////////////////////////////
// Attente l'aquitement de lecture de ?         //
//////////////////////////////////////////////////

ATT_RD :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_RD) && (m.addr == addr) ) -> 
			goto Idle ;
		:: else -> goto ERROR ;
		fi ;

ATT_WR :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_WR) && (m.addr == addr) ) ->
			goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
//                Etat d'erreur                 //
//////////////////////////////////////////////////

ERROR : 

	do 
	:: (err == 1) -> assert (err == 1) ; skip ;
	od ;

}

#endif



#ifndef _CACHEL1_H
#define _CACHEL1_H

/********** Modèle de l'automate lu cache L1 ****************************/
/* Voir rapport de spécification,                                       */
/* pour les listes des canaux de communications en argument             */ 
/************************************************************************/
#include "lib_project.h"

proctype CacheL1 (chan PL1DTREQ, L1PDTACK, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK ; byte c_id){

///////////////////////////////////////
// Déclaration des variables locales.//
// msg : variable qui contient la    //
// valeur recue dans 1 des canaux.   //
// val : Valeur du blocs mémoire     //
// dans la cache.                    //
///////////////////////////////////////

	byte v_cache   ; 		// valeur du block mémoire dans la cache.
	bit v_cache_valide ; 		// bit de validité ,la cache elle modifier cette valeur 0: invalide , 1: valide.
	bit v_addr ;			// entier qui contient l'adresse , X oubien Y .
	 msg m;
	 bit Vcl = 1 ;		// variable d'état de cleanup.

///////////////////////////////////////
// Etat initial, le cache est vide   //
///////////////////////////////////////

Empty :
	printf("Cache L1 => STATE : EMPTY\n");
	v_cache_valide = INVALIDE;
	

	do
	:: MCL1CPREQ ? M_INV , m.addr, m.val,eval(c_id)   -> goto Empty ; 
	:: MCL1CPREQ ? <B_INV, m.addr, m.val, m.cache_id> -> goto Empty ;
	:: PL1DTREQ ? m -> 
		if
		:: ((m.type == DT_RD) && (m.addr == X)) -> goto MISS_X  ;
		:: ((m.type == DT_RD) && (m.addr == Y)) -> goto MISS_Y  ;
		:: ((m.type == DT_WR) && (m.addr == X)) ->
			atomic {
				m.type= WR        ; 
				m.addr = X        ; 
				m.cache_id = c_id ; 
				L1MCDTREQ ! m     ; 
				goto ATT_WR1_X 
			};	
		:: ((m.type == DT_WR) && (m.addr == Y)) ->
			atomic {
				m.type= WR        ; 
				m.addr = Y        ; 
				m.cache_id = c_id ; 
				L1MCDTREQ ! m     ; 
				goto ATT_WR1_Y 
			};
		:: else -> printf("Etat EMPTY : Requête invalide chan PL1DTREQ : val %d\n",m.type);
		fi ;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto Empty 
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto Empty  
			};
		:: else -> printf("Etat EMPTY : Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;



///////////////////////////////////////
//Attente d'écriture X et cache vide //
///////////////////////////////////////

ATT_WR1_X :
	printf ("CacheL1 => STATE : ATT_WR1_X\n");

	do
	:: MCL1CPREQ ?   M_INV,m.addr, m.val,eval(c_id) -> goto ATT_WR1_X ;
	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> -> goto ATT_WR1_X ;
	:: MCL1DTACK ? m.type,m.addr,m.val,eval(c_id) -> 
		if
		:: ((m.type == ACK_WR) && (m.addr == X) ) ->
			atomic {
				m.type = ACK_DT_WR ; 
				m.addr = X         ; 
				L1PDTACK ! m       ; 
				goto Empty 
			};
		:: else -> printf("Etat ATT_WR1_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi ;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_WR1_X
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_WR1_X  
			};
		:: else -> printf("Etat ATT_WR1_X : Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od ;



///////////////////////////////////////
// Attente d'écriture Y et cache vide//
///////////////////////////////////////

ATT_WR1_Y :
	printf("CacheL1 => STATE : ATT_WR1_Y\n");
	do
	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id)   -> goto ATT_WR1_Y ;
	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> -> goto ATT_WR1_Y ;
	:: MCL1DTACK ? m.type,m.addr,m.val,eval(c_id) -> 
		if
		:: ((m.type == ACK_WR) && (m.addr == Y) ) ->
			atomic {
				m.type = ACK_DT_WR ; 
				m.addr = Y         ; 
				L1PDTACK ! m       ;
				goto Empty 
			};
		:: else -> printf("Etat ATT_WR1_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi ;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_WR1_Y
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_WR1_Y  
			};
		:: else -> printf("Etat ATT_WR1_Y : Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od ;

///////////////////////////////////////
// Miss sur la valeur X              //
///////////////////////////////////////

MISS_X :
	printf("CacheL1 => STATE : MISS_X\n");
	
	do

	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id) -> 	
		if
		:: ( m.addr == X ) ->  goto MISS_X ; 
		:: ( m.addr == Y ) -> 	
			if 
			:: (v_addr == Y) ->
				atomic {
					v_cache_valide = INVALIDE ; 
					m.type = ACK_M_INV        ; 
					m.addr = Y                ; 
					m.cache_id = c_id         ; 
					L1MCCPACK ! m 
				};
			:: else -> skip ;
			fi ;
		:: else -> skip ;
		fi;

	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> -> 	
		if
		:: ( m.addr == X ) ->  goto MISS_X ;
		:: ( m.addr == Y) -> 
			if 
			 :: (v_addr == Y) ->
				atomic {
					v_cache_valide = INVALIDE ; 
					m.type = ACK_B_INV        ; 
					m.addr = Y                ;
					m.cache_id = c_id         ; 
					L1MCCPACK ! m 
				};
			 :: else -> skip ;
			 fi ;
		:: else -> skip ;
		fi ;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  MISS_X
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto MISS_X  
			};
		:: else -> printf("Etat MISS_X: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	:: break ;
	od;

	// si la case contient Y, on réalise une invalidation sur Y et puis on demande une lecture de X.
	if 
	:: ((v_addr == Y) && (v_cache_valide == VALIDE)) -> 
		if
		:: (Vcl == 1) ->	// pas de ACK_CLNUP attendue.
			atomic {
				Vcl = 0                   ;
				v_cache_valide = INVALIDE ; 
				m.type = CLNUP            ; 
				m.addr = Y                ; 
				m.cache_id = c_id         ; 
				L1MCCUREQ ! m 
			}; 
		:: else ->		// ACK_CLNUP attendue => bloquage.
			MCL1CUACK ? m.type,m.addr, m.val,eval(c_id) ->   
				if
				:: (m.type == ACK_CLNUP) -> skip ;
				:: else -> printf("Etat MISS_X : Requête invalide chan MCL1CUACK : val %d\n",m.type);
				fi ;
			// sending CLNUP sur Y.
			Vcl = 0                   ;
			v_cache_valide = INVALIDE ; 
			m.type = CLNUP            ; 
			m.addr = Y                ; 
			m.cache_id = c_id         ; 
			L1MCCUREQ ! m 
		fi ;
		

	:: else -> skip;
	fi ;
	
	// read de X.
	m.type = RD       ; 
	m.addr = X        ; 
	m.cache_id = c_id ; 
	L1MCDTREQ ! m     ; 
	goto ATT_D_X ;

///////////////////////////////////////
// Miss sur la valeur Y              //
///////////////////////////////////////

MISS_Y :
	printf("CacheL1 => STATE : MISS_Y\n");
	do 

	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id) -> 	
		if
		:: ( m.addr == Y ) ->  goto MISS_Y ;
		:: ( m.addr == X ) ->  
			if 
			 :: (v_addr == X) ->
				atomic {
					v_cache_valide = INVALIDE ; 
					m.type = ACK_M_INV        ; 
					m.cache_id = c_id         ; 
					m.addr = X                ; 
					L1MCCPACK ! m 
				};
			 :: else -> skip ;
			 fi ;
		:: else -> skip ;
		fi ;

	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> -> 	
		if
		:: ( m.addr == Y ) ->  goto MISS_Y 
		:: ( m.addr == X ) ->  
			if 
			:: (v_addr == X) ->
				atomic {
					v_cache_valide = INVALIDE ; 
					m.type = ACK_B_INV        ; 
					m.cache_id = c_id         ; 
					m.addr = X                ; 
					L1MCCPACK ! m 
				};
			:: else -> skip ;
			fi ;
		:: else -> skip ;
		fi ;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  MISS_Y
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto MISS_Y  
			};
		:: else -> printf("Etat MISS_Y: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	:: break ;
	od ;

	// si la case contient X, on réalise une invalidation sur X et puis on demande une lecture de Y.
	if 
	:: ((v_addr == X) && (v_cache_valide == VALIDE)) -> 
		if
		:: (Vcl == 1) ->	// pas de ACK_CLNUP attendue.
			atomic {
				Vcl = 0                   ;
				v_cache_valide = INVALIDE ; 
				m.type = CLNUP            ; 
				m.addr = X                ; 
				m.cache_id = c_id         ; 
				L1MCCUREQ ! m 
			}; 
		:: else ->		// ACK_CLNUP attendue => bloquage.
			MCL1CUACK ? m.type,m.addr, m.val,eval(c_id) ->   
				if
				:: (m.type == ACK_CLNUP) -> skip ;
				:: else -> printf("Etat MISS_Y : Requête invalide chan MCL1CUACK : val %d\n",m.type);
				fi ;
			// sending CLNUP sur X.
			Vcl = 0                   ;
			v_cache_valide = INVALIDE ; 
			m.type = CLNUP            ; 
			m.addr = X                ; 
			m.cache_id = c_id         ; 
			L1MCCUREQ ! m 
		fi ;

	:: else -> skip;
	fi ;

	// read sur Y.
	m.type = RD       ; 
	m.addr = Y        ; 
	m.cache_id = c_id ; 
	L1MCDTREQ ! m     ; 
	goto ATT_D_Y ;

///////////////////////////////////////
// Attente la transfer de X          //
///////////////////////////////////////

ATT_D_X :
	printf("CacheL1 => STATE : ATT_D_X\n");
	do
	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: (m.addr == Y) -> goto ATT_D_X ;
		:: (m.addr == X) -> goto CL_R_X  ;
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ? <B_INV,m.addr, m.val,eval(c_id)> ->   
		if
		:: (m.addr == Y) -> goto ATT_D_X ;
		:: (m.addr == X) -> goto CL_R_X  ;
		:: else -> skip ;
		fi ;

	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   
		if
		:: ((m.type == ACK_RD) && (m.addr == X)) ->
			atomic {
				v_cache_valide= VALIDE ; 
				v_addr = X             ; 
				v_cache = m.val        ; 
				m.type = ACK_DT_RD     ; 
				m.addr = X             ; 
				L1PDTACK ! m           ; 
				goto V_X 
			}; 
		:: else -> printf("Etat ATT_D_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_D_X
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_D_X 
			};
		:: else -> printf("Etat ATT_D_X: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;


///////////////////////////////////////
// Attente la transfer de Y          //
///////////////////////////////////////

ATT_D_Y :
	printf("CacheL1 => STATE : ATT_D_Y\n");
	do
	:: MCL1CPREQ ?  M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: ( m.addr == X) -> goto ATT_D_Y ;
		:: ( m.addr == Y) -> goto CL_R_Y  ;
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ?  <B_INV,m.addr, m.val,eval(c_id)> ->   
		if
		:: ( m.addr == X) -> goto ATT_D_Y ;
		:: ( m.addr == Y) -> goto CL_R_Y  ;
		:: else -> skip ;
		fi ;
	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   
		if
		:: ((m.type == ACK_RD) && (m.addr == Y)) ->
			atomic {
				v_cache_valide= VALIDE ; 
				v_addr = Y             ; 
				v_cache = m.val        ; 
				m.type = ACK_DT_RD     ; 
				m.addr = Y             ; 
				L1PDTACK ! m           ; 
				goto V_Y 
			};
		:: else -> printf("Etat ATT_D_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_D_Y
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_D_Y 
			};
		:: else -> printf("Etat ATT_D_Y: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;


///////////////////////////////////////
// CLEANUP de READ sur X             //
///////////////////////////////////////

CL_R_X :
	printf("CacheL1 => STATE : CL_R_X\n");
	do
	:: MCL1CPREQ ?  M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: (m.addr == Y ) -> goto CL_R_X ;
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ?  <B_INV,m.addr, m.val,m.cache_id> ->   
		if
		:: (m.addr == Y ) -> goto CL_R_X ;
		:: else -> skip ;
		fi ;
	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   
		if
		:: ((m.type == ACK_RD) && (m.addr == X)) -> 
			atomic {
				Vcl = 0   ;
				m.type =CLNUP     ; 
				m.addr = X        ; 
				m.cache_id = c_id ; 
				L1MCCUREQ ! m     ;
				goto MISS_X 
			};	
		:: else -> printf("Etat CL_R_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  CL_R_X
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto CL_R_X 
			};
		:: else -> printf("Etat CL_R_X: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;


///////////////////////////////////////
// CLEANUP de READ sur X             //
///////////////////////////////////////

CL_R_Y :
	printf("CacheL1 => STATE : CL_R_Y\n");
	do
	:: MCL1CPREQ ?  M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: (m.addr == X) -> goto CL_R_Y ;
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ?  <B_INV,m.addr, m.val,m.cache_id> ->   
		if
		:: (m.addr == X) -> goto CL_R_Y ;
		:: else -> skip ;
		fi ;
	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   
		if
		:: ((m.type == ACK_RD) && (m.addr == Y)) -> 
			atomic {
				Vcl = 0           ; 
				m.type= CLNUP     ; 
				m.addr = Y        ; 
				m.cache_id = c_id ; 
				L1MCCUREQ ! m     ;
				goto MISS_Y 
			};
		:: else -> printf("Etat CL_R_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi;

	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  CL_R_Y
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto CL_R_Y 
			};
		:: else -> printf("Etat CL_R_Y: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;

///////////////////////////////////////
// Cache contient la valeur X        //
///////////////////////////////////////

V_X :
	printf("CacheL1 => STATE : V_X , Valeur : %d\n",v_cache);
	do 
	:: MCL1CPREQ ?  M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: (m.addr == Y) -> goto V_X ;
		:: (m.addr == X) ->
			atomic{
				v_cache_valide = INVALIDE ; 
				m.type = ACK_M_INV ; 
				m.addr = X ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto Empty 
			};
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ?  <B_INV,m.addr, m.val,m.cache_id> ->   
		if
		:: (m.addr == Y) -> goto V_X ;
		:: (m.addr == X) ->
			atomic{
				v_cache_valide = INVALIDE ; 
				m.type = ACK_B_INV; m.addr = X ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto Empty 
			};
		:: else -> skip ;
		fi ;
	:: PL1DTREQ ? m ->    
		if
		:: ((m.type == DT_RD) && (m.addr == X)) -> 
			atomic {
				m.type = ACK_DT_RD ; 
				m.addr = X         ; 
				m.val = v_cache    ; 
				L1PDTACK ! m       ; 
				goto V_X 
			};
		:: ((m.type == DT_RD) && (m.addr == Y)) -> goto MISS_Y ;
		:: ((m.type == DT_WR) && (m.addr == X)) -> 
			atomic {
				m.type = WR       ; 
				m.addr=X          ;
				m.cache_id = c_id ; 
				L1MCDTREQ ! m     ;
				goto ATT_WR2_X 
			};		
		:: ((m.type == DT_WR) && (m.addr == Y)) -> 
			atomic {
				m.type = WR       ; 
				m.addr=Y          ; 
				m.cache_id = c_id ; 
				L1MCDTREQ ! m     ; 
				goto ATT_WR2_Y 
			};
		:: else -> printf("Etat V_X : Requête invalide chan PL1DTREQ : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			if
			:: (m.addr == X) -> 
				v_cache = m.val       ;	// récupère la nouvelle valeur.
				m.type = ACK_M_UP ;
				m.cache_id = c_id ;	// m.addr, et m.val non modifier.
				L1MCCPACK ! m     ;
			:: else -> skip ;			// M_UP sur un autre adresse.
			fi ;

			goto  V_X
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto V_X 
			};
		:: else -> printf("Etat V_X: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;	
	od;

///////////////////////////////////////
// Cache contient la valeur Y        //
///////////////////////////////////////

V_Y :
	printf("CacheL1 => STATE : V_Y, valeur : %d\n",v_cache);
	do 
	:: MCL1CPREQ ?  M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: (m.addr == X) -> goto V_Y ;
		:: (m.addr == Y) ->
			atomic{
				v_cache_valide = INVALIDE ; 
				m.type = ACK_M_INV ; 
				m.addr = Y ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto Empty 
			};
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ?  <B_INV,m.addr, m.val,m.cache_id> ->   
		if
		:: (m.addr == X) -> goto V_Y ;
		:: (m.addr == Y) ->
			atomic{
				v_cache_valide = INVALIDE ; 
				m.type = ACK_B_INV ; 
				m.addr = Y ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto Empty 
			};
		:: else -> skip ;
		fi;	
	:: PL1DTREQ ? m  ->    
		if
		:: ((m.type == DT_RD) && (m.addr == Y)) -> 
			atomic {
				m.type = ACK_DT_RD ; 
				m.addr = Y         ; 
				m.val = v_cache    ; 
				L1PDTACK ! m       ; 
				goto V_Y 
			};
		:: ((m.type == DT_RD) && (m.addr == X)) -> goto MISS_X ;
		:: ((m.type == DT_WR) && (m.addr == X)) -> 
			atomic {
				m.type = WR       ; 
				m.addr = X        ;
				m.cache_id = c_id ;
				L1MCDTREQ ! m     ;
				goto ATT_WR3_X 
			};		
		:: ((m.type == DT_WR) && (m.addr == Y)) -> 
			atomic {
				m.type = WR       ; 
				m.addr = Y        ;
				m.cache_id = c_id ;
				L1MCDTREQ ! m     ;
				goto ATT_WR3_Y
			};
		:: else -> printf("Etat V_Y : Requête invalide chan PL1DTREQ : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			if
			:: (m.addr == Y) -> 
				v_cache = m.val       ;	// récupère la nouvelle valeur.
				m.type = ACK_M_UP ;
				m.cache_id = c_id ;	// m.addr, et m.val non modifier.
				L1MCCPACK ! m     ;
			:: else -> skip ;			// M_UP sur un autre adresse.
			fi;
			goto  V_Y
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto V_Y 
			};
		:: else -> printf("Etat V_Y: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;	
	od;


/////////////////////////////////////////
// Etat d'attente écriture X lié à V_X //
/////////////////////////////////////////

ATT_WR2_X :
	printf("CacheL1 => STATE : ATT_WR2_X\n");
	do 
	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: ( m.addr == Y) -> goto ATT_WR2_X ;
		:: ( m.addr == X) -> 
			atomic {
				v_cache_valide = INVALIDE ; 
				m.type = ACK_M_INV ; 
				m.addr = X ; 
				m.cache_id = c_id; 
				L1MCCPACK ! m ; 
				goto ATT_WR1_X 
			};
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> ->   
		if
		:: ( m.addr == Y) -> goto ATT_WR2_X ;
		:: ( m.addr == X) -> 
			atomic {
				v_cache_valide = INVALIDE ; 
				m.type = ACK_B_INV ; 
				m.addr = X ; 
				m.cache_id = c_id; 
				L1MCCPACK ! m ; 
				goto ATT_WR1_X 
			};
		:: else -> skip ;
		fi ;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   
		if
		:: ((m.type == ACK_WR) && (m.addr == X)) -> 
			atomic {
				m.type = ACK_DT_WR ; 
				m.addr = X         ;
				v_cache = m.val    ; 
				L1PDTACK ! m       ; 
				goto V_X 
			};
		:: else -> printf("Etat ATT_WR2_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			if
			:: (m.addr == X) -> v_cache = m.val       ;	// récupère la nouvelle valeur.
			:: else -> skip ;			// M_UP sur un autre adresse.
			fi;
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_WR2_X
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_WR2_X 
			};
		:: else -> printf("Etat ATT_WR2_X: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture Y lié à V_X //
/////////////////////////////////////////

ATT_WR2_Y :
	printf("CacheL1 => STATE : ATT_WR2_Y\n");
	do 
	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: ( m.addr == Y ) -> goto ATT_WR2_Y ;
		:: ( m.addr == X ) ->
			atomic {
				v_cache_valide = INVALIDE ; 
				m.type = ACK_M_INV ; 
				m.addr = X ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto ATT_WR1_Y 
			};
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> ->   
		if
		:: ( m.addr == Y ) -> goto ATT_WR2_Y ;
		:: ( m.addr == X ) ->
			atomic {
				v_cache_valide = INVALIDE ; 
				m.type = ACK_B_INV ; 
				m.addr = X ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto ATT_WR1_Y 
			};
		:: else -> skip ;
		fi;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   
		if
		:: ((m.type == ACK_WR) && (m.addr == Y)) -> 
			atomic {
				m.type = ACK_DT_WR ; 
				m.addr = Y         ; 
				L1PDTACK ! m       ; 
				goto V_X 
			};
		:: else -> printf("Etat ATT_WR2_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			if
			:: (m.addr == X) -> v_cache = m.val       ;	// récupère la nouvelle valeur.
			:: else -> skip ;			// M_UP sur un autre adresse.
			fi;
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_WR2_Y
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_WR2_Y 
			};
		:: else -> printf("Etat ATT_WR2_Y: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture X lié à V_Y //
/////////////////////////////////////////

ATT_WR3_X :
	printf("CacheL1 : STATE : ATT_WR3_X\n");
	do 
	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id) ->   
		if
		:: ( m.addr == X ) -> goto ATT_WR3_X ;
		:: ( m.addr == Y ) ->
			atomic {
				v_cache_valide = INVALIDE ; 
				m.type = ACK_M_INV ; 
				m.addr = Y ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto ATT_WR1_X 
			};
		:: else -> skip ;
		fi ;
	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> ->   
		if
		:: ( m.addr == X ) -> goto ATT_WR3_X ;
		:: ( m.addr == Y ) ->
			atomic {
				v_cache_valide = INVALIDE ; 
				m.type = ACK_B_INV ; 
				m.addr = Y ; 
				m.cache_id = c_id ; 
				L1MCCPACK ! m ; 
				goto ATT_WR1_X 
			};
		:: else -> skip ;
		fi ;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   
		if
		:: ((m.type == ACK_WR) && (m.addr == X)) -> 
			atomic {
				m.type = ACK_DT_WR ; 
				m.addr = X         ; 
				L1PDTACK ! m       ; 
				goto V_Y 
			};
		:: else -> printf("Etat ATT_WR3_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
		fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			if
			:: (m.addr == Y) -> v_cache = m.val       ;	// récupère la nouvelle valeur.
			:: else -> skip ;			// M_UP sur un autre adresse.
			fi;
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_WR3_X
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_WR3_X 
			};
		:: else -> printf("Etat ATT_WR3_X: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture Y lié à V_X //
/////////////////////////////////////////

ATT_WR3_Y :
	printf("CacheL1 => STATE : ATT_WR3_Y\n");
	do 
	:: MCL1CPREQ ? M_INV,m.addr, m.val,eval(c_id) ->   
				if
				:: ( m.addr == X ) -> goto ATT_WR3_Y ;
				:: ( m.addr == Y ) ->
					atomic {
						v_cache_valide = INVALIDE ; 
						m.type = ACK_M_INV; 
						m.addr = Y ; 
						m.cache_id = c_id ; 
						L1MCCPACK ! m ; 
						goto ATT_WR1_Y 
					};
				:: else -> skip ;
				fi ;
	:: MCL1CPREQ ? <B_INV,m.addr, m.val,m.cache_id> ->   
				if
				:: ( m.addr == X ) -> goto ATT_WR3_Y ;
				:: ( m.addr == Y ) ->
					atomic {
						v_cache_valide = INVALIDE ; 
						m.type = ACK_B_INV; 
						m.addr = Y ; 
						m.cache_id = c_id ; 
						L1MCCPACK ! m ; 
						goto ATT_WR1_Y 
					};
				:: else -> skip ;
				fi ;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_WR) && (m.addr == Y)) -> m.type = ACK_DT_WR ; m.addr = Y ; v_cache= m.val;L1PDTACK ! m ; goto V_Y ;
				:: else -> printf("Etat ATT_WR3_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	// traitement des multicastes updates .
	:: MCL1CPREQ ? M_UP, m.addr, m.val, eval(c_id) -> 
		atomic {
			if
			:: (m.addr == Y) -> v_cache = m.val       ;	// récupère la nouvelle valeur.
			:: else -> skip ;			// M_UP sur un autre adresse.
			fi;
			m.type = ACK_M_UP ;
			m.cache_id = c_id ;	// m.addr, et m.val non modifier.
			L1MCCPACK ! m     ;
			goto  ATT_WR3_Y
		};
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, m.val, eval(c_id) ->
		if 
		:: (m.type == ACK_CLNUP) ->
			atomic {
			Vcl = 1   ; 
			goto ATT_WR3_Y 
			};
		:: else -> printf("Etat ATT_WR3_Y: Requête invalide chan MCL1CUACK : val %d\n",m.type);
		fi;
	od;

}

# endif

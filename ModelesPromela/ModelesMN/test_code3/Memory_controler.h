/*********** Modèle de l'automate du controleur mémoire *****************************/
/*           Pour une adresse donné                                                 */
/*           Voire rapport de spécification pour plus de détail                     */
/************************************************************************************/

#ifndef _MEMORY_CONTROLER_H
#define _MEMORY_CONTROLER_H

///////////////////////////////////////////
//            INCLUDES                   //
///////////////////////////////////////////

#include "lib_project.h"

#define CACHE_TH 2		// supposé 2.

///////////////////////////////////////////
//	Controleur mémoire pour X        //
///////////////////////////////////////////

proctype MC_X (chan L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK){


	msg m ;
	byte src ;
	byte v_x ;
	byte c_id_tmp ;		// case temporelle à sauvegarder le cache id.
	byte cpt,rep_cpt ;	// compteurs temporelles.
	byte c_id[CACHE_TH];	// tableau qui contient les id des caches
	bit v_c_id[CACHE_TH];	// tableau qui contient la validité des cases dans c_id ;
	byte n_cache ;		// nombre de cache qui contient des copies.

///////////////////////////////////////
//Case X vide                        //
///////////////////////////////////////

Empty_X :

	printf("------> MC_X ETAT : EMPTY_X \n");

	L1MCDTREQ ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == RD) -> 
			atomic {
				src = m.cache_id ;
				m.type = GET     ;
				m.addr = X       ;
				MCMEMDTREQ ! m   ;
				goto ATT_RD_X 
			};
		:: (m.type == WR) ->
			atomic {
				v_x = m.val           ;
				c_id_tmp = m.cache_id ;
				m.type = GET          ;
				m.addr = X            ;
				MCMEMDTREQ ! m        ;		// GET du bloc qui contient X.
				m.type = ACK_WR       ;
				m.val = v_x           ;
				m.addr = X            ;
				m.cache_id = c_id_tmp ;
				MCL1DTACK ! m         ;		// ACK_WR pour L1.
				goto ATT_WR_X 
			};
		:: else -> printf("MC_X : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

////////////////////////////////////////
//ATTENTE LE BLOC POUR ECRIRE VAL DE X//
////////////////////////////////////////

ATT_WR_X :

	printf("------> MC_X ETAT : ATT_WR_X \n");

	do
	:: L1MCDTREQ ? WR , X, m.val, m.cache_id -> 
		atomic {
			v_x = m.val     ;
			m.type = ACK_WR ;	// m.val et m.cache_id non modifier.
			m.addr = X      ;
			MCL1DTACK ! m   ;
			goto ATT_WR_X   
		};
	:: MEMMCDTACK ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == ACK_GET) ->
			atomic {
				n_cache = 0;
				// on modifie pas la valeur de X, car elle vient apres un write.
				goto V_M_D_X 
			};
		:: else -> printf("MC_X : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi;
	od;


/////////////////////////////////////////
//ATTENTE LE BLOC POUR LECTURE VAL DE X//
/////////////////////////////////////////

ATT_RD_X :

	printf("------> MC_X ETAT : ATT_RD_X \n");

	do 
	:: L1MCDTREQ ? WR, X, m.val,m.cache_id -> 
		atomic{
			v_x = m.val     ;
			m.type = ACK_WR ;
			m.addr = X      ;	// m.val et m.cache_id non modifier.
			MCL1DTACK ! m   ;
			goto ATT_RD_WR_X 
		};
	:: MEMMCDTACK ? m.type, X, m.val,m.cache_id -> 
		if
		:: (m.type == ACK_GET) ->
			atomic{
				v_x = m.val       ;
				m.type = ACK_RD   ;
				m.addr =  X       ;
				m.cache_id = src  ;		// m.val non modifier.
				MCL1DTACK ! m     ;
				c_id[0] = src     ;
				v_c_id[0] = VALIDE;
				n_cache = 1       ;
				goto V_M_X 
			};
		:: else -> printf("MC_X : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi;
	od;


/////////////////////////////////////////////////////
//ATTENTE LE BLOC POUR LECTURE VAL DE X APRES WRITE//
/////////////////////////////////////////////////////

ATT_RD_WR_X :

	printf("------> MC_X ETAT : ATT_RD_WR_X \n");

	do
	:: L1MCDTREQ ? WR, X, m.val, m.cache_id -> 
		atomic{
			v_x = m.val      ;
			m.type == ACK_WR ;
			m.addr = X       ;		// m.cache_id non modifier.
			MCL1DTACK ! m    ;
			goto ATT_RD_WR_X 
		};
	:: MEMMCDTACK ? m.type, X,m.val, m.cache_id -> 
		if
		:: (m.type == ACK_GET)->
			atomic{
				m.type = ACK_RD  ;
				m.addr = X       ;
				m.val = v_x      ;		// resultat de la derniere ecriture.
				m.cache_id = src ;
				MCL1DTACK ! m    ;		// ACK_RD pour src.
				c_id[0]=src      ;
				v_c_id[0]=VALIDE   ;
				n_cache = 1      ;
				goto V_M_D_X 			
			};
		:: else -> printf("MC_X : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi ;
	od;
		
		
///////////////////////
//CM CONTIENT X DIRTY//
///////////////////////

V_M_D_X :

	printf("------> MC_X ETAT : V_M_D_X \n");

	do
	:: L1MCDTREQ ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic {
				src = m.cache_id;
				// 2 cas n_cache < CACHE_TH => on reste dans V_M_D_X 
				// else on passe à V_B_D_X
				if 
				:: (n_cache < CACHE_TH) ->
					m.type = ACK_RD ;
					m.addr = X      ;
					m.val = v_x     ;		// m.cache_id non modifié.
					MCL1DTACK ! m   ;		// sending ACK_RD.
					cpt = 0         ;
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt< CACHE_TH) && (v_c_id[cpt] == INVALIDE)) ->
								c_id[cpt] = src      ;   
								v_c_id[cpt] = VALIDE ;
								n_cache = n_cache +1 ;
								break                ;
						:: else -> cpt = cpt +1 ;
						fi ;
					od ;
					goto V_M_D_X ;
				:: else ->
					n_cache = n_cache +1;
					m.type = ACK_RD     ;
					m.addr = X          ;
					m.val = v_x         ;
					m.cache_id = src    ;
					MCL1DTACK ! m       ;
					goto V_B_D_X        ;
				fi		
											
			};		
		:: (m.type == WR) ->
			atomic {						
			src = m.cache_id  ;		// sauvegarde la destination du WR.
			v_x = m.val       ;		
			rep_cpt = n_cache ;		// n_cache >1 forcement, compteur d'aquitement.
			cpt = 0 ;
			// recherche du source.
			if
			:: (n_cache >0)->
				do
				:: if
					:: (cpt == CACHE_TH) -> break ;
					:: ((cpt < CACHE_TH) && (c_id[cpt] == src) && (v_c_id[cpt] == VALIDE)) -> 
						rep_cpt = rep_cpt -1; 
						break               ;
					:: else -> cpt = cpt+1;
					fi ;
				od;
			:: else -> skip ;
			fi ;
			// coherence.
			if 
			:: (( n_cache > 1 ) || ((n_cache == 1) && (cpt == CACHE_TH))) -> 
				// sending multicast update.
				m.type = M_UP ;
				m.addr = X    ;
				m.val = v_x   ;
				cpt=0         ;
				do
				:: if
					:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] != src)) ->
						m.cache_id = c_id[cpt] ;
						MCL1CPREQ ! m          ;
						cpt = cpt + 1          ;
						:: (cpt == CACHE_TH) -> break ;
						:: else -> cpt = cpt+1 ;
					fi ;
				od;
			:: else -> skip ;
			fi ;	
			goto ATT_UP_X 
		};
		:: else -> printf("MC_X : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) -> 
			atomic {
				c_id_tmp = m.cache_id   ;
				m.type = ACK_CLNUP ;
				m.addr = X         ; 		// m.val et m.cache_id non modifier.
				MCL1CUACK ! m      ;
				// supression de l'élement de la liste.
				cpt = 0 ;
				do
				:: if
					:: (cpt == CACHE_TH) -> break ;
					:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == c_id_tmp)) ->
						v_c_id[cpt] = INVALIDE ;
						n_cache = n_cache -1   ;
						break                  ;
					:: else -> cpt = cpt+1 ;
					fi ;
				od;
				goto V_M_D_X 
			};
		:: else -> printf("MC_X : requete invalide chan L1MCCUREQ val: %d\n",m.type);
		fi;
	od ;


///////////////////////
//CM CONTIENT X      //
///////////////////////

V_M_X :

	printf("------> MC_X ETAT : V_M_X \n");

	do
	:: L1MCDTREQ ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic {
				src = m.cache_id;
				// 2 cas n_cache < CACHE_TH => on reste dans V_M_X 
				// else on passe à V_B_X
				if 
				:: (n_cache < CACHE_TH) ->
					m.type = ACK_RD ;
					m.addr = X      ;
					m.val = v_x     ;		// m.cache_id non modifié.
					MCL1DTACK ! m   ; 		// sending ACK_RD.
					cpt = 0         ;
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt< CACHE_TH) && (v_c_id[cpt] == INVALIDE)) ->
							c_id[cpt] = src      ;
							v_c_id[cpt] = VALIDE ;
							n_cache = n_cache +1 ;
							break                ;
						:: else -> cpt = cpt +1 ;
						fi ;
					od ;
					goto V_M_X ;
				:: else ->
					n_cache = n_cache +1 ;
					m.type = ACK_RD      ;
					m.addr = X           ;
					m.val = v_x          ;
					m.cache_id = src     ;
					MCL1DTACK ! m        ;
					goto V_B_X           ;
				fi;							
			};		
		:: (m.type == WR) ->
			atomic {
				src = m.cache_id  ;
				v_x = m.val       ;
				rep_cpt = n_cache ;		// n_cache >1 forcement, compteur d'aquitement.

				cpt = 0 ;
				// recherche du source.
				if
				:: (n_cache  > 0)->
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt < CACHE_TH) && (c_id[cpt] == src) && (v_c_id[cpt] == VALIDE)) -> 
							rep_cpt = rep_cpt -1 ; 
							break                ;
						:: else -> cpt = cpt+1;
						fi ;
					od;
				:: else -> skip ;
				fi ;
				// coherence.
				if 
				:: (( n_cache > 1 ) || ((n_cache == 1) && (cpt == CACHE_TH))) -> 
					// sending multicast update.
					m.type = M_UP ;
					m.addr = X    ;
					m.val = v_x   ;
					cpt=0         ;
					do
					:: if
						:: (cpt == CACHE_TH) ->break ;
						:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] != src)) ->
								m.cache_id = c_id[cpt] ;
								MCL1CPREQ ! m          ;
								cpt = cpt + 1          ;
						:: else -> cpt = cpt+1 ;
						fi ;
					od;
					goto ATT_UP_X ;
				:: else -> goto ATT_UP_X ;
				fi ;
			};
		:: else -> printf("MC_X : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) -> 
			atomic {
				c_id_tmp = m.cache_id   ;
				m.type = ACK_CLNUP ;
				m.addr = X         ;		// m.val et m.cache_id non modifier.
				MCL1CUACK ! m      ;
				// supression de l'élement de la liste.
				cpt = 0 ;
				do
				:: if
					:: (cpt == CACHE_TH) -> break ;
					:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == c_id_tmp)) ->
						v_c_id[cpt] = INVALIDE ;
						n_cache = n_cache -1   ;
						break                  ;
					:: else -> cpt = cpt+1 ;
					fi ;
				od;
				goto V_M_X 
			};
		:: else -> printf("MC_X : requete invalide chan L1MCCUREQ val: %d\n",m.type);
		fi;
	od ;

////////////////////////////
//CM attend ACK de update //
////////////////////////////

ATT_UP_X :
	
	printf("------> MC_X ETAT : ATT_UP_X \n");

	// src contient l'id du cache qui demande WR.
	// rep_cpt contient le nombre d'aquitement à attendre.
	// rep_cpt = 0 donc 1 cache demande un read puis un write => aucune UPDATE envoyer.

	if
	:: (rep_cpt > 0)->
		do
		:: L1MCCPACK ? m.type, X, m.val, m.cache_id -> 
			if
			:: (m.type == ACK_M_UP) -> 
				atomic{
					rep_cpt = rep_cpt - 1;
					goto ATT_UP_X 	
				};
			:: else -> printf("MC_X : requete invalide chan L1MCCPACK val: %d\n",m.type);
			fi;
		:: L1MCCUREQ ? m.type, X, m.val,m.cache_id -> 
			if
			:: (m.type == CLNUP) ->
				atomic{
					rep_cpt = rep_cpt - 1;
					c_id_tmp = m.cache_id   ;
					m.type = ACK_CLNUP ;
					m.addr = X         ; 		// m.val et m.cache_id non modifier.
					MCL1CUACK ! m      ;
					// supression de l'élement de la liste.
					cpt = 0 ;
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == c_id_tmp)) ->
							v_c_id[cpt] = INVALIDE ;
							n_cache = n_cache -1   ;
							break                  ;
						:: else -> cpt = cpt+1 ;
						fi ;
					od;
					goto ATT_UP_X 
				};
			:: else -> printf("MC_X : requete invalide chan L1MCCUREQ val: %d\n",m.type);
			fi;
		od;
	:: else -> skip ;
	fi; 

// Envoie ACK_WR au src.

	m.type = ACK_WR  ;
	m.addr = X       ;
	m.val = v_x      ;
	m.cache_id = src ;
	MCL1DTACK ! m    ;
	goto V_M_D_X     ;

///////////////////////////////
//CM contient x en broadcast //
///////////////////////////////

V_B_X :

	printf("------> MC_X ETAT : V_B_X \n");

	do
	:: L1MCDTREQ ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic{
				n_cache = n_cache + 1;
				m.type = ACK_RD      ;
				m.addr = X           ;
				m.val = v_x          ;		// m.cache_id non modifié.
				MCL1DTACK ! m        ;		// ACK_RD.
				goto V_B_X  
			};
		:: (m.type == WR) ->
			src = m.cache_id ;
			atomic{
				// sending B_INV pour tous les caches.
				m.type = B_INV    ;
				m.val = v_x       ;
				m.addr = X        ;
				MCL1CPREQ ! m     ;	
				//src aussi invalide son copie car liste perdue.
				rep_cpt = n_cache ;
				goto ATT_B_INV_X 
			};
		:: else -> printf("MC_X : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type , X, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) ->
			atomic {
				n_cache = n_cache -1 ;
				m.type = ACK_CLNUP   ;
				m.addr = X           ;
				m.val = v_x          ;	// m.cache_id non modifié.
				MCL1CUACK ! m        ;
				goto V_B_X 									
			};
		:: else -> printf("MC_X : Requete invalide");
		fi;
	od;

/////////////////////////////////////
//CM contient x dirty en broadcast //
/////////////////////////////////////

V_B_D_X :

	printf("------> MC_X ETAT : V_B_D_X \n");

	do
	:: L1MCDTREQ ? m.type, X, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic{
				n_cache = n_cache + 1;
				m.type = ACK_RD      ;
				m.addr = X           ;
				m.val = v_x          ;		// m.cache_id non modifié.
				MCL1DTACK ! m        ;		// ACK_RD.
				goto V_B_D_X  
			};
		:: (m.type == WR) ->
			src = m.cache_id ;
			atomic{
				// B_INV pour tous les caches.
				m.type = B_INV ;
				m.val = v_x    ;
				m.addr = X     ;
				MCL1CPREQ ! m  ;

				//src aussi invalide son copie car liste perdue.
				rep_cpt = n_cache ;

				goto ATT_B_INV_X 
			};
		:: else -> printf("MC_X : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type , X, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) ->
			atomic {
				n_cache = n_cache -1 ;
				m.type = ACK_CLNUP   ;
				m.addr = X           ;
				m.val = v_x          ;	// m.cache_id non modifié.
				MCL1CUACK ! m        ;
				goto V_B_D_X									
			};
		:: else -> printf("MC_X : requete invalide chan L1MCCUREQ val: %d\n",m.type);
		fi;
	od;

/////////////////////////////////////
//CM attend broadcast invalidation //
/////////////////////////////////////

ATT_B_INV_X :

	printf("------> MC_X ETAT : ATT_B_INV_X \n");

	// rep_cpt = nombre d'aquitement à recevoire.
	// src contient l'adresse du source qui a demander un WR.

	if
	:: (rep_cpt > 0) ->
		do
		:: L1MCCPACK ? m.type, X, m.val, m.cache_id -> 
			if
			:: (m.type == ACK_B_INV) ->
				atomic{
					rep_cpt = rep_cpt -1 ;
					goto ATT_B_INV_X 
				};
			:: else -> printf("MC_X : requete invalide chan L1MCCPACK val: %d\n",m.type);
			fi;
		:: L1MCCUREQ ? m.type, X, m.val, m.cache_id -> 
			if
			:: (m.type == CLNUP) ->
				atomic{
					n_cache = n_cache -1 ;
					rep_cpt = rep_cpt -1 ;
					goto ATT_B_INV_X 
				};
			:: else -> printf("MC_X : requete invalide chan L1MCCPACK val: %d\n",m.type);
			fi;
		od ;
	:: else -> skip;
	fi ;

	// envoie ACK_WR au source.
	m.type = ACK_WR  ;
	m.addr = X       ;
	m.cache_id = src ;
	m.val = v_x      ;
	MCL1DTACK ! m    ;
	
	// Envoie put au mémoire.
	m.type = PUT     ;
	m.addr = X       ;
	m.val = v_x      ;
	MCMEMDTREQ ! m   ;
	
	// on vide la liste =>
	cpt = 0 ;
	do
	:: (cpt == CACHE_TH) -> break ;
	:: else -> v_c_id[cpt] = INVALIDE; cpt = cpt+1 ;
	od ;
	n_cache = 0 ;

	// on consomme le message de B_INV
	MCL1CPREQ ? m  ;
	goto ATT_PUT_X ;
  
////////////////////
//CM attend PUT X //
////////////////////

ATT_PUT_X :

	printf("------> MC_X ETAT : ATT_PUT_X \n");

	MEMMCDTACK ? m.type, X , m.val, m.cache_id -> 
		if
		:: (m.type == ACK_PUT) -> goto Empty_X ;
		:: else -> printf("MC_X : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi;

}


///////////////////////////////////////////
//	Controleur mémoire pour Y        //
///////////////////////////////////////////

proctype MC_Y (chan L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK){


	msg m ;
	byte src ;
	byte v_y ;
	byte c_id_tmp ;		// case temporelle à sauvegarder le cache id.
	byte cpt,rep_cpt ;	// compteurs temporelles.
	byte c_id[CACHE_TH];	// tableau qui contient les id des caches
	bit v_c_id[CACHE_TH];	// tableau qui contient la validité des cases dans c_id ;
	byte n_cache ;		// nombre de cache qui contient des copies.

///////////////////////////////////////
//Case Y vide                        //
///////////////////////////////////////

Empty_Y :

	printf("------> MC_Y ETAT : EMPTY_Y \n");

	L1MCDTREQ ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == RD) -> 
			atomic {
				src = m.cache_id ;
				m.type = GET     ;
				m.addr = Y       ;
				MCMEMDTREQ ! m   ;
				goto ATT_RD_Y 
			};
		:: (m.type == WR) ->
			atomic {
				v_y = m.val           ;
				c_id_tmp = m.cache_id ;
				m.type = GET          ;
				m.addr = Y            ;
				MCMEMDTREQ ! m        ;		// GET du bloc qui contient Y.
				m.type = ACK_WR       ;
				m.val = v_y           ;
				m.addr = Y            ;
				m.cache_id = c_id_tmp ;
				MCL1DTACK ! m         ;		// ACK_WR pour L1.
				goto ATT_WR_Y 
			};
		:: else -> printf("MC_Y : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

////////////////////////////////////////
//ATTENTE LE BLOC POUR ECRIRE VAL DE Y//
////////////////////////////////////////

ATT_WR_Y :

	printf("------> MC_Y ETAT : ATT_WR_Y \n");

	do
	:: L1MCDTREQ ? WR , Y, m.val, m.cache_id -> 
		atomic {
			v_y = m.val     ;
			m.type = ACK_WR ;	// m.val et m.cache_id non modifier.
			m.addr = Y      ;
			MCL1DTACK ! m   ;
			goto ATT_WR_Y   
		};
	:: MEMMCDTACK ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == ACK_GET) ->
			atomic {
				n_cache = 0;
				// on modifie pas la valeur de Y, car elle vient apres un write.
				goto V_M_D_Y 
			};
		:: else -> printf("MC_Y : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi;
	od;


/////////////////////////////////////////
//ATTENTE LE BLOC POUR LECTURE VAL DE Y//
/////////////////////////////////////////

ATT_RD_Y :

	printf("------> MC_Y ETAT : ATT_RD_Y \n");

	do 
	:: L1MCDTREQ ? WR, Y, m.val,m.cache_id -> 
		atomic{
			v_y = m.val     ;
			m.type = ACK_WR ;
			m.addr = Y      ;	// m.val et m.cache_id non modifier.
			MCL1DTACK ! m   ;
			goto ATT_RD_WR_Y 
		};
	:: MEMMCDTACK ? m.type, Y, m.val,m.cache_id -> 
		if
		:: (m.type == ACK_GET) ->
			atomic{
				v_y = m.val       ;
				m.type = ACK_RD   ;
				m.addr =  Y       ;
				m.cache_id = src  ;		// m.val non modifier.
				MCL1DTACK ! m     ;
				c_id[0] = src     ;
				v_c_id[0] = VALIDE;
				n_cache = 1       ;
				goto V_M_Y 
			};
		:: else -> printf("MC_Y : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi;
	od;


/////////////////////////////////////////////////////
//ATTENTE LE BLOC POUR LECTURE VAL DE Y APRES WRITE//
/////////////////////////////////////////////////////

ATT_RD_WR_Y :

	printf("------> MC_Y ETAT : ATT_RD_WR_Y \n");

	do
	:: L1MCDTREQ ? WR, Y, m.val, m.cache_id -> 
		atomic{
			v_y = m.val      ;
			m.type == ACK_WR ;
			m.addr = Y       ;		// m.cache_id non modifier.
			MCL1DTACK ! m    ;
			goto ATT_RD_WR_Y 
		};
	:: MEMMCDTACK ? m.type, Y,m.val, m.cache_id -> 
		if
		:: (m.type == ACK_GET)->
			atomic{
				m.type = ACK_RD  ;
				m.addr = Y       ;
				m.val = v_y      ;		// resultat de la derniere ecriture.
				m.cache_id = src ;
				MCL1DTACK ! m    ;		// ACK_RD pour src.
				c_id[0]=src      ;
				v_c_id[0]=VALIDE   ;
				n_cache = 1      ;
				goto V_M_D_Y 			
			};
		:: else -> printf("MC_Y : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi ;
	od;
		
		
///////////////////////
//CM CONTIENT Y DIRTY//
///////////////////////

V_M_D_Y :

	printf("------> MC_Y ETAT : V_M_D_Y \n");

	do
	:: L1MCDTREQ ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic {
				src = m.cache_id;
				// 2 cas n_cache < CACHE_TH => on reste dans V_M_D_Y 
				// else on passe à V_B_D_Y
				if 
				:: (n_cache < CACHE_TH) ->
					m.type = ACK_RD ;
					m.addr = Y      ;
					m.val = v_y     ;		// m.cache_id non modifié.
					MCL1DTACK ! m   ;		// sending ACK_RD.
					cpt = 0         ;
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt< CACHE_TH) && (v_c_id[cpt] == INVALIDE)) ->
								c_id[cpt] = src      ;   
								v_c_id[cpt] = VALIDE ;
								n_cache = n_cache +1 ;
								break                ;
						:: else -> cpt = cpt +1 ;
						fi ;
					od ;
					goto V_M_D_Y ;
				:: else ->
					n_cache = n_cache +1;
					m.type = ACK_RD     ;
					m.addr = Y          ;
					m.val = v_y         ;
					m.cache_id = src    ;
					MCL1DTACK ! m       ;
					goto V_B_D_Y        ;
				fi		
											
			};		
		:: (m.type == WR) ->
			atomic {						
			src = m.cache_id  ;		// sauvegarde la destination du WR.
			v_y = m.val       ;		
			rep_cpt = n_cache ;		// n_cache >1 forcement, compteur d'aquitement.
			cpt = 0 ;

			// recherche du source.
			if
			:: (n_cache >0)->
				do
				:: if
					:: (cpt == CACHE_TH) -> break ;
					:: ((cpt < CACHE_TH) && (c_id[cpt] == src) && (v_c_id[cpt] == VALIDE)) -> 
						rep_cpt = rep_cpt -1; 
						break               ;
					:: else -> cpt = cpt+1;
					fi ;
				od;
			:: else -> skip ;
			fi ;
			// coherence.
			if 
			:: (( n_cache > 1 ) || ((n_cache == 1) && (cpt == CACHE_TH))) -> 
				// sending multicast update.
				m.type = M_UP ;
				m.addr = Y    ;
				m.val = v_y   ;
				cpt=0         ;
				do
				:: if
					:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] != src)) ->
						m.cache_id = c_id[cpt] ;
						MCL1CPREQ ! m          ;
						cpt = cpt + 1          ;
						:: (cpt == CACHE_TH) -> break ;
						:: else -> cpt = cpt+1 ;
					fi ;
				od;
			:: else -> skip ;
			fi ;	
			goto ATT_UP_Y 
		};
		:: else -> printf("MC_Y : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) -> 
			atomic {
				c_id_tmp = m.cache_id   ;
				m.type = ACK_CLNUP ;
				m.addr = Y         ; 		// m.val et m.cache_id non modifier.
				MCL1CUACK ! m      ;
				// supression de l'élement de la liste.
				cpt = 0 ;
				do
				:: if
					:: (cpt == CACHE_TH) -> break ;
					:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == c_id_tmp)) ->
						v_c_id[cpt] = INVALIDE ;
						n_cache = n_cache -1   ;
						break                  ;
					:: else -> cpt = cpt+1 ;
					fi ;
				od;
				goto V_M_D_Y 
			};
		:: else -> printf("MC_Y : requete invalide chan L1MCCUREQ val: %d\n",m.type);
		fi;
	od ;


///////////////////////
//CM CONTIENT Y      //
///////////////////////

V_M_Y :

	printf("------> MC_Y ETAT : V_M_Y \n");

	do
	:: L1MCDTREQ ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic {
				src = m.cache_id;
				// 2 cas n_cache < CACHE_TH => on reste dans V_M_Y 
				// else on passe à V_B_Y
				if 
				:: (n_cache < CACHE_TH) ->
					m.type = ACK_RD ;
					m.addr = Y      ;
					m.val = v_y     ;		// m.cache_id non modifié.
					MCL1DTACK ! m   ; 		// sending ACK_RD.
					cpt = 0         ;
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt< CACHE_TH) && (v_c_id[cpt] == INVALIDE)) ->
							c_id[cpt] = src      ;
							v_c_id[cpt] = VALIDE ;
							n_cache = n_cache +1 ;
							break                ;
						:: else -> cpt = cpt +1 ;
						fi ;
					od ;
					goto V_M_Y ;
				:: else ->
					n_cache = n_cache +1 ;
					m.type = ACK_RD      ;
					m.addr = Y           ;
					m.val = v_y          ;
					m.cache_id = src     ;
					MCL1DTACK ! m        ;
					goto V_B_Y           ;
				fi;							
			};		
		:: (m.type == WR) ->
			atomic {
				src = m.cache_id  ;
				v_y = m.val       ;
				rep_cpt = n_cache ;		// n_cache >1 forcement, compteur d'aquitement.
				// recherche du source.
				cpt = 0 ;
				if
				:: (n_cache  > 0)->
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt < CACHE_TH) && (c_id[cpt] == src) && (v_c_id[cpt] == VALIDE)) -> 
							rep_cpt = rep_cpt -1 ; 
							break                ;
						:: else -> cpt = cpt+1;
						fi ;
					od;
				:: else -> skip ;
				fi ;
				// coherence.
				if 
				:: (( n_cache > 1 ) || ((n_cache == 1) && (cpt == CACHE_TH))) -> 
					// sending multicast update.
					m.type = M_UP ;
					m.addr = Y    ;
					m.val = v_y   ;
					cpt=0         ;
					do
					:: if
						:: (cpt == CACHE_TH) ->break ;
						:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] != src)) ->
								m.cache_id = c_id[cpt] ;
								MCL1CPREQ ! m          ;
								cpt = cpt + 1          ;
						:: else -> cpt = cpt+1 ;
						fi ;
					od;
					goto ATT_UP_Y ;
				:: else -> goto ATT_UP_Y ;
				fi ;
			};
		:: else -> printf("MC_Y : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) -> 
			atomic {
				c_id_tmp = m.cache_id   ;
				m.type = ACK_CLNUP ;
				m.addr = Y         ;		// m.val et m.cache_id non modifier.
				MCL1CUACK ! m      ;
				// supression de l'élement de la liste.
				cpt = 0 ;
				do
				:: if
					:: (cpt == CACHE_TH) -> break ;
					:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == c_id_tmp)) ->
						v_c_id[cpt] = INVALIDE ;
						n_cache = n_cache -1   ;
						break                  ;
					:: else -> cpt = cpt+1 ;
					fi ;
				od;
				goto V_M_Y 
			};
		:: else -> printf("MC_Y : requete invalide chan L1MCCUREQ val: %d\n",m.type);
		fi;
	od ;

////////////////////////////
//CM attend ACK de update //
////////////////////////////

ATT_UP_Y :
	
	printf("------> MC_Y ETAT : ATT_UP_Y \n");

	// src contient l'id du cache qui demande WR.
	// rep_cpt contient le nombre d'aquitement à attendre.
	// rep_cpt = 0 donc 1 cache demande un read puis un write => aucune UPDATE envoyer.

	if
	:: (rep_cpt > 0)->
		do
		:: L1MCCPACK ? m.type, Y, m.val, m.cache_id -> 
			if
			:: (m.type == ACK_M_UP) -> 
				atomic{
					rep_cpt = rep_cpt - 1;
					goto ATT_UP_Y 	
				};
			:: else -> printf("MC_Y : requete invalide chan L1MCCPACK val: %d\n",m.type);
			fi;
		:: L1MCCUREQ ? m.type, Y, m.val,m.cache_id -> 
			if
			:: (m.type == CLNUP) ->
				atomic{
					rep_cpt = rep_cpt - 1;
					c_id_tmp = m.cache_id   ;
					m.type = ACK_CLNUP ;
					m.addr = X         ; 		// m.val et m.cache_id non modifier.
					MCL1CUACK ! m      ;
					// supression de l'élement de la liste.
					cpt = 0 ;
					do
					:: if
						:: (cpt == CACHE_TH) -> break ;
						:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == c_id_tmp)) ->
							v_c_id[cpt] = INVALIDE ;
							n_cache = n_cache -1   ;
							break                  ;
						:: else -> cpt = cpt+1 ;
						fi ;
					od;
					goto ATT_UP_Y 
				};
			:: else -> printf("MC_Y : requete invalide chan L1MCCUREQ val: %d\n",m.type);
			fi;
		od;
	:: else -> skip ;
	fi; 

// Envoie ACK_WR au src.

	m.type = ACK_WR  ;
	m.addr = Y       ;
	m.val = v_y      ;
	m.cache_id = src ;
	MCL1DTACK ! m    ;
	goto V_M_D_Y     ;

///////////////////////////////
//CM contient Y en broadcast //
///////////////////////////////

V_B_Y :

	printf("------> MC_Y ETAT : V_B_Y \n");

	do
	:: L1MCDTREQ ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic{
				n_cache = n_cache + 1;
				m.type = ACK_RD      ;
				m.addr = Y           ;
				m.val = v_y          ;		// m.cache_id non modifié.
				MCL1DTACK ! m        ;		// ACK_RD.
				goto V_B_Y  
			};
		:: (m.type == WR) ->
			src = m.cache_id ;
			atomic{
				// sending B_INV pour tous les caches.
				m.type = B_INV    ;
				m.val = v_y       ;
				m.addr = Y        ;
				MCL1CPREQ ! m     ;	
				//src aussi invalide son copie car liste perdue.
				rep_cpt = n_cache ;
				goto ATT_B_INV_Y 
			};
		:: else -> printf("MC_Y : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type , Y, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) ->
			atomic {
				n_cache = n_cache -1 ;
				m.type = ACK_CLNUP   ;
				m.addr = Y           ;
				m.val = v_y          ;	// m.cache_id non modifié.
				MCL1CUACK ! m        ;
				goto V_B_Y 									
			};
		:: else -> printf("MC_Y : Requete invalide");
		fi;
	od;

/////////////////////////////////////
//CM contient Y dirty en broadcast //
/////////////////////////////////////

V_B_D_Y :

	printf("------> MC_Y ETAT : V_B_D_Y \n");

	do
	:: L1MCDTREQ ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == RD) ->
			atomic{
				n_cache = n_cache + 1;
				m.type = ACK_RD      ;
				m.addr = Y           ;
				m.val = v_y          ;		// m.cache_id non modifié.
				MCL1DTACK ! m        ;		// ACK_RD.
				goto V_B_D_Y  
			};
		:: (m.type == WR) ->
			src = m.cache_id ;
			atomic{
				// B_INV pour tous les caches.
				m.type = B_INV ;
				m.val = v_y    ;
				m.addr = Y     ;
				MCL1CPREQ ! m  ;

				//src aussi invalide son copie car liste perdue.
				rep_cpt = n_cache ;

				goto ATT_B_INV_Y 
			};
		:: else -> printf("MC_Y : requete invalide chan L1MCDTREQ val: %d\n",m.type);
		fi;

	:: L1MCCUREQ ? m.type , Y, m.val, m.cache_id -> 
		if
		:: (m.type == CLNUP) ->
			atomic {
				n_cache = n_cache -1 ;
				m.type = ACK_CLNUP   ;
				m.addr = Y           ;
				m.val = v_y          ;	// m.cache_id non modifié.
				MCL1CUACK ! m        ;
				goto V_B_D_Y									
			};
		:: else -> printf("MC_Y : requete invalide chan L1MCCUREQ val: %d\n",m.type);
		fi;
	od;

/////////////////////////////////////
//CM attend broadcast invalidation //
/////////////////////////////////////

ATT_B_INV_Y :

	printf("------> MC_Y ETAT : ATT_B_INV_Y \n");

	// rep_cpt = nombre d'aquitement à recevoire.
	// src contient l'adresse du source qui a demander un WR.

	if
	:: (rep_cpt > 0) ->
		do
		:: L1MCCPACK ? m.type, Y, m.val, m.cache_id -> 
			if
			:: (m.type == ACK_B_INV) ->
				atomic{
					rep_cpt = rep_cpt -1 ;
					goto ATT_B_INV_Y 
				};
			:: else -> printf("MC_Y : requete invalide chan L1MCCPACK val: %d\n",m.type);
			fi;
		:: L1MCCUREQ ? m.type, Y, m.val, m.cache_id -> 
			if
			:: (m.type == CLNUP) ->
				atomic{
					rep_cpt = rep_cpt -1 ;
					n_cache = n_cache -1 ;
					goto ATT_B_INV_Y 
				};
			:: else -> printf("MC_Y : requete invalide chan L1MCCPACK val: %d\n",m.type);
			fi;
		od ;
	:: else -> skip;
	fi ;

	// envoie ACK_WR au source.
	m.type = ACK_WR  ;
	m.addr = Y       ;
	m.cache_id = src ;
	m.val = v_y      ;
	MCL1DTACK ! m    ;
	
	// Envoie put au mémoire.
	m.type = PUT     ;
	m.addr = Y       ;
	m.val = v_y      ;
	MCMEMDTREQ ! m   ;
	
	// on vide la liste =>
	cpt = 0 ;
	do
	:: (cpt == CACHE_TH) -> break ;
	:: else -> v_c_id[cpt] = INVALIDE; cpt = cpt+1 ;
	od ;
	n_cache = 0 ;

	// on consomme le message de B_INV
	MCL1CPREQ ? m  ;
	goto ATT_PUT_Y ;
  
////////////////////
//CM attend PUT Y //
////////////////////

ATT_PUT_Y :

	printf("------> MC_Y ETAT : ATT_PUT_Y \n");

	MEMMCDTACK ? m.type, Y, m.val, m.cache_id -> 
		if
		:: (m.type == ACK_PUT) -> goto Empty_Y ;
		:: else -> printf("MC_Y : requete invalide chan MEMMCDTACK val: %d\n",m.type);
		fi;

}

#endif

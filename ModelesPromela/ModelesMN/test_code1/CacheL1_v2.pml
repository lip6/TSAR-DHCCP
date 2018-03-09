/************************************************************************/
/* Modèle du cache L1 du machine multiprocesseur TSAR *******************/
/* en language Promela **************************************************/
/************************************************************************/

/* Je définie le case mémoire qui contient la valeur du cache comme     */
/* Une variable globale, puisqu'elle sera modifier par le controleur    */
/* mémoire.                                                             */

/************************************************************************/
/*************************** Structures *********************************/
/************************************************************************/

typedef  msg {
	mtype type ;	// type du message.
	int addr;	// adresse du case mémoire.
	int val ;	// valeur du case mémoire.
	byte cache_id ;	// identifiant du processeur.
	}

/******************** Definition des 2 valeurs X et Y ********************/

#define X 100
#define Y 101


/************* Definition des 2 valeurs INVALIDE et VALIDE ***************/

#define INVALIDE 0
#define VALIDE   1


/********** Type de message dans les canaux de communications ***********/

mtype = {DT_RD, DT_WR, ACK_DT_RD, ACK_DT_WR, RD, WR, ACK_RD, ACK_WR, CLNUP, ACK_CLNUP, B_INV, M_INV, M_UP, ACK_B_INV, ACK_M_INV, ACK_M_UP, GET, PUT, ACK_GET, ACK_PUT}


/************************* Variable globale ******************************/

// le processeur comme le controleur mémoire , lit directement la valeur de v_cache une fois il recoit l'aquitement de lecture.

int v_cache   ; 		// valeur du block mémoire dans la cache, et le controleur écrit directement dans cette case la valeur.
bit v_cache_valide ; 		// bit de validité ,la cache elle modifier cette valeur 0: invalide , 1: valide.
int v_addr ;			// entier qui contient l'adresse , X oubien Y .


/********** Processus représentant le cache L1 **************************/
/* Voir rapport de spécification,                                       */
/* pour les listes des canaux de communications en argument             */ 
/************************************************************************/

proctype CacheL1 (chan PL1DTREQ, L1PDTACK, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK ; byte c_id){

///////////////////////////////////////
// Déclaration des variables locales.//
// msg : variable qui contient la    //
// valeur recue dans 1 des canaux.   //
// val : Valeur du blocs mémoire     //
// dans la cache.                    //
///////////////////////////////////////

	 msg m;

///////////////////////////////////////
// Etat initial, le cache est vide   //
///////////////////////////////////////

Empty :
	printf("Cache L1 => STATE : EMPTY\n");
	v_cache_valide = INVALIDE;
	

	do
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) -> if 
				//!Peut ne pas etre mise, mais on modélise chaque transition explicitement.
				:: ((m.type == M_INV) || (m.type == B_INV) ) -> goto Empty ;
				:: else -> printf("Etat Empty : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
    			      fi ; 						  //! manque cas de M_UP ?
	
	:: PL1DTREQ ? m -> if
				:: ((m.type == DT_RD) && (m.addr == X)) -> goto MISS_X    ;
				:: ((m.type == DT_RD) && (m.addr == Y)) -> goto MISS_Y    ;
				:: ((m.type == DT_WR) && (m.addr == X)) -> 
					 m.type= WR ; m.addr = X ; m.cache_id = c_id ; L1MCDTREQ ! m; goto ATT_WR1_X ;	
				:: ((m.type == DT_WR) && (m.addr == Y)) ->
					 m.type= WR ; m.addr = Y ; m.cache_id = c_id ; L1MCDTREQ ! m ; goto ATT_WR1_Y ;
				:: else -> printf("Etat EMPTY : Requête invalide chan PL1DTREQ : val %d\n",m.type);
			     fi ;

	od;



///////////////////////////////////////
//Attente d'écriture X et cache vide //
///////////////////////////////////////

ATT_WR1_X :
	printf ("CacheL1 => STATE : ATT_WR1_X\n");

	do
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) -> if
				//!Peut ne pas etre mise, mais on modélise chaque transition explicitement.
				:: ((m.type == M_INV) || (m.type == B_INV)) -> goto ATT_WR1_X ;
				:: else -> printf("Etat ATT_WR1_X : Requête invalide chan MCL1CPREQ : val %d\n",m.type);		
			      fi ;
	:: MCL1DTACK ? m.type,m.addr,m.val,eval(c_id) -> if
				:: ((m.type == ACK_WR) && (m.addr == X) ) ->
					 m.type = ACK_DT_WR ; m.addr = X ; L1PDTACK ! m ; goto Empty ;
				:: else -> printf("Etat ATT_WR1_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
			      fi ;
	od ;



///////////////////////////////////////
// Attente d'écriture Y et cache vide//
///////////////////////////////////////

ATT_WR1_Y :
	printf("CacheL1 => STATE : ATT_WR1_Y\n");
	do
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) -> if
				//!Peut ne pas etre mise, mais on modélise chaque transition explicitement.
				:: ((m.type == M_INV) || (m.type == B_INV)) -> goto ATT_WR1_Y ;
				:: else -> printf("Etat ATT_WR1_Y : Requête invalide chan MCL1CPREQ : val %d\n",m.type);		
			      fi ;
	:: MCL1DTACK ? m.type,m.addr,m.val,eval(c_id) -> if
				:: ((m.type == ACK_WR) && (m.addr == Y) ) ->  
					m.type = ACK_DT_WR ; m.addr = Y ; L1PDTACK ! m ; goto Empty ;
				:: else -> printf("Etat ATT_WR1_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
			      fi ;
	od ;

///////////////////////////////////////
// Miss sur la valeur X              //
///////////////////////////////////////

MISS_X :
	printf("CacheL1 => STATE : MISS_X\n");
	
	do

	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) -> 	if
				:: ( ((m.type == M_INV) || (m.type == B_INV)) && (m.addr == X) ) ->  goto MISS_X ; 	
				:: ( (m.type == M_INV) && (m.addr == Y) ) -> 
					if 
					 :: (v_addr == Y) -> 
							v_cache_valide = INVALIDE ; m.type = ACK_M_INV ; m.addr = Y ; m.cache_id = c_id ; L1MCCPACK ! m ;
					fi ;
				:: ( (m.type == B_INV) && (m.addr == Y) ) -> 
					if 
					 :: (v_addr == Y) -> 
							v_cache_valide = INVALIDE ; m.type = ACK_B_INV ; m.addr = Y ;m.cache_id = c_id ; L1MCCPACK ! m ;
					 fi ;

				:: else -> printf("Etat MISS_X : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: break ;
	od;

	// si la case contient Y, on réalise une invalidation sur Y et puis on demande une lecture de X.
	if 
	:: ((v_addr == Y) && (v_cache_valide == VALIDE)) -> v_cache_valide = INVALIDE ; m.type = CLNUP ; m.addr = Y ; m.cache_id = c_id; L1MCCUREQ ! m ; 
		do
		:: MCL1CUACK ? m.type,m.addr, m.val,eval(c_id) ->   if
					:: ((m.type == ACK_CLNUP) && (m.addr == Y)) -> m.type = RD ; m.addr = X ; m.cache_id == c_id; L1MCDTREQ ! m ; goto ATT_D_X ;
					:: else -> printf("Etat MISS_X : Requête invalide chan MCL1CUACK : val %d\n",m.type);
					fi ;
		od;
	:: else -> m.type = RD ; m.addr = X ; m.cache_id = c_id ; L1MCDTREQ ! m ; goto ATT_D_X ;
	fi ;

///////////////////////////////////////
// Miss sur la valeur Y              //
///////////////////////////////////////

MISS_Y :
	printf("CacheL1 => STATE : MISS_Y\n");
	do 

	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) -> 	if
				:: ( ((m.type == M_INV) || (m.type == B_INV)) && (m.addr == Y) ) ->  goto MISS_Y ; 	
				:: ( (m.type == M_INV) && (m.addr == X) ) -> if 
							 :: (v_addr == X) -> 
								v_cache_valide = INVALIDE ; m.type = ACK_M_INV ; m.cache_id = c_id ; m.addr = X ; L1MCCPACK ! m ;
							 fi ;
				:: ( (m.type == B_INV) && (m.addr == X) ) -> if 
							 :: (v_addr == X) -> 
								v_cache_valide = INVALIDE ; m.type = ACK_B_INV ; m.cache_id = c_id ; m.addr = X ; L1MCCPACK ! m ;
							 fi ;

				:: else -> printf("Etat MISS_Y : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: break ;
	od ;

	// si la case contient X, on réalise une invalidation sur X et puis on demande une lecture de Y.
	if 
	:: ((v_addr == X) && (v_cache_valide == VALIDE)) -> v_cache_valide = INVALIDE ; m.type = CLNUP ; m.addr = X ; m.cache_id = c_id; L1MCCUREQ ! m ; 
		do
		:: MCL1CUACK ?  m.type,m.addr, m.val,eval(c_id)->   if
					:: ((m.type == ACK_CLNUP) && (m.addr == X)) -> m.type = RD ; m.addr = Y ; m.cache_id == c_id; L1MCDTREQ ! m ; goto ATT_D_Y ;
					:: else -> printf("Etat MISS_Y : Requête invalide chan MCL1CUACK : val %d\n",m.type) ; break;
					fi ;
		od;
	:: else -> m.type = RD ; m.addr = Y ; m.cache_id = c_id ; L1MCDTREQ ! m ; goto ATT_D_Y ;
	fi ;


///////////////////////////////////////
// Attente la transfer de X          //
///////////////////////////////////////

ATT_D_X :
	printf("CacheL1 => STATE : ATT_D_X\n");
	do
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == Y)) -> goto ATT_D_X ;
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == X)) -> goto CL_R_X  ;
				:: else -> printf("Etat ATT_D_X : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_RD) && (m.addr == X)) -> 
					v_cache_valide= VALIDE ; v_addr = X ; v_cache = m.val ; m.type = ACK_DT_RD ; m.addr = X ; L1PDTACK ! m ; goto V_X ; 
				:: else -> printf("Etat ATT_D_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;


///////////////////////////////////////
// Attente la transfer de Y          //
///////////////////////////////////////

ATT_D_Y :
	printf("CacheL1 => STATE : ATT_D_Y\n");
	do
	:: MCL1CPREQ ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == X)) -> goto ATT_D_Y ;
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == Y)) -> goto CL_R_Y  ;
				:: else -> printf("Etat ATT_D_Y : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_RD) && (m.addr == Y)) -> 
					v_cache_valide= VALIDE ; v_addr = Y ; v_cache = m.val ; m.type = ACK_DT_RD ; m.addr = Y; L1PDTACK ! m ; goto V_Y ;
				:: else -> printf("Etat ATT_D_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;


///////////////////////////////////////
// CLEANUP de READ sur X             //
///////////////////////////////////////

CL_R_X :
	printf("CacheL1 => STATE : CL_R_X\n");
	do
	:: MCL1CPREQ ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == Y)) -> goto CL_R_X  ;
				:: else -> printf("Etat CL_R_X : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_RD) && (m.addr == X)) -> m.type =CLNUP ; m.addr = X ; m.cache_id = c_id ; L1MCCUREQ ! m; 
					// boucle d'attente du réponse de clean up sur X.
					do 
					:: MCL1CUACK ?  m.type,m.addr, m.val,eval(c_id) ->   if 
								:: ((m.type == ACK_CLNUP) && (m.addr ==X )) -> goto MISS_X ;
								:: else -> printf("Etat CL_R_X : Requête invalide chan MCL1CUACK : val %d\n",m.type);
								fi;
					od;	
				:: else -> printf("Etat CL_R_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;


///////////////////////////////////////
// CLEANUP de READ sur X             //
///////////////////////////////////////

CL_R_Y :
	printf("CacheL1 => STATE : CL_R_Y\n");
	do
	:: MCL1CPREQ ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == X)) -> goto CL_R_Y  ;
				:: else -> printf("Etat CL_R_Y : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_RD) && (m.addr == Y)) -> m.type= CLNUP ; m.addr = Y ; m.cache_id = c_id ; L1MCCUREQ ! m; 
					// boucle d'attente du réponse de clean up sur X.
					do 
					:: MCL1CUACK ?  m.type,m.addr, m.val,eval(c_id) ->   if 
								:: ((m.type == ACK_CLNUP) && (m.addr == Y) ) -> goto MISS_Y ;
								:: else -> printf("Etat CL_R_Y : Requête invalide chan MCL1CUACK : val %d\n",m.type);
								fi;
					od;	
				:: else -> printf("Etat CL_R_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;

///////////////////////////////////////
// Cache contient la valeur X        //
///////////////////////////////////////

V_X :
	printf("CacheL1 => STATE : V_X , Valeur : %d\n",v_cache);
	do 
	:: MCL1CPREQ ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == Y)) -> goto V_X ;	
				:: ((m.type == M_INV) && (m.addr == X)) ->
					v_cache_valide = INVALIDE ; m.type = ACK_M_INV; m.addr = X ; m.cache_id = c_id ; L1MCCPACK ! m ; goto Empty ;	
 				:: ((m.type == B_INV) && (m.addr == X)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_B_INV; m.addr = X ; m.cache_id = c_id ; L1MCCPACK ! m ; goto Empty ;	
				:: else -> printf("Etat V_X : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: PL1DTREQ ? m ->    if
				:: ((m.type == DT_RD) && (m.addr == X)) -> m.type = ACK_DT_RD ; m.addr = X ; m.val = v_cache ; L1PDTACK ! m ; goto V_X ;
				:: ((m.type == DT_RD) && (m.addr == Y)) -> goto MISS_Y ;
				:: ((m.type == DT_WR) && (m.addr == X)) -> m.type = WR ; m.addr=X ; m.cache_id = c_id ; L1MCDTREQ ! m ;goto ATT_WR2_X ;		
				:: ((m.type == DT_WR) && (m.addr == Y)) -> m.type = WR ; m.addr=Y ; m.cache_id = c_id ; L1MCDTREQ ! m ; goto ATT_WR2_Y ;
				:: else -> printf("Etat V_X : Requête invalide chan PL1DTREQ : val %d\n",m.type);
				fi;	
	od;

///////////////////////////////////////
// Cache contient la valeur Y        //
///////////////////////////////////////

V_Y :
	printf("CacheL1 => STATE : V_Y, valeur : %d\n",v_cache);
	do 
	:: MCL1CPREQ ?  m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == X)) -> goto V_Y ;	
				:: ((m.type == M_INV) && (m.addr == Y)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_M_INV ; m.addr = Y ; m.cache_id = c_id ; L1MCCPACK ! m ; goto Empty ;	
 				:: ((m.type == B_INV) && (m.addr == Y)) ->
					v_cache_valide = INVALIDE ; m.type = ACK_B_INV ; m.addr = Y ; m.cache_id = c_id ; L1MCCPACK ! m ; goto Empty ;	
				:: else -> printf("Etat V_Y : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: PL1DTREQ ? m  ->    if
				:: ((m.type == DT_RD) && (m.addr == Y)) -> m.type = ACK_DT_RD ; m.addr = Y ; m.val = v_cache ; L1PDTACK ! m ; goto V_Y ;
				:: ((m.type == DT_RD) && (m.addr == X)) -> goto MISS_X ;
				:: ((m.type == DT_WR) && (m.addr == X)) -> m.type = WR ; m.addr = X ;m.cache_id = c_id ;L1MCDTREQ ! m ;goto ATT_WR3_X ;		
				:: ((m.type == DT_WR) && (m.addr == Y)) -> m.type = WR ; m.addr = Y ;m.cache_id = c_id ;L1MCDTREQ ! m ;goto ATT_WR3_Y ;
				:: else -> printf("Etat V_Y : Requête invalide chan PL1DTREQ : val %d\n",m.type);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture X lié à V_X //
/////////////////////////////////////////

ATT_WR2_X :
	printf("CacheL1 => STATE : ATT_WR2_X\n");
	do 
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == Y)) -> goto ATT_WR2_X ;	
				:: ((m.type == M_INV) && (m.addr == X)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_M_INV ; m.addr = X ; m.cache_id = c_id; L1MCCPACK ! m ; goto ATT_WR1_X ;	
 				:: ((m.type == B_INV) && (m.addr == X)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_B_INV ; m.addr = X ; m.cache_id = c_id; L1MCCPACK ! m ; goto ATT_WR1_X ;	
				:: else -> printf("Etat ATT_WR2_X : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_WR) && (m.addr == X)) -> m.type = ACK_DT_WR ; m.addr = X ;v_cache = m.val ; L1PDTACK ! m ; goto V_X ;
				:: else -> printf("Etat ATT_WR2_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture Y lié à V_X //
/////////////////////////////////////////

ATT_WR2_Y :
	printf("CacheL1 => STATE : ATT_WR2_Y\n");
	do 
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == Y)) -> goto ATT_WR2_Y ;	
				:: ((m.type == M_INV) && (m.addr == X)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_M_INV ; m.addr = X ; m.cache_id = c_id ; L1MCCPACK ! m ; goto ATT_WR1_Y ;	
 				:: ((m.type == B_INV) && (m.addr == X)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_B_INV ; m.addr = X ; m.cache_id = c_id ; L1MCCPACK ! m ; goto ATT_WR1_Y ;	
				:: else -> printf("Etat ATT_WR2_Y : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_WR) && (m.addr == Y)) -> m.type = ACK_DT_WR ; m.addr = Y ; L1PDTACK ! m ; goto V_X ;
				:: else -> printf("Etat ATT_WR2_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture X lié à V_Y //
/////////////////////////////////////////

ATT_WR3_X :
	printf("CacheL1 : STATE : ATT_WR3_X\n");
	do 
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == X)) -> goto ATT_WR3_X ;	
				:: ((m.type == M_INV) && (m.addr == Y)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_M_INV ; m.addr = Y ; m.cache_id = c_id ; L1MCCPACK ! m ; goto ATT_WR1_X ;	
 				:: ((m.type == B_INV) && (m.addr == Y)) ->
					v_cache_valide = INVALIDE ; m.type = ACK_B_INV ; m.addr = Y ; m.cache_id = c_id ; L1MCCPACK ! m ; goto ATT_WR1_X ;	
				:: else -> printf("Etat ATT_WR3_X : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_WR) && (m.addr == X)) -> m.type = ACK_DT_WR ; m.addr = X ; L1PDTACK ! m ; goto V_Y ;
				:: else -> printf("Etat ATT_WR3_X : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture Y lié à V_X //
/////////////////////////////////////////

ATT_WR3_Y :
	printf("CacheL1 => STATE : ATT_WR3_Y\n");
	do 
	:: MCL1CPREQ ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: (((m.type == M_INV) || (m.type == B_INV)) && (m.addr == X)) -> goto ATT_WR3_Y ;	
				:: ((m.type == M_INV) && (m.addr == Y)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_M_INV; m.addr = Y ; m.cache_id = c_id ; L1MCCPACK ! m ; goto ATT_WR1_Y ;	
 				:: ((m.type == B_INV) && (m.addr == Y)) -> 
					v_cache_valide = INVALIDE ; m.type = ACK_B_INV; m.addr = Y ; m.cache_id = c_id ; L1MCCPACK ! m ; goto ATT_WR1_Y ;	
				:: else -> printf("Etat ATT_WR3_Y : Requête invalide chan MCL1CPREQ : val %d\n",m.type);
				fi;
	:: MCL1DTACK ? m.type,m.addr, m.val,eval(c_id) ->   if
				:: ((m.type == ACK_WR) && (m.addr == Y)) -> m.type = ACK_DT_WR ; m.addr = Y ; v_cache= m.val;L1PDTACK ! m ; goto V_Y ;
				:: else -> printf("Etat ATT_WR3_Y : Requête invalide chan MCL1DTACK : val %d\n",m.type);
				fi;
	od;

}

/********** Processus représentant le processeur ************************/
/* Test_P1 : un composant qui teste le modème du cache L1 pour          */
/* les requêtes de lecture et et d'écriture géré par le processeur      */
/* Pour les requêtes envoyer , voir rapport de spécification            */ 
/************************************************************************/


proctype Test_P1( chan PL1DTREQ , L1PDTACK ; byte p_id ) {


	msg m;
	bit flag =0 ;

	// 1 . demande un Read sur X.
	m.type = DT_RD ;
	m.addr = X ;	
	PL1DTREQ ! m ;
	// attente l'aquitement de lecture.
	goto ATT_ACK_RD_X ;
s1:
	flag=1 ;
	// 2. demande un Read sur X.
	m.type = DT_RD ;
	m.addr = X ;	
	PL1DTREQ ! m ;
	// attente l'aquitement de lecture.
	goto ATT_ACK_RD_X ;
s2:
	flag = 0;
	// 3. Ecriture sur X la valeur 10.
	m.type = DT_WR ;
	m.addr = X ;	
	m.val = 10 ;
	PL1DTREQ ! m ;
	goto ATT_ACK_WR_X ;
s3:
	// 4. Ecriture sur Y la valeur 11.
	m.type = DT_WR ;
	m.addr = Y ;	
	m.val = 11 ;
	PL1DTREQ ! m ;
	goto ATT_ACK_WR_Y ;

s4:
	// 5. Lecture sur Y.
	m.type = DT_RD ;
	m.addr = Y ;	
	PL1DTREQ ! m ;
	// attend l'aquitement de lecture de Y.
	goto ATT_ACK_RD_Y ;

s5:
	flag =1;
	// 6. Lecture sur Y.
	m.type = DT_RD ;
	m.addr = Y ;	
	PL1DTREQ ! m ;
	// attend l'aquitement de lecture de Y.
	goto ATT_ACK_RD_Y ;

s6:
	// 3. Ecriture sur X la valeur 20.
	m.type = DT_WR ;
	m.addr = X ;	
	m.val = 20 ;
	PL1DTREQ ! m ;
	// attend l'aquitement sur l'écriture de X .
	goto ATT_ACK_WR_X ;
	
	
s7:
	// 4. Ecriture sur Y la valeur 21.
	m.type = DT_WR ;
	m.addr = Y ;	
	m.val = 21 ;
	PL1DTREQ ! m ;
	// attend l'aquitement sur l'écriture de Y.
	goto ATT_ACK_WR_Y ;
	

ATT_ACK_RD_X :

	do
	:: L1PDTACK ? m ->    if
				:: ((m.type == ACK_DT_RD) && (m.addr == X)) -> printf("Test_P1 : Lecture X réussit , valeur = %d \n",m.val); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",m.type);
				fi;
	od ;
	if
	:: (flag == 0) -> goto s1 ;
	:: else -> goto s2 ;
	fi;



ATT_ACK_WR_X :

	do
	:: L1PDTACK ? m ->    if
				:: ((m.type == ACK_DT_WR) && (m.addr == X)) -> printf("Test_P1 : Ecriture X réussit , valeur écrite = %d \n",m.val); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",m.type);
				fi;
	od ;
	if
	:: (flag == 0) -> goto s3;
	:: else -> goto s7 ;
	fi;

ATT_ACK_WR_Y :

	do
	:: L1PDTACK ? m ->    if
				:: ((m.type == ACK_DT_WR) && (m.addr == Y)) -> printf("Test_P1 : Ecriture Y réussit , valeur écrite = %d \n",m.val); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",m.type);
				fi;
	od ;
	if
	:: (flag == 0) -> goto s4;
	:: else -> goto fin ;
	fi;

ATT_ACK_RD_Y :

	do
	:: L1PDTACK ? m ->    if
				:: ((m.type == ACK_DT_RD)&& (m.addr == Y)) -> printf("Test_P1 : Lecture Y réussit , valeur  = %d \n",m.val); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",m.type);
				fi;
	od ;

	if
	:: (flag == 0) -> goto s5;
	:: else -> goto s6 ;
	fi;

fin :
	skip ;
	
}

/********** Processus représentant le controleur mémoire ****************/
/* Test_C1 : un composant qui répond au requete du cache L1             */
/* Pour les états , voir rapport de spécification                       */ 
/************************************************************************/

proctype Test_C1(chan L1MCDTREQ, MCL1DTACK, L1MCCUREQ,MCL1CUACK,MCL1CPREQ,L1MCCPACK) {

	int v_x = 0;	// la valeur du case X initialisé à 0.
	int v_y = 0;	// la valeur du case y initialisé à 0.
	int cpt =0;
	msg m;

Idle :
	do
	:: L1MCDTREQ ? m ->   if
				:: ((m.type == RD) && (m.addr == X)) -> goto Read_x ;
				:: ((m.type == RD) && (m.addr == Y)) -> goto Read_y ;
				:: ((m.type == WR) && (m.addr == X)) -> goto Write_x ;
				:: ((m.type == WR) && (m.addr == Y)) -> goto Write_y ;
				:: else -> printf("Test_C1 : Requete invalide, chan L1MCDTREQ '%d' \n", m.type);
				fi;
	:: L1MCCUREQ ? m -> if
				:: ((m.type == CLNUP) && (m.addr == X)) -> goto CLN_X;
				:: ((m.type == CLNUP) && (m.addr == Y)) -> goto CLN_Y ;
				:: else -> printf ("Test_C1 : Requete invalide , chan L1MCCUREQ : %d \n , m.type");
			   	fi;
	od;

CLN_X :
	
	m.type = ACK_CLNUP ;
	m.addr = X ;
	m.cache_id = 0;	// to cache 0
	MCL1CUACK ! m;
	goto Idle;

CLN_Y :
	m.type = ACK_CLNUP ;
	m.addr = Y ;
	m.cache_id = 0;	// to cache 0
	MCL1CUACK ! m ;
	goto Idle ;

Read_x :
	cpt = 0;
	do
	:: if 
		:: (cpt == 10) ->break ;
		:: else -> cpt = cpt+1;
	   fi ;
	od; 
	m.type = ACK_RD ; 
	m.addr = X ;
	m.val = v_x ;
	MCL1DTACK ! m ;
	goto Idle ;

Read_y:
	cpt = 0;
	do
	:: if 	
		:: (cpt == 10) ->break ;
		:: else -> cpt = cpt+1;
	   fi ;
	od;
	m.type = ACK_RD ;
	m.addr = Y;
	m.val = v_y ;
	MCL1DTACK ! m;
	goto Idle ;

Write_x :
	cpt= 0;
	do
	:: if 
		:: (cpt == 10) ->break ;
		:: else -> cpt = cpt+1;
	   fi ;
	od;
	v_x = m.val ;
	m.type = ACK_WR ;
	m.addr = X ;
	MCL1DTACK ! m;
	goto Idle ;

Write_y :
	cpt = 0;
	do
	:: if 
		:: (cpt == 10) ->break ;
		:: else -> cpt = cpt+1;
	   fi ;
	od;
	v_y = m.val ;
	m.type = ACK_WR ;
	m.addr = Y ;
	MCL1DTACK ! m ;
	goto Idle ;

}

init {
	int cpt = 0;
	chan PL1DTREQ = [1] of {msg} ;
	chan L1PDTACK= [1] of {msg} ;
	chan L1MCDTREQ = [1] of {msg} ;
	chan MCL1DTACK = [1] of {msg};
	chan L1MCCUREQ = [1] of {msg} ;
	chan MCL1CUACK= [1] of {msg};
	chan MCL1CPREQ = [1] of {msg};
	chan L1MCCPACK = [1] of  {msg} ;
	
	
	run CacheL1(PL1DTREQ, L1PDTACK,L1MCDTREQ, MCL1DTACK,L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, 0);
	run Test_P1(PL1DTREQ, L1PDTACK , 0);
	run Test_C1(L1MCDTREQ, MCL1DTACK,L1MCCUREQ,MCL1CUACK,MCL1CPREQ,L1MCCPACK);


	do
	:: if
		:: (cpt == 200 ) -> break ;
		:: else -> cpt = cpt+1 ;
	fi ;
	od;

}


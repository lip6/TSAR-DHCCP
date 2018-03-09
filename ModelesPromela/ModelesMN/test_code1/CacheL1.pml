/************************************************************************/
/* Modèle du cache L1 du machine multiprocesseur TSAR *******************/
/* en language Promela **************************************************/
/************************************************************************/

/* Je définie le case mémoire qui contient la valeur du cache comme     */
/* Une variable globale, puisqu'elle sera modifier par le controleur    */
/* mémoire.                                                             */


/******************** Definition des 2 valeurs X et Y ********************/

#define X 100
#define Y 101


/************* Definition des 2 valeurs INVALIDE et VALIDE ***************/

#define INVALIDE 0
#define VALIDE   1

/************* Definition des 2 valeurs BEMPTY et BFULL *****************/

#define BEMPTY 2	// ! le processeur écrit son valeur dans buffer_cache et met le flag E_buffer_cache à BFULL, pour bloquer les autres écritures.
#define BFULL  3	// ! La cache demande l'écriture et à la réception des aquitement d'écriture met E_buffer_cache à BEMPTY.

/********** Type de message dans les canaux de communications ***********/

mtype = {DT_RD_X, DT_WR_X, ACK_DT_RD_X, ACK_DT_WR_X, RD_X, WR_X, ACK_RD_X, ACK_WR_X, CLNUP_X, ACK_CLNUP_X, B_INV_X, M_INV_X, M_UP_X, ACK_B_INV_X, ACK_M_INV_X, ACK_M_UP_X, GET_X, PUT_X, ACK_GET_X, ACK_PUT_X, DT_RD_Y, DT_WR_Y, ACK_DT_RD_Y, ACK_DT_WR_Y, RD_Y, WR_Y, ACK_RD_Y, ACK_WR_Y, CLNUP_Y, ACK_CLNUP_Y, B_INV_Y, M_INV_Y, M_UP_Y, ACK_B_INV_Y, ACK_M_INV_Y, ACK_M_UP_Y, GET_Y, PUT_Y, ACK_GET_Y, ACK_PUT_Y}

// on différencie les adresses par les requetes, sinon il faut ajouté un canal ou on envoie l'adresse du case en paralèlle avec la commande .


// ! Chaque canal ne contient q'un groupe de cette liste et on définie un canal qui contient tous ces valeurs chan c {mtype}.
// ! Pour cela on doit traiter les "else" des if pour que le système ne bloquera jamais dans une état.


/************************* Variable globale ******************************/

// le processeur comme le controleur mémoire , lit directement la valeur de v_cache une fois il recoit l'aquitement de lecture.

int v_cache   ; 		// valeur du block mémoire dans la cache, et le controleur écrit directement dans cette case la valeur.
bit v_cache_valide ; 		// bit de validité ,la cache elle modifier cette valeur 0: invalide , 1: valide.
int v_addr ;			// entier qui contient l'adresse , X oubien Y .
int buffer_cache ;		// Contient la valeur à écrire dans la buffer cache.
bit E_buffer_cache ;		// flag signalant si le buffer cache est vide.



/********** Processus représentant le cache L1 **************************/
/* Voir rapport de spécification,                                       */
/* pour les listes des canaux de communications en argument             */ 
/************************************************************************/

proctype CacheL1 (chan PL1DTREQ, L1PDTACK, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK){

///////////////////////////////////////
// Déclaration des variables locales.//
// msg : variable qui contient la    //
// valeur recue dans 1 des canaux.   //
// val : Valeur du blocs mémoire     //
// dans la cache.                    //
///////////////////////////////////////

	mtype msg ;

///////////////////////////////////////
// Etat initial, le cache est vide   //
///////////////////////////////////////

Empty :
	v_cache_valide = INVALIDE;

	do
	:: MCL1CPREQ ? msg -> if 
				//!Peut ne pas etre mise, mais on modélise chaque transition explicitement.
				:: ((msg == M_INV_X) || (msg == B_INV_X) || (msg == M_INV_Y) || (msg == B_INV_Y)) -> goto Empty ;
				:: else -> printf("Etat Empty : Requête invalide chan MCL1CPREQ : val %d\n",msg);
    			      fi ; 						  //! manque cas de M_UP ?
	
	:: PL1DTREQ ? msg -> if
				:: (msg == DT_RD_X) -> goto MISS_X    ;
				:: (msg == DT_RD_Y) -> goto MISS_Y    ;
				:: (msg == DT_WR_X) -> L1MCDTREQ ! WR_X ; goto ATT_WR1_X ;		// la valeur à écrire dans le buffer cache.
				:: (msg == DT_WR_Y) -> L1MCDTREQ ! WR_Y ; goto ATT_WR1_Y ;
				:: else -> printf("Etat EMPTY : Requête invalide chan PL1DTREQ : val %d\n",msg);
			     fi ;

	od;



///////////////////////////////////////
//Attente d'écriture X et cache vide //
///////////////////////////////////////

ATT_WR1_X :

	do
	:: MCL1CPREQ ? msg -> if
				//!Peut ne pas etre mise, mais on modélise chaque transition explicitement.
				:: ((msg == M_INV_X) || (msg == B_INV_X) || (msg == M_INV_Y) || (msg == B_INV_Y)) -> goto ATT_WR1_X ;
				:: else -> printf("Etat ATT_WR1_X : Requête invalide chan MCL1CPREQ : val %d\n",msg);		
			      fi ;
	:: MCL1DTACK ? msg -> if
				:: (msg == ACK_WR_X ) -> E_buffer_cache = BEMPTY ; L1PDTACK ! ACK_DT_WR_X ; goto Empty ;
				:: else -> printf("Etat ATT_WR1_X : Requête invalide chan MCL1DTACK : val %d\n",msg);
			      fi ;
	od ;



///////////////////////////////////////
// Attente d'écriture Y et cache vide//
///////////////////////////////////////

ATT_WR1_Y :

	do
	:: MCL1CPREQ ? msg -> if
				//!Peut ne pas etre mise, mais on modélise chaque transition explicitement.
				:: ((msg == M_INV_X) || (msg == B_INV_X) || (msg == M_INV_Y) || (msg == B_INV_Y)) -> goto ATT_WR1_Y ;
				:: else -> printf("Etat ATT_WR1_Y : Requête invalide chan MCL1CPREQ : val %d\n",msg);		
			      fi ;
	:: MCL1DTACK ? msg -> if
				:: (msg == ACK_WR_Y ) -> E_buffer_cache = BEMPTY ; L1PDTACK ! ACK_DT_WR_Y ; goto Empty ;
				:: else -> printf("Etat ATT_WR1_Y : Requête invalide chan MCL1DTACK : val %d\n",msg);
			      fi ;
	od ;

///////////////////////////////////////
// Miss sur la valeur X              //
///////////////////////////////////////

MISS_X :

	MCL1CPREQ ? msg -> 	if
				:: ( (msg == M_INV_X) || (msg == B_INV_X) ) ->  goto MISS_X ; 	
				:: ( msg == M_INV_Y ) -> if 
							 :: (v_cache == Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_Y ;
							 fi ;
				:: ( msg == B_INV_Y ) -> if 
							 :: (v_cache == Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_Y ;
							 fi ;

				:: else -> printf("Etat MISS_X : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;

	// si la case contient Y, on réalise une invalidation sur Y et puis on demande une lecture de X.
	if 
	:: ((v_addr == Y) && (v_cache_valide == VALIDE)) -> v_cache_valide = INVALIDE ; L1MCCUREQ ! CLNUP_Y ; 
		do
		:: MCL1CUACK ? msg ->   if
					:: (msg == ACK_CLNUP_Y) -> L1MCDTREQ ! RD_X ; goto ATT_D_X ;
					:: else -> printf("Etat MISS_X : Requête invalide chan MCL1CUACK : val %d\n",msg);
					fi ;
		od;
	:: else -> L1MCDTREQ ! RD_X ; goto ATT_D_X ;
	fi ;

///////////////////////////////////////
// Miss sur la valeur Y              //
///////////////////////////////////////

MISS_Y :

	MCL1CPREQ ? msg -> 	if
				:: ( (msg == M_INV_Y) || (msg == B_INV_Y) ) ->  goto MISS_Y ; 	
				:: ( msg == M_INV_X ) -> if 
							 :: (v_cache == X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_X ;
							 fi ;
				:: ( msg == B_INV_X ) -> if 
							 :: (v_cache == X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_X ;
							 fi ;

				:: else -> printf("Etat MISS_Y : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;

	// si la case contient X, on réalise une invalidation sur X et puis on demande une lecture de Y.
	if 
	:: ((v_addr == X) && (v_cache_valide == VALIDE)) -> v_cache_valide = INVALIDE ; L1MCCUREQ ! CLNUP_X ; 
		do
		:: MCL1CUACK ? msg ->   if
					:: (msg == ACK_CLNUP_X) -> L1MCDTREQ ! RD_Y ; goto ATT_D_Y ;
					:: else -> printf("Etat MISS_Y : Requête invalide chan MCL1CUACK : val %d\n",msg);
					fi ;
		od;
	:: else -> L1MCDTREQ ! RD_Y ; goto ATT_D_Y ;
	fi ;


///////////////////////////////////////
// Attente la transfer de X          //
///////////////////////////////////////

ATT_D_X :
	
	do
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_Y) || (msg == B_INV_Y)) -> goto ATT_D_X ;
				:: ((msg == M_INV_X) || (msg == B_INV_X)) -> goto CL_R_X  ;
				:: else -> printf("Etat ATT_D_X : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_RD_X) -> v_cache_valide= VALIDE ; v_addr = X ; L1PDTACK ! ACK_DT_RD_X ; goto V_X ; 	// CM écrit la valeur X dans v_cache.
				:: else -> printf("Etat ATT_D_X : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;


///////////////////////////////////////
// Attente la transfer de Y          //
///////////////////////////////////////

ATT_D_Y :

	do
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_X) || (msg == B_INV_X)) -> goto ATT_D_Y ;
				:: ((msg == M_INV_Y) || (msg == B_INV_Y)) -> goto CL_R_Y  ;
				:: else -> printf("Etat ATT_D_Y : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_RD_Y) -> v_cache_valide= VALIDE ; v_addr = Y ; L1PDTACK ! ACK_DT_RD_Y ; goto V_Y ; 	// CM écrit la valeur X dans v_cache.
				:: else -> printf("Etat ATT_D_Y : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;


///////////////////////////////////////
// CLEANUP de READ sur X             //
///////////////////////////////////////

CL_R_X :

	do
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_Y) || (msg == B_INV_Y)) -> goto CL_R_X  ;
				:: else -> printf("Etat CL_R_X : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_RD_X) -> L1MCCUREQ ! CLNUP_X; 
					// boucle d'attente du réponse de clean up sur X.
					do 
					:: MCL1CUACK ? msg ->   if 
								:: (msg == ACK_CLNUP_X ) -> goto MISS_X ;
								:: else -> printf("Etat CL_R_X : Requête invalide chan MCL1CUACK : val %d\n",msg);
								fi;
					od;	
				:: else -> printf("Etat CL_R_X : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;


///////////////////////////////////////
// CLEANUP de READ sur X             //
///////////////////////////////////////

CL_R_Y :

	do
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_X) || (msg == B_INV_X)) -> goto CL_R_Y  ;
				:: else -> printf("Etat CL_R_Y : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_RD_Y) -> L1MCCUREQ ! CLNUP_Y; 
					// boucle d'attente du réponse de clean up sur X.
					do 
					:: MCL1CUACK ? msg ->   if 
								:: (msg == ACK_CLNUP_Y ) -> goto MISS_Y ;
								:: else -> printf("Etat CL_R_Y : Requête invalide chan MCL1CUACK : val %d\n",msg);
								fi;
					od;	
				:: else -> printf("Etat CL_R_Y : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;

///////////////////////////////////////
// Cache contient la valeur X        //
///////////////////////////////////////

V_X :

	do 
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_Y) || (msg == B_INV_Y)) -> goto V_X ;	
				:: (msg == M_INV_X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_X ; goto Empty ;	
 				:: (msg == B_INV_X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_X ; goto Empty ;	
				:: else -> printf("Etat V_X : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: PL1DTREQ ? msg ->    if
				:: (msg == DT_RD_X) -> L1PDTACK ! ACK_DT_RD_X ; goto V_X ;
				:: (msg == DT_RD_Y) -> goto MISS_Y ;
				:: (msg == DT_WR_X) -> L1MCDTREQ ! WR_X ; goto ATT_WR2_X ;		
				:: (msg == DT_WR_Y) -> L1MCDTREQ ! WR_Y ; goto ATT_WR2_Y ;
				:: else -> printf("Etat V_X : Requête invalide chan PL1DTREQ : val %d\n",msg);
				fi;	
	od;

///////////////////////////////////////
// Cache contient la valeur Y        //
///////////////////////////////////////

V_Y :

	do 
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_X) || (msg == B_INV_X)) -> goto V_Y ;	
				:: (msg == M_INV_Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_Y ; goto Empty ;	
 				:: (msg == B_INV_Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_Y ; goto Empty ;	
				:: else -> printf("Etat V_Y : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: PL1DTREQ ? msg ->    if
				:: (msg == DT_RD_Y) -> L1PDTACK ! ACK_DT_RD_Y ; goto V_X ;
				:: (msg == DT_RD_X) -> goto MISS_X ;
				:: (msg == DT_WR_X) -> L1MCDTREQ ! WR_X ; goto ATT_WR3_X ;		
				:: (msg == DT_WR_Y) -> L1MCDTREQ ! WR_Y ; goto ATT_WR3_Y ;
				:: else -> printf("Etat V_Y : Requête invalide chan PL1DTREQ : val %d\n",msg);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture X lié à V_X //
/////////////////////////////////////////

ATT_WR2_X :

	do 
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_Y) || (msg == B_INV_Y)) -> goto ATT_WR2_X ;	
				:: (msg == M_INV_X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_X ; goto ATT_WR1_X ;	
 				:: (msg == B_INV_X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_X ; goto ATT_WR1_X ;	
				:: else -> printf("Etat ATT_WR2_X : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_WR_X) -> E_buffer_cache = BEMPTY ; L1PDTACK ! ACK_DT_WR_X ; goto V_X ;
				:: else -> printf("Etat ATT_WR2_X : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture Y lié à V_X //
/////////////////////////////////////////

ATT_WR2_Y :

	do 
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_X) || (msg == B_INV_X)) -> goto ATT_WR2_Y ;	
				:: (msg == M_INV_Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_Y ; goto ATT_WR1_Y ;	
 				:: (msg == B_INV_Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_Y ; goto ATT_WR1_Y ;	
				:: else -> printf("Etat ATT_WR2_Y : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_WR_Y) -> E_buffer_cache = BEMPTY ; L1PDTACK ! ACK_DT_WR_Y ; goto V_X ;
				:: else -> printf("Etat ATT_WR2_Y : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture X lié à V_Y //
/////////////////////////////////////////

ATT_WR3_X :

	do 
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_Y) || (msg == B_INV_Y)) -> goto ATT_WR3_X ;	
				:: (msg == M_INV_X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_X ; goto ATT_WR1_X ;	
 				:: (msg == B_INV_X) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_X ; goto ATT_WR1_X ;	
				:: else -> printf("Etat ATT_WR3_X : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_WR_X) -> E_buffer_cache = BEMPTY ; L1PDTACK ! ACK_DT_WR_X ; goto V_Y ;
				:: else -> printf("Etat ATT_WR3_X : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;


/////////////////////////////////////////
// Etat d'attente écriture Y lié à V_X //
/////////////////////////////////////////

ATT_WR3_Y :

	do 
	:: MCL1CPREQ ? msg ->   if
				:: ((msg == M_INV_X) || (msg == B_INV_X)) -> goto ATT_WR3_Y ;	
				:: (msg == M_INV_Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_M_INV_Y ; goto ATT_WR1_Y ;	
 				:: (msg == B_INV_Y) -> v_cache_valide = INVALIDE ; L1MCCPACK ! ACK_B_INV_Y ; goto ATT_WR1_Y ;	
				:: else -> printf("Etat ATT_WR3_Y : Requête invalide chan MCL1CPREQ : val %d\n",msg);
				fi;
	:: MCL1DTACK ? msg ->   if
				:: (msg == ACK_WR_Y) -> E_buffer_cache = BEMPTY ; L1PDTACK ! ACK_DT_WR_Y ; goto V_Y ;
				:: else -> printf("Etat ATT_WR3_Y : Requête invalide chan MCL1DTACK : val %d\n",msg);
				fi;
	od;

}

/********** Processus représentant le processeur ************************/
/* Test_P1 : un composant qui teste le modème du cache L1 pour          */
/* les requêtes de lecture et et d'écriture géré par le processeur      */
/* Pour les requêtes envoyer , voir rapport de spécification            */ 
/************************************************************************/


proctype Test_P1( chan PL1DTREQ , L1PDTACK) {


	mtype msg;
	bit flag =0 ;

	// 1 . demande un Read sur X.
	PL1DTREQ ! DT_RD_X ;
	// attente l'aquitement de lecture.
	goto ATT_ACK_RD_X ;
s1:
	flag=1 ;
	// 2. demande un Read sur X.
	PL1DTREQ ! DT_RD_X ;
	// attente l'aquitement de lecture.
	goto ATT_ACK_RD_X ;
s2:
	flag = 0;
	// 3. Ecriture sur X la valeur 10.
		// on boucle tant que le buffer n'est pas vide.
	do
	:: (E_buffer_cache == BEMPTY ) -> E_buffer_cache = BFULL ; buffer_cache = 10 ; PL1DTREQ ! DT_WR_X ; goto ATT_ACK_WR_X ;
	od;
s3:
	// 4. Ecriture sur Y la valeur 11.
		// on boucle tant que le buffer n'est pas vide.
	do
	:: (E_buffer_cache == BEMPTY ) -> E_buffer_cache = BFULL ; buffer_cache = 11 ; PL1DTREQ ! DT_WR_Y ; goto ATT_ACK_WR_Y ;
	od;

s4:
	// 5. Lecture sur Y.
	PL1DTREQ ! DT_RD_Y ;
	// attend l'aquitement de lecture de Y.
	goto ATT_ACK_RD_Y ;

s5:
	flag =1;
	// 6. Lecture sur Y.
	PL1DTREQ ! DT_RD_Y ;
	// attend l'aquitement de lecture de Y.
	goto ATT_ACK_RD_Y ;

s6:
	// 3. Ecriture sur X la valeur 20.
		// on boucle tant que le buffer n'est pas vide.
	do
	:: (E_buffer_cache == BEMPTY ) -> E_buffer_cache = BFULL ; buffer_cache = 20 ; PL1DTREQ ! DT_WR_X ; goto ATT_ACK_WR_X ;
	od;
	
s7:
	// 4. Ecriture sur Y la valeur 21.
	// on boucle tant que le buffer n'est pas vide.
	do
	:: (E_buffer_cache == BEMPTY ) -> E_buffer_cache = BFULL ; buffer_cache = 21 ; PL1DTREQ ! DT_WR_Y ; goto ATT_ACK_WR_Y ;
	od ;

ATT_ACK_RD_X :

	do
	:: L1PDTACK ? msg ->    if
				:: (msg == ACK_DT_RD_X) -> printf("Test_P1 : Lecture X réussit , valeur du cache = %d \n",v_cache); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",msg);
				fi;
	od ;
	if
	:: (flag == 0) -> goto s1 ;
	:: else -> goto s2 ;
	fi;



ATT_ACK_WR_X :

	do
	:: L1PDTACK ? msg ->    if
				:: (msg == ACK_DT_WR_X) -> printf("Test_P1 : Ecriture X réussit , valeur écrite = %d \n",buffer_cache); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",msg);
				fi;
	od ;
	if
	:: (flag == 0) -> goto s3;
	:: else -> goto s7 ;
	fi;

ATT_ACK_WR_Y :

	do
	:: L1PDTACK ? msg ->    if
				:: (msg == ACK_DT_WR_Y) -> printf("Test_P1 : Ecriture Y réussit , valeur écrite = %d \n",buffer_cache); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",msg);
				fi;
	od ;
	if
	:: (flag == 0) -> goto s4;
	:: else -> goto fin ;
	fi;

ATT_ACK_RD_Y :

	do
	:: L1PDTACK ? msg ->    if
				:: (msg == ACK_DT_RD_Y) -> printf("Test_P1 : Lecture Y réussit , valeur du cache = %d \n",v_cache); break ;
				:: else -> printf("Test_P1 : Requete invalide, L1PDTACK '%d' \n",msg);
				fi;
	od ;

	if
	:: (flag == 0) -> goto s5;
	:: else -> goto s6 ;
	fi;

fin :
	do
	:: (flag!=0) 	// boucle infinie.
	od;
	
}

/********** Processus représentant le controleur mémoire ****************/
/* Test_C1 : un composant qui répond au requete du cache L1             */
/* Pour les états , voir rapport de spécification                       */ 
/************************************************************************/

proctype Test_C1(chan L1MCDTREQ, MCL1DTACK) {

	int v_x = 0;	// la valeur du case X initialisé à 0.
	int v_y = 0;	// la valeur du case y initialisé à 0.
	int cpt =0;
	mtype msg ;

Idle :
	do
	:: L1MCDTREQ ? msg ->   if
				:: (msg == RD_X) -> goto Read_x ;
				:: (msg == RD_Y) -> goto Read_y ;
				:: (msg == WR_X) -> goto Write_x ;
				:: (msg == WR_Y) -> goto Write_y ;
				:: else -> printf("Test_C1 : Requete invalide, chan L1MCDTREQ '%d' \n", msg);
				fi;
	od;

Read_x :
	cpt = 0;
	v_cache = v_x ;
	do
	:: (cpt == 10) ->break ;
	:: else -> cpt = cpt+1;
	od; 
	MCL1DTACK ! ACK_RD_X ;
	goto Idle ;

Read_y:
	cpt = 0;
	v_cache = v_y ;
	do
	:: (cpt == 10) -> break ;
	:: else -> cpt= cpt+1;
	od;
	MCL1DTACK ! ACK_RD_Y ;
	goto Idle ;

Write_x :
	cpt= 0;
	v_x = buffer_cache ;
	do
	:: (cpt == 10) -> break ;
	:: else -> cpt = cpt+1 ;
	od;
	E_buffer_cache = BEMPTY ;
	MCL1DTACK ! ACK_WR_X ;
	goto Idle ;

Write_y :
	cpt = 0;
	v_y = buffer_cache ;
	do
	:: (cpt == 10) -> break;
	:: else -> cpt= cpt+1 ;
	od;
	E_buffer_cache = BEMPTY ;
	MCL1DTACK ! ACK_WR_Y ;
	goto Idle ;

}

init {
chan PL1DTREQ, L1PDTACK, L1MCDTREQ, MCL1DTACK,L1MCCUREQ,MCL1CUACK, MCL1CPREQ,L1MCCPACK ;
run CacheL1(PL1DTREQ, L1PDTACK,L1MCDTREQ, MCL1DTACK,L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK);
run Test_P1(PL1DTREQ, L1PDTACK);
run Test_C1(L1MCDTREQ, MCL1DTACK);



}


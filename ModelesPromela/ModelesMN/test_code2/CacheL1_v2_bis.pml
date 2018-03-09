/************************************************************************/
/* Modèle du cache L1 du machine multiprocesseur TSAR *******************/
/* en language Promela **************************************************/
/* Dans ce fichier on teste les requetes élémentaires envoyés par le ****/
/* processeur avec un controleur mémoire minimisé qui répond à ces ******/
/* Requetes . Pour plus d'information voir rapport de spécification *****/
/************************************************************************/

#include "CacheL1.h"

/* Je définie le case mémoire qui contient la valeur du cache comme     */
/* Une variable globale, puisqu'elle sera modifier par le controleur    */
/* mémoire.                                                             */

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
	skip;	
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


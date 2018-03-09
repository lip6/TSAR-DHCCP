//////////////////////////////////////////////////
////////// minim _model.h ////////////////////////
//////////////////////////////////////////////////

// modele des composant minimaux représentant   //
// les 2 composant caches L1 et la mémoire      //
// et qui seront utile pour tester le           //
// le fonctionnemenet du controleur mémoire     //

#ifndef _minim_model_h
#define _minim_model_h

#include "lib_project.h"

// modèle à placer à la place du cache L1        //
// voir rapport de spécification...              //
// généraliser sur un adresse passer en paramètre//

proctype minim_L1(chan L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK; bit addr){

	msg m ;
	byte val0=0 ;
	byte val1=1 ;
	byte val2=2 ;
	byte val3=3 ;
	byte cpt = 0;

	// 1. WR , id = 0
	m.type = WR ;
	m.addr = addr ;
	m.val = val0 ;
	m.cache_id = 0 ;
	L1MCDTREQ ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 0)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 2. WR, id = 1
	m.type = WR ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 1)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// boucle d'attente.
	cpt = 0 ;
	do
	:: if 
		:: (cpt == 10) -> break ; 
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;	

	// 3. RD, id = 0 

	m.type = RD ;
	m.addr = addr ;
	m.val = val0 ;
	m.cache_id = 0 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 0)) -> val0=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 4. RD, id = 1 

	m.type = RD ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 1)) -> val1=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;
	
	// 5. CLNUP, id = 1 

	m.type = CLNUP ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCCUREQ ! m ;
	
	// ACK CLNUP ;
	MCL1CUACK ? m -> if
			:: ((m.type == ACK_CLNUP) && (m.addr == addr) && (m.cache_id == 1)) -> printf("minim_L1 : CLEANUP de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1CUACK %d", m.type);
			fi ;

	// 6. WR, id = 1
	val1 = 11 ;
	m.type = WR ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;


	// 7. M_UP, id = 0 ;
	MCL1CPREQ ? m -> if
			:: ((m.type == M_UP) && (m.addr == addr) && (m.cache_id == 0)) -> val0 = m.val ; printf("minim_L1 : M_UP de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// ACK_M_UP, id = 0 
	m.type = ACK_M_UP ;
	m.addr = addr ;
	m.val = val0 ;
	m.cache_id = 0 ;
	L1MCCPACK ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 1)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 8. RD, id = 1 

	m.type = RD ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 1)) -> val1=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 9. RD, id = 2 

	m.type = RD ;
	m.addr = addr ;
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 2)) -> val2=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 10. RD, id = 3 

	m.type = RD ;
	m.addr = addr ;
	m.val = val3 ;
	m.cache_id = 3 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 3)) -> val3=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 11. CLNUP, id = 3 

	m.type = CLNUP ;
	m.addr = addr ;
	m.val = val3 ;
	m.cache_id = 3 ;
	L1MCCUREQ ! m ;
	
	// ACK CLNUP ;
	MCL1CUACK ? m -> if
			:: ((m.type == ACK_CLNUP) && (m.addr == addr) && (m.cache_id == 3)) -> printf("minim_L1 : CLEANUP de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1CUACK %d", m.type);
			fi ;

	// 12. WR, id = 2
	val2 = 12 ;
	m.type = WR ;
	m.addr = addr ;
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCDTREQ ! m ;

	// B_INV
	MCL1CPREQ ? <m.type, addr, m.val, m.cache_id> -> if
							:: (m.type == B_INV) -> printf("minim_L1 : B_INV réussit");
							:: else -> printf("minim_L1 : EREEUR MCL1CPREQ %d ", m.type);
							fi ;
	// 13. ACK_B_INV, id = 0
	m.type = ACK_B_INV ;
	m.addr = addr ;
	m.val = val0;
	m.cache_id = 0 ;
	L1MCCPACK ! m ;

	// 14. ACK_B_INV, id= 1 
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCCPACK ! m ;

	// 15. ACK_B_INV, id = 2
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCCPACK ! m ;


	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 2)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;


	// 16. boucle d'attente.
	cpt = 0 ;
	do
	:: if 
		:: (cpt == 10) -> break ; 
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;

	// 17 RD, id = 0. 
	m.type = RD ;
	m.addr = addr ;
	m.val = val0 ;
	m.cache_id = 0 ;
	L1MCDTREQ ! m ;

	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 0)) -> val0 = m.val ; printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 18. boucle d'attente.
	cpt = 0 ;
	do
	:: if 
		:: (cpt == 10) -> break ; 
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;

	// 19. RD, id = 1 

	m.type = RD ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 1)) -> val1=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 20. CLNUP, id = 1 

	m.type = CLNUP ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCCUREQ ! m ;
	
	// ACK CLNUP ;
	MCL1CUACK ? m -> if
			:: ((m.type == ACK_CLNUP) && (m.addr == addr) && (m.cache_id == 1)) -> printf("minim_L1 : CLEANUP de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1CUACK %d", m.type);
			fi ;

	// 21. RD, id = 1 

	m.type = RD ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 1)) -> val1=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 22. RD, id = 2 

	m.type = RD ;
	m.addr = addr ;
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 2)) -> val2=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 23. RD, id = 3 

	m.type = RD ;
	m.addr = addr ;
	m.val = val3 ;
	m.cache_id = 3 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 3)) -> val3=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 24. CLNUP, id = 3 

	m.type = CLNUP ;
	m.addr = addr ;
	m.val = val3 ;
	m.cache_id = 3 ;
	L1MCCUREQ ! m ;
	
	// ACK CLNUP ;
	MCL1CUACK ? m -> if
			:: ((m.type == ACK_CLNUP) && (m.addr == addr) && (m.cache_id == 3)) -> printf("minim_L1 : CLEANUP de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1CUACK %d", m.type);
			fi ;

	// 25. WR, id = 2
	val2 = 22 ;
	m.type = WR ;
	m.addr = addr ;
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCDTREQ ! m ;

	// B_INV
	MCL1CPREQ ? <m.type, addr, m.val, m.cache_id> -> if
							:: (m.type == B_INV) -> printf("minim_L1 : B_INV réussit");
							:: else -> printf("minim_L1 : EREEUR MCL1CPREQ %d ", m.type);
							fi ;
	// 26. ACK_B_INV, id = 0
	m.type = ACK_B_INV ;
	m.addr = addr ;
	m.val = val0;
	m.cache_id = 0 ;
	L1MCCPACK ! m ;

	// 27. ACK_B_INV, id= 1 
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCCPACK ! m ;

	// 28. ACK_B_INV, id = 2
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCCPACK ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 2)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;



	// 29. boucle d'attente.
	cpt = 0 ;
	do
	:: if 
		:: (cpt == 10) -> break ; 
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;

	// 30. RD, id = 0. 
	m.type = RD ;
	m.addr = addr ;
	m.val = val0 ;
	m.cache_id = 0 ;
	L1MCDTREQ ! m ;


	// 31. WR, id = 1
	val2 = 21 ;
	m.type = WR ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 1)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;


	// 32. WR, id = 2
	val2 = 32 ;
	m.type = WR ;
	m.addr = addr ;
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCDTREQ ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 2)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

		// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 0)) -> val0 = m.val ; printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 33. boucle d'attente.
	cpt = 0 ;
	do
	:: if 
		:: (cpt == 10) -> break ; 
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;

	// 34. RD, id = 1 

	m.type = RD ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 1)) -> val1=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 35. RD, id = 2 

	m.type = RD ;
	m.addr = addr ;
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 2)) -> val2=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 36. WR, id = 3
	val3 = 13 ;
	m.type = WR ;
	m.addr = addr ;
	m.val = val3 ;
	m.cache_id = 3 ;
	L1MCDTREQ ! m ;

	// B_INV
	MCL1CPREQ ? <m.type, addr, m.val, m.cache_id> -> if
							:: (m.type == B_INV) -> printf("minim_L1 : B_INV réussit");
							:: else -> printf("minim_L1 : EREEUR MCL1CPREQ %d ", m.type);
							fi ;
	// 37. ACK_B_INV, id = 0
	m.type = ACK_B_INV ;
	m.addr = addr ;
	m.val = val0;
	m.cache_id = 0 ;
	L1MCCPACK ! m ;

	// 38. ACK_B_INV, id= 1 
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCCPACK ! m ;

	// 39. ACK_B_INV, id = 2
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCCPACK ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 3)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;



	// 40. boucle d'attente.
	cpt = 0 ;
	do
	:: if 
		:: (cpt == 10) -> break ; 
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;

	// 41. RD, id = 0. 
	m.type = RD ;
	m.addr = addr ;
	m.val = val0 ;
	m.cache_id = 0 ;
	L1MCDTREQ ! m ;

	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 0)) -> val0 = m.val ; printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 42. boucle d'attente.
	cpt = 0 ;
	do
	:: if 
		:: (cpt == 10) -> break ; 
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;

	// 43. RD, id = 1 

	m.type = RD ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCDTREQ ! m ;
	
	// ACK RD ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_RD) && (m.addr == addr) && (m.cache_id == 1)) -> val1=m.val ; printf("minim_L1 : Lecture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// 44. WR, id = 2
	val2 = 42 ;
	m.type = WR ;
	m.addr = addr ;
	m.val = val2 ;
	m.cache_id = 2 ;
	L1MCDTREQ ! m ;

	// 45. M_UP, id = 0 ;
	MCL1CPREQ ? m -> if
			:: ((m.type == M_UP) && (m.addr == addr) && (m.cache_id == 0)) -> val0 = m.val ; printf("minim_L1 : M_UP de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// ACK_M_UP, id = 0 
	m.type = ACK_M_UP ;
	m.addr = addr ;
	m.val = val0 ;
	m.cache_id = 0 ;
	L1MCCPACK ! m ;

	// 46. M_UP, id = 1 ;
	MCL1CPREQ ? m -> if
			:: ((m.type == M_UP) && (m.addr == addr) && (m.cache_id == 1)) -> val1 = m.val ; printf("minim_L1 : M_UP de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;

	// ACK_M_UP, id = 1 
	m.type = ACK_M_UP ;
	m.addr = addr ;
	m.val = val1 ;
	m.cache_id = 1 ;
	L1MCCPACK ! m ;

	// ACK WR ;
	MCL1DTACK ? m -> if
			:: ((m.type == ACK_WR) && (m.addr == addr) && (m.cache_id == 2)) -> printf("minim_L1 : Ecriture de X cache_id = %d réussit", m.cache_id);
			:: else -> printf("minim_L1 : ERREUR MCL1DTACK %d", m.type);
			fi ;


}

//////////////////////////////////////////////////
// modèle à placer à la place de la mémoire     //
// voir rapport de spécification...             //
//////////////////////////////////////////////////

// utiliser pour les testes du controleur mémoire
// et comme composant mémoire dans les plates
// formes.


proctype minim_MEM(chan MCMEMDTREQ, MEMMCDTACK){


	msg m ;
	byte v_x = 0;
	byte v_y = 0;

// Mode IDLE du controleur mémoire.

MEM_IDLE :

	MCMEMDTREQ ? m -> if
			:: ((m.type == GET) && (m.addr == X) ) -> goto GET_X ;
			:: ((m.type == GET) && (m.addr == Y) ) -> goto GET_Y ;
			:: ((m.type == PUT) && (m.addr == X) ) -> goto PUT_X ;
			:: ((m.type == PUT) && (m.addr == Y) ) -> goto PUT_Y ;
			:: else -> printf("mimin_MEM : Requete invalide MCMEMDTREQ %d", m.type);
			fi;

// Lecture de X.
GET_X :

	m.type = ACK_GET ;
	m.val = v_x ;
	m.addr = X ;
	
	// boucle d'attente.

	do
	:: (1) -> goto GET_X ;
	:: (1) -> break ;
	od ;

	// aquitement.

	MEMMCDTACK ! m ;
	goto MEM_IDLE ;

// lecture de Y

GET_Y :

	m.type = ACK_GET ;
	m.val = v_y ;
	m.addr = Y ;
	
	// boucle d'attente.

	do
	:: (1) -> goto GET_Y ;
	:: (1) -> break ;
	od ;

	// aquitement.

	MEMMCDTACK ! m ;
	goto MEM_IDLE ;

// PUT de X

PUT_X :

	m.type = ACK_PUT ;
	v_x = m.val ;
	m.addr = X ;
	
	// boucle d'attente.

	do
	:: (1) -> goto PUT_X ;
	:: (1) -> break ;
	od ;

	// aquitement.

	MEMMCDTACK ! m ;
	goto MEM_IDLE ;


// PUT de Y

PUT_Y :

	m.type = ACK_PUT ;
	v_y = m.val ;
	m.addr = Y ;
	
	// boucle d'attente.

	do
	:: (1) -> goto PUT_Y ;
	:: (1) -> break ;
	od ;


	// aquitement.

	MEMMCDTACK ! m ;
	goto MEM_IDLE ;
}

///////////////////////////////////////////////////
//       Processeur a commande aléatoire         //
///////////////////////////////////////////////////

// processeur utilisé dans les plates formes comme
// un processeur qui génère des requetes aléatoire


proctype proc (chan PL1DTREQ, L1PDTACK){

	msg m   ;
	bit val ;
	bit cpt = 1 ;		// valeur aléatoire.
	bit err = 1 ;

//////////////////////////////////////////////////
// Etat initiale ou le processeur               //
// déscide aléatoirement l'opération à réaliser //
//////////////////////////////////////////////////

Idle :
	do
	:: goto DT_RD_X ;
	:: goto DT_RD_Y ;
	:: goto DT_WR_X ;
	:: goto DT_WR_Y ;
	:: goto Idle    ;
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

	// valeur pseudo-aléatoire à écrire.
	cpt = cpt + 1 ;

	m.type = DT_WR  ;
	m.addr = X      ;
	m.val  = cpt    ;
	PL1DTREQ ! m    ;
	goto ATT_WR_X   ;

//////////////////////////////////////////////////
// Etat demande l'écriture de l'adresse Y       //
//////////////////////////////////////////////////

DT_WR_Y :

	// valeur pseudo-aléatoire à écrire.
	cpt = cpt + 1 ;

	m.type = DT_WR  ;
	m.addr = Y      ;
	m.val  = cpt    ;
	PL1DTREQ ! m    ;
	goto ATT_WR_Y   ;

//////////////////////////////////////////////////
// Attente l'aquitement de lecture de X         //
//////////////////////////////////////////////////

ATT_RD_X :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_RD) && (m.addr == X) ) -> val = m.val ; goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
// Attente l'aquitement de lecture de Y         //
//////////////////////////////////////////////////

ATT_RD_Y :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_RD) && (m.addr == Y) ) -> val = m.val ; goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
// Attente l'aquitement de l'écriture de X      //
//////////////////////////////////////////////////

ATT_WR_X :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_WR) && (m.addr == X) ) -> val = cpt ; goto Idle ;
		:: else -> goto ERROR ;
		fi ;

//////////////////////////////////////////////////
// Attente l'aquitement de l'écriture de Y      //
//////////////////////////////////////////////////

ATT_WR_Y :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_WR) && (m.addr == Y) ) -> val = cpt ; goto Idle ;
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

proctype proc_rd(chan PL1DTREQ, L1PDTACK ; bit addr) {


	msg m   ;
	bit val ;
	bit cpt = 1 ;		// valeur aléatoire.
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
		:: ((m.type == ACK_DT_RD) && (m.addr == addr) ) -> val = m.val ; goto Idle ;
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

proctype proc_rd_wr(chan PL1DTREQ, L1PDTACK ; bit addr) {


	msg m   ;
	bit val ;
	bit cpt = 1 ;		// valeur aléatoire.
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

	// valeur pseudo-aléatoire à écrire.

	cpt = cpt + 1 ;

	m.type = DT_WR  ;
	m.addr = addr   ;
	m.val = cpt     ;
	PL1DTREQ ! m    ;
	goto ATT_WR     ;



//////////////////////////////////////////////////
// Attente l'aquitement de lecture de ?         //
//////////////////////////////////////////////////

ATT_RD :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_RD) && (m.addr == addr) ) -> val = m.val ; goto Idle ;
		:: else -> goto ERROR ;
		fi ;

ATT_WR :

	L1PDTACK ? m ->
		if 
		:: ((m.type == ACK_DT_WR) && (m.addr == addr) ) -> goto Idle ;
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

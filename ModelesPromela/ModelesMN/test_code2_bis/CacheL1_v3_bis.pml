/************************************************************************/
/* Modèle du cache L1 du machine multiprocesseur TSAR                   */
/* en language Promela                                                  */
/* Dans ce fichier on teste les requetes élémentaires de cohérences de  */
/* envoyer par un controleur mémoire minimisé, voir rapport de          */
/* spécification                                                        */
/************************************************************************/

#include "CacheL1.h"

/********** Processus représentant le processeur ************************/
/* Test_P1 : un composant qui teste le modème du cache L1 pour          */
/* les requêtes de lecture et et d'écriture géré par le processeur      */
/* Pour les requêtes envoyer , voir rapport de spécification            */ 
/************************************************************************/


proctype Test_P2( chan PL1DTREQ , L1PDTACK ; bit p_id ) {


	msg m;
	byte cpt=0;
	int flag =1 ;

	// attente.
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;

	// 1. lecture sur X.
	m.type = DT_RD ;
	m.addr = X;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 2. attente d'aquitement sur lecture de X.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_RD) && (m.addr == X)) -> printf("Test_P1 : Lecture de X réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;
	// attente.
	cpt=0 ;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;

	// 3. Ecriture sur X .
	m.type = DT_WR ;
	m.addr = X ;
	m.val = 20 ;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 4. attente d'aquitement sur l'écriture de X.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == X)) -> printf("Test_P1 : Ecriture de X réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;

	// 5. lecture sur X.
	m.type = DT_RD ;
	m.addr = X;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 6. attente d'aquitement sur lecture de X.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_RD) && (m.addr == X)) -> printf("Test_P1 : Lecture de X réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;
	// attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;

	// 7. Ecriture sur X .
	m.type = DT_WR ;
	m.addr = X ;
	m.val = 20 ;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 8. attente d'aquitement sur l'écriture de X.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == X)) -> printf("Test_P1 : Ecriture de X réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;

	//attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;

	// 9. Ecriture sur Y .
	m.type = DT_WR ;
	m.addr = Y ;
	m.val = 21 ;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 10. attente d'aquitement sur l'écriture de Y.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == Y)) -> printf("Test_P1 : Ecriture de Y réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;
// Test de Y.

	// 11. lecture sur Y.
	m.type = DT_RD ;
	m.addr = Y;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 12. attente d'aquitement sur lecture de Y.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_RD) && (m.addr == Y)) -> printf("Test_P1 : Lecture de Y réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;
	// attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;

	// 13. Ecriture sur Y .
	m.type = DT_WR ;
	m.addr = Y ;
	m.val = 22 ;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 14. attente d'aquitement sur l'écriture de Y.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == Y)) -> printf("Test_P1 : Ecriture de Y réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;

	//attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;
	// 15. lecture sur Y.
	m.type = DT_RD ;
	m.addr = Y;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 16. attente d'aquitement sur lecture de Y.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_RD) && (m.addr == Y)) -> printf("Test_P1 : Lecture de Y réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;
	// attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;

	// 17. Ecriture sur Y .
	m.type = DT_WR ;
	m.addr = Y ;
	m.val = 22 ;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 18. attente d'aquitement sur l'écriture de Y.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == Y)) -> printf("Test_P1 : Ecriture de Y réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;

	// 19. Ecriture sur X .
	m.type = DT_WR ;
	m.addr = X ;
	m.val = 23 ;
	m.cache_id = p_id ;
	PL1DTREQ ! m ;

	// 20. attente d'aquitement sur l'écriture de X.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == X)) -> printf("Test_P1 : Ecriture de X réussit val =%d\n",m.val);
			:: else -> printf("Test_P1 : Requete invalide chan L1PDTACK val =%d\n",m.type);
			fi ;


// test des �tats : ATT_WR2_Y, ATT_WR3_Y, CL_R_X et CL_R_Y.

	// 21. lecture de X.
	m.type = DT_RD;
	m.addr = X ;
	m.cache_id = 0;
	PL1DTREQ ! m;

	// 22. aquitement de lecture.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_RD) && (m.addr == X)) -> printf("Test_P2 : Lecture de X r�ussit\n");
			:: else -> printf("Test_P2 : Requete invalide chan L1PDTACK val %d \n",m.type);
			fi ;

	// attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;
	
	// 23. Ecriture sur Y.
	m.type = DT_WR ;
	m.addr= Y ;
	m.cache_id = 0;
	m.val = 55 ;
	PL1DTREQ ! m ;

	// 24. aquitement d'�criture.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == Y)) -> printf("Test_P2 : Ecriture de Y r�ussit val %d\n",m.val);
			:: else -> printf("Test_P2 : Requete invalide chan L1PDTACK val %d \n",m.type);
			fi ;

	// attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;

	// 25. lecture de Y.
	m.type = DT_RD;
	m.addr = Y ;
	m.cache_id = 0;
	PL1DTREQ ! m;

	// 26. aquitement de lecture.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_RD) && (m.addr == Y)) -> printf("Test_P2 : Lecture de Y r�ussit\n");
			:: else -> printf("Test_P2 : Requete invalide chan L1PDTACK val %d \n",m.type);
			fi ;

	// attente.
	cpt = 0;
	do
	:: if
		:: (cpt == 20) -> break;
		:: else -> cpt = cpt+1 ;
		fi;
	od ;
	
	// 27. Ecriture sur X.
	m.type = DT_WR ;
	m.addr= X ;
	m.cache_id = 0;
	m.val = 55 ;
	PL1DTREQ ! m ;

	// 28. aquitement d'�criture.
	L1PDTACK ? m -> if
			:: ((m.type == ACK_DT_WR) && (m.addr == X)) -> printf("Test_P2 : Ecriture de X r�ussit val %d\n",m.val);
			:: else -> printf("Test_P2 : Requete invalide chan L1PDTACK val %d \n",m.type);
			fi ;



	
}

/********** Processus représentant le controleur mémoire ****************/
/* Test_C1 : un composant qui répond au requete du cache L1             */
/* Pour les états , voir rapport de spécification                       */ 
/************************************************************************/

proctype Test_C2(chan L1MCDTREQ, MCL1DTACK, L1MCCUREQ,MCL1CUACK,MCL1CPREQ,L1MCCPACK) {

	byte v_x = 0;	// la valeur du case X initialisé à 0.
	byte v_y = 0;	// la valeur du case y initialisé à 0.
	byte cpt =0;
	msg m;

/******************* Test sur la lecture de X ************************/

	//1. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	// 2. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	//3. B_INV sur X .
	m.type = B_INV ;
	m.addr = X;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	//4. B_INV sur Y .
	m.type = B_INV ;
	m.addr = Y;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	// 5.attente de lecture sur X.
	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == X)) -> printf("Test_C2 : Lecture sur X recue");	
			fi ;
	
	//6. M_INV sur X.
	//m.type = M_INV ;
	//m.addr = X ;
	//m.cache_id = 0 ;
	//MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	// 7. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	//8. B_INV sur X .
	//m.type = B_INV ;
	//m.addr = X;
	//m.cache_id=0 ;
	//MCL1CPREQ ! m;
	// pas d'attente d'acquitement.

	//9. B_INV sur Y .
	m.type = B_INV ;
	m.addr = Y;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	// 10.envoie acquitement sur la lecture de X .
	m.type = ACK_RD ;
	m.addr = X; 
	m.val = v_x ;
	m.cache_id = 0;
	MCL1DTACK ! m;

	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 10) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 

	// 11. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0;
	MCL1CPREQ ! m ;
	// pas d'attente d'aquitement.

	// 12. B_INV sur Y
	m.type = B_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'aquitement.

	// 13. attente d'écriture sur X.
	L1MCDTREQ ? m -> if
			:: ((m.type == WR) && (m.addr == X)) -> v_x = m.val ;
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ val = %d\n", m.type);
			fi;

	// 14. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0;
	MCL1CPREQ ! m ;
	// pas d'attente d'aquitement.

	// 15. B_INV sur Y
	m.type = B_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'aquitement.

	// 12 . invalidation par M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente d'aquitement .
	
	L1MCCPACK ? m -> if
			:: ((m.type ==ACK_M_INV) && (m.addr == X)) -> printf("Test_C2 : acquitement d'invalidation sur Y recue \n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCPACK val = %d",m.type);
			fi ;

	// 13. acquitement d'écriture.
	m.type = ACK_WR ;
	m.addr = X ;
	m.val = v_x ;
	m.cache_id = 0 ;
	MCL1DTACK ! m ;

	// 14.attente d'une lecture sur X .

	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == X)) -> printf("Test_C2 : Lecture sur X recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ val %d\n",m.type);
			fi;
	// aquitement de lecture .
	m.type = ACK_RD ;
	m.addr = X ;
	m.val = v_x ;
	m.cache_id =0;
	MCL1DTACK ! m ;

	// 15. attente d'écriture sur X.

	L1MCDTREQ ? m -> if
			:: ((m.type == WR) && (m.addr == X)) -> v_x = m.val ;
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ val %d\n",m.type);
			fi;

	// 15. invalidation par B_INV sur X.
	m.type = B_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// attente d'aquitement .
	
	L1MCCPACK ? m -> if
			:: ((m.type ==ACK_B_INV) && (m.addr == X)) -> printf("Test_C2 : acquitement d'invalidation sur Y recue \n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCPACK val = %d",m.type);
			fi ;
	//16. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	// 17. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	//18. B_INV sur X .
	m.type = B_INV ;
	m.addr = X;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	//19. B_INV sur Y .
	m.type = B_INV ;
	m.addr = Y;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.
	
	// 20. acquitement d'écriture.
	m.type = ACK_WR ;
	m.addr = X ;
	m.val = v_x ;
	m.cache_id = 0 ;
	MCL1DTACK ! m ;

	// 21. attente d'écriture de Y.
	
	L1MCDTREQ ? m -> if
			:: ((m.type == WR) && (m.addr == Y)) -> v_y = m.val ; printf ("Test_C2 : Ecriture sur Y recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d \n",m.type);
		fi;

	//16. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	// 17. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	//18. B_INV sur X .
	m.type = B_INV ;
	m.addr = X;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;
	// pas d'attente d'acquitement.

	//19. B_INV sur Y .
	m.type = B_INV ;
	m.addr = Y;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.
	
	cpt = 0;
	do
	:: if
		:: (cpt == 10) -> break ;
		:: else -> cpt=cpt+1 ;
		fi ;
	od;

	// 20. acquitement d'écriture.
	m.type = ACK_WR ;
	m.addr = Y ;
	m.val = v_y ;
	m.cache_id = 0 ;
	MCL1DTACK ! m ;

/******************* Test sur la lecture de Y ************************/


	//21. attente de lecture sur Y.
	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == Y)) -> printf ("Test_C2 : Requete de lecture sur Y recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d\n", m.type);
			fi;

	//22. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	// 23. M_INV sur Y.
	//m.type = M_INV ;
	//m.addr = Y ;
	//m.cache_id = 0 ;
	//MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	//24. B_INV sur X .
	m.type = B_INV ;
	m.addr = X;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	//25. B_INV sur Y .
	//m.type = B_INV ;
	//m.addr = Y;
	//m.cache_id=0 ;
	//MCL1CPREQ ! m;
	// pas d'attente d'acquitement.

	// aquitement sur la lecture de Y .
	m.type = ACK_RD;
	m.addr = Y ;
	m.val = v_y ;
	m.cache_id = 0;
	MCL1DTACK ! m ;

	cpt = 0;
	do
	:: if
		:: (cpt == 10 ) -> break ;
		:: else -> cpt = cpt+1 ;
		fi ;
	od ;
	//26. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.
	
	//27. B_INV sur X .
	m.type = B_INV ;
	m.addr = X;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	//28. attente d'écriture sur Y.
	L1MCDTREQ ? m -> if
			:: ((m.type == WR) && (m.addr == Y)) -> v_y = m.val ;
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ val = %d\n", m.type);
			fi;

	// 29. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0;
	MCL1CPREQ ! m ;
	// pas d'attente d'aquitement.

	// 30. B_INV sur X
	m.type = B_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'aquitement.

	// 31 . invalidation par M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente d'aquitement .
	
	L1MCCPACK ? m -> if
			:: ((m.type ==ACK_M_INV) && (m.addr == Y)) -> printf("Test_C2 : acquitement d'invalidation sur Y recue \n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCPACK val = %d",m.type);
			fi ;

	// 32. acquitement d'écriture.
	m.type = ACK_WR ;
	m.addr = Y ;
	m.val = v_y ;
	m.cache_id = 0 ;
	MCL1DTACK ! m ;

	//33.attente d'une lecture sur Y .

	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == Y)) -> printf("Test_C2 : Lecture sur Y recue");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ val %d\n",m.type);
			fi;

	// aquitement de lecture .
	m.type = ACK_RD ;
	m.addr = Y ;
	m.val = v_y ;
	m.cache_id =0 ;
	MCL1DTACK ! m ;

	// 34. attente d'écriture sur Y.

	L1MCDTREQ ? m -> if
			:: ((m.type == WR) && (m.addr == Y)) -> v_y = m.val ;
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ val %d\n",m.type);
			fi;

	// 35. invalidation par B_INV sur Y.
	m.type = B_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// attente d'aquitement .
	
	L1MCCPACK ? m -> if
			:: ((m.type ==ACK_B_INV) && (m.addr == Y)) -> printf("Test_C2 : acquitement d'invalidation sur Y recue \n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCPACK val = %d",m.type);
			fi;
	
	//36. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	// 37. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// pas d'attente d'acquitement.

	//38. B_INV sur X .
	m.type = B_INV ;
	m.addr = X;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	//39. B_INV sur Y .
	m.type = B_INV ;
	m.addr = Y;
	m.cache_id=0 ;
	MCL1CPREQ ! m;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// pas d'attente d'acquitement.

	// 40. Aquitement d'écriture sur Y
	m.type = ACK_WR ;
	m.addr = Y ;
	m.val = v_y ;
	m.cache_id = 0 ;
	MCL1DTACK ! m ;

	//41. Reception d'�criture sur X.
	L1MCDTREQ ? m -> if
			:: ((m.type == WR) && (m.addr == X)) -> v_x = m.val ;
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d",m.type);
			fi;

	// 41. M_INV sur X
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0;
	MCL1CPREQ ! m;

	// 42. M_INV sur Y
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0;
	MCL1CPREQ ! m;

	// 43. B_INV sur X
	m.type=B_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// 44. B_INV sur Y
	m.type = B_INV ;
	m.addr = Y ;
	m.cache_id= 0;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// 45. aquitement d'�criture sur X.
	m.type = ACK_WR ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1DTACK ! m ;

// test de ATT_WR2_Y, ATT_WR3_X, CL_R_X et CL_R_Y

	//46. attente de lecture de X.
	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == X)) -> printf("Test_C2 : Lecture sur X recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d \n",m.type);
			fi;

	// 47. M_INV sur X.
	m.type = M_INV ;
	m.addr = X ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	
	// 47_bis B_INV sur Y.
	m.type = B_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// 48. aquitement de lecture de X.
	m.type = ACK_RD ;
	m.addr = X ;
	m.val = v_x ;
	m.cache_id = 0;
	MCL1DTACK ! m ;

	// 49. attente de clean up.
	L1MCCUREQ ? m -> if
			:: ((m.type == CLNUP) && (m.addr == X)) -> printf("Test_C2 : CLEAN UP sur X recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCUREQ %d\n",m.type);
			fi;

	// 50. aquitement de clean up.
	m.type = ACK_CLNUP ;
	m.addr = X ;
	m.cache_id = 0;
	MCL1CUACK ! m ;

	// 51. attente la redemande automatique du cache de la lecture de x.
	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == X)) -> printf("Test_C2 : Lecture de X recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d\n",m.type);
			fi ;

	// 52. aquitement de lecture de X.
	m.type = ACK_RD;
	m.val = v_x ;
	m.addr = X ;
	m.cache_id=0;
	MCL1DTACK ! m ;

	// 53. attente l'�criture sur Y.	
	L1MCDTREQ ? m ->if
			:: ((m.type == WR) && (m.addr == Y)) -> v_y = m.val ;
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d \n", m.type);
			fi ;

	// 54. B_INV sur X .
	m.type = B_INV ;
	m.addr = X ;
	m.cache_id =0;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// 55. attente l'aquitement de B_INV.
	L1MCCPACK ? m -> if
			:: ((m.type == ACK_B_INV) && (m.addr == X)) -> printf("Test_C2 : Invalidation sur X recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCPACK %d",m.type);
			fi ;

	// 56. aquitement sur l'ecriture de Y.
	m.type = ACK_WR;
	m.addr = Y ;
	m.val = v_y ;
	m.cache_id = 0;
	MCL1DTACK ! m;

	//57. attente de lecture de Y.
	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == Y)) -> printf("Test_C2 : Lecture sur Y recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d \n",m.type);
			fi;

	// 58. M_INV sur Y.
	m.type = M_INV ;
	m.addr = Y ;
	m.cache_id = 0 ;
	MCL1CPREQ ! m ;

	// 58_bis B_INV sur X.
	m.type = B_INV ;
	m.addr = X ;
	m.cache_id = 0;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// 59. aquitement de lecture de Y.
	m.type = ACK_RD;
	m.addr = Y ;
	m.val = v_y ;
	m.cache_id = 0;
	MCL1DTACK ! m ;

	// 60. attente de clean up.
	L1MCCUREQ ? m -> if
			:: ((m.type == CLNUP) && (m.addr == Y)) -> printf("Test_C2 : CLEAN UP sur Y recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCUREQ %d\n",m.type);
			fi;

	// 61. aquitement de clean up.
	m.type = ACK_CLNUP ;
	m.addr = Y ;
	m.cache_id = 0;
	MCL1CUACK ! m ;

	// 62. attente la redemande automatique du cache de la lecture de x.
	L1MCDTREQ ? m -> if
			:: ((m.type == RD) && (m.addr == Y)) -> printf("Test_C2 : Lecture de Y recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d\n",m.type);
			fi ;

	// 63. aquitement de lecture de Y.
	m.type = ACK_RD;
	m.val = v_y ;
	m.addr = Y ;
	m.cache_id =0;
	MCL1DTACK ! m ;

	// 64. attente l'�criture sur X.	
	L1MCDTREQ ? m ->if
			:: ((m.type == WR) && (m.addr == X)) -> v_x = m.val ;
			:: else -> printf("Test_C2 : Requete invalide chan L1MCDTREQ %d \n", m.type);
			fi ;

	// 65. B_INV sur Y .
	m.type = B_INV ;
	m.addr = Y ;
	m.cache_id =0;
	MCL1CPREQ ! m ;
	// attente.
	cpt = 0;
	do 
	:: if 
		:: (cpt == 5) -> break ;
		:: else -> cpt = cpt+1 ;
	fi;
	od ; 
	// consomation du message
	MCL1CPREQ ? m ;

	// 66. attente l'aquitement de B_INV.
	L1MCCPACK ? m -> if
			:: ((m.type == ACK_B_INV) && (m.addr == Y)) -> printf("Test_C2 : Invalidation sur Y recue\n");
			:: else -> printf("Test_C2 : Requete invalide chan L1MCCPACK %d",m.type);
			fi ;

	// 67. aquitement sur l'ecriture de X.
	m.type = ACK_WR;
	m.addr = X ;
	m.val = v_x ;
	m.cache_id = 0;
	MCL1DTACK ! m;



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
	run Test_P2(PL1DTREQ, L1PDTACK , 0);
	run Test_C2(L1MCDTREQ, MCL1DTACK,L1MCCUREQ,MCL1CUACK,MCL1CPREQ,L1MCCPACK);


	do
	:: if
		:: (cpt == 600 ) -> break ;
		:: else -> cpt = cpt+1 ;
	fi ;
	od;

}


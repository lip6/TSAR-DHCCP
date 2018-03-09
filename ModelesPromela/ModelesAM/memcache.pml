// To do: réinitialiser les variables locales et asserter leur valeur à 0 dans les états où elles ne sont pas utilisées

/*********** Modèle de l'automate du controleur mémoire *****************************/
/*           Pour une adresse donné                                                 */
/*           Voire rapport de spécification pour plus de détail                     */
/************************************************************************************/

#ifndef _MEMCACHE_PML_
#define _MEMCACHE_PML_

///////////////////////////////////////////
//            INCLUDES                   //
///////////////////////////////////////////

#include "defs.h"


//////////////////////////////////////////////
// Controleur mémoire pour une ligne donnée //
//////////////////////////////////////////////

proctype MemCache(chan L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK; bit line_addr) {

	msg m;
	byte src_save = 0;
	byte cpt = 0;
	byte rsp_cpt = 0;	// compteurs temporels.
								// rsp_cpt contient le nombre d'aquitements à recevoir lors d'un broadcast inval
	byte c_id[CACHE_TH];	// tableau qui contient les id des caches
	bit v_c_id[CACHE_TH];// tableau qui contient la validité des cases dans c_id ;
	byte n_cache = 0;			// nombre de cache qui contient des copies.
	bit dirty = 0;       // 1 ssi la copie du memcache est modifiée par rapport à la mémoire

///////////////////////////////////////
// Case vide                         //
///////////////////////////////////////

	if
	:: (line_addr == X) ->
		n_cache = 2;
		c_id[0] = 0;
		c_id[1] = 1;
		v_c_id[0] = 1;
		v_c_id[1] = 1;
		goto VALID_MULTICAST;
	:: (line_addr != X) ->
		skip;
	fi;

EMPTY :

	printf("------> MC @%d ETAT : EMPTY\n", line_addr);

	assert(src_save == 0);
	assert(cpt == 0);
	assert(rsp_cpt == 0);

	L1MCDTREQ ? m.type, eval(line_addr), m.cache_id -> 
		if
		:: (m.type == RD) -> 
				src_save = m.cache_id;
				m.type = GET;
				m.addr = line_addr;
				MCMEMDTREQ ! m;
				goto READ_WAIT;
		:: (m.type == WR) ->
			atomic { // Car deux écritures
				dirty = 1;
				m.type = GET;
				m.addr = line_addr;
				MCMEMDTREQ ! m;		// GET du bloc qui contient line_addr.

				m.type = ACK_WR;
				m.addr = line_addr; // m.cache_id non modifié
				MCL1DTACK ! m;		// ACK_WR pour L1.
				goto WRITE_WAIT; 
			}
		:: else -> assert(0);
		fi;


//////////////////////////////////////////////////
// ATTENTE DU BLOC POUR ECRIRE VAL DE line_addr //
//////////////////////////////////////////////////

WRITE_WAIT :

	printf("------> MC @%d ETAT : WRITE_WAIT\n", line_addr);

	do
	:: L1MCDTREQ ? WR, eval(line_addr), m.cache_id -> 
		assert(dirty == 1);
		m.type = ACK_WR;	// m.cache_id non modifié
		m.addr = line_addr;
		MCL1DTACK ! m;
		goto WRITE_WAIT;

	:: MEMMCDTACK ? m.type, eval(line_addr), m.cache_id -> 
		assert(m.type == ACK_GET);
		n_cache = 0;
		goto VALID_MULTICAST;
	od;


/////////////////////////////////////////
// ATTENTE DU BLOC EN LECTURE          //
// On "ack" toutes les écritures       //
/////////////////////////////////////////

READ_WAIT :

	printf("------> MC @%d ETAT : READ_WAIT\n", line_addr);

	do 
	:: L1MCDTREQ ? WR, eval(line_addr), m.cache_id -> 
		dirty = 1;
		m.type = ACK_WR;
		m.addr = line_addr;
		MCL1DTACK ! m;
		goto READ_WAIT;

	:: MEMMCDTACK ? m.type, eval(line_addr), m.cache_id -> 
		assert(m.type == ACK_GET);
		// Comme le memcache ne peut contenir qu'une case de la mémoire, il est ici forcément vide (on vient de EMPTY) ; on peut donc utiliser n'importe quelle case de c_id/v_c_id
		c_id[0] = src_save;
		v_c_id[0] = VALIDE;
		n_cache = 1;

		m.type = ACK_RD;
		m.addr = line_addr;
		m.cache_id = src_save;
		src_save = 0;
		MCL1DTACK ! m;
		goto VALID_MULTICAST;
	od;

	
		
/////////////////////////////////////////////////////
// Le MemCache contient la ligne en mode multicast //
/////////////////////////////////////////////////////

VALID_MULTICAST:

	printf("------> MC @%d ETAT : VALID_MULTICAST\n", line_addr);

	assert(src_save == 0);

	do
	:: L1MCDTREQ ? m.type, eval(line_addr), m.cache_id -> 
		if
		:: (m.type == RD) ->
			src_save = m.cache_id;
			// si n_cache < CACHE_TH => on reste dans VALID_MULTICAST
			// sinon on passe à VALID_BROADCAST
			if
			:: (n_cache < CACHE_TH) ->
				atomic { // Dans le doute...
					assert(cpt == 0);
					do
					:: if
						:: (cpt == CACHE_TH) -> assert(0); // QM : comment ce cas est-il possible ? Dans le doute je mets un assert(false) au lieu d'un break
						:: ((cpt < CACHE_TH) && (v_c_id[cpt] == INVALIDE)) ->
							c_id[cpt] = src_save;
							src_save = 0;
							v_c_id[cpt] = VALIDE;
							n_cache = n_cache + 1;
							break;
						:: else -> cpt = cpt + 1;
						fi;
					od;
					cpt = 0;
					m.type = ACK_RD;
					m.addr = line_addr; 		// m.cache_id non modifié
					MCL1DTACK ! m;		// sending ACK_RD.
					goto VALID_MULTICAST;
				}
			:: else ->
				n_cache = n_cache + 1;
				m.type = ACK_RD;
				m.addr = line_addr;
				m.cache_id = src_save;
				src_save = 0;
				MCL1DTACK ! m;
				goto VALID_BROADCAST;
			fi;
		:: (m.type == WR) ->
			dirty = 1;	
			src_save = m.cache_id;		// sauvegarde la destination du WR.
			assert(rsp_cpt == 0);		// n_cache > 1 forcement, compteur d'aquitement.
			assert(cpt == 0);
			
			// sending multicast update.
			m.type = M_UP;
			m.addr = line_addr;
			do
			:: (cpt == CACHE_TH) -> break;
			:: ((cpt < CACHE_TH) && (c_id[cpt] != src_save) && (v_c_id[cpt] == VALIDE)) ->
				atomic { // On envoie plusieurs messages
					m.cache_id = c_id[cpt];
					rsp_cpt = rsp_cpt + 1;
					cpt = cpt + 1;
					MCL1CPREQ ! m;
				}
			:: else -> cpt = cpt + 1;
			od;
			cpt = 0; // src_save n'est pas réinitialisé : il est utilisé dans UPDATE_WAIT
			goto UPDATE_WAIT;
		:: else -> assert(0);
		fi;

	:: L1MCCUREQ ? m.type, eval(line_addr), m.cache_id -> 
		atomic { // Pour la boucle "for"
			assert(m.type == CLNUP);
			// On supprime le cache id qui a fait la requête de la liste des copies
			assert(cpt == 0);
			do
			:: (cpt == CACHE_TH) -> break ;
			:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == m.cache_id)) ->
				v_c_id[cpt] = INVALIDE;
				n_cache = n_cache - 1;
				break;
			:: else -> cpt = cpt + 1;
			od;
			cpt = 0;
			// On ACK le cleanup
			m.type = ACK_CLNUP;
			m.addr = line_addr; 		// m.cache_id non modifié
			MCL1CUACK ! m;
			goto VALID_MULTICAST;
		}
	od ;


//////////////////////////////////////////////////
// Le MemCache attend l'ACK du multicast update //
//////////////////////////////////////////////////

UPDATE_WAIT :
	
	printf("------> MC @%d ETAT : UPDATE_WAIT\n", line_addr);

	// src_save contient l'id du cache qui demande WR.
	// rsp_cpt contient le nombre d'aquitement à attendre.
	// rsp_cpt = 0 donc 1 cache demande un read puis un write => aucun UPDATE à envoyer.

	if
	:: (rsp_cpt > 0) ->
		do
		:: L1MCCPACK ? m.type, eval(line_addr), m.cache_id -> 
			assert(m.type == ACK_M_UP);
			rsp_cpt = rsp_cpt - 1;
			goto UPDATE_WAIT;

		:: L1MCCUREQ ? m.type, eval(line_addr), m.cache_id -> 
			assert(m.type == CLNUP);
			atomic { // Pour la boucle "for"
				rsp_cpt = rsp_cpt - 1;
				// supression de l'élément de la liste.
				assert(cpt == 0);
				do
				:: (cpt == CACHE_TH) -> break ;
				:: ((cpt < CACHE_TH) && (v_c_id[cpt] == VALIDE) && (c_id[cpt] == m.cache_id)) ->
					v_c_id[cpt] = INVALIDE;
					n_cache = n_cache - 1;
					break;
				:: else -> cpt = cpt + 1;
				od;
				cpt = 0;
				m.type = ACK_CLNUP;
				m.addr = line_addr; 		// m.cache_id non modifié
				MCL1CUACK ! m;
				goto UPDATE_WAIT;
			}
		od;
	:: else -> skip;
	fi; 

	// Envoie ACK_WR au src_save
	m.type = ACK_WR;
	m.addr = line_addr;
	m.cache_id = src_save;
	src_save = 0;
	MCL1DTACK ! m;
	goto VALID_MULTICAST;


/////////////////////////////////////////////////////
// Le MemCache contient la ligne en mode broadcast //
/////////////////////////////////////////////////////

VALID_BROADCAST :

	printf("------> MC @%d ETAT : VALID_BROADCAST\n", line_addr);

	do
	:: L1MCDTREQ ? m.type, eval(line_addr), m.cache_id -> 
		if
		:: (m.type == RD) ->
			n_cache = n_cache + 1;
			m.type = ACK_RD;
			m.addr = line_addr; // m.cache_id non modifié
			MCL1DTACK ! m;
			goto VALID_BROADCAST;
		:: (m.type == WR) ->
			atomic { // Car plusieurs messages envoyés
				dirty = 1;
				src_save = m.cache_id; // utilisé dans l'état BROADCAST_INV_WAIT
				// sending B_INV pour tous les caches
				// src_save aussi invalide sa copie
				m.type = B_INV;
				m.addr = line_addr;
				assert(cpt == 0);
				assert(rsp_cpt == 0);
				rsp_cpt = n_cache;
				// QM : réduit-on le nombre de cas en imposant un ordre sur la consommation des messages ? étant donné la symétrie des caches L1, je ne pense pas (il faut que les procs fassent la même chose). Sinon, utiliser un canal sans ordre ?
				do
				:: (cpt == NB_CACHES) -> break;
				:: else ->
					m.cache_id = cpt;
					cpt = cpt + 1;
					MCL1CPREQ ! m;
				od;
				cpt = 0;
				goto BROADCAST_INV_WAIT;
			}
		:: else -> assert(0);
		fi;

	:: L1MCCUREQ ? m.type, eval(line_addr), m.cache_id -> 
		assert(m.type == CLNUP);
		n_cache = n_cache - 1;
		m.type = ACK_CLNUP;
		m.addr = line_addr; // m.cache_id non modifié
		MCL1CUACK ! m;
		goto VALID_BROADCAST;								
	od;


///////////////////////////////////////////
// Le MemCache attend le broadcast inval //
///////////////////////////////////////////

BROADCAST_INV_WAIT :

	printf("------> MC @%d ETAT : BROADCAST_INV_WAIT\n", line_addr);

	// rsp_cpt = nombre d'acquitements à recevoir
	// src_save contient l'id de la source qui a demandé un WR.

	if
	:: (rsp_cpt > 0) ->
		do
		:: L1MCCPACK ? m.type, eval(line_addr), m.cache_id -> 
			assert(m.type == ACK_B_INV);
			rsp_cpt = rsp_cpt - 1;
			goto BROADCAST_INV_WAIT;

		:: L1MCCUREQ ? m.type, eval(line_addr), m.cache_id -> 
			assert(m.type == CLNUP);
			rsp_cpt = rsp_cpt - 1;
			// On acquitte le CLEANUP (QM : pas sûr)
			m.type = ACK_CLNUP;
			m.addr = line_addr;
			MCL1CUACK ! m;
			goto BROADCAST_INV_WAIT;
		od ;
	:: else -> skip;
	fi;

	// On envoie ACK_WR à l'initiateur
	atomic { // Deux messages envoyés
		m.type = ACK_WR;
		m.addr = line_addr;
		m.cache_id = src_save;
		src_save = 0;
		MCL1DTACK ! m;

		// on vide la liste des copies
		assert(cpt == 0);
		do
		:: (cpt == CACHE_TH) -> break;
		:: else ->
			v_c_id[cpt] = INVALIDE;
			cpt = cpt + 1;
		od;
		cpt = 0;
		n_cache = 0;

		// On recopie la ligne en mémoire
		assert(dirty == 1);
		m.type = PUT;
		m.addr = line_addr;
		dirty = 0;
		MCMEMDTREQ ! m;
	}

	// on consomme le message de B_INV
	//MCL1CPREQ ? m;

	goto PUT_WAIT;
  

//////////////////////////////
// CM attend PUT @line_addr //
//////////////////////////////

PUT_WAIT :

	printf("------> MC @%d ETAT : PUT_WAIT\n", line_addr);

	MEMMCDTACK ? m.type, eval(line_addr), m.cache_id -> 
		assert(m.type == ACK_PUT);
		goto EMPTY;
}


#endif

// vim: filetype=promela:noexpandtab


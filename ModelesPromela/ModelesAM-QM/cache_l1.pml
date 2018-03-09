// To do :
// vérifier où les m.cache_id sont utilisés par le memcache et où il ne le sont pas
// faire une passe pour chaque variable locale pour vérifier qu'elle soit bien remise à 0 quand elle n'est pas utile
//


/* Cas particuliers :
- On reçoit un M_UP sur une ligne que l'on ne possède pas : cela veut dire que l'on a déjà recopié la ligne dans le memcache avec un cleanup : le memcache va compter cela comme un ACK_M_UP en plus de nous enlever de la liste des sharers. Il ne faut donc pas répondre.
- On reçoit un B_INV ou un M_INV sur une ligne que l'on a pas : même chose

*/

#ifndef _CACHE_L1_PML_
#define _CACHE_L1_PML_

/********** Modèle de l'automate du cache L1 ****************************/
/* Voir rapport de spécification,                                       */
/* pour les listes des canaux de communications en argument             */
/************************************************************************/

#include "defs.h"

proctype CacheL1(chan PL1DTREQ, L1PDTACK, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK; byte c_id) {

	bit v_cache_valide = INVALIDE; // bit de validité, le cache modifie cette valeur : 0 -> invalide, 1 -> valide
	bit v_addr = 0;			// le cache contient la ligne X (0) ou Y (1)
	msg m;					// contient un message lu depuis un canal
	bit vcl = 1;			// variable d'état de cleanup : 1 <=> on peut envoyer un cleanup ; 0 : on doit attendre la réponse à un cleanup avant d'en renvoyer un
	bit addr_save = 0; // Sauvegarde l'adresse où a eu lieu l'écriture. 0 : X ; 1 : Y
	bit ignore_miss_rsp = 0;

///////////////////////////////////////
// Etat initial, le cache est vide   //
///////////////////////////////////////

	if
	:: (c_id <= 1) ->
		atomic {
			v_cache_valide = VALIDE;
			v_addr = X;
		}
		goto VALID_DATA;
	:: (c_id > 1) ->
		skip;
	fi;
		

EMPTY :

	assert(v_addr == 0);
	assert(addr_save == 0);
	assert(ignore_miss_rsp == 0);
	assert(c_id < NB_CACHES);

	printf("Cache L1 %d => STATE : EMPTY\n", c_id);

	do
	:: MCL1CPREQ ? M_INV, m.addr, eval(c_id) -> goto EMPTY;
	:: MCL1CPREQ ? B_INV, m.addr, eval(c_id) -> goto EMPTY;
	:: atomic {
			(nfull(L1MCDTREQ) && nempty(PL1DTREQ)) ->
				PL1DTREQ ? m;
				if
				:: (m.type == DT_RD) ->
					addr_save = m.addr;
					goto MISS;
				:: (m.type == DT_WR) ->
					addr_save = m.addr;
					m.type = WR; // m.addr non modifié
					m.cache_id = c_id;
					L1MCDTREQ ! m;
					goto WRITE_WAIT_EMPTY;
				:: else -> assert(0);
				fi;
		}

	// traitement des multicasts updates.
	:: MCL1CPREQ ? M_UP, m.addr, eval(c_id) ->
		m.type = ACK_M_UP;
		m.cache_id = c_id;	// m.addr non modifié
		L1MCCPACK ! m;
		goto EMPTY;

	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, eval(c_id) ->
		if
		:: (m.type == ACK_CLNUP) ->
			vcl = 1;
			goto EMPTY;
		:: else -> assert(0);
		fi;
	od;



///////////////////////////////////////
//  Attente d'écriture et cache vide //
///////////////////////////////////////

WRITE_WAIT_EMPTY :

	assert(ignore_miss_rsp == 0);

	printf ("CacheL1 %d => STATE : WRITE_WAIT_EMPTY @%d\n", c_id, addr_save);

	do
	:: MCL1CPREQ ? M_INV, m.addr, eval(c_id) -> goto WRITE_WAIT_EMPTY;
	:: MCL1CPREQ ? B_INV, m.addr, eval(c_id) -> goto WRITE_WAIT_EMPTY;
	:: MCL1DTACK ? m.type, m.addr, eval(c_id) ->
		assert((m.type == ACK_WR) && (m.addr == addr_save));
		m.type = ACK_DT_WR; // m.addr non modifié
		addr_save = 0;
		L1PDTACK ! m;
		goto EMPTY;

	// traitement des multicasts update
	:: MCL1CPREQ ? M_UP, m.addr, eval(c_id) ->
		m.type = ACK_M_UP;
		m.cache_id = c_id;	// m.addr non modifié
		L1MCCPACK ! m;
		goto WRITE_WAIT_EMPTY;

	// traitement des acquitements des cleanups
	:: MCL1CUACK ? m.type, m.addr, eval(c_id) ->
		assert(m.type == ACK_CLNUP);
		vcl = 1;
		goto WRITE_WAIT_EMPTY;
	od;



///////////////////////
// Miss              //
///////////////////////

MISS :
	printf("CacheL1 %d => STATE : MISS @%d\n", c_id, addr_save);

	assert(!(v_cache_valide == VALIDE && v_addr == addr_save));
	assert(ignore_miss_rsp == 0);

	// si la case contient l'autre adresse que celle en miss,
	// on réalise un cleanup pour informer le memcache ;
	// puis dans tous les cas on demande addr_save
	if
	:: ((v_addr != addr_save) && (v_cache_valide == VALIDE)) ->
		if
		:: (vcl == 1) -> skip; // pas de ACK_CLNUP attendu
		:: else ->		// ACK_CLNUP attendu => bloquage.
			MCL1CUACK ? m.type, m.addr, eval(c_id) -> assert(m.type == ACK_CLNUP);
		fi;

		// sending CLNUP
		vcl = 0;
		m.type = CLNUP;
		m.cache_id = c_id;
		m.addr = v_addr;

		v_cache_valide = INVALIDE;
		v_addr = 0;
		L1MCCUREQ ! m;
	:: else -> skip;
	fi;
	
	// Envoi de la requête de lecture
	do
	:: atomic {
			nfull(L1MCDTREQ) ->
				m.type = RD;
				m.addr = addr_save; // on ne réinitialise pas tout de suite addr_save car on s'en sert encore dans DATA_WAIT
				m.cache_id = c_id;
				L1MCDTREQ ! m;
				goto DATA_WAIT;
		}

	// Il peut y avoir un cleanup pour lequel on attend la réponse
	:: MCL1CUACK ? m.type, m.addr, eval(c_id) ->
			assert(m.type == ACK_CLNUP);
			vcl = 1;
	od;




///////////////////////////////////////
// Attente du transfert              //
///////////////////////////////////////

DATA_WAIT :
	printf("CacheL1 %d => STATE : DATA_WAIT @%d\n", c_id, addr_save);

	do
	:: MCL1CPREQ ? M_INV, m.addr, eval(c_id) -> 	
		if
		:: (m.addr == addr_save) -> // Cas possible (l'inval a doublé la réponse) : il ne faudra pas prendre en compte la réponse au DATA_WAIT, il faut envoyer un ACK_M_INV (Note : ces derniers ne sont pas gérés par le memcache, qui n'envoie de toutes façons jamais de M_INV)
				ignore_miss_rsp = 1;
				m.type = ACK_M_INV; // m.addr unchanged, m.cache_id non utilisé
				L1MCCPACK ! m;
				goto DATA_WAIT;
		:: (m.addr != addr_save) -> // Cas possible : le memcache qui a envoyé le M_INV n'a pas encore reçu le CLEANUP de l'ancienne ligne
			// On ne répond pas, le memcache va recevoir le cleanup
			assert(v_cache_valide == INVALIDE);
			goto DATA_WAIT;
		fi;

	:: MCL1CPREQ ? B_INV, m.addr, eval(c_id) ->
		if
		:: (m.addr == addr_save) -> // Cas possible : il ne faudra pas prendre en compte la réponse au DATA_WAIT, il faut envoyer un ACK_B_INV
				ignore_miss_rsp = 1;
				m.type = ACK_B_INV; // m.addr unchanged, m.cache_id non utilisé
				L1MCCPACK ! m;
				goto DATA_WAIT;
		:: (m.addr != addr_save) -> // Cas possible : le memcache qui a envoyé le B_INV n'a pas encore reçu le CLEANUP de l'ancienne ligne
			// On ne répond pas, le memcache va recevoir le cleanup
			assert(v_cache_valide == INVALIDE);
			goto DATA_WAIT;
		fi;

	// traitement des multicasts update
	// Il faut envoyer un CLEANUP
	:: MCL1CPREQ ? M_UP, m.addr, eval(c_id) ->
		if
		:: (m.addr == addr_save) ->
				ignore_miss_rsp = 1;
				m.type = CLNUP;
				m.cache_id = c_id;	// m.addr non modifié
				L1MCCUREQ ! m;
				goto DATA_WAIT;
		:: (m.addr != addr_save) -> // Cas possible : le memcache qui a envoyé le M_UP n'a pas encore reçu le CLEANUP de l'ancienne ligne
			// On ne répond pas, le memcache va recevoir le cleanup
			assert(v_cache_valide == INVALIDE);
			goto DATA_WAIT;
		fi;
				
	// traitement des aquitements des cleanup.
	:: MCL1CUACK ? m.type, m.addr, eval(c_id) ->
		assert(m.type == ACK_CLNUP);
		vcl = 1;
		goto DATA_WAIT;

	// On reçoit la réponse à la requête de lecture
	:: MCL1DTACK ? m.type, m.addr, eval(c_id) ->
		assert((m.type == ACK_RD) && (m.addr == addr_save));
		if
		:: (ignore_miss_rsp == 1) ->
				ignore_miss_rsp = 0;
				goto MISS;
		:: (ignore_miss_rsp == 0) ->
				v_cache_valide = VALIDE;
				v_addr = addr_save;

				m.type = ACK_DT_RD;
				m.addr = addr_save;
				addr_save = 0;
				L1PDTACK ! m;
				goto VALID_DATA;
		fi;
	od;



////////////////////////////////////////
// Le cache contient une ligne valide //
////////////////////////////////////////

VALID_DATA :

	assert(addr_save == 0);
	assert(ignore_miss_rsp == 0);

	printf("CacheL1 %d => STATE : VALID_DATA @%d\n", c_id, v_addr);

	do
	:: MCL1CPREQ ? M_INV, m.addr, eval(c_id) ->
		if
		:: (m.addr != v_addr) -> goto VALID_DATA; // On vient d'envoyer un cleanup qui servira d'acquitement au multicast inval, rien à faire (pas sûr que ce cas soit possible avec une seule ligne)
		:: (m.addr == v_addr) ->
			m.type = ACK_M_INV;
			m.addr = v_addr;
			m.cache_id = c_id;

			v_cache_valide = INVALIDE;
			v_addr = 0;
			L1MCCPACK ! m;
			goto EMPTY;
		fi;

	:: MCL1CPREQ ? B_INV, m.addr, eval(c_id) ->
		if
		:: (m.addr != v_addr) -> goto VALID_DATA; // Même cas qu'au dessus
		:: (m.addr == v_addr) ->
			m.type = ACK_B_INV;
			m.addr = v_addr;
			m.cache_id = c_id;

			v_cache_valide = INVALIDE;
			v_addr = 0;
			L1MCCPACK ! m;
			goto EMPTY;
		fi;

	:: atomic {
		(nfull(L1MCDTREQ) && nempty(PL1DTREQ)) ->
			PL1DTREQ ? m;
			if
			:: ((m.type == DT_RD) && (m.addr == v_addr)) ->
				m.type = ACK_DT_RD;
				L1PDTACK ! m;
				goto VALID_DATA;
			:: ((m.type == DT_RD) && (m.addr != v_addr)) ->
				addr_save = m.addr;
				goto MISS;
			:: ((m.type == DT_WR) && (m.addr == v_addr)) ->
#ifdef USE_DATA
#endif
				addr_save = m.addr;
				m.type = WR;
				m.cache_id = c_id;
				L1MCDTREQ ! m;
				goto WRITE_WAIT_VALID;
			:: ((m.type == DT_WR) && (m.addr != v_addr)) ->
					addr_save = m.addr;
					m.type = WR;
					m.cache_id = c_id;
					L1MCDTREQ ! m;
					goto WRITE_WAIT_VALID;
			:: else -> assert(0);
			fi;
		}

	// traitement des multicasts update
	:: MCL1CPREQ ? M_UP, m.addr, eval(c_id) ->
		if
		:: (m.addr == v_addr) ->
#ifdef USE_DATA
#endif
			m.type = ACK_M_UP;
			m.cache_id = c_id;	// m.addr non modifié
			L1MCCPACK ! m;
			goto VALID_DATA;
		:: else -> goto VALID_DATA;			// M_UP sur un autre adresse
		fi;

	// traitement des acquitements de cleanup
	:: MCL1CUACK ? m.type, m.addr, eval(c_id) ->
		assert(m.type == ACK_CLNUP);
		vcl = 1;
		goto VALID_DATA;
	od;



/////////////////////////////////////////
// Etat d'attente écriture             //
/////////////////////////////////////////

WRITE_WAIT_VALID :

	printf("CacheL1 %d => STATE : WRITE_WAIT_VALID (VALID @%d - WRITE @%d)\n", c_id, v_addr, addr_save);

	do
	:: MCL1CPREQ ? M_INV, m.addr, eval(c_id) ->
		if
		:: (m.addr != v_addr) -> goto WRITE_WAIT_VALID; // M_INV ignored
		:: (m.addr == v_addr) ->
				m.type = ACK_M_INV;
				m.cache_id = c_id; // m.addr inchangé

				v_cache_valide = INVALIDE;
				v_addr = 0;
				L1MCCPACK ! m;
				goto WRITE_WAIT_EMPTY;
		fi;

	:: MCL1CPREQ ? B_INV, m.addr, eval(c_id) ->
		if
		:: (m.addr != v_addr) -> goto WRITE_WAIT_VALID;
		:: (m.addr == v_addr) ->
				m.type = ACK_B_INV;
				m.cache_id = c_id; // m.addr inchangé

				v_cache_valide = INVALIDE;
				v_addr = 0;
				L1MCCPACK ! m;
				goto WRITE_WAIT_EMPTY;
		fi;

	:: MCL1DTACK ? m.type, m.addr, eval(c_id) ->
		assert((m.type == ACK_WR) && (m.addr == addr_save));
		addr_save = 0;
		m.type = ACK_DT_WR; // m.addr inchangé
		L1PDTACK ! m;
		goto VALID_DATA;

	// traitement des multicasts update
	:: MCL1CPREQ ? M_UP, m.addr, eval(c_id) ->
			m.type = ACK_M_UP;
			m.cache_id = c_id;	// m.addr non modifié
			L1MCCPACK ! m;
			goto WRITE_WAIT_VALID;

	// traitement des acquitements de cleanup
	:: MCL1CUACK ? m.type, m.addr, eval(c_id) ->
		assert(m.type == ACK_CLNUP) ->
		vcl = 1;
		goto WRITE_WAIT_VALID;
	od;

}

# endif

// vim: filetype=promela:noexpandtab


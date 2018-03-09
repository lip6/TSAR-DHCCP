///////////////////////////////////////////////
////// Plat forme à 3 proc + 1 CM + 1 MEM /////
///////////////////////////////////////////////

// simulation + vérification des propriétés suivantes :
// ** abscence de bloquage **

#include "memory.pml"
#include "proc.pml"
#include "memcache.pml"
#include "cache_l1.pml"

init {

   chan PL1DTREQ1  = [1] of { msg };
   chan L1PDTACK1  = [1] of { msg };
   chan PL1DTREQ2  = [1] of { msg };
   chan L1PDTACK2  = [1] of { msg };
   chan PL1DTREQ3  = [1] of { msg };
   chan L1PDTACK3  = [1] of { msg };
   chan L1MCDTREQ  = [1] of { msg };
   chan MCL1DTACK  = [1] of { msg };
   chan L1MCCUREQ  = [1] of { msg };
   chan MCL1CUACK  = [1] of { msg };
   chan L1MCCPACK  = [1] of { msg };
   chan MCL1CPREQ  = [1] of { msg };
   chan MCMEMDTREQ = [1] of { msg };
   chan MEMMCDTACK = [1] of { msg };


   atomic {
      // Processeur 1
      run proc(PL1DTREQ1, L1PDTACK1);
      // Cache L1 1
      run CacheL1(PL1DTREQ1, L1PDTACK1, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK , 0);

      // Processeur 2
      run proc(PL1DTREQ2, L1PDTACK2);
      // Cache L1 2
      run CacheL1(PL1DTREQ2, L1PDTACK2, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, 1);

      // Processeur 3
      run proc(PL1DTREQ3, L1PDTACK3);
      // Cache L1 3
      run CacheL1(PL1DTREQ3, L1PDTACK3, L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK , 2);

      // MemC pour X
      run MemCache(L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK, X);
      // MemC pour Y
      run MemCache(L1MCDTREQ, MCL1DTACK, L1MCCUREQ, MCL1CUACK, MCL1CPREQ, L1MCCPACK, MCMEMDTREQ, MEMMCDTACK, Y);

      // Memoire
      run minim_MEM(MCMEMDTREQ, MEMMCDTACK);
   }

}


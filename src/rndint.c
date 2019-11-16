/* Internal RNG - added 15/9/18 by MikeS.
 * The implementation is taken from Bob Jenkins' ISAAC64, which is in the public domain,
 * and modified to use integer.h definitions and remove unused code.
 **/

#include "hack.h"
#include "integer.h"

/*
------------------------------------------------------------------------------
Standard definitions and types, Bob Jenkins
------------------------------------------------------------------------------
*/

#if defined(__STDC__) && __STDC_VERSION__ >= 199101L
typedef uint64_t uint64;
#else
typedef  unsigned long long  uint64;
#endif

/*
------------------------------------------------------------------------------
isaac64.h: definitions for a random number generator
Bob Jenkins, 1996, Public Domain
------------------------------------------------------------------------------
*/
#define RANDSIZL   (8)
#define RANDSIZ    (1<<RANDSIZL)

/*
------------------------------------------------------------------------------
isaac64.c: My random number generator for 64-bit machines.
By Bob Jenkins, 1996.  Public Domain.
------------------------------------------------------------------------------
*/
static    uint64 randrsl[RANDSIZ], randcnt;
static    uint64 mm[RANDSIZ];
static    uint64 aa=0, bb=0, cc=0;

#define ind(mm,x)  (*(uint64 *)((uint8 *)(mm) + ((x) & ((RANDSIZ-1)<<3))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (mix) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}

static void isaac64()
{
  register uint64 a,b,x,y,*m,*m2,*r,*mend;
  m=mm; r=randrsl;
  a = aa; b = bb + (++cc);
  for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; )
  {
    rngstep(~(a^(a<<21)), a, b, mm, m, m2, r, x);
    rngstep(  a^(a>>5)  , a, b, mm, m, m2, r, x);
    rngstep(  a^(a<<12) , a, b, mm, m, m2, r, x);
    rngstep(  a^(a>>33) , a, b, mm, m, m2, r, x);
  }
  for (m2 = mm; m2<mend; )
  {
    rngstep(~(a^(a<<21)), a, b, mm, m, m2, r, x);
    rngstep(  a^(a>>5)  , a, b, mm, m, m2, r, x);
    rngstep(  a^(a<<12) , a, b, mm, m, m2, r, x);
    rngstep(  a^(a>>33) , a, b, mm, m, m2, r, x);
  }
  bb = b; aa = a;
}

#define mix(a,b,c,d,e,f,g,h) \
{ \
   a-=e; f^=h>>9;  h+=a; \
   b-=f; g^=a<<9;  a+=b; \
   c-=g; h^=b>>23; b+=c; \
   d-=h; a^=c<<15; c+=d; \
   e-=a; b^=d>>14; d+=e; \
   f-=b; c^=e<<20; e+=f; \
   g-=c; d^=f>>17; f+=g; \
   h-=d; e^=g<<14; g+=h; \
}

/*
------------------------------------------------------------------------------
 If (flag==TRUE), then use the contents of randrsl[0..255] as the seed.
------------------------------------------------------------------------------
*/
static void randinit(flag)
int flag;
{
   int i;
   uint64 a,b,c,d,e,f,g,h;
   aa=bb=cc=(uint64)0;
   a=b=c=d=e=f=g=h=0x9e3779b97f4a7c13LL;  /* the golden ratio */

   for (i=0; i<4; ++i)                    /* scramble it */
   {
     mix(a,b,c,d,e,f,g,h);
   }

   for (i=0; i<RANDSIZ; i+=8)   /* fill in mm[] with messy stuff */
   {
     if (flag)                  /* use all the information in the seed */
     {
       a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
       e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
     }
     mix(a,b,c,d,e,f,g,h);
     mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
     mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
   }

   if (flag)
   {        /* do a second pass to make all of the seed affect all of mm */
     for (i=0; i<RANDSIZ; i+=8)
     {
       a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
       e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
       mix(a,b,c,d,e,f,g,h);
       mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
       mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
     }
   }

   isaac64();          /* fill in the first set of results */
   randcnt=RANDSIZ;    /* prepare to use the first set of results */
}

void seed_rng(seed)
unsigned long seed;
{
    int i;
    for (i=0; i<(int)(RANDSIZ * sizeof(uint64)/sizeof(seed)); ++i) ((unsigned long *)randrsl)[i]=seed;
    randinit(TRUE);
}

unsigned long get_rng(void)
{
    uint64 rand = (!randcnt-- ? (isaac64(), randcnt=RANDSIZ-1, randrsl[randcnt]) : randrsl[randcnt]);
    return (rand == (unsigned long)rand) ? rand : (uint32)rand ^ (uint32)(rand >> 32);
}

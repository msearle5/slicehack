/* NetHack 3.6	rnd.c	$NHDT-Date: 1524689470 2018/04/25 20:51:10 $  $NHDT-Branch: NetHack-3.6.0 $:$NHDT-Revision: 1.18 $ */
/*      Copyright (c) 2004 by Robert Patrick Rankin               */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include <assert.h>

#ifdef USE_ISAAC64
#include "isaac64.h"

#if 0
static isaac64_ctx rng_state;
#endif

struct rnglist_t {
    int FDECL((*fn), (int));
    boolean init;
    isaac64_ctx rng_state;
};

enum { CORE = 0, DISP = 1 };

static struct rnglist_t rnglist[] = {
    { rn2, FALSE, { 0 } },                      /* CORE */
    { rn2_on_display_rng, FALSE, { 0 } },       /* DISP */
};

int
whichrng(fn)
int FDECL((*fn), (int));
{
    int i;

    for (i = 0; i < SIZE(rnglist); ++i)
        if (rnglist[i].fn == fn)
            return i;
    return -1;
}

void
init_isaac64(seed, fn)
unsigned long seed;
int FDECL((*fn), (int));
{
    unsigned char new_rng_state[sizeof seed];
    unsigned i;
    int rngindx = whichrng(fn);

    if (rngindx < 0)
        panic("Bad rng function passed to init_isaac64().");

    for (i = 0; i < sizeof seed; i++) {
        new_rng_state[i] = (unsigned char) (seed & 0xFF);
        seed >>= 8;
    }
    isaac64_init(&rnglist[rngindx].rng_state, new_rng_state,
                 (int) sizeof seed);
}

static int
RND(int x)
{
    return (isaac64_next_uint64(&rnglist[CORE].rng_state) % x);
}

/* 0 <= rn2(x) < x, but on a different sequence from the "main" rn2;
   used in cases where the answer doesn't affect gameplay and we don't
   want to give users easy control over the main RNG sequence. */
int
rn2_on_display_rng(x)
register int x;
{
    return (isaac64_next_uint64(&rnglist[DISP].rng_state) % x);
}

#else   /* USE_ISAAC64 */

/* "Rand()"s definition is determined by [OS]conf.h */
#if defined(LINT) && defined(UNIX) /* rand() is long... */
extern int NDECL(rand);
#define RND(x) (rand() % x)
#else /* LINT */
#if defined(UNIX) || defined(RANDOM)
#define RND(x) ((int) (Rand() % (long) (x)))
#else
/* Good luck: the bottom order bits are cyclic. */
#define RND(x) ((int) ((Rand() >> 3) % (x)))
#endif
#endif /* LINT */
int
rn2_on_display_rng(x)
register int x;
{
    static unsigned seed = 1;
    seed *= 2739110765;
    return (int)((seed >> 16) % (unsigned)x);
}
#endif  /* USE_ISAAC64 */

/* 0 <= rn2(x) < x */
int
rn2(x)
register int x;
{
#if (NH_DEVEL_STATUS != NH_STATUS_RELEASED)
    if (x <= 0) {
        impossible("rn2(%d) attempted", x);
        return 0;
    }
    x = RND(x);
    return x;
#else
    return RND(x);
#endif
}

/* 0 <= rnl(x) < x; sometimes subtracting Luck;
   good luck approaches 0, bad luck approaches (x-1) */
int
rnl(x)
register int x;
{
    register int i, adjustment;

#if (NH_DEVEL_STATUS != NH_STATUS_RELEASED)
    if (x <= 0) {
        impossible("rnl(%d) attempted", x);
        return 0;
    }
#endif

    adjustment = Luck;
    if (x <= 15) {
        /* for small ranges, use Luck/3 (rounded away from 0);
           also guard against architecture-specific differences
           of integer division involving negative values */
        adjustment = (abs(adjustment) + 1) / 3 * sgn(adjustment);
        /*
         *       11..13 ->  4
         *        8..10 ->  3
         *        5.. 7 ->  2
         *        2.. 4 ->  1
         *       -1,0,1 ->  0 (no adjustment)
         *       -4..-2 -> -1
         *       -7..-5 -> -2
         *      -10..-8 -> -3
         *      -13..-11-> -4
         */
    }

    i = RND(x);
    if (adjustment && rn2(37 + abs(adjustment))) {
        i -= adjustment;
        if (i < 0)
            i = 0;
        else if (i >= x)
            i = x - 1;
    }
    return i;
}

/* 1 <= rnd(x) <= x */
int
rnd(x)
register int x;
{
#if (NH_DEVEL_STATUS != NH_STATUS_RELEASED)
    if (x <= 0) {
        impossible("rnd(%d) attempted", x);
        return 1;
    }
#endif
    x = RND(x) + 1;
    return x;
}

/* d(N,X) == NdX == dX+dX+...+dX N times; n <= d(n,x) <= (n*x) */
int
d(n, x)
register int n, x;
{
    register int tmp = n;

#if (NH_DEVEL_STATUS != NH_STATUS_RELEASED)
    if (x < 0 || n < 0 || (x == 0 && n != 0)) {
        impossible("d(%d,%d) attempted", n, x);
        return 1;
    }
#endif
    while (n--)
        tmp += RND(x);
    return tmp; /* Alea iacta est. -- J.C. */
}

/* 1 <= rne(x) <= max(u.ulevel/3,5) */
int
rne(x)
register int x;
{
    register int tmp, utmp;

    utmp = (u.ulevel < 15) ? 5 : u.ulevel / 3;
    tmp = 1;
    while (tmp < utmp && !rn2(x))
        tmp++;
    return tmp;

    /* was:
     *  tmp = 1;
     *  while (!rn2(x))
     *    tmp++;
     *  return min(tmp, (u.ulevel < 15) ? 5 : u.ulevel / 3);
     * which is clearer but less efficient and stands a vanishingly
     * small chance of overflowing tmp
     */
}

/* Produce a random enchantment for weapons, armor or charged rings.
 * The enchantments are distributed exponentially, but unlike rne()
 * they are biased according to Luck (but not affected by level).
 *
 * They are also distributed in a way that is intended to model the
 * behaviour of scrolls of enchant armor, enchant weapon and charging
 * against highly enchanted items.
 *
 * For armor enchanted above +3 (armor) / +5 (elven armor) the
 * chance of survival is 1 in (enchantment). "Not surviving" here
 * means starting over from +1.
 *
 * For weapons enchanted above +5 the chance of survival is 1 in 3,
 * so a similar check is used.
 *
 * For armor and weapons, from +9 there is a second 1-in-enchantment
 * check for no effect (meaning keep the current enchantment but
 * don't add anything.)
 *
 * Rings have a current charge / 7 change of not surviving, but can
 * be charged up to d3 points at once giving a maximum possible ring
 * charge of +9. (Armor and weapons can be enchanted by d2 from their
 * maximum safe enchantment - but this can be ignored if you assume
 * that such arms getting only +1 would be drained and retried.)
 *
 * There is no absolute limit for weapons or armor.
 *
 * Average results from a test (1e9 reps of each):
 *
 * Luck -13:
 * Armor:       1:0.830095	2:0.141121	3:0.0239911	4:0.00407551	5:0.000692566	6:2.3776e-05	7:6.11e-07	    8:1.9e-08	                                                            Average 1.204223
 * Elven Armor: 1:0.829999	2:0.141102	3:0.023991	4:0.00407717	5:0.000692839	6:0.000117782	7:1.9936e-05	8:4.91e-07	    9:1e-08	                                                Average 1.204799
 * WeaponL      1:0.830001	2:0.141104	3:0.0239823	4:0.00408001	5:0.00069301	6:0.000118003	7:2.022e-05	    8:1.108e-06	    9:7.4e-08	                                            Average 1.204801
 * Ring:        1:0.767481	2:0.177814	3:0.0434097	4:0.00942346	5:0.00156621	6:0.000268194	7:3.5526e-05	8:2.569e-06	    9:1.42e-07	                                            Average 1.300742
 *
 * Luck 0:
 * Armor:       1:0.701468	2:0.210433	3:0.0631245	4:0.018939	    5:0.0056782	    6:0.000339627	7:1.6896e-05	8:7.36e-07	    9:3.3e-08	                                            Average 1.418016
 * Elven Armor: 1:0.700126	2:0.210051	3:0.06301	4:0.0189081	    5:0.00566787	6:0.00170305	7:0.000511327	8:2.1954e-05	9:8.76e-07	    10:4e-09	                            Average 1.427211
 * Weapon:      1:0.700102	2:0.210047	3:0.0630073	4:0.0189044	    5:0.00567031	6:0.00170189	7:0.000510719	8:5.0898e-05	9:5.646e-06	    10:7.4e-08	                            Average 1.427432
 * Ring:        1:0.657527	2:0.226064	3:0.0816975	4:0.0262375	    5:0.0064746	    6:0.00164317	7:0.000317859	8:3.5491e-05	9:3.004e-06	                                            Average 1.504465
 *
 * Luck 13:
 * Armor:       1:0.577462	2:0.248288	3:0.106766	4:0.0459098	    5:0.0197488	    6:0.00169662	7:0.00012125	8:7.551e-06	    9:4e-07	                                                Average 1.687811
 * Elven Armor: 1:0.571437	2:0.245697	3:0.105655	4:0.0454326	    5:0.0195353	    6:0.00839446	7:0.0036141	    8:0.000221525	9:1.2295e-05	10:8.1e-08	                            Average 1.736753
 * Weapon:      1:0.571208	2:0.245612	3:0.105629	4:0.0454136	    5:0.0195258	    6:0.00839673	7:0.0036099	    8:0.000518002	9:8.5108e-05	10:1.583e-06	11:2e-08	12:1e-09	Average 1.739179
 * Ring:        1:0.552074	2:0.252107	3:0.120509	4:0.0511995	    5:0.0167925	    6:0.00563639	7:0.00144528	8:0.00021222	9:2.402e-05	                                            Average 1.752425
 */
int
rnew(limit, armor, ring)
int limit;
boolean armor;
boolean ring;
{
    boolean ok;
    int tmp;
    int check = 0;
    int chance = (Luck * 10)+300;
    if (ring) chance += 100;
    do
    {
        ok = TRUE;
        tmp = 1;
        while (rn2(1000) < chance) {
            check--;
            if (tmp > limit) {
                if (ring) {
                    if (check >= 0)
                        tmp++;
                    else if (rn2(7) >= tmp) {
                        check = rn2(3);
                        tmp++;
                    } else
                        ok = FALSE;
                }
                else if (rn2(armor ? tmp : 3))
                    ok = FALSE;
                else if ((tmp < 9) || (!rn2(tmp)))
                    tmp++;
            } else
                tmp++;
        }
        assert(tmp < 128);
        assert(tmp > 0);
    } while (!ok);

    return tmp;
}

/* rnz: everyone's favorite! */
int
rnz(i)
int i;
{
#ifdef LINT
    int x = i;
    int tmp = 1000;
#else
    register long x = (long) i;
    register long tmp = 1000L;
#endif

    tmp += rn2(1000);
    tmp *= rne(4);
    if (rn2(2)) {
        x *= tmp;
        x /= 1000;
    } else {
        x *= 1000;
        x /= tmp;
    }
    return (int) x;
}

/*rnd.c*/

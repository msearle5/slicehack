/* NetHack 3.6	were.c	$NHDT-Date: 1550524568 2019/02/18 21:16:08 $  $NHDT-Branch: NetHack-3.6.2-beta01 $:$NHDT-Revision: 1.23 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2011. */
/* NetHack may be freely redistributed.  See license for details. */

/* Edited on 3/7/18 by NullCGT */

#include "hack.h"

void
were_change(mon)
register struct monst *mon;
{
    if (!is_were(mon->data))
        return;

    if (is_human(mon->data)) {
        if (!Protection_from_shape_changers
            && !rn2(night() ? (flags.moonphase == FULL_MOON ? 3 : 30)
                            : (flags.moonphase == FULL_MOON ? 10 : 50))) {
            new_were(mon); /* change into animal form */
            if (!Deaf && !canseemon(mon)) {
                const char *howler, *howl;

                switch (monsndx(mon->data)) {
                case PM_WERETIGER:
                    howler = "tiger";
                    howl = "yowling";
                    break;
                case PM_WERECOCKATRICE:
                    howler = "chicken";
                    howl = "squawking";
                    break;
                case PM_WEREBEAR:
                    howler = "bear";
                    howl = "roaring";
                    break;
                case PM_ALPHA_WEREWOLF:
                    howler = "pack of wolves";
                    howl = "howling";
                    break;
                case PM_WEREWOLF:
                    howler = "wolf";
                    howl = "howling";
                    break;
                case PM_DRAUGLIR:
                    howler = "great wolf";
                    howl = "howling";
                    break;
                case PM_WEREJACKAL:
                    howler = "jackal";
                    howl = "howling";
                    break;
                case PM_WERERAT:
                    if (Hallucination) {
                        howler = "mouse";
                        howl = "squeaking";
                    }
                    /* fallthru */
                default:
                    howler = howl = (char *) 0;
                    break;
                }

                if (howler) {
                    if (Hallucination)
                        You_hear("the moon %s like a %s.", howl, howler);
                    else
                        You_hear("a %s %s at the moon.", howler, howl);
                }
            }
        }
    } else if (!rn2(30) || Protection_from_shape_changers) {
        if (mon->data != &mons[PM_DRAUGLIR])
            new_were(mon); /* change back into human form */
    }
    /* update innate intrinsics (mainly Drain_resistance) */
    set_uasmon(); /* new_were() doesn't do this */
}

int
counter_were(pm)
int pm;
{
    switch (pm) {
    case PM_ALPHA_WEREWOLF:
        return PM_PACK_LORD;
    case PM_PACK_LORD:
        return PM_ALPHA_WEREWOLF;
    case PM_DRAUGLIR:
    case PM_WEREWOLF:
        return PM_HUMAN_WEREWOLF;
    case PM_HUMAN_WEREWOLF:
        return PM_WEREWOLF;
    case PM_WEREBEAR:
        return PM_HUMAN_WEREBEAR;
    case PM_HUMAN_WEREBEAR:
        return PM_WEREBEAR;
    case PM_WERECOCKATRICE:
        return PM_HUMAN_WERECOCKATRICE;
    case PM_HUMAN_WERECOCKATRICE:
        return PM_WERECOCKATRICE;
    case PM_WERETIGER:
        return PM_HUMAN_WERETIGER;
    case PM_HUMAN_WERETIGER:
        return PM_WERETIGER;
    case PM_WEREJACKAL:
        return PM_HUMAN_WEREJACKAL;
    case PM_HUMAN_WEREJACKAL:
        return PM_WEREJACKAL;
    case PM_WERERAT:
        return PM_HUMAN_WERERAT;
    case PM_HUMAN_WERERAT:
        return PM_WERERAT;
    default:
        return NON_PM;
    }
}

/* convert monsters similar to werecritters into appropriate werebeast */
int
were_beastie(pm)
int pm;
{
    switch (pm) {
    case PM_WERECOCKATRICE:
    case PM_COCKATRICE:
    case PM_CHICKATRICE:
    case PM_PYROLISK:
        return PM_WERECOCKATRICE;
    case PM_TIGER:
    case PM_WERETIGER:
        return PM_TIGER;
    case PM_WEREBEAR:
    case PM_OWLBEAR:
    case PM_BEAR:
    case PM_DROP_BEAR:
        return PM_WEREBEAR;
    case PM_PACKRAT:
    case PM_WERERAT:
    case PM_SEWER_RAT:
    case PM_GIANT_RAT:
    case PM_RABID_RAT:
        return PM_WERERAT;
    case PM_WEREJACKAL:
    case PM_JACKAL:
    case PM_FOX:
    case PM_COYOTE:
        return PM_WEREJACKAL;
    case PM_WEREWOLF:
    case PM_WOLF:
    case PM_WARG:
    case PM_WINTER_WOLF:
        return PM_WEREWOLF;
    default:
        break;
    }
    return NON_PM;
}

void
new_were(mon)
register struct monst *mon;
{
    register int pm;

    pm = counter_were(monsndx(mon->data));
    if (mon->data == &mons[PM_DRAUGLIR]) {
        if (mon->msleeping || !mon->mcanmove) {
            mon->msleeping = 0;
            mon->mcanmove = 1;
        }
        if (canseemon(mon)) {
            pline("The fur of %s ripples.", mon_nam(mon));
            mon->mhp += (mon->mhpmax - mon->mhp) / 4;
            return;
        }
    }
    if (pm < LOW_PM) {
        impossible("unknown lycanthrope %s.", mon->data->mname);
        return;
    }

    if (canseemon(mon) && !Hallucination)
        pline("%s changes into a %s.", Monnam(mon),
              is_human(&mons[pm]) ? "human" : mons[pm].mname + 4);

    set_mon_data(mon, &mons[pm]);
    if (mon->msleeping || !mon->mcanmove) {
        /* transformation wakens and/or revitalizes */
        mon->msleeping = 0;
        mon->mfrozen = 0; /* not asleep or paralyzed */
        mon->mcanmove = 1;
    }
    /* regenerate by 1/4 of the lost hit points */
    mon->mhp += (mon->mhpmax - mon->mhp) / 4;
    newsym(mon->mx, mon->my);
    mon_break_armor(mon, FALSE);
    possibly_unwield(mon, FALSE);
}

/* were-creature (even you) summons a horde */
int
were_summon(ptr, yours, visible, genbuf)
struct permonst *ptr;
boolean yours;
int *visible; /* number of visible helpers created */
char *genbuf;
{
    int i, typ, pm = monsndx(ptr);
    struct monst *mtmp;
    int total = 0;
    int cap;

    *visible = 0;
    if (Protection_from_shape_changers && !yours)
        return 0;
    for (i = rnd(5); i > 0; i--) {
        switch (pm) {
        case PM_WERERAT:
        case PM_HUMAN_WERERAT:
        case PM_NOSFERATU: /* the rats _should_ appear around the Nosferatu*/
            typ = rn2(3) ? PM_SEWER_RAT
                         : rn2(3) ? PM_GIANT_RAT : PM_RABID_RAT;
            cap = near_capacity();
            if (cap)
                if (rn2(5) < cap)
                    typ = PM_PACKRAT;
            if (genbuf)
                Strcpy(genbuf, "rat");
            break;
        case PM_WEREJACKAL:
        case PM_HUMAN_WEREJACKAL:
            typ = rn2(7) ? PM_JACKAL : rn2(3) ? PM_COYOTE : PM_FOX;
            if (genbuf)
                Strcpy(genbuf, "jackal");
            break;
        case PM_WERECOCKATRICE:
        case PM_HUMAN_WERECOCKATRICE:
            typ = rn2(3) ? PM_CHICKATRICE : rn2(3) ? PM_PYROLISK : PM_CHICKATRICE;
            if (genbuf)
                Strcpy(genbuf, "cockatrice");
            break;
        case PM_WERETIGER:
            typ = PM_TIGER;
            if (genbuf)
                Strcpy(genbuf, "tiger");
            break;
        case PM_HUMAN_WEREBEAR:
        case PM_WEREBEAR:
            typ = rn2(15) ? PM_BEAR : PM_HELLBEAR;
            if (genbuf)
                Strcpy(genbuf, "bear");
            break;
        case PM_DRAUGLIR:
            typ = rn2(2) ? PM_THOUGHT_HOUND : PM_MEMORY_HOUND;
            break;
        case PM_WEREWOLF:
        case PM_HUMAN_WEREWOLF:
            typ = rn2(5) ? PM_WOLF : rn2(2) ? PM_WARG : PM_WINTER_WOLF;
            if (genbuf)
                Strcpy(genbuf, "wolf");
            break;
        case PM_ALPHA_WEREWOLF:
        case PM_PACK_LORD:
            if (!yours)
                typ = rn2(4) ? PM_WOLF : rn2(3) ? PM_WINTER_WOLF : PM_WEREWOLF;
            else
                typ = rn2(4) ? PM_WOLF : PM_WINTER_WOLF;
            if (genbuf)
                Strcpy(genbuf, "wolf");
            break;
        default:
            if (is_summoner(ptr)) {
                /* Generic summoners can summon anything of the same class, of the
                 * same or lower level. Usually it must also not itself be a summoner,
                 * but bosses can do this. This relies on levels being in order, and
                 * summoners not being the weakest in the class.
                 */
                do {
                    typ = rn2(NUMMONS);
                } while ((mons[typ].mlet != ptr->mlet) || (mons[typ].mlevel > ptr->mlevel) ||
                    (is_summoner(&mons[typ]) && (!is_prince(ptr) || yours)));
                Strcpy(genbuf, def_monsyms[ptr->mlet].name);
            } else continue;
        }
        mtmp = makemon(&mons[typ], u.ux, u.uy, NO_MM_FLAGS);
        if (mtmp) {
            total++;
            if (canseemon(mtmp))
                *visible += 1;
        }
        if (yours && mtmp)
            (void) tamedog(mtmp, (struct obj *) 0);
    }
    return total;
}

void
you_were()
{
    char qbuf[QBUFSZ];
    boolean controllable_poly = Polymorph_control && !(Stunned || Unaware);

    if (Unchanging || u.umonnum == u.ulycn)
        return;
    if (controllable_poly) {
        /* `+4' => skip "were" prefix to get name of beast */
        Sprintf(qbuf, "Do you want to change into %s?",
                an(mons[u.ulycn].mname + 4));
        if (!paranoid_query(ParanoidWerechange, qbuf))
            return;
    }
    (void) polymon(u.ulycn);
    (void) angry_guards(FALSE);
}

void
you_unwere(purify)
boolean purify;
{

    boolean in_wereform = (u.umonnum == u.ulycn);
    boolean controllable_poly = Polymorph_control && !(Stunned || Unaware);
    if (purify) {
        if (Race_if(PM_HUMAN_WEREWOLF)) {
            /* An attempt to purify you has been made! */
            if (in_wereform && Unchanging) {
                killer.format = NO_KILLER_PREFIX;
                Sprintf(killer.name, "purified while stuck in creature form");
                pline_The("purification was deadly...");
                done(DIED);
            } else {
                You_feel("very bad!");
                if (in_wereform)
              rehumanize();
                (void) adjattrib(A_STR, -rn1(3,3), 2);
                (void) adjattrib(A_CON, -rn1(3,3), 1);
                losehp(u.uhp - (u.uhp > 10 ? rnd(5) : 1), "purification",
                  KILLED_BY);
            }
            return;
        }
        You_feel("purified.");
        set_ulycn(NON_PM); /* cure lycanthropy */
    }
    if (!Unchanging && is_were(youmonst.data)
        && (!controllable_poly
            || !paranoid_query(ParanoidWerechange, "Remain in beast form?")))
        rehumanize();
    else if (is_were(youmonst.data) && !u.mtimedone)
        u.mtimedone = rn1(200, 200); /* 40% of initial were change */
}

/* lycanthropy is being caught or cured, but no shape change is involved */
void
set_ulycn(which)
int which;
{
    u.ulycn = which;
    /* add or remove lycanthrope's innate intrinsics (Drain_resistance) */
    set_uasmon();
}

/*were.c*/

/* NetHack 3.6	explode.c	$NHDT-Date: 1522454717 2018/03/31 00:05:17 $  $NHDT-Branch: NetHack-3.6.0 $:$NHDT-Revision: 1.56 $ */
/*      Copyright (C) 1990 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

/* Edited on 4/23/18 by NullCGT */
/* Edited on 7/28/18 by MS */

#include "hack.h"

/* Note: Arrays are column first, while the screen is row first */
static int explosion[3][3] = { { S_explode1, S_explode4, S_explode7 },
                               { S_explode2, S_explode5, S_explode8 },
                               { S_explode3, S_explode6, S_explode9 } };

/* ExplodeRegions share some commonalities with NhRegions, but not enough to
 * make it worth trying to create a common implementation.
 */
typedef struct {
    xchar x, y;
    xchar blast;	/* blast symbol */
    xchar shielded;	/* True if this location is shielded */
} ExplodeLocation;

typedef struct {
    ExplodeLocation *locations;
    short nlocations, alocations;
} ExplodeRegion;

STATIC_DCL ExplodeRegion *
create_explode_region()
{
    ExplodeRegion *reg;

    reg = (ExplodeRegion *)alloc(sizeof(ExplodeRegion));
    reg->locations = (ExplodeLocation *)0;
    reg->nlocations = 0;
    reg->alocations = 0;
    return reg;
}

STATIC_DCL void
add_location_to_explode_region(reg, x, y)
ExplodeRegion *reg;
xchar x, y;
{
    int i;
    ExplodeLocation *new;
    for(i = 0; i < reg->nlocations; i++)
    if (reg->locations[i].x == x && reg->locations[i].y == y)
        return;
    if (reg->nlocations == reg->alocations) {
    reg->alocations = reg->alocations ? 2 * reg->alocations : 32;
    new = (ExplodeLocation *)
        alloc(reg->alocations * sizeof(ExplodeLocation));
    (void) memcpy((genericptr_t)new, (genericptr_t)reg->locations,
        reg->nlocations * sizeof(ExplodeLocation));
    free((genericptr_t)reg->locations);
    reg->locations = new;
    }
    reg->locations[reg->nlocations].x = x;
    reg->locations[reg->nlocations].y = y;
    /* reg->locations[reg->nlocations].blast = 0; */
    /* reg->locations[reg->nlocations].shielded = 0; */
    reg->nlocations++;
}

STATIC_DCL int
compare_explode_location(loc1, loc2)
ExplodeLocation *loc1, *loc2;
{
    return loc1->y == loc2->y ? loc1->x - loc2->x : loc1->y - loc2->y;
}

STATIC_DCL void
set_blast_symbols(reg)
ExplodeRegion *reg;
{
    int i, j, bitmask;
    /* The index into the blast symbol array is a bitmask containing 4 bits:
     * bit 3: True if the location immediately to the north is present
     * bit 2: True if the location immediately to the south is present
     * bit 1: True if the location immediately to the east is present
     * bit 0: True if the location immediately to the west is present
     */
    static int blast_symbols[16] = {
    S_explode5, S_explode6, S_explode4, S_explode5,
    S_explode2, S_explode3, S_explode1, S_explode2,
    S_explode8, S_explode9, S_explode7, S_explode8,
    S_explode5, S_explode6, S_explode4, S_explode5,
    };
    /* Sort in order of North -> South, West -> East */
    qsort(reg->locations, reg->nlocations, sizeof(ExplodeLocation),
        compare_explode_location);
    /* Pass 1: Build the bitmasks in the blast field */
    for(i = 0; i < reg->nlocations; i++)
    reg->locations[i].blast = 0;
    for(i = 0; i < reg->nlocations; i++) {
    bitmask = 0;
    if (i && reg->locations[i-1].y == reg->locations[i].y &&
        reg->locations[i-1].x == reg->locations[i].x-1) {
        reg->locations[i].blast |= 1;	/* Location to the west */
        reg->locations[i-1].blast |= 2;	/* Location to the east */
    }
    for(j = i-1; j >= 0; j--) {
        if (reg->locations[j].y < reg->locations[i].y-1)
        break;
        else if (reg->locations[j].y == reg->locations[i].y-1 &&
            reg->locations[j].x == reg->locations[i].x) {
        reg->locations[i].blast |= 8;	/* Location to the north */
        reg->locations[j].blast |= 4;	/* Location to the south */
        break;
        }
    }
    }
    /* Pass 2: Set the blast symbols */
    for(i = 0; i < reg->nlocations; i++)
    reg->locations[i].blast = blast_symbols[reg->locations[i].blast];
}

STATIC_DCL void
free_explode_region(reg)
ExplodeRegion *reg;
{
    free((genericptr_t)reg->locations);
    free((genericptr_t)reg);
}

/* This is the "do-it-all" explosion command */
STATIC_DCL void FDECL(do_explode,
    (int,int,ExplodeRegion *,int,int,CHAR_P,int,int,BOOLEAN_P));
void
explode(x, y, type, dam, olet, expltype)
int x, y;
int type; /* the same as in zap.c */
int dam;
char olet;
int expltype;
{
    int i, j;
    ExplodeRegion *area;
    area = create_explode_region();
    for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
        if (isok(i+x-1,j+y-1))
        add_location_to_explode_region(area, i+x-1, j+y-1);
    do_explode(x, y, area, type, dam, olet, expltype, 0, !context.mon_moving);
    free_explode_region(area);
}

void
explode2(x, y, type, dam, olet, expltype)
int x, y;
int type; /* the same as in zap.c */
int dam;
char olet;
int expltype;
{
    int i, j;
    ExplodeRegion *area;
    area = create_explode_region();
    for(i = 0; i < 5; i++)
    for(j = 0; j < 5; j++)
        if (isok(i+x-2,j+y-2) && (ZAP_POS((&levl[i+x-2][j+y-2])->typ) || (i<4 && i>0 && j<4 && j>0)))
        add_location_to_explode_region(area, i+x-2, j+y-2);
    do_explode(x, y, area, type, dam, olet, expltype, 0, !context.mon_moving);
    free_explode_region(area);
}

void
big_explode(x, y, type, dam, olet, expltype, radius)
int x, y;
int type; /* the same as in zap.c */
int dam;
char olet;
int expltype;
int radius;
{
    int i, j;
    ExplodeRegion *area;
    area = create_explode_region();
    for(i = 0; i < radius*2+1; i++)
    for(j = 0; j < radius*2+1; j++)
        if (isok(i+x-radius,j+y-radius) && ZAP_POS((&levl[i+x-radius][j+y-radius])->typ))
        add_location_to_explode_region(area, i+x-radius, j+y-radius);
    do_explode(x, y, area, type, dam, olet, expltype, 0, !context.mon_moving);
    free_explode_region(area);
}

/* Note: I had to choose one of three possible kinds of "type" when writing
 * this function: a wand type (like in zap.c), an adtyp, or an object type.
 * Wand types get complex because they must be converted to adtyps for
 * determining such things as fire resistance.  Adtyps get complex in that
 * they don't supply enough information--was it a player or a monster that
 * did it, and with a wand, spell, or breath weapon?  Object types share both
 * these disadvantages....
 *
 * Important note about Half_physical_damage:
 *      Unlike losehp(), explode() makes the Half_physical_damage adjustments
 *      itself, so the caller should never have done that ahead of time.
 *      It has to be done this way because the damage value is applied to
 *      things beside the player. Care is taken within explode() to ensure
 *      that Half_physical_damage only affects the damage applied to the hero.
 */
void
do_explode(x, y, area, type, dam, olet, expltype, dest, yours)
int x, y;
ExplodeRegion *area;
int type; /* the same as in zap.c; passes -(wand typ) for some WAND_CLASS */
int dam;
char olet;
int expltype;
int dest; /* 0 = normal, 1 = silent, 2 = silent/remote */
boolean yours; /* is it your fault (for killing monsters) */
{
    int i, j, k, damu = dam;
    boolean starting = 1, silver = FALSE;
    boolean visible, any_shield;
    int uhurt = 0; /* 0=unhurt, 1=items damaged, 2=you and items damaged */
    const char *str = (const char *) 0;
    int idamres, idamnonres;
    struct monst *mtmp, *mdef = 0;
    uchar adtyp;
    boolean explmask;
    boolean shopdamage = FALSE, generic = FALSE, physical_dmg = FALSE,
            do_hallu = FALSE, inside_engulfer, grabbed, grabbing;
    boolean silent = FALSE, remote = FALSE;
    xchar xi, yi;
    coord grabxy;
    char hallu_buf[BUFSZ], killr_buf[BUFSZ];
    short exploding_wand_typ = 0;

    if (dest > 0) silent = TRUE;
    if (dest == 2) remote = TRUE;

    if (olet == WAND_CLASS) { /* retributive strike */
        /* 'type' is passed as (wand's object type * -1); save
           object type and convert 'type' itself to zap-type */
        if (type < 0) {
            type = -type;
            exploding_wand_typ = (short) type;
            /* most attack wands produce specific explosions;
               other types produce a generic magical explosion */
            if (objects[type].oc_dir == RAY
                && type != WAN_DIGGING && type != WAN_SLEEP_RAY) {
                type -= WAN_MISSILE;
                if (type < 0 || type > 9) {
                    impossible("explode: device has bad zap type (%d).", type);
                    type = 0;
                }
            } else
                type = 0;
        }
        switch (Role_switch) {
        case PM_PRIEST:
        case PM_MONK:
        case PM_WIZARD:
            damu /= 5;
            break;
        case PM_HEALER:
        case PM_KNIGHT:
            damu /= 2;
            break;
        default:
            break;
        }
    }
    /* muse_unslime: SCR_FIRE */
    if (expltype < 0) {
        /* hero gets credit/blame for killing this monster, not others */
        mdef = m_at(x, y);
        expltype = -expltype;
    }
    /* if hero is engulfed and caused the explosion, only hero and
       engulfer will be affected */
    inside_engulfer = (u.uswallow && type >= 0);
    /* held but not engulfed implies holder is reaching into second spot
       so might get hit by double damage */
    grabbed = grabbing = FALSE;
    if (u.ustuck && !u.uswallow) {
        if (Upolyd && sticks(youmonst.data))
            grabbing = TRUE;
        else
            grabbed = TRUE;
        grabxy.x = u.ustuck->mx;
        grabxy.y = u.ustuck->my;
    } else
        grabxy.x = grabxy.y = 0; /* lint suppression */
    /* FIXME:
     *  It is possible for a grabber to be outside the explosion
     *  radius and reaching inside to hold the hero.  If so, it ought
     *  to take damage (the extra half of double damage).  It is also
     *  possible for poly'd hero to be outside the radius and reaching
     *  in to hold a monster.  Hero should take damage in that situation.
     *
     *  Probably the simplest way to handle this would be to expand
     *  the radius used when collecting targets but exclude everything
     *  beyond the regular radius which isn't reaching inside.  Then
     *  skip harm to gear of any extended targets when inflicting damage.
     */

    if (olet == MON_EXPLODE) {
        /* when explode() is called recursively, killer.name might change so
           we need to retain a copy of the current value for this explosion */
        str = strcpy(killr_buf, killer.name);
        do_hallu = (Hallucination
                    && (strstri(str, "'s explosion")
                        || strstri(str, "s' explosion")));
        adtyp = AD_PHYS;
    }

    switch (abs(type) % 10) {
    case 0:
        str = (olet == POTION_CLASS) ? "alchemical blast" : "magical blast";
        adtyp = AD_MAGM;
        break;
    case 1:
        str = (olet == BURNING_OIL) ? "burning oil"
                 : (olet == SCROLL_CLASS) ? "tower of flame" : "fireball";
        /* fire damage, not physical damage */
        adtyp = AD_FIRE;
        break;
    case 2:
        str = "ball of cold";
        adtyp = AD_COLD;
        break;
    case 3:
        str = "blast";
        adtyp = AD_PHYS;
        break;
    case 4:
        str = (olet == WAND_CLASS) ? "death field"
                                   : "disintegration field";
        adtyp = AD_DISN;
        break;
    case 5:
        str = "ball of lightning";
        adtyp = AD_ELEC;
        break;
    case 6:
        str = "poison gas cloud";
        adtyp = AD_DRST;
        break;
    case 7:
        str = "splash of acid";
        adtyp = AD_ACID;
        break;
    case 8:
        str = "sonicboom";
        adtyp = AD_LOUD;
        break;
    case 9:
        str = "psionic explosion";
        adtyp = AD_PSYC;
        break;
    default:
        impossible("explosion base type %d?", type);
        return;
    }

    any_shield = visible = FALSE;
    for(i = 0; i < area->nlocations; i++) {
        explmask = FALSE;
        xi = area->locations[i].x;
        yi = area->locations[i].y;
        if (!isok(xi, yi))
            continue;

        if (xi == u.ux && yi == u.uy) {
            switch (adtyp) {
            case AD_PHYS:
                explmask = 0;
                break;
            case AD_MAGM:
                explmask = !!Antimagic;
                break;
            case AD_FIRE:
                explmask = !!Fire_resistance;
                break;
            case AD_COLD:
                explmask = !!Cold_resistance;
                break;
            case AD_DISN:
                explmask = (olet == WAND_CLASS)
                                     ? !!(nonliving(youmonst.data)
                                          || is_demon(youmonst.data))
                                     : !!Disint_resistance;
                break;
            case AD_ELEC:
                explmask = !!Shock_resistance;
                break;
            case AD_DRST:
                explmask = !!Poison_resistance;
                break;
            case AD_ACID:
                explmask = !!Acid_resistance;
                physical_dmg = TRUE;
                break;
                case AD_LOUD:
                    explmask = !!Sonic_resistance;
                    physical_dmg = TRUE;
                    break;
                case AD_PSYC:
                    explmask = !!Psychic_resistance;
                    break;
                default:
                    impossible("explosion type %d?", adtyp);
                    break;
            }
        }
        /* can be both you and mtmp if you're swallowed or riding */
        mtmp = m_at(xi, yi);
        if (!mtmp && xi == u.ux && yi == u.uy)
            mtmp = u.usteed;
        if (mtmp) {
            if (DEADMONSTER(mtmp))
                explmask = 2; /* FILE_NOT_FOUND */
            else
                switch (adtyp) {
                case AD_PHYS:
                    break;
                case AD_MAGM:
                    explmask |= resists_magm(mtmp);
                    break;
                case AD_FIRE:
                    explmask |= resists_fire(mtmp);
                    break;
                case AD_COLD:
                    explmask |= resists_cold(mtmp);
                    break;
                case AD_DISN:
                    explmask |= (olet == WAND_CLASS)
                                          ? (nonliving(mtmp->data)
                                             || is_demon(mtmp->data))
                                          : resists_disint(mtmp);
                    break;
                case AD_ELEC:
                    explmask |= resists_elec(mtmp);
                    break;
                case AD_DRST:
                    explmask |= resists_poison(mtmp);
                    break;
                case AD_ACID:
                    explmask |= resists_acid(mtmp);
                    break;
                case AD_LOUD:
                    explmask |= resists_sonic(mtmp);
                    break;
                case AD_PSYC:
                    explmask |= resists_psychic(mtmp);
                    break;
                default:
                    impossible("explosion type %d?", adtyp);
                    break;
            }
        }
        if (mtmp && cansee(xi, yi) && !canspotmon(mtmp))
            map_invisible(xi, yi);
        else if (!mtmp && glyph_is_invisible(levl[xi][yi].glyph))
            (void) unmap_invisible(xi, yi);
        if (cansee(xi, yi))
            visible = TRUE;
        if (explmask)
            any_shield = TRUE;
        area->locations[i].shielded = explmask;
    }

    /* Not visible if remote */
    if (remote) visible = FALSE;

    if (visible) {
        set_blast_symbols(area);
        /* Start the explosion */
        for(i = 0; i < area->nlocations; i++) {
            tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
                   explosion_to_glyph(expltype, area->locations[i].blast));
            tmp_at(area->locations[i].x, area->locations[i].y);
            starting = 0;
        }
        curs_on_u(); /* will flush screen and output */

        if (any_shield && flags.sparkle) { /* simulate shield effect */
            for (k = 0; k < SHIELD_COUNT; k++) {
                for(i = 0; i < area->nlocations; i++) {
                    if (area->locations[i].shielded)
                        /*
                         * Bypass tmp_at() and send the shield glyphs
                         * directly to the buffered screen.  tmp_at()
                         * will clean up the location for us later.
                         */
                        show_glyph(area->locations[i].x,
                                    area->locations[i].y,
                                    cmap_to_glyph(shield_static[k]));
                }
                curs_on_u(); /* will flush screen and output */
                delay_output();
            }

            /* Cover last shield glyph with blast symbol. */
            for(i = 0; i < area->nlocations; i++) {
                if (area->locations[i].shielded)
                    show_glyph(area->locations[i].x,
                        area->locations[i].y,
                        explosion_to_glyph(expltype,
                        area->locations[i].blast));
            }

        } else { /* delay a little bit. */
            delay_output();
            delay_output();
        }

        tmp_at(DISP_END, 0); /* clear the explosion */
    } else if (!remote) {
        if (olet == MON_EXPLODE) {
            str = "explosion";
            generic = TRUE;
        }
        if (!Deaf && olet != SCROLL_CLASS)
            You_hear(is_pool(x, y) ? "a muffled explosion." : "a blast.");
    }
    
    if ((olet == POTION_CLASS) || (olet == MON_EXPLODE))
        exploding_wand_typ = 10;

    if (dam) {
        for(i = 0; i < area->nlocations; i++) {
            xi = area->locations[i].x;
            yi = area->locations[i].y;
            if (xi == u.ux && yi == u.uy)
                uhurt = area->locations[i].shielded ? 1 : 2;
            idamres = idamnonres = 0;

            /* DS: Allow monster induced explosions also */
            if (type >= 0 || type <= -10)
                (void)zap_over_floor((xchar)xi, (xchar)yi, type, &shopdamage, exploding_wand_typ);

            mtmp = m_at(xi, yi);
            if (xi == u.ux && yi == u.uy)
                uhurt = (explmask == 1) ? 1 : 2;
            /* for inside_engulfer, only <u.ux,u.uy> is affected */
            else if (inside_engulfer)
                continue;
            idamres = idamnonres = 0;

            if (!mtmp && xi == u.ux && yi == u.uy)
                mtmp = u.usteed;
            if (!mtmp)
                continue;
            if (DEADMONSTER(mtmp))
                continue;
            if (do_hallu) {
                /* replace "gas spore" with a different description
                   for each target (we can't distinguish personal names
                   like "Barney" here in order to suppress "the" below,
                   so avoid any which begins with a capital letter) */
                do {
                    Sprintf(hallu_buf, "%s explosion",
                            s_suffix(rndmonnam((char *) 0)));
                } while (*hallu_buf != lowc(*hallu_buf));
                str = hallu_buf;
            }
            if (u.uswallow && mtmp == u.ustuck) {
                const char *adj = (char *) 0;

                if (is_animal(u.ustuck->data)) {
                    switch (adtyp) {
                    case AD_FIRE:
                        adj = "heartburn";
                        break;
                    case AD_COLD:
                        adj = "chilly";
                        break;
                    case AD_DISN:
                        if (olet == WAND_CLASS)
                            adj = "irradiated by pure energy";
                        else
                            adj = "perforated";
                        break;
                    case AD_ELEC:
                        adj = "shocked";
                        break;
                    case AD_DRST:
                        adj = "poisoned";
                        break;
                    case AD_ACID:
                        adj = "an upset stomach";
                        break;
                    case AD_LOUD:
                        adj = "an upset stomach";
                        break;
                    case AD_PSYC:
                        adj = "mentally blasted";
                        break;
                    case AD_PHYS:
                        adj = "blasted";
                        break;
                    default:
                        adj = "an uppended stomach";
                        break;
                    }
                    pline("%s gets %s!", Monnam(u.ustuck), adj);
                } else {
                    switch (adtyp) {
                    case AD_FIRE:
                        adj = "toasted";
                        break;
                    case AD_COLD:
                        adj = "chilly";
                        break;
                    case AD_DISN:
                        if (olet == WAND_CLASS)
                            adj = "overwhelmed by pure energy";
                        else
                            adj = "perforated";
                        break;
                    case AD_ELEC:
                        adj = "shocked";
                        break;
                    case AD_DRST:
                        adj = "intoxicated";
                        break;
                    case AD_ACID:
                        adj = "burned";
                        break;
                    case AD_LOUD:
                        adj = "blasted";
                        break;
                    case AD_PSYC:
                        /* not a great adjective, or even a real one */
                        adj = "mindblasted";
                        break;
                    case AD_PHYS:
                        adj = "blasted";
                        break;
                    default:
                        adj = "fried";
                        break;
                    }
                    pline("%s gets slightly %s!", Monnam(u.ustuck), adj);
                }
            } else if (!silent && cansee(xi, yi)) {
                if (mtmp->m_ap_type)
                    seemimic(mtmp);
                pline("%s is caught in the %s!", Monnam(mtmp), str);
            }

            if (!(area->locations[i].shielded)) { /* Was affected */
                idamres += destroy_mitem(mtmp, SCROLL_CLASS, (int) adtyp);
                idamres += destroy_mitem(mtmp, SPBOOK_CLASS, (int) adtyp);
                idamnonres += destroy_mitem(mtmp, POTION_CLASS, (int) adtyp);
                idamnonres += destroy_mitem(mtmp, WAND_CLASS, (int) adtyp);
                idamnonres += destroy_mitem(mtmp, RING_CLASS, (int) adtyp);
            }
            if (area->locations[i].shielded) {
                golemeffects(mtmp, (int) adtyp, dam + idamres);
                mtmp->mhp -= idamnonres;
            } else {
                /* call resist with 0 and do damage manually so 1) we can
                 * get out the message before doing the damage, and 2) we
                 * can call mondied, not killed, if it's not your blast
                 */
                int mdam = dam;

                if (resist(mtmp, olet, 0, FALSE)) {
                    /* inside_engulfer: <i+x-1,j+y-1> == <u.ux,u.uy> */
                    if (!silent && cansee(xi, yi) || inside_engulfer)
                        pline("%s resists the %s!", Monnam(mtmp), str);
                    mdam = (dam + 1) / 2;
                }
                /* if grabber is reaching into hero's spot and
                   hero's spot is within explosion radius, grabber
                   gets hit by double damage */
                if (grabbed && mtmp == u.ustuck && distu(x, y) <= 2)
                    mdam *= 2;
                /* being resistant to opposite type of damage makes
                   target more vulnerable to current type of damage
                   (when target is also resistant to current type,
                   we won't get here) */
                if (resists_cold(mtmp) && adtyp == AD_FIRE)
                    mdam *= 2;
                else if (resists_fire(mtmp) && adtyp == AD_COLD)
                    mdam *= 2;
                mtmp->mhp -= mdam;
                mtmp->mhp -= (idamres + idamnonres);
            }
            if (DEADMONSTER(mtmp)) {
                int xkflg = ((adtyp == AD_FIRE
                              && completelyburns(mtmp->data))
                             ? XKILL_NOCORPSE : 0);

                if (!context.mon_moving) {
                    xkilled(mtmp, XKILL_GIVEMSG | xkflg);
                } else if (mdef && mtmp == mdef) {
                    /* 'mdef' killed self trying to cure being turned
                     * into slime due to some action by the player.
                     * Hero gets the credit (experience) and most of
                     * the blame (possible loss of alignment and/or
                     * luck and/or telepathy depending on mtmp) but
                     * doesn't break pacifism.  xkilled()'s message
                     * would be "you killed <mdef>" so give our own.
                     */
                    if (cansee(mtmp->mx, mtmp->my) || canspotmon(mtmp))
                        pline("%s is %s!", Monnam(mtmp),
                              xkflg ? "burned completely"
                                    : nonliving(mtmp->data) ? "destroyed"
                                                            : "killed");
                    xkilled(mtmp, XKILL_NOMSG | XKILL_NOCONDUCT | xkflg);
                } else {
                    if (xkflg)
                        adtyp = AD_RBRE; /* no corpse */
                    monkilled(mtmp, mdef, "", (int) adtyp);
                }
            } else if (!context.mon_moving) {
                /* all affected monsters, even if mdef is set */
                setmangry(mtmp, TRUE);
            }
        }
    }

    /* Do your injury last */

    /* You are not hurt if this is remote */
    if (remote) uhurt = FALSE;

    if (uhurt) {
        /* [ALI] Give message if it's a weapon (grenade) exploding */
        if (flags.verbose && (type < 0 || adtyp == AD_PHYS || olet == WEAPON_CLASS)) {
            if (do_hallu) { /* (see explanation above) */
                do {
                    Sprintf(hallu_buf, "%s explosion",
                            s_suffix(rndmonnam((char *) 0)));
                } while (*hallu_buf != lowc(*hallu_buf));
                str = hallu_buf;
            }
            You("are caught in the %s!", str);
            iflags.last_msg = PLNMSG_CAUGHT_IN_EXPLOSION;
        }
        /* do property damage first, in case we end up leaving bones */
        if (adtyp == AD_FIRE)
            burn_away_slime();
        if (Invulnerable) {
            damu = 0;
            You("are unharmed!");
        } else if (adtyp == AD_PHYS || physical_dmg)
            damu = Maybe_Half_Phys(damu);
        if (adtyp == AD_FIRE)
            (void) burnarmor(&youmonst);
        if (uhurt == 2) {
            destroy_item(SCROLL_CLASS, (int) adtyp);
            destroy_item(SPBOOK_CLASS, (int) adtyp);
            destroy_item(POTION_CLASS, (int) adtyp);
            destroy_item(RING_CLASS, (int) adtyp);
            destroy_item(WAND_CLASS, (int) adtyp);
            destroy_item(TOOL_CLASS, (int) adtyp);
        }

        ugolemeffects((int) adtyp, damu);
        if (uhurt == 2) {
            /* if poly'd hero is grabbing another victim, hero takes
               double damage (note: don't rely on u.ustuck here because
               that victim might have been killed when hit by the blast) */
            if (grabbing && dist2((int) grabxy.x, (int) grabxy.y, x, y) <= 2)
                damu *= 2;
            /* hero does not get same fire-resistant vs cold and
               cold-resistant vs fire double damage as monsters [why not?] */
            if (Upolyd)
                u.mh -= damu;
            else
                u.uhp -= damu;
            context.botl = 1;
        }

        if (u.uhp <= 0 || (Upolyd && u.mh <= 0)) {
            if (Upolyd) {
                rehumanize();
            } else {
                if (olet == MON_EXPLODE) {
                    if (generic) /* explosion was unseen; str=="explosion", */
                        ;        /* killer.name=="gas spore's explosion"    */
                    else if (str != killer.name && str != hallu_buf)
                        Strcpy(killer.name, str);
                    killer.format = KILLED_BY_AN;
                } else if (type >= 0 && olet != SCROLL_CLASS) {
                    killer.format = NO_KILLER_PREFIX;
                    Sprintf(killer.name, "caught %sself in %s own %s", uhim(),
                            uhis(), str);
                } else {
                    killer.format = (!strcmpi(str, "tower of flame")
                                     || !strcmpi(str, "fireball"))
                                        ? KILLED_BY_AN
                                        : KILLED_BY;
                    Strcpy(killer.name, str);
                }
                if (iflags.last_msg == PLNMSG_CAUGHT_IN_EXPLOSION
                    || iflags.last_msg == PLNMSG_TOWER_OF_FLAME) /*seffects()*/
                    pline("It is fatal.");
                else
                    pline_The("%s is fatal.", str);
                /* Known BUG: BURNING suppresses corpse in bones data,
                   but done does not handle killer reason correctly */
                done((adtyp == AD_FIRE) ? BURNING : DIED);
            }
        }
        exercise(A_STR, FALSE);
    }

    if (shopdamage) {
        pay_for_damage((adtyp == AD_FIRE) ? "burn away"
                          : (adtyp == AD_COLD) ? "shatter"
                             : (adtyp == AD_DISN) ? "disintegrate"
                                : "destroy",
                       FALSE);
    }

    /* explosions are noisy */
    i = dam * dam;
    if (i < 50)
        i = 50; /* in case random damage is very small */
    if (inside_engulfer)
        i = (i + 3) / 4;
    wake_nearto(x, y, i);
}

struct scatter_chain {
    struct scatter_chain *next; /* pointer to next scatter item */
    struct obj *obj;            /* pointer to the object        */
    xchar ox;                   /* location of                  */
    xchar oy;                   /*      item                    */
    schar dx;                   /* direction of                 */
    schar dy;                   /*      travel                  */
    int range;                  /* range of object              */
    boolean stopped;            /* flag for in-motion/stopped   */
};

/*
 * scflags:
 *      VIS_EFFECTS     Add visual effects to display
 *      MAY_HITMON      Objects may hit monsters
 *      MAY_HITYOU      Objects may hit hero
 *      MAY_HIT         Objects may hit you or monsters
 *      MAY_DESTROY     Objects may be destroyed at random
 *      MAY_FRACTURE    Stone objects can be fractured (statues, boulders)
 */

/* returns number of scattered objects */
long
scatter(sx, sy, blastforce, scflags, obj)
int sx, sy;     /* location of objects to scatter */
int blastforce; /* force behind the scattering */
unsigned int scflags;
struct obj *obj; /* only scatter this obj        */
{
    register struct obj *otmp;
    register int tmp;
    int farthest = 0;
    uchar typ;
    long qtmp;
    boolean used_up;
    boolean individual_object = obj ? TRUE : FALSE;
    struct monst *mtmp;
    struct scatter_chain *stmp, *stmp2 = 0;
    struct scatter_chain *schain = (struct scatter_chain *) 0;
    long total = 0L;

    while ((otmp = (individual_object ? obj : level.objects[sx][sy])) != 0) {
        if (otmp->quan > 1L) {
            qtmp = otmp->quan - 1L;
            if (qtmp > LARGEST_INT)
                qtmp = LARGEST_INT;
            qtmp = (long) rnd((int) qtmp);
            otmp = splitobj(otmp, qtmp);
        } else {
            obj = (struct obj *) 0; /* all used */
        }
        obj_extract_self(otmp);
        used_up = FALSE;

        /* 9 in 10 chance of fracturing boulders or statues */
        if ((scflags & MAY_FRACTURE) != 0
            && (otmp->otyp == BOULDER || otmp->otyp == STATUE)
            && rn2(10)) {
            if (otmp->otyp == BOULDER) {
                if (cansee(sx, sy))
                    pline("%s apart.", Tobjnam(otmp, "break"));
                else
                    You_hear("stone breaking.");
                fracture_rock(otmp);
                place_object(otmp, sx, sy);
                if ((otmp = sobj_at(BOULDER, sx, sy)) != 0) {
                    /* another boulder here, restack it to the top */
                    obj_extract_self(otmp);
                    place_object(otmp, sx, sy);
                }
            } else {
                struct trap *trap;

                if ((trap = t_at(sx, sy)) && trap->ttyp == STATUE_TRAP)
                    deltrap(trap);
                if (cansee(sx, sy))
                    pline("%s.", Tobjnam(otmp, "crumble"));
                else
                    You_hear("stone crumbling.");
                (void) break_statue(otmp);
                place_object(otmp, sx, sy); /* put fragments on floor */
            }
            newsym(sx, sy); /* in case it's beyond radius of 'farthest' */
            used_up = TRUE;

            /* 1 in 10 chance of destruction of obj; glass, egg destruction */
        } else if ((scflags & MAY_DESTROY) != 0
                   && (!rn2(10) || (otmp->material == GLASS
                                    || otmp->otyp == EGG))) {
            if (breaks(otmp, (xchar) sx, (xchar) sy))
                used_up = TRUE;
        }

        if (!used_up) {
            stmp = (struct scatter_chain *) alloc(sizeof *stmp);
            stmp->next = (struct scatter_chain *) 0;
            stmp->obj = otmp;
            stmp->ox = sx;
            stmp->oy = sy;
            tmp = rn2(8); /* get the direction */
            stmp->dx = xdir[tmp];
            stmp->dy = ydir[tmp];
            tmp = blastforce - (otmp->owt / 40);
            if (tmp < 1)
                tmp = 1;
            stmp->range = rnd(tmp); /* anywhere up to that determ. by wt */
            if (farthest < stmp->range)
                farthest = stmp->range;
            stmp->stopped = FALSE;
            if (!schain)
                schain = stmp;
            else
                stmp2->next = stmp;
            stmp2 = stmp;
        }
    }

    while (farthest-- > 0) {
        for (stmp = schain; stmp; stmp = stmp->next) {
            if ((stmp->range-- > 0) && (!stmp->stopped)) {
                bhitpos.x = stmp->ox + stmp->dx;
                bhitpos.y = stmp->oy + stmp->dy;
                typ = levl[bhitpos.x][bhitpos.y].typ;
                if (!isok(bhitpos.x, bhitpos.y)) {
                    bhitpos.x -= stmp->dx;
                    bhitpos.y -= stmp->dy;
                    stmp->stopped = TRUE;
                } else if (!ZAP_POS(typ)
                           || closed_door(bhitpos.x, bhitpos.y)) {
                    bhitpos.x -= stmp->dx;
                    bhitpos.y -= stmp->dy;
                    stmp->stopped = TRUE;
                } else if ((mtmp = m_at(bhitpos.x, bhitpos.y)) != 0) {
                    if (scflags & MAY_HITMON) {
                        stmp->range--;
                        if (ohitmon(mtmp, stmp->obj, 1, FALSE)) {
                            stmp->obj = (struct obj *) 0;
                            stmp->stopped = TRUE;
                        }
                    }
                } else if (bhitpos.x == u.ux && bhitpos.y == u.uy) {
                    if (scflags & MAY_HITYOU) {
                        int hitvalu, hitu;

                        if (multi)
                            nomul(0);
                        hitvalu = 8 + stmp->obj->spe;
                        if (bigmonst(youmonst.data))
                            hitvalu++;
                        hitu = thitu(hitvalu, dmgval(stmp->obj, &youmonst),
                                     &stmp->obj, (char *) 0);
                        if (!stmp->obj)
                            stmp->stopped = TRUE;
                        if (hitu) {
                            stmp->range -= 3;
                            stop_occupation();
                        }
                    }
                } else {
                    if (scflags & VIS_EFFECTS) {
                        /* tmp_at(bhitpos.x, bhitpos.y); */
                        /* delay_output(); */
                    }
                }
                stmp->ox = bhitpos.x;
                stmp->oy = bhitpos.y;
            }
        }
    }
    for (stmp = schain; stmp; stmp = stmp2) {
        int x, y;

        stmp2 = stmp->next;
        x = stmp->ox;
        y = stmp->oy;
        if (stmp->obj) {
            if (x != sx || y != sy)
                total += stmp->obj->quan;
            place_object(stmp->obj, x, y);
            stackobj(stmp->obj);
        }
        free((genericptr_t) stmp);
        newsym(x, y);
    }

    return total;
}

/*
 * Splatter burning oil from x,y to the surrounding area.
 *
 * This routine should really take a how and direction parameters.
 * The how is how it was caused, e.g. kicked verses thrown.  The
 * direction is which way to spread the flaming oil.  Different
 * "how"s would give different dispersal patterns.  For example,
 * kicking a burning flask will splatter differently from a thrown
 * flask hitting the ground.
 *
 * For now, just perform a "regular" explosion.
 */
void
splatter_burning_oil(x, y)
int x, y;
{
/* ZT_SPELL(ZT_FIRE) = ZT_SPELL(AD_FIRE-1) = 10+(2-1) = 11 */
#define ZT_SPELL_O_FIRE 11 /* value kludge, see zap.c */
    explode(x, y, ZT_SPELL_O_FIRE, d(4, 4), BURNING_OIL, EXPL_FIERY);
}

#define BY_OBJECT       ((struct monst *)0)

STATIC_DCL int
dp(n, p)        /* 0 <= dp(n, p) <= n */
int n, p;
{
    int tmp = 0;
    while (n--) tmp += !rn2(p);
    return tmp;
}

#define GRENADE_TRIGGER(obj) \
    if ((obj)->otyp == FRAG_GRENADE) { \
        delquan = dp((obj)->quan, 10); \
        no_fiery += delquan; \
    } else if ((obj)->otyp == GAS_GRENADE) { \
        delquan = dp((obj)->quan, 10); \
        no_gas += delquan; \
    } else if ((obj)->otyp == STICK_OF_DYNAMITE) { \
        delquan = (obj)->quan; \
        no_fiery += (obj)->quan * 2; \
        no_dig += (obj)->quan; \
    } else if (is_bullet(obj)) \
        delquan = (obj)->quan; \
    else \
        delquan = 0

struct grenade_callback {
    ExplodeRegion *fiery_area, *gas_area, *dig_area;
    boolean isyou;
};

STATIC_DCL void FDECL(grenade_effects, (struct obj *,XCHAR_P,XCHAR_P,
    ExplodeRegion *,ExplodeRegion *,ExplodeRegion *,BOOLEAN_P));

STATIC_DCL int
grenade_fiery_callback(data, x, y)
genericptr_t data;
int x, y;
{
    int is_accessible = ZAP_POS(levl[x][y].typ);
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (is_accessible) {
    add_location_to_explode_region(gc->fiery_area, x, y);
    grenade_effects((struct obj *)0, x, y,
        gc->fiery_area, gc->gas_area, gc->dig_area, gc->isyou);
    }
    return !is_accessible;
}

STATIC_DCL int
grenade_gas_callback(data, x, y)
genericptr_t data;
int x, y;
{
    int is_accessible = ZAP_POS(levl[x][y].typ);
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (is_accessible)
    add_location_to_explode_region(gc->gas_area, x, y);
    return !is_accessible;
}

STATIC_DCL int
grenade_dig_callback(data, x, y)
genericptr_t data;
int x, y;
{
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (dig_check(BY_OBJECT, FALSE, x, y))
    add_location_to_explode_region(gc->dig_area, x, y);
    return !ZAP_POS(levl[x][y].typ);
}

STATIC_DCL void
grenade_effects(source, x, y, fiery_area, gas_area, dig_area, isyou)
struct obj *source;
xchar x, y;
ExplodeRegion *fiery_area, *gas_area, *dig_area;
boolean isyou;
{
    int i, r;
    struct obj *obj, *obj2;
    struct monst *mon;
    /*
     * Note: These count explosive charges in arbitary units. Grenades
     *       are counted as 1 and sticks of dynamite as 2 fiery and 1 dig.
     */
    int no_gas = 0, no_fiery = 0, no_dig = 0;
    int delquan;
    boolean shielded = FALSE, redraw;
    struct grenade_callback gc;

    if (source) {
        if (source->otyp == GAS_GRENADE)
            no_gas += source->quan;
        else if (source->otyp == FRAG_GRENADE)
            no_fiery += source->quan;
        else if (source->otyp == STICK_OF_DYNAMITE) {
            no_fiery += source->quan * 2;
            no_dig += source->quan;
        }
        redraw = source->where == OBJ_FLOOR;
        obj_extract_self(source);
        obfree(source, (struct obj *)0);
        if (redraw) newsym(x, y);
    }
    mon = m_at(x, y);
#ifdef STEED
    if (!mon && x == u.ux && y == u.uy)
    mon = u.usteed;
#endif
    if (mon && !DEADMONSTER(mon)) {
        if (resists_fire(mon)) {
            shielded = TRUE;
        } else {
            for(obj = mon->minvent; obj; obj = obj2) {
                obj2 = obj->nobj;
                GRENADE_TRIGGER(obj);
                for(i = 0; i < delquan; i++)
                    m_useup(mon, obj);
            }
        }
    }
    if (x == u.ux && y == u.uy) {
        if (Fire_resistance) {
            shielded = TRUE;
        } else {
            for(obj = invent; obj; obj = obj2) {
                obj2 = obj->nobj;
                GRENADE_TRIGGER(obj);
                for(i = 0; i < delquan; i++)
                    useup(obj);
            }
        }
    }
    if (!shielded)
    for(obj = level.objects[x][y]; obj; obj = obj2) {
        obj2 = obj->nexthere;
        GRENADE_TRIGGER(obj);
        if (delquan) {
            if (isyou)
                useupf(obj, delquan);
            else if (delquan < obj->quan)
                obj->quan -= delquan;
            else
                delobj(obj);
        }
    }
    gc.fiery_area = fiery_area;
    gc.gas_area = gas_area;
    gc.dig_area = dig_area;
    gc.isyou = isyou;
    if (no_gas) {
        /* r = floor(log2(n))+1 */
        r = 0;
        while(no_gas) {
            r++;
            no_gas /= 2;
        }
        xpathto(r, x, y, grenade_gas_callback, (genericptr_t)&gc);
    }
    if (no_fiery) {
        /* r = floor(log2(n))+1 */
        r = 0;
        while(no_fiery) {
            r++;
            no_fiery /= 2;
        }
        xpathto(r, x, y, grenade_fiery_callback, (genericptr_t)&gc);
    }
    if (no_dig) {
        /* r = floor(log2(n))+1 */
        r = 0;
        while(no_dig) {
            r++;
            no_dig /= 2;
        }
        xpathto(r, x, y, grenade_dig_callback, (genericptr_t)&gc);
    }
}

/*
 * Note: obj is not valid after return
 */

void
grenade_explode(obj, x, y, isyou, dest)
struct obj *obj;
int x, y;
boolean isyou;
int dest;
{
    int i, ztype;
    boolean shop_damage = FALSE;
    int ox, oy;
    ExplodeRegion *fiery_area, *gas_area, *dig_area;
    struct trap *trap;

    fiery_area = create_explode_region();
    gas_area = create_explode_region();
    dig_area = create_explode_region();
    grenade_effects(obj, x, y, fiery_area, gas_area, dig_area, isyou);
    if (fiery_area->nlocations) {
        ztype = isyou ? ZT_SPELL(ZT_FIRE) : -ZT_SPELL(ZT_FIRE);
        do_explode(x, y, fiery_area, ztype, d(3,6), WEAPON_CLASS,
          EXPL_FIERY, dest, isyou);
    }
    wake_nearto(x, y, 400);
    /* Like cartoons - the explosion first, then
     * the world deals with the holes produced ;)
     */
    for(i = 0; i < dig_area->nlocations; i++) {
        ox = dig_area->locations[i].x;
        oy = dig_area->locations[i].y;
        if (IS_WALL(levl[ox][oy].typ) || IS_DOOR(levl[ox][oy].typ)) {
            watch_dig((struct monst *)0, ox, oy, TRUE);
            if (*in_rooms(ox, oy, SHOPBASE)) shop_damage = TRUE;
        }
        dodigactualhole(ox, oy, BY_OBJECT, PIT, TRUE, FALSE);
    }
    free_explode_region(dig_area);
    for(i = 0; i < fiery_area->nlocations; i++) {
        ox = fiery_area->locations[i].x;
        oy = fiery_area->locations[i].y;
        if ((trap = t_at(ox, oy)) != 0 && trap->ttyp == LANDMINE)
            blow_up_landmine(trap);
    }
    free_explode_region(fiery_area);
    if (gas_area->nlocations) {
        ztype = isyou ? ZT_SPELL(ZT_POISON_GAS) : -ZT_SPELL(ZT_POISON_GAS);
        do_explode(x, y, gas_area, ztype, d(3,6), WEAPON_CLASS,
          EXPL_NOXIOUS, dest, isyou);
    }
    free_explode_region(gas_area);
    if (shop_damage)
        pay_for_damage("damage", FALSE);
}

void arm_bomb(obj, yours)
struct obj *obj;
boolean yours;
{
    if (is_grenade(obj)) {
        attach_bomb_blow_timeout(obj,
            (obj->cursed ? rn2(5) + 2 : obj->blessed ? 4 :
            rn2(2) + 3) , yours);
    } else
        impossible("arming a bomb that is not a bomb?");
    /* Otherwise, do nothing */
}

/* lit potion of oil is exploding; extinguish it as a light source before
   possibly killing the hero and attempting to save bones */
void
explode_oil(obj, x, y)
struct obj *obj;
int x, y;
{
    if (!obj->lamplit)
        impossible("exploding unlit oil");
    end_burn(obj, TRUE);
    splatter_burning_oil(x, y);
}

/*explode.c*/

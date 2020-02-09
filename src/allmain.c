/* NetHack 3.6	allmain.c	$NHDT-Date: 1555552624 2019/04/18 01:57:04 $  $NHDT-Branch: NetHack-3.6.2-beta01 $:$NHDT-Revision: 1.100 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2012. */
/* NetHack may be freely redistributed.  See license for details. */

/* various code that was replicated in *main.c */

/* Edited on 5/11/18 by NullCGT */

#include "hack.h"
#include <ctype.h>

#ifndef NO_SIGNAL
#include <signal.h>
#endif

#ifdef POSITIONBAR
STATIC_DCL void NDECL(do_positionbar);
#endif
STATIC_DCL void FDECL(regen_hp, (int));
STATIC_DCL void FDECL(interrupt_multi, (const char *));
STATIC_DCL void FDECL(debug_fields, (const char *));

#ifdef EXTRAINFO_FN
static long prev_dgl_extrainfo = 0;
#endif

/* Cursed land mines - recurse into containers. Only one mine explodes. */
boolean
checklandmine(pobj)
struct obj *pobj;
{
    if (pobj) {
        if (Has_contents(pobj)) {
            struct obj *cobj;
            for(cobj = pobj->cobj; cobj; cobj = cobj->nobj) {
                if (checklandmine(cobj))
                    return TRUE;
            }
        } else if (pobj->cursed) {
            if (pobj->otyp == LAND_MINE) {
                pline("KABOOM! The cursed landmine explodes in your %s!",
                                            pobj->ocontainer ? xname(pobj->ocontainer) : "pack");
                setnotworn(pobj);
                delobj(pobj);
                explode(u.ux, u.uy, 11, d(3,6), TOOL_CLASS, EXPL_FIERY);
                return TRUE;
            } else if (pobj->otyp == PUMPKIN) {
                char where[BUFSIZ];
                int maxmons = 5+rnd(4);
                int mademons = 0;
                int i;

                if (pobj == uarmh) {
                    Helmet_off();
                    Sprintf(where, ", over your %s", body_part(FACE));
                } else {
                    Sprintf(where, " in your %s", pobj->ocontainer ? xname(pobj->ocontainer) : "pack");
                }
                
                for (i=0;i<1000;i++) {
                    struct permonst *pm;
                    int type = S_SPIDER;
                    switch(rnd(4)) {
                        case 1:
                            type = S_SPIDER;
                            break;
                        case 2:
                            type = S_WORM;
                            break;
                        case 3:
                            type = S_SNAKE;
                            break;
                        case 4:
                            type = S_ANT;
                            break;
                    }
                    pm = mkclass(type, 0);
                    if (pm) {
                        if (makemon(pm, u.ux, u.uy, NO_MM_FLAGS)) {
                            mademons++;
                        }
                    }
                    if (mademons >= maxmons) {
                        break;
                    }
                }
                
                if (!mademons) {
                    pline("The pumpkin instantly rots away%s!", where);
                } else {
                    pline("Horrible biting things fall from the rotting pumpkin%s!", where);
                }
                delobj(pobj);
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* Check for a change of staff */
void
staff_wield(void)
{
    if ((uwep) && (uwep->otyp == WIZARDSTAFF) && (Role_if(PM_WIZARD))) {
        if ((u.ustaff_id) && (u.ustaff_id != uwep->o_id)) {
            if (uwep->ovar1 > 0) {
                uwep->spe = 0;
                staff_set_en(uwep);
                pline("The new staff's energy dissipates as you attune to it.");
            }
        }
        staff_set_spe(uwep);
        u.ustaff_id = uwep->o_id;
    }
}

/* Returns the maximum energy a wizardstaff can hold.
 * 10 at level 1, 53 at level 15, 70 at level 30.
 **/
int
staff_max_en(void)
{
    if (u.ulevel < 17)
        return (7 + (u.ulevel * 3)) * STAFF_TURNS_PER_EN;
    else
        return (7 + (16 * 3) + 1 + (u.ulevel - 16)) * STAFF_TURNS_PER_EN;
}

/* Sets the + of a staff to be consistent with its
 * stored energy
 */
void
staff_set_spe(otmp)
struct obj *otmp;
{
    if ((otmp) && (otmp->otyp == WIZARDSTAFF))
        otmp->spe = otmp->ovar1 / (STAFF_EN_PER_SPE * STAFF_TURNS_PER_EN);
}

/* Sets the stored energy of a staff to be consistent
 * with its enchantment
 */
void
staff_set_en(otmp)
struct obj *otmp;
{
    if ((otmp) && (otmp->otyp == WIZARDSTAFF))
        otmp->ovar1 = otmp->spe * (STAFF_EN_PER_SPE * STAFF_TURNS_PER_EN);
}

/* Adjust the stored energy of a staff and set the
 * spe appropriately
 */
void
staff_adj_en(otmp, adj)
struct obj *otmp;
int adj;
{
    otmp->ovar1 += adj;
    if (otmp->ovar1 > staff_max_en())
        otmp->ovar1 = staff_max_en();
    staff_set_spe(otmp);
}

void
moveloop(resuming)
boolean resuming;
{
#if defined(MICRO) || defined(WIN32)
    char ch;
    int abort_lev;
#endif
    struct obj *pobj;
    int moveamt = 0, wtcap = 0, change = 0;
    boolean monscanmove = FALSE;

    /* Note:  these initializers don't do anything except guarantee that
            we're linked properly.
    */
    decl_init();
    monst_init();
    objects_init();

    /* if a save file created in normal mode is now being restored in
       explore mode, treat it as normal restore followed by 'X' command
       to use up the save file and require confirmation for explore mode */
    if (resuming && iflags.deferred_X)
        (void) enter_explore_mode();

    /* side-effects from the real world */
    flags.moonphase = phase_of_the_moon();
    if (flags.moonphase == FULL_MOON) {
        You("are lucky!  Full moon tonight.");
        change_luck(1);
    } else if (flags.moonphase == NEW_MOON) {
        pline("Be careful!  New moon tonight.");
    }
    flags.friday13 = friday_13th();
    if (flags.friday13) {
        pline("Watch out!  Bad things can happen on Friday the 13th.");
        change_luck(-1);
    }
    /* these are contained in elses in order to avoid spam */
    else if (pi_day()) {
        pline("Happy pi day!");
    } else if (mayfourth()) {
        pline("May the fourth be with you!");
    }

    if (!resuming) { /* new game */
        context.rndencode = rnd(9000);
        set_wear((struct obj *) 0); /* for side-effects of starting gear */
        (void) pickup(1);      /* autopickup at initial location */
    }
    context.botlx = TRUE; /* for STATUS_HILITES */
    update_inventory(); /* for perm_invent */
    if (resuming) { /* restoring old game */
        read_engr_at(u.ux, u.uy); /* subset of pickup() */
    }

    (void) encumber_msg(); /* in case they auto-picked up something */
    if (defer_see_monsters) {
        defer_see_monsters = FALSE;
        see_monsters();
    }
    initrack();

    u.uz0.dlevel = u.uz.dlevel;
    youmonst.movement = NORMAL_SPEED; /* give the hero some movement points */
    context.move = 0;

    program_state.in_moveloop = 1;

#ifdef WHEREIS_FILE
    touch_whereis();
#endif

    for (;;) {
#ifdef SAFERHANGUP
        if (program_state.done_hup)
            end_of_input();
#endif
        get_nh_event();
#ifdef POSITIONBAR
        do_positionbar();
#endif

        if (context.move) {
            /* actual time passed */
            if (u.uquick) {
                youmonst.movement -= NORMAL_SPEED / 2;
                u.uquick = FALSE;
            }
            else
                youmonst.movement -= NORMAL_SPEED;

            do { /* hero can't move this turn loop */
                wtcap = encumber_msg();

                context.mon_moving = TRUE;
                do {
                    monscanmove = movemon();
                    if (youmonst.movement >= NORMAL_SPEED)
                        break; /* it's now your turn */
                } while (monscanmove);
                context.mon_moving = FALSE;

                if (!monscanmove && youmonst.movement < NORMAL_SPEED) {
                    /* both hero and monsters are out of steam this round */
                    struct monst *mtmp;

                    /* set up for a new turn */
                    mcalcdistress(); /* adjust monsters' trap, blind, etc */

                    /* reallocate movement rations to monsters; don't need
                       to skip dead monsters here because they will have
                       been purged at end of their previous round of moving */
                    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
                        mtmp->movement += mcalcmove(mtmp);

                    /* occasionally add another monster; since this takes
                       place after movement has been allotted, the new
                       monster effectively loses its first turn */
                    if (!rn2((u.uevent.udemigod || u.uhave.amulet) ? 25
                             : (depth(&u.uz) > depth(&stronghold_level)) ? 50
                               : 70))
                        (void) makemon((struct permonst *) 0, 0, 0,
                                       NO_MM_FLAGS);

                    /* calculate how much time passed. */
                    if (u.usteed && u.umoved) {
                        /* your speed doesn't augment steed's speed */
                        moveamt = mcalcmove(u.usteed);
                    } else {
                        moveamt = youmonst.data->mmove;

                        if (Very_fast) { /* speed boots, potion, or spell */
                            /* gain a free action on 2/3 of turns */
                            if (rn2(3) != 0)
                                moveamt += NORMAL_SPEED;
                        } else if (Fast) { /* intrinsic */
                            /* gain a free action on 1/3 of turns */
                            if (rn2(3) == 0)
                                moveamt += NORMAL_SPEED;
                        }
                        if (tech_inuse(T_BLINK)) { /* TECH: Blinking! */
                  			    /* Case    Average  Variance
                  			     * -------------------------
                  			     * Normal    12         0
                  			     * Fast      16        12
                  			     * V fast    20        12
                  			     * Blinking  24        12
                  			     * F & B     28        18
                  			     * V F & B   30        18
                  			     */
                  			    moveamt += NORMAL_SPEED * 2 / 3;
                  			    if (rn2(3) == 0) moveamt += NORMAL_SPEED / 2;
                  			}
                    }

                    switch (wtcap) {
                    case UNENCUMBERED:
                        break;
                    case SLT_ENCUMBER:
                        moveamt -= (moveamt / 4);
                        break;
                    case MOD_ENCUMBER:
                        moveamt -= (moveamt / 2);
                        break;
                    case HVY_ENCUMBER:
                        moveamt -= ((moveamt * 3) / 4);
                        break;
                    case EXT_ENCUMBER:
                        moveamt -= ((moveamt * 7) / 8);
                        break;
                    default:
                        break;
                    }

                    youmonst.movement += moveamt;
                    if (youmonst.movement < 0)
                        youmonst.movement = 0;
                    settrack();

                    monstermoves++;
                    moves++;

                    /********************************/
                    /* once-per-turn things go here */
                    /********************************/
                    
                    /* Cursed land mines are a bad idea to carry around */
                    if (invent && !rn2(100)) {
                        for(pobj = invent; pobj; pobj = pobj->nobj) {
                            if (checklandmine(pobj)) break;
                        }
                    }

                    if (Glib)
                        glibr();
                    nh_timeout();
                    run_regions();

                    if (u.ublesscnt)
                        u.ublesscnt--;

                    /* Wearing a noncursed holy symbol increases the prayer timeout rate,
                     * by 25% - or 50% for Priests.
                     **/
                    if (uamul && (uamul->otyp == HOLY_SYMBOL) && (!uamul->cursed) &&
                        ((Role_if(PM_PRIEST)) ? (moves % 2) : (!(moves % 4))))
                            u.ublesscnt--;
                    u.ublesstim++;
                    if (flags.time && !context.run) {
                        context.botl = 1;
                        iflags.time_botl = TRUE;
                    }
#ifdef EXTRAINFO_FN
                    if ((prev_dgl_extrainfo == 0) || (prev_dgl_extrainfo < (moves + 250))) {
                        prev_dgl_extrainfo = moves;
                        mk_dgl_extrainfo();
                    }
#endif
                    if (u.ukinghill) {
                        if(u.protean > 0) u.protean--;
                        else{
                            for(pobj = invent; pobj; pobj=pobj->nobj)
                                if(pobj->oartifact == ART_TREASURY_OF_PROTEUS)
                                    break;
                            if(!pobj) pline("Treasury not actually in inventory??");
                            else if(pobj->cobj){
                                arti_poly_contents(pobj);
                            }
                            u.protean = rnz(100)+d(3,10);
                            update_inventory();
                        }
                    }
                    /* One possible result of prayer is healing.  Whether or
                     * not you get healed depends on your current hit points.
                     * If you are allowed to regenerate during the prayer,
                     * the end-of-prayer calculation messes up on this.
                     * Another possible result is rehumanization, which
                     * requires that encumbrance and movement rate be
                     * recalculated.
                     */
                    if (u.uinvulnerable) {
                        /* for the moment at least, you're in tiptop shape */
                        wtcap = UNENCUMBERED;
                    } else if (!Upolyd ? (u.uhp < u.uhpmax)
                                       : (u.mh < u.mhmax
                                          || youmonst.data->mlet == S_EEL)) {
                        /* maybe heal */
                        regen_hp(wtcap);
                    }

                    if (Withering && !Regeneration) {
                        losehp(1, "withering away", KILLED_BY);
                        context.botl = TRUE;
                        interrupt_multi("You are slowly withering away.");
                    }
                    if (!u.uinvulnerable && u.uen > 0 && u.uhp < u.uhpmax &&
                        tech_inuse(T_CHI_HEALING)) {
                        u.uen--;
                        u.uhp++;
                        context.botl = TRUE;
                    }
                    /* moving around while encumbered is hard work */
                    if (wtcap > MOD_ENCUMBER && u.umoved) {
                        if (!(wtcap < EXT_ENCUMBER ? moves % 30
                                                   : moves % 10)) {
                            if (Upolyd && u.mh > 1) {
                                u.mh--;
                                context.botl = TRUE;
                            } else if (!Upolyd && u.uhp > 1) {
                                u.uhp--;
                                context.botl = TRUE;
                            } else {
                                You("pass out from exertion!");
                                exercise(A_CON, FALSE);
                                fall_asleep(-10, FALSE);
                            }
                        }
                    }

                    if (u.uen < u.uenmax
                        && ((wtcap < MOD_ENCUMBER
                             && (!(moves % ((MAXULEV + 8 - u.ulevel)
                                            * (Role_if(PM_WIZARD) ? 3 : 4)
                                            / 6)))) || Energy_regeneration)) {
                        u.uen += rn1(
                            (int) (ACURR(A_WIS) + ACURR(A_INT)) / 15 + 1, 1);
                        if (u.uen > u.uenmax)
                            u.uen = u.uenmax;
                        context.botl = TRUE;
                        if (u.uen == u.uenmax)
                            interrupt_multi("You feel full of energy.");
                    }

                    if (u.uen >= u.uenmax) {
                        if ((uwep) && (Role_if(PM_WIZARD)) && (uwep->otyp == WIZARDSTAFF)) {
                            staff_adj_en(uwep,1);
                        }
                    }

                    if (!u.uinvulnerable) {
                        if (Teleportation && !rn2(85)) {
                            xchar old_ux = u.ux, old_uy = u.uy;

                            tele();
                            if (u.ux != old_ux || u.uy != old_uy) {
                                if (!next_to_u()) {
                                    check_leash(old_ux, old_uy);
                                }
                                /* clear doagain keystrokes */
                                pushch(0);
                                savech(0);
                            }
                        }
                        /* delayed change may not be valid anymore */
                        if ((change == 1 && !Polymorph)
                            || (change == 2 && u.ulycn == NON_PM))
                            change = 0;
                        if (Polymorph && !rn2(100))
                            change = 1;
                        else if (u.ulycn >= LOW_PM && !Upolyd
                                 && !rn2(80 - (20 * night())))
                            change = 2;
                        if (change && !Unchanging) {
                            if (multi >= 0) {
                                stop_occupation();
                                if (change == 1)
                                    polyself(0);
                                else
                                    you_were();
                                change = 0;
                            }
                        }
                    }

                    if (Searching && multi >= 0)
                        (void) dosearch0(1);
                    if (Warning)
                        warnreveal();
                    mkot_trap_warn();
                    dosounds();
                    do_storms();
                    gethungry();
                    age_spells();
                    exerchk();
                    invault();
                    if (u.uhave.amulet)
                        amulet();
                    if (!rn2(40 + (int) (ACURR(A_DEX) * 3)))
                        u_wipe_engr(rnd(3));
                    if (u.uevent.udemigod && !u.uinvulnerable) {
                        if (u.udg_cnt)
                            u.udg_cnt--;
                        if (!u.udg_cnt) {
                            intervene();
                            u.udg_cnt = rn1(200, 50);
                        }
                    }
                    /*
                    if (u.uevent.udemigod && !u.uinvulnerable &&
                        !In_endgame(&u.uz)) {
                        if (u.uin_cnt)
                            u.uin_cnt--;
                        if (!u.uin_cnt) {
                            dungeon_crumble();
                            u.uin_cnt = rn1(125, 50);
                        }
                    }
                    */
                    restore_attrib();
                    /* underwater and waterlevel vision are done here */
                    if (Is_waterlevel(&u.uz) || Is_airlevel(&u.uz))
                        movebubbles();
                    else if (Is_gemlevel(&u.uz) && !rn2(7))
                        do_earthquake(5, rn2(76), rn2(20));
                    else if (Is_firelevel(&u.uz))
                        fumaroles();
                    else if (Underwater)
                        under_water(0);
                    /* vision while buried done here */
                    else if (u.uburied)
                        under_ground(0);

                    /* when immobile, count is in turns */
                    if (multi < 0) {
                        if (++multi == 0) { /* finished yet? */
                            unmul((char *) 0);
                            /* if unmul caused a level change, take it now */
                            if (u.utotype)
                                deferred_goto();
                        }
                    }
                }
            } while (youmonst.movement < NORMAL_SPEED); /* hero can't move */

            /******************************************/
            /* once-per-hero-took-time things go here */
            /******************************************/

#ifdef STATUS_HILITES
            if (iflags.hilite_delta)
                status_eval_next_unhilite();
#endif
            if (context.bypasses)
                clear_bypasses();
            if ((u.uhave.amulet || Clairvoyant) && !In_endgame(&u.uz)
                && !BClairvoyant && !(moves % 15) && !rn2(2))
                do_vicinity_map((struct obj *) 0);
            if (u.utrap && u.utraptype == TT_LAVA)
                sink_into_lava();
            /* when/if hero escapes from lava, he can't just stay there */
            else if (!u.umoved)
                (void) pooleffects(FALSE);

        } /* actual time passed */

        /****************************************/
        /* once-per-player-input things go here */
        /****************************************/

        clear_splitobjs();
        find_ac();
        if (!context.mv || Blind) {
            /* redo monsters if hallu or wearing a helm of telepathy */
            if (Hallucination) { /* update screen randomly */
                see_monsters();
                see_objects();
                see_traps();
                if (u.uswallow)
                    swallowed(0);
            } else if (Unblind_telepat) {
                see_monsters();
            } else if (Warning || Warn_of_mon)
                see_monsters();

            if (vision_full_recalc)
                vision_recalc(0); /* vision! */
        }
        if (context.botl || context.botlx) {
            bot();
            curs_on_u();
        } else if (iflags.time_botl) {
            timebot();
            curs_on_u();
        }

        context.move = 1;

        if (multi >= 0 && occupation) {
#if defined(MICRO) || defined(WIN32)
            abort_lev = 0;
            if (kbhit()) {
                if ((ch = pgetchar()) == ABORT)
                    abort_lev++;
                else
                    pushch(ch);
            }
            if (!abort_lev && (*occupation)() == 0)
#else
            if ((*occupation)() == 0)
#endif
                occupation = 0;
            if (
#if defined(MICRO) || defined(WIN32)
                abort_lev ||
#endif
                monster_nearby()) {
                stop_occupation();
                reset_eat();
            }
#if defined(MICRO) || defined(WIN32)
            if (!(++occtime % 7))
                display_nhwindow(WIN_MAP, FALSE);
#endif
            continue;
        }

        if (iflags.sanity_check || iflags.debug_fuzzer)
            sanity_check();

#ifdef CLIPPING
        /* just before rhack */
        cliparound(u.ux, u.uy);
#endif

        u.umoved = FALSE;

        if (multi > 0) {
            lookaround();
            if (!multi) {
                /* lookaround may clear multi */
                context.move = 0;
                if (flags.time)
                    context.botl = TRUE;
                continue;
            }
            if (context.mv) {
                if (multi < COLNO && !--multi)
                    context.travel = context.travel1 = context.mv =
                        context.run = 0;
                domove();
            } else {
                --multi;
                rhack(save_cm);
            }
        } else if (multi == 0) {
#ifdef MAIL
            ckmailstatus();
#endif
            /* Update the holy symbol - as late as possible to ensure accuracy */
            holy_symbol();
            rhack((char *) 0);
        }
        if (u.utotype)       /* change dungeon level */
            deferred_goto(); /* after rhack() */
        /* !context.move here: multiple movement command stopped */
        else if (flags.time && (!context.move || !context.mv))
            context.botl = TRUE;

        if (vision_full_recalc)
            vision_recalc(0); /* vision! */
        /* when running in non-tport mode, this gets done through domove() */
        if ((!context.run || flags.runmode == RUN_TPORT)
            && (multi && (!context.travel ? !(multi % 7) : !(moves % 7L)))) {
            if (flags.time && context.run)
                context.botl = TRUE;
            /* [should this be flush_screen() instead?] */
            display_nhwindow(WIN_MAP, FALSE);
        }
    }
}

/* maybe recover some lost health (or lose some when an eel out of water) */
STATIC_OVL void
regen_hp(wtcap)
int wtcap;
{
    int heal = 0;
    boolean reached_full = FALSE,
            encumbrance_ok = (wtcap < MOD_ENCUMBER || !u.umoved);

    if (u.uroleplay.marathon)
        return;
    if (Upolyd) {
        if (u.mh < 1) { /* shouldn't happen... */
            rehumanize();
        } else if (youmonst.data->mlet == S_EEL
                   && !is_pool(u.ux, u.uy) && !Is_waterlevel(&u.uz)) {
            /* eel out of water loses hp, similar to monster eels;
               as hp gets lower, rate of further loss slows down */
            if (u.mh > 1 && !Regeneration && rn2(u.mh) > rn2(8)
                && (!Half_physical_damage || !(moves % 2L)))
                heal = -1;
        } else if (u.mh < u.mhmax) {
            if (Regeneration || (encumbrance_ok && !(moves % 20L)))
                heal = 1;
        }
        if (heal) {
            context.botl = TRUE;
            u.mh += heal;
            reached_full = (u.mh == u.mhmax);
        }

    /* !Upolyd */
    } else {
        /* [when this code was in-line within moveloop(), there was
           no !Upolyd check here, so poly'd hero recovered lost u.uhp
           once u.mh reached u.mhmax; that may have been convenient
           for the player, but it didn't make sense for gameplay...] */
        if (u.uhp < u.uhpmax && (encumbrance_ok || Regeneration) && !Withering) {
            if (u.ulevel > 9) {
                if (!(moves % 3L)) {
                    int Con = (int) ACURR(A_CON);

                    if (Con <= 12) {
                        heal = 1;
                    } else {
                        heal = rnd(Con);
                        if (heal > u.ulevel - 9)
                            heal = u.ulevel - 9;
                    }
                }
            } else { /* u.ulevel <= 9 */
                if (!(moves % (long) ((MAXULEV + 12) / (u.ulevel + 2) + 1)))
                    heal = 1;
            }
            if (Regeneration && !Withering && !heal)
                heal = 1;

            if (heal) {
                context.botl = TRUE;
                u.uhp += heal;
                if (u.uhp > u.uhpmax)
                    u.uhp = u.uhpmax;
                /* stop voluntary multi-turn activity if now fully healed */
                reached_full = (u.uhp == u.uhpmax);
            }
        }
    }

    if (reached_full)
        interrupt_multi("You are in full health.");
}

void
stop_occupation()
{
    if (occupation) {
        if (!maybe_finished_meal(TRUE))
            You("stop %s.", occtxt);
        occupation = 0;
        context.botl = TRUE; /* in case u.uhs changed */
        nomul(0);
        pushch(0);
    } else if (multi >= 0) {
        nomul(0);
    }
}

void
display_gamewindows()
{
    WIN_MESSAGE = create_nhwindow(NHW_MESSAGE);
    if (VIA_WINDOWPORT()) {
        status_initialize(0);
    } else {
        WIN_STATUS = create_nhwindow(NHW_STATUS);
        status_initialize(0);
    }
    WIN_MAP = create_nhwindow(NHW_MAP);
    WIN_INVEN = create_nhwindow(NHW_MENU);
    /* in case of early quit where WIN_INVEN could be destroyed before
       ever having been used, use it here to pacify the Qt interface */
    start_menu(WIN_INVEN), end_menu(WIN_INVEN, (char *) 0);

#ifdef MAC
    /* This _is_ the right place for this - maybe we will
     * have to split display_gamewindows into create_gamewindows
     * and show_gamewindows to get rid of this ifdef...
     */
    if (!strcmp(windowprocs.name, "mac"))
        SanePositions();
#endif

    /*
     * The mac port is not DEPENDENT on the order of these
     * displays, but it looks a lot better this way...
     */
#ifndef STATUS_HILITES
    display_nhwindow(WIN_STATUS, FALSE);
#endif
    display_nhwindow(WIN_MESSAGE, FALSE);
    clear_glyph_buffer();
    display_nhwindow(WIN_MAP, FALSE);
}

void
newgame()
{
    int i;

#ifdef MFLOPPY
    gameDiskPrompt();
#endif

    context.botlx = TRUE;
    context.ident = 1;
    context.stethoscope_move = -1L;
    context.warnlevel = 1;
    context.next_attrib_check = 600L; /* arbitrary first setting */
    context.tribute.enabled = TRUE;   /* turn on 3.6 tributes    */
    context.tribute.tributesz = sizeof(struct tribute_info);

    for (i = LOW_PM; i < NUMMONS; i++)
        mvitals[i].mvflags = mons[i].geno & G_NOCORPSE;

    init_objects(); /* must be before u_init() */

    flags.pantheon = -1; /* role_init() will reset this */
    role_init();         /* must be before init_dungeons(), u_init(),
                          * and init_artifacts() */

    init_dungeons();  /* must be before u_init() to avoid rndmonst()
                       * creating odd monsters for any tins and eggs
                       * in hero's initial inventory */
    init_artifacts(); /* before u_init() in case $WIZKIT specifies
                       * any artifacts */
    u_init();

#ifndef NO_SIGNAL
    (void) signal(SIGINT, (SIG_RET_TYPE) done1);
#endif
#ifdef NEWS
    if (iflags.news)
        display_file(NEWS, FALSE);
#endif
    load_qtlist();          /* load up the quest text info */
    /* quest_init();  --  Now part of role_init() */

    mklev();
    u_on_upstairs();
    if (wizard)
        obj_delivery(FALSE); /* finish wizkit */
    vision_reset();          /* set up internals for level (after mklev) */
    check_special_room(FALSE);

    if (MON_AT(u.ux, u.uy))
        mnexto(m_at(u.ux, u.uy));
    (void) makedog();
    docrt();

    if (Role_if(PM_CONVICT)) {
        setworn(mkobj(CHAIN_CLASS, TRUE), W_CHAIN, TRUE);
        setworn(mkobj(BALL_CLASS, TRUE), W_BALL, TRUE);
        uball->spe = 1;
        placebc();
        newsym(u.ux,u.uy);
    }

    if (flags.legacy) {
        flush_screen(1);
        if (Role_if(PM_CONVICT)) {
		    com_pager(199);
        } else {
		    com_pager(1);
        }
    }

    urealtime.realtime = 0L;
    urealtime.start_timing = getnow();
#ifdef INSURANCE
    save_currentstate();
#endif
    program_state.something_worth_saving++; /* useful data now exists */

    /* Success! */
    welcome(TRUE);
    return;
}

/* show "welcome [back] to nethack" message at program startup */
void
welcome(new_game)
boolean new_game; /* false => restoring an old game */
{
    char buf[BUFSZ];
    char tipbuf[BUFSZ];
    char ebuf[BUFSZ];
    char racebuf[BUFSZ];
    char rolebuf[BUFSZ];
    int currentgend = Upolyd ? u.ugender : flags.gender;

    /* skip "welcome back" if restoring a doomed character */
    if (!new_game && Upolyd && ugenocided()) {
        /* death via self-genocide is pending */
        pline("You're back, but you still feel %s inside.", udeadinside());
        return;
    }
    if (new_game && u.uroleplay.marathon) {
        u.uhp = 999;
        u.uhpmax = 999;
    }
    if (new_game) {
        (void) random_engraving(ebuf, FALSE);
        memcpy(explengr, ebuf, BUFSZ);
        /* Sprintf(flags.explengr, "%s", ebuf);*/
    }

    /*
     * The "welcome back" message always describes your innate form
     * even when polymorphed or wearing a helm of opposite alignment.
     * Alignment is shown unconditionally for new games; for restores
     * it's only shown if it has changed from its original value.
     * Sex is shown for new games except when it is redundant; for
     * restores it's only shown if different from its original value.
     */
    *buf = '\0';
    if (new_game || u.ualignbase[A_ORIGINAL] != u.ualignbase[A_CURRENT])
        Sprintf(eos(buf), " %s", align_str(u.ualignbase[A_ORIGINAL]));
    if (!urole.name.f
        && (new_game
                ? (urole.allow & ROLE_GENDMASK) == (ROLE_MALE | ROLE_FEMALE)
                : currentgend != flags.initgend))
        Sprintf(eos(buf), " %s", genders[currentgend].adj);

    pline(new_game ? "%s %s, welcome to SliceHack!  You are a%s %s %s %s."
                   : "%s %s, the%s %s %s %s, welcome back to SliceHack!",
          Hello((struct monst *) 0), plname, buf, genders[currentgend].adj, urace.adj,
            rolename_gender(currentgend));
    if (flags.tips) {
        if (new_game) {
            /* display race info */
            Sprintf(racebuf, "race %s", urace.noun);
            checkfile(racebuf, 0, TRUE, TRUE, (char *) 0);
            /* display role info */
            Sprintf(rolebuf, "role %s", urole.filecode);
            checkfile(rolebuf, 0, TRUE, TRUE, (char *) 0);
        }
        /* Display tip of the day */
        get_rnd_text(SLICETIPSFILE, tipbuf, rn2_on_display_rng);
        pline("Slicehack Tip of the Day: %s", tipbuf);
    }
}

#ifdef POSITIONBAR
STATIC_DCL void
do_positionbar()
{
    static char pbar[COLNO];
    char *p;

    p = pbar;
    /* up stairway */
    if (upstair.sx
        && (glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph)
                == S_upstair
            || glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph)
                   == S_upladder)) {
        *p++ = '<';
        *p++ = upstair.sx;
    }
    if (sstairs.sx
        && (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                == S_upstair
            || glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                   == S_upladder)) {
        *p++ = '<';
        *p++ = sstairs.sx;
    }

    /* down stairway */
    if (dnstair.sx
        && (glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph)
                == S_dnstair
            || glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph)
                   == S_dnladder)) {
        *p++ = '>';
        *p++ = dnstair.sx;
    }
    if (sstairs.sx
        && (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                == S_dnstair
            || glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                   == S_dnladder)) {
        *p++ = '>';
        *p++ = sstairs.sx;
    }

    /* hero location */
    if (u.ux) {
        *p++ = '@';
        *p++ = u.ux;
    }
    /* fence post */
    *p = 0;

    update_positionbar(pbar);
}
#endif

STATIC_DCL void
interrupt_multi(msg)
const char *msg;
{
    if (multi > 0 && !context.travel && !context.run) {
        nomul(0);
        if (flags.verbose && msg)
            Norep("%s", msg);
    }
}

/*
 * Argument processing helpers - for xxmain() to share
 * and call.
 *
 * These should return TRUE if the argument matched,
 * whether the processing of the argument was
 * successful or not.
 *
 * Most of these do their thing, then after returning
 * to xxmain(), the code exits without starting a game.
 *
 */

static struct early_opt earlyopts[] = {
    {ARG_DEBUG, "debug", 5, TRUE},
    {ARG_VERSION, "version", 4, TRUE},
    {ARG_SHOWPATHS, "showpaths", 9, FALSE},
#ifdef WIN32
    {ARG_WINDOWS, "windows", 4, TRUE},
#endif
};

#ifdef WIN32
extern int FDECL(windows_early_options, (const char *));
#endif

/*
 * Returns:
 *    0 = no match
 *    1 = found and skip past this argument
 *    2 = found and trigger immediate exit
 */

int
argcheck(argc, argv, e_arg)
int argc;
char *argv[];
enum earlyarg e_arg;
{
    int i, idx;
    boolean match = FALSE;
    char *userea = (char *)0;
    const char *dashdash = "";

    for (idx = 0; idx < SIZE(earlyopts); idx++) {
        if (earlyopts[idx].e == e_arg)
            break;
    }
    if ((idx >= SIZE(earlyopts)) || (argc <= 1))
            return FALSE;

    for (i = 0; i < argc; ++i) {
        if (argv[i][0] != '-')
            continue;
        if (argv[i][1] == '-') {
            userea = &argv[i][2];
            dashdash = "-";
        } else {
            userea = &argv[i][1];
        }
        match = match_optname(userea, earlyopts[idx].name,
                              earlyopts[idx].minlength,
                              earlyopts[idx].valallowed);
        if (match) break;
    }

    if (match) {
        const char *extended_opt = index(userea, ':');

        if (!extended_opt)
            extended_opt = index(userea, '=');
        switch(e_arg) {
        case ARG_DEBUG:
            if (extended_opt) {
                extended_opt++;
                debug_fields(extended_opt);
            }
            return 1;
        case ARG_VERSION: {
            boolean insert_into_pastebuf = FALSE;

            if (extended_opt) {
                extended_opt++;
                if (match_optname(extended_opt, "paste", 5, FALSE)) {
                    insert_into_pastebuf = TRUE;
                } else {
                    raw_printf(
                   "-%sversion can only be extended with -%sversion:paste.\n",
                               dashdash, dashdash);
                    return TRUE;
                }
            }
            early_version_info(insert_into_pastebuf);
            return 2;
        }
        case ARG_SHOWPATHS: {
            return 2;
        }
#ifdef WIN32
        case ARG_WINDOWS: {
            if (extended_opt) {
                extended_opt++;
                return windows_early_options(extended_opt);
            }
        }
#endif
        default:
            break;
        }
    };
    return FALSE;
}

/*
 * These are internal controls to aid developers with
 * testing and debugging particular aspects of the code.
 * They are not player options and the only place they
 * are documented is right here. No gameplay is altered.
 *
 * test             - test whether this parser is working
 * ttystatus        - TTY:
 * immediateflips   - WIN32: turn off display performance
 *                    optimization so that display output
 *                    can be debugged without buffering.
 */
STATIC_OVL void
debug_fields(opts)
const char *opts;
{
    char *op;
    boolean negated = FALSE;

    while ((op = index(opts, ',')) != 0) {
        *op++ = 0;
        /* recurse */
        debug_fields(op);
    }
    if (strlen(opts) > BUFSZ / 2)
        return;


    /* strip leading and trailing white space */
    while (isspace((uchar) *opts))
        opts++;
    op = eos((char *) opts);
    while (--op >= opts && isspace((uchar) *op))
        *op = '\0';

    if (!*opts) {
        /* empty */
        return;
    }
    while ((*opts == '!') || !strncmpi(opts, "no", 2)) {
        if (*opts == '!')
            opts++;
        else
            opts += 2;
        negated = !negated;
    }
    if (match_optname(opts, "test", 4, FALSE))
        iflags.debug.test = negated ? FALSE : TRUE;
#ifdef TTY_GRAPHICS
    if (match_optname(opts, "ttystatus", 9, FALSE))
        iflags.debug.ttystatus = negated ? FALSE : TRUE;
#endif
#ifdef WIN32
    if (match_optname(opts, "immediateflips", 14, FALSE))
        iflags.debug.immediateflips = negated ? FALSE : TRUE;
#endif
    return;
}
/*allmain.c*/

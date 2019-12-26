/* NetHack 3.6	potion.c	$NHDT-Date: 1573848199 2019/11/15 20:03:19 $  $NHDT-Branch: NetHack-3.6 $:$NHDT-Revision: 1.167 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2013. */
/* NetHack may be freely redistributed.  See license for details. */

/* Edited on 5/11/18 by NullCGT */

#include "hack.h"
#include <assert.h>

boolean notonhead = FALSE;

static NEARDATA int nothing, unkn;
static NEARDATA const char dippables[] = { POTION_CLASS, 0 };
static NEARDATA const char beverages[] = { TOOL_CLASS, POTION_CLASS, 0 };

STATIC_DCL long FDECL(itimeout, (long));
STATIC_DCL long FDECL(itimeout_incr, (long, int));
STATIC_DCL void NDECL(ghost_from_bottle);
STATIC_DCL boolean
FDECL(H2Opotion_dip, (struct obj *, struct obj *, BOOLEAN_P, const char *));
STATIC_DCL short FDECL(mixtype, (struct obj *, struct obj *));

/* force `val' to be within valid range for intrinsic timeout value */
STATIC_OVL long
itimeout(val)
long val;
{
    if (val >= TIMEOUT)
        val = TIMEOUT;
    else if (val < 1)
        val = 0;

    return val;
}

/* increment `old' by `incr' and force result to be valid intrinsic timeout */
STATIC_OVL long
itimeout_incr(old, incr)
long old;
int incr;
{
    return itimeout((old & TIMEOUT) + (long) incr);
}

/* set the timeout field of intrinsic `which' */
void
set_itimeout(which, val)
long *which, val;
{
    *which &= ~TIMEOUT;
    *which |= itimeout(val);
}

/* increment the timeout field of intrinsic `which' */
void
incr_itimeout(which, incr)
long *which;
int incr;
{
    set_itimeout(which, itimeout_incr(*which, incr));
}

void
make_confused(xtime, talk)
long xtime;
boolean talk;
{
    long old = HConfusion;

    if (Unaware)
        talk = FALSE;

    if (!xtime && old) {
        if (talk)
            You_feel("less %s now.", Hallucination ? "trippy" : "confused");
    }
    if ((xtime && !old) || (!xtime && old))
        context.botl = TRUE;

    set_itimeout(&HConfusion, xtime);
}

void
make_stunned(xtime, talk)
long xtime;
boolean talk;
{
    long old = HStun;

    if (Unaware)
        talk = FALSE;

    if (!xtime && old) {
        if (talk)
            You_feel("%s now.",
                     Hallucination ? "less wobbly" : "a bit steadier");
    }
    if (xtime && !old) {
        if (talk) {
            if (u.usteed)
                You("wobble in the saddle.");
            else
                You("%s...", stagger(youmonst.data, "stagger"));
        }
    }
    if ((!xtime && old) || (xtime && !old))
        context.botl = TRUE;

    set_itimeout(&HStun, xtime);
}

/* Sick is overloaded with both fatal illness and food poisoning (via
   u.usick_type bit mask), but delayed killer can only support one or
   the other at a time.  They should become separate intrinsics.... */
void
make_sick(xtime, cause, talk, type)
long xtime;
const char *cause; /* sickness cause */
boolean talk;
int type;
{
    struct kinfo *kptr;
    long old = Sick;

#if 0   /* tell player even if hero is unconscious */
    if (Unaware)
        talk = FALSE;
#endif
    if (xtime > 0L) {
        int copperarmor = 0;
        struct obj* otmp;
        if (Sick_resistance)
            return;

        /* Copper's anti-microbial properties make it effective in warding off
         * sickness. */
        for (otmp = invent; otmp; otmp = otmp->nobj) {
            if ((otmp->owornmask & W_ARMOR) && otmp->material == COPPER) {
                copperarmor++;
            }
        }
        if (rn2(5) < copperarmor) {
            /* practially, someone could have copper helm, boots, body armor,
             * shield, gloves. If they're *all* copper, you're immune to
             * sickness. */
            You_feel("briefly ill.");
            return;
        }

        if (!old) {
            /* newly sick */
            You_feel("deathly sick.");
        } else {
            /* already sick */
            if (talk)
                You_feel("%s worse.", xtime <= Sick / 2L ? "much" : "even");
        }
        set_itimeout(&Sick, xtime);
        u.usick_type |= type;
        context.botl = TRUE;
    } else if (old && (type & u.usick_type)) {
        /* was sick, now not */
        u.usick_type &= ~type;
        if (u.usick_type) { /* only partly cured */
            if (talk)
                You_feel("somewhat better.");
            set_itimeout(&Sick, Sick * 2); /* approximation */
        } else {
            if (talk)
                You_feel("cured.  What a relief!");
            Sick = 0L; /* set_itimeout(&Sick, 0L) */
        }
        context.botl = TRUE;
    }

    kptr = find_delayed_killer(SICK);
    if (Sick) {
        exercise(A_CON, FALSE);
        /* setting delayed_killer used to be unconditional, but that's
           not right when make_sick(0) is called to cure food poisoning
           if hero was also fatally ill; this is only approximate */
        if (xtime || !old || !kptr) {
            int kpfx = ((cause && !strcmp(cause, "#wizintrinsic"))
                        ? KILLED_BY : KILLED_BY_AN);

            delayed_killer(SICK, kpfx, cause);
        }
    } else
        dealloc_killer(kptr);
}

void
make_carrier(xtime, talk)
long xtime;
boolean talk;
{
    long old = LarvaCarrier;

    if (Unaware)
        talk = FALSE;

    set_itimeout(&LarvaCarrier, xtime);
    context.botl = TRUE;
    if (!xtime && old)
        if (talk)
            You_feel("much more yourself.");
}

void
make_slimed(xtime, msg)
long xtime;
const char *msg;
{
    long old = Slimed;

#if 0   /* tell player even if hero is unconscious */
    if (Unaware)
        msg = 0;
#endif
    set_itimeout(&Slimed, xtime);
    if ((xtime != 0L) ^ (old != 0L)) {
        context.botl = TRUE;
        if (msg)
            pline("%s", msg);
    }
    if (!Slimed) {
        dealloc_killer(find_delayed_killer(SLIMED));
        /* fake appearance is set late in turn-to-slime countdown */
        if (U_AP_TYPE == M_AP_MONSTER
            && youmonst.mappearance == PM_GREEN_SLIME) {
            youmonst.m_ap_type = M_AP_NOTHING;
            youmonst.mappearance = 0;
        }
    }
}

/* start or stop petrification */
void
make_stoned(xtime, msg, killedby, killername)
long xtime;
const char *msg;
int killedby;
const char *killername;
{
    long old = Stoned;

#if 0   /* tell player even if hero is unconscious */
    if (Unaware)
        msg = 0;
#endif
    set_itimeout(&Stoned, xtime);
    if ((xtime != 0L) ^ (old != 0L)) {
        context.botl = TRUE;
        if (msg)
            pline("%s", msg);
    }
    if (!Stoned)
        dealloc_killer(find_delayed_killer(STONED));
    else if (!old)
        delayed_killer(STONED, killedby, killername);
}

void
make_vomiting(xtime, talk)
long xtime;
boolean talk;
{
    long old = Vomiting;

    if (Unaware)
        talk = FALSE;

    set_itimeout(&Vomiting, xtime);
    context.botl = TRUE;
    if (!xtime && old)
        if (talk)
            You_feel("much less nauseated now.");
}

static const char vismsg[] = "vision seems to %s for a moment but is %s now.";
static const char eyemsg[] = "%s momentarily %s.";

void
make_blinded(xtime, talk)
long xtime;
boolean talk;
{
    long old = Blinded;
    boolean u_could_see, can_see_now;
    const char *eyes;

    /* we need to probe ahead in case the Eyes of the Overworld
       are or will be overriding blindness */
    u_could_see = !Blind;
    Blinded = xtime ? 1L : 0L;
    can_see_now = !Blind;
    Blinded = old; /* restore */

    if (Unaware)
        talk = FALSE;

    if (can_see_now && !u_could_see) { /* regaining sight */
        if (talk) {
            if (Hallucination)
                pline("Far out!  Everything is all cosmic again!");
            else
                You("can see again.");
        }
    } else if (old && !xtime) {
        /* clearing temporary blindness without toggling blindness */
        if (talk) {
            if (!haseyes(youmonst.data)) {
                strange_feeling((struct obj *) 0, (char *) 0);
            } else if (Blindfolded) {
                eyes = body_part(EYE);
                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your(eyemsg, eyes, vtense(eyes, "itch"));
            } else { /* Eyes of the Overworld */
                Your(vismsg, "brighten", Hallucination ? "sadder" : "normal");
            }
        }
    }

    if (u_could_see && !can_see_now) { /* losing sight */
        if (talk) {
            if (Hallucination)
                pline("Oh, bummer!  Everything is dark!  Help!");
            else
                pline("A cloud of darkness falls upon you.");
        }
        /* Before the hero goes blind, set the ball&chain variables. */
        if (Punished)
            set_bc(0);
    } else if (!old && xtime) {
        /* setting temporary blindness without toggling blindness */
        if (talk) {
            if (!haseyes(youmonst.data)) {
                strange_feeling((struct obj *) 0, (char *) 0);
            } else if (Blindfolded) {
                eyes = body_part(EYE);
                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your(eyemsg, eyes, vtense(eyes, "twitch"));
            } else { /* Eyes of the Overworld */
                Your(vismsg, "dim", Hallucination ? "happier" : "normal");
            }
        }
    }

    set_itimeout(&Blinded, xtime);

    if (u_could_see ^ can_see_now) { /* one or the other but not both */
        toggle_blindness();
    }
}

/* blindness has just started or just ended--caller enforces that;
   called by Blindf_on(), Blindf_off(), and make_blinded() */
void
toggle_blindness()
{
    boolean Stinging = (uwep && (EWarn_of_mon & W_WEP) != 0L);

    /* blindness has just been toggled */
    context.botl = TRUE; /* status conditions need update */
    vision_full_recalc = 1; /* vision has changed */
    /* this vision recalculation used to be deferred until moveloop(),
       but that made it possible for vision irregularities to occur
       (cited case was force bolt hitting an adjacent potion of blindness
       and then a secret door; hero was blinded by vapors but then got the
       message "a door appears in the wall" because wall spot was IN_SIGHT) */
    vision_recalc(0);
    if (Blind_telepat || Infravision || Stinging)
        see_monsters(); /* also counts EWarn_of_mon monsters */
    /*
     * Avoid either of the sequences
     * "Sting starts glowing", [become blind], "Sting stops quivering" or
     * "Sting starts quivering", [regain sight], "Sting stops glowing"
     * by giving "Sting is quivering" when becoming blind or
     * "Sting is glowing" when regaining sight so that the eventual
     * "stops" message matches the most recent "Sting is ..." one.
     */
    if (Stinging)
        Sting_effects(-1);
    /* update dknown flag for inventory picked up while blind */
    if (!Blind)
        learn_unseen_invent();
}

boolean
make_hallucinated(xtime, talk, mask)
long xtime; /* nonzero if this is an attempt to turn on hallucination */
boolean talk;
long mask; /* nonzero if resistance status should change by mask */
{
    long old = HHallucination;
    boolean changed = 0;
    const char *message, *verb;

    if (Unaware)
        talk = FALSE;

    message = (!xtime) ? "Everything %s SO boring now."
                       : "Oh wow!  Everything %s so cosmic!";
    verb = (!Blind) ? "looks" : "feels";

    if (mask) {
        if (HHallucination)
            changed = TRUE;

        if (!xtime)
            EHalluc_resistance |= mask;
        else
            EHalluc_resistance &= ~mask;
    } else {
        if (!EHalluc_resistance && (!!HHallucination != !!xtime))
            changed = TRUE;
        set_itimeout(&HHallucination, xtime);

        /* clearing temporary hallucination without toggling vision */
        if (!changed && !HHallucination && old && talk) {
            if (!haseyes(youmonst.data)) {
                strange_feeling((struct obj *) 0, (char *) 0);
            } else if (Blind) {
                const char *eyes = body_part(EYE);

                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your(eyemsg, eyes, vtense(eyes, "itch"));
            } else { /* Grayswandir */
                Your(vismsg, "flatten", "normal");
            }
        }
    }

    if (changed) {
        /* in case we're mimicking an orange (hallucinatory form
           of mimicking gold) update the mimicking's-over message */
        if (!Hallucination)
            eatmupdate();

        if (u.uswallow) {
            swallowed(0); /* redraw swallow display */
        } else {
            /* The see_* routines should be called *before* the pline. */
            see_monsters();
            see_objects();
            see_traps();
        }

        /* for perm_inv and anything similar
        (eg. Qt windowport's equipped items display) */
        update_inventory();

        context.botl = TRUE;
        if (talk)
            pline(message, verb);
    }
    return changed;
}

void
make_deaf(xtime, talk)
long xtime;
boolean talk;
{
    long old = HDeaf;

    if (Unaware)
        talk = FALSE;

    set_itimeout(&HDeaf, xtime);
    if ((xtime != 0L) ^ (old != 0L)) {
        context.botl = TRUE;
        if (talk)
            You(old ? "can hear again." : "are unable to hear anything.");
    }
}

/* set or clear "slippery fingers" */
void
make_glib(xtime)
int xtime;
{
    set_itimeout(&Glib, xtime);
    /* may change "(being worn)" to "(being worn; slippery)" or vice versa */
    if (uarmg)
        update_inventory();
}

void
self_invis_message()
{
    if(Role_if(PM_PIRATE)){
       	pline("%s %s.",
       	    Hallucination ? "Arr, Matey!  Ye" : "Avast!  All of a sudden, ye",
       	    See_invisible ? "can see right through yerself" :
       		"can't see yerself");
       	}
    else{
        pline("%s %s.",
              Hallucination ? "Far out, man!  You"
                            : "Gee!  All of a sudden, you",
              See_invisible ? "can see right through yourself"
                            : "can't see yourself");
    }
}

/* KMH, balance patch -- idea by Dylan O'Donnell <dylanw@demon.net>
 * The poor hacker's polypile.  This includes weapons, armor, and tools.
 * To maintain balance, magical categories (amulets, scrolls, spellbooks,
 * potions, rings, and wands) should NOT be supported.
 * Polearms are not currently implemented.
 */
int
upgrade_obj(obj)
register struct obj *obj;
/* returns 1 if something happened (potion should be used up)
 * returns 0 if nothing happened
 * returns -1 if object exploded (potion should be used up)
 */
{
	int chg, otyp = obj->otyp;
  short otyp2;
	xchar ox, oy;
	long owornmask;
	struct obj *otmp;
	boolean explodes;

	/* Check to see if object is valid */
	if (!obj)
		return 0;
	/* (void)snuff_lit(obj);*/
	if (obj->oartifact)
		/* WAC -- Could have some funky fx */
		return 0;
	switch (obj->otyp)
	{
		/* weapons */
		case ORCISH_DAGGER:
    case ELVEN_DAGGER:
			obj->otyp = DAGGER;
			break;
		case DAGGER:
        if (!rn2(2)) obj->otyp = ELVEN_DAGGER;
        else obj->otyp = DARK_ELVEN_DAGGER;
        break;
		case KNIFE:
			obj->otyp = STILETTO;
			break;
		case STILETTO:
			obj->otyp = KNIFE;
			break;
		case AXE:
			obj->otyp = BATTLE_AXE;
			break;
		case BATTLE_AXE:
			obj->otyp = AXE;
			break;
		case PICK_AXE:
			obj->otyp = DWARVISH_MATTOCK;
			break;
		case DWARVISH_MATTOCK:
			obj->otyp = PICK_AXE;
			break;
		case ORCISH_SHORT_SWORD:
			obj->otyp = SHORT_SWORD;
			break;
		case ELVEN_SHORT_SWORD:
    case DARK_ELVEN_SHORT_SWORD:
		case SHORT_SWORD:
			obj->otyp = DWARVISH_SHORT_SWORD;
			break;
		case DWARVISH_SHORT_SWORD:
        if (!rn2(2)) obj->otyp = ELVEN_SHORT_SWORD;
        else obj->otyp = DARK_ELVEN_SHORT_SWORD;
        break;
		case BROADSWORD:
			obj->otyp = ELVEN_BROADSWORD;
			break;
		case ELVEN_BROADSWORD:
			obj->otyp = BROADSWORD;
			break;
		case CLUB:
			obj->otyp = AKLYS;
			break;
		case AKLYS:
			obj->otyp = CLUB;
			break;
		case ELVEN_BOW:
    case DARK_ELVEN_BOW:
		case YUMI:
		case ORCISH_BOW:
			obj->otyp = BOW;
			break;
		case BOW:
      switch (rn2(3)) {
        case 0: obj->otyp = ELVEN_BOW; break;
        case 1: obj->otyp = DARK_ELVEN_BOW; break;
        case 2: obj->otyp = YUMI; break;
      }
      break;
        case LIGHT_ARROW:
            obj->otyp = ARROW;
            break;
		case ELVEN_ARROW:
        case DARK_ELVEN_ARROW:
		case YA:
		case ORCISH_ARROW:
			obj->otyp = LIGHT_ARROW;
			break;
		case ARROW:
      switch (rn2(3)) {
        case 0: obj->otyp = ELVEN_ARROW; break;
        case 1: obj->otyp = DARK_ELVEN_ARROW; break;
        case 2: obj->otyp = YA; break;
      }
			break;
		/* armour */
    case ELVEN_RING_MAIL:
      obj->otyp = DARK_ELVEN_RING_MAIL;
      break;
    case DARK_ELVEN_RING_MAIL:
			obj->otyp = ELVEN_RING_MAIL;
			break;
		case ORCISH_RING_MAIL:
			obj->otyp = RING_MAIL;
			break;
		case CHAIN_MAIL:
			obj->otyp = ORCISH_RING_MAIL;
			break;
		case STUDDED_ARMOR:
		case JACKET:
			obj->otyp = LIGHT_ARMOR;
			break;
		case LIGHT_ARMOR:
			obj->otyp = STUDDED_ARMOR;
			break;
		/* robes */
		/* cloaks */
		case CLOAK_OF_PROTECTION:
		case CLOAK_OF_INVISIBILITY:
		case CLOAK_OF_MAGIC_RESISTANCE:
		case CLOAK_OF_DISPLACEMENT:
    case CLOAK_OF_FLIGHT:
    case CLOAK_OF_REFLECTION:
		case DWARVISH_CLOAK:
		case ORCISH_CLOAK:
			if (!rn2(2)) obj->otyp = OILSKIN_CLOAK;
			else obj->otyp = ELVEN_CLOAK;
			break;
		case OILSKIN_CLOAK:
		case ELVEN_CLOAK:
			switch (rn2(6)) {
				case 0: obj->otyp = CLOAK_OF_PROTECTION; break;
				case 1: obj->otyp = CLOAK_OF_INVISIBILITY; break;
				case 2: obj->otyp = CLOAK_OF_MAGIC_RESISTANCE; break;
				case 3: obj->otyp = CLOAK_OF_DISPLACEMENT; break;
        case 4: obj->otyp = CLOAK_OF_FLIGHT; break;
        case 5: obj->otyp = CLOAK_OF_REFLECTION; break;
			}
			break;
		/* helms */
		case FEDORA:
			obj->otyp = ELVEN_HELM;
			break;
		case ELVEN_HELM:
			obj->otyp = FEDORA;
			break;
		case DENTED_POT:
			obj->otyp = ORCISH_HELM;
			break;
		case ORCISH_HELM:
		case HELM_OF_BRILLIANCE:
		case HELM_OF_TELEPATHY:
    case HELM_OF_OPAQUE_THOUGHTS:
			obj->otyp = DWARVISH_HELM;
			break;
		case DWARVISH_HELM:
      switch(rn2(3)) {
        case 0: obj->otyp = HELM_OF_BRILLIANCE; break;
        case 1: obj->otyp = HELM_OF_TELEPATHY; break;
        case 2: obj->otyp = HELM_OF_OPAQUE_THOUGHTS; break;
      }
			break;
		case CORNUTHAUM:
			obj->otyp = DUNCE_CAP;
			break;
		case DUNCE_CAP:
			obj->otyp = CORNUTHAUM;
			break;
		/* gloves */
		case GAUNTLETS:
		case GLOVES:
      switch(rn2(3)) {
        case 0: obj->otyp = GAUNTLETS_OF_DEXTERITY; break;
        case 1: obj->otyp = BOXING_GLOVES; break;
        case 2: obj->otyp = ROGUES_GLOVES; break;
      }
			break;
		case GAUNTLETS_OF_DEXTERITY:
    case BOXING_GLOVES:
    case ROGUES_GLOVES:
			obj->otyp = GLOVES;
			break;
		/* shields */
		case ELVEN_SHIELD:
			if (!rn2(2)) obj->otyp = URUK_HAI_SHIELD;
			else obj->otyp = ORCISH_SHIELD;
			break;
		case URUK_HAI_SHIELD:
		case ORCISH_SHIELD:
			obj->otyp = ELVEN_SHIELD;
			break;
		case DWARVISH_ROUNDSHIELD:
			obj->otyp = LARGE_SHIELD;
			break;
		case LARGE_SHIELD:
			obj->otyp = DWARVISH_ROUNDSHIELD;
			break;
		/* boots */
		case LOW_BOOTS:
			obj->otyp = HIGH_BOOTS;
			break;
		case HIGH_BOOTS:
			obj->otyp = LOW_BOOTS;
			break;
		/* NOTE:  Supposedly,  HIGH_BOOTS should upgrade to any of the
			other magic leather boots (except for fumble).  IRON_SHOES
			should upgrade to the iron magic boots,  unless
			the iron magic boots are fumble */
		/* rings,  amulets */
		case LARGE_BOX:
        case COFFIN:
		case ICE_BOX:
        case KEG:
            /* Technically, this allows for a strat in which you upgrade your
               keg to obtain a large number of potions of booze. This is silly
               enough that I'm going to allow it. */
			obj->otyp = CHEST;
			break;
		case CHEST:
			obj->otyp = ICE_BOX;
			break;
		case SACK:
			obj->otyp = rn2(5) ? OILSKIN_SACK : BAG_OF_HOLDING;
			break;
		case OILSKIN_SACK:
			obj->otyp = BAG_OF_HOLDING;
			break;
		case BAG_OF_HOLDING:
			obj->otyp = OILSKIN_SACK;
			break;
        case EARMUFFS:
		case TOWEL:
			obj->otyp = BLINDFOLD;
			break;
		case BLINDFOLD:
			obj->otyp = TOWEL;
			break;
		case CREDIT_CARD:
		case LOCK_PICK:
			obj->otyp = SKELETON_KEY;
			break;
		case SKELETON_KEY:
			obj->otyp = LOCK_PICK;
			break;
		case TALLOW_CANDLE:
			obj->otyp = WAX_CANDLE;
			break;
		case WAX_CANDLE:
			obj->otyp = TALLOW_CANDLE;
			break;
		case OIL_LAMP:
			obj->otyp = LANTERN;
			break;
		case LANTERN:
			obj->otyp = OIL_LAMP;
			break;
		case PEA_WHISTLE:
			obj->otyp = MAGIC_WHISTLE;
			break;
		case MAGIC_WHISTLE:
			obj->otyp = PEA_WHISTLE;
			break;
		case FLUTE:
			obj->otyp = MAGIC_FLUTE;
			obj->spe = rn1(5,10);
			break;
		case MAGIC_FLUTE:
			obj->otyp = FLUTE;
			break;
		case TOOLED_HORN:
			obj->otyp = rn1(HORN_OF_PLENTY - TOOLED_HORN, FROST_HORN);
			obj->spe = rn1(5,10);
			obj->known = 0;
			break;
		case HORN_OF_PLENTY:
		case FIRE_HORN:
		case FROST_HORN:
			obj->otyp = TOOLED_HORN;
			break;
		case HARP:
			obj->otyp = MAGIC_HARP;
			obj->spe = rn1(5,10);
			obj->known = 0;
			break;
		case MAGIC_HARP:
			obj->otyp = HARP;
			break;
		case LEASH:
			obj->otyp = SADDLE;
			break;
		case SADDLE:
			obj->otyp = LEASH;
			break;
		case TIN_OPENER:
			obj->otyp = TINNING_KIT;
			obj->spe = rn1(30,70);
			obj->known = 0;
			break;
		case TINNING_KIT:
			obj->otyp = TIN_OPENER;
			break;
		case CRYSTAL_BALL:
			/* "ball-point pen" */
			obj->otyp = MAGIC_MARKER;
			/* Keep the charges (crystal ball usually less than marker) */
			break;
		case MAGIC_MARKER:
			obj->otyp = CRYSTAL_BALL;
			chg = rn1(10,3);
			if (obj->spe > chg)
				obj->spe = chg;
			obj->known = 0;
			break;
		case K_RATION:
		case C_RATION:
		case LEMBAS_WAFER:
			if (!rn2(2)) obj->otyp = CRAM_RATION;
			else obj->otyp = FOOD_RATION;
			break;
		case FOOD_RATION:
		case CRAM_RATION:
			obj->otyp = LEMBAS_WAFER;
			break;
		case LOADSTONE:
			obj->otyp = FLINT;
			break;
		case FLINT:
			obj->otyp = LUCKSTONE;
			break;
		default:
			/* This object is not upgradable */
			return 0;
	}

	if ((!carried(obj) || obj->unpaid) &&
		get_obj_location(obj, &ox, &oy, BURIED_TOO|CONTAINED_TOO) &&
		costly_spot(ox, oy)) {
	    char objroom = *in_rooms(ox, oy, SHOPBASE);
	    register struct monst *shkp = shop_keeper(objroom);

	    if ((!obj->no_charge ||
		 (Has_contents(obj) &&
		    (contained_cost(obj, shkp, 0L, FALSE, FALSE) != 0L)))
	       && inhishop(shkp)) {
		if(shkp->mpeaceful) {
		    if(*u.ushops && *in_rooms(u.ux, u.uy, 0) ==
			    *in_rooms(shkp->mx, shkp->my, 0) &&
			    !costly_spot(u.ux, u.uy))
			make_angry_shk(shkp, ox, oy);
		    else {
			pline("%s gets angry!", Monnam(shkp));
			hot_pursuit(shkp);
		    }
		} else Norep("%s is furious!", Monnam(shkp));
		otyp2 = obj->otyp;
		obj->otyp = otyp;
		/*
		 * [ALI] When unpaid containers are upgraded, the
		 * old container is billed as a dummy object, but
		 * it's contents are unaffected and will remain
		 * either unpaid or not as appropriate.
		 */
		otmp = obj->cobj;
		obj->cobj = NULL;
		if (costly_spot(u.ux, u.uy) && objroom == *u.ushops)
		    bill_dummy_object(obj);
		else
		    (void) stolen_value(obj, ox, oy, FALSE, FALSE);
		obj->otyp = otyp2;
		obj->cobj = otmp;
	    }
	}

	/* The object was transformed */
	obj->owt = weight(obj);
	obj->oclass = objects[obj->otyp].oc_class;
	if (!objects[obj->otyp].oc_uses_known)
	    obj->known = 1;

	if (carried(obj)) {
	    if (obj == uskin) rehumanize();
	    /* Quietly remove worn item if no longer compatible --ALI */
	    owornmask = obj->owornmask;
	    if (owornmask & W_ARM && !is_suit(obj))
		owornmask &= ~W_ARM;
	    if (owornmask & W_ARMC && !is_cloak(obj))
		owornmask &= ~W_ARMC;
	    if (owornmask & W_ARMH && !is_helmet(obj))
		owornmask &= ~W_ARMH;
	    if (owornmask & W_ARMS && !is_shield(obj))
		owornmask &= ~W_ARMS;
	    if (owornmask & W_ARMG && !is_gloves(obj))
		owornmask &= ~W_ARMG;
	    if (owornmask & W_ARMF && !is_boots(obj))
		owornmask &= ~W_ARMF;
	    if (owornmask & W_ARMU && !is_shirt(obj))
		owornmask &= ~W_ARMU;
	    if (owornmask & W_TOOL && obj->otyp != BLINDFOLD &&
	      obj->otyp != TOWEL && obj->otyp != LENSES && obj->otyp != EARMUFFS)
		owornmask &= ~W_TOOL;
	    otyp2 = obj->otyp;
	    obj->otyp = otyp;
	    if (obj->otyp == LEASH && obj->leashmon) o_unleash(obj);
	    remove_worn_item(obj, TRUE);
	    obj->otyp = otyp2;
	    obj->owornmask = owornmask;
	    setworn(obj, obj->owornmask, FALSE);
      #if 0 /* I believe this is now handled with update_inventory - agulp */
	    puton_worn_item(obj);
      #endif
	}

	if (obj->otyp == BAG_OF_HOLDING && Has_contents(obj)) {
	    explodes = FALSE;

	    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
		if (mbag_explodes(otmp, 0)) {
		    explodes = TRUE;
		    break;
		}

            if (explodes) {
		pline("As you upgrade your bag, you are blasted by a magical explosion!");
		delete_contents(obj);
		if (carried(obj))
		    useup(obj);
		else
		    useupf(obj, obj->quan);
		losehp(d(6,6), "magical explosion", KILLED_BY_AN);
		return -1;
	    }
	}
	return 1;
}

STATIC_OVL void
ghost_from_bottle()
{
    struct monst *mtmp = makemon(&mons[PM_GHOST], u.ux, u.uy, NO_MM_FLAGS);

    if (!mtmp) {
        pline("This bottle turns out to be empty.");
        return;
    }
    if (Blind) {
        pline("As you open the bottle, %s emerges.", something);
        return;
    }
    pline("As you open the bottle, an enormous %s emerges!",
          Hallucination ? rndmonnam(NULL) : (const char *) "ghost");
    if (flags.verbose)
        You("are frightened to death, and unable to move.");
    nomul(-3);
    multi_reason = "being frightened to death";
    nomovemsg = "You regain your composure.";
}

/* "Quaffing is like drinking, except you spill more." - Terry Pratchett */
int
dodrink()
{
    register struct obj *otmp;
    const char *potion_descr;

    if (Strangled) {
        pline("If you can't breathe air, how can you drink liquid?");
        return 0;
    }
    /* Is there a fountain to drink from here? */
    if (IS_FOUNTAIN(levl[u.ux][u.uy].typ)
        /* not as low as floor level but similar restrictions apply */
        && can_reach_floor(FALSE)) {
        if (yn("Drink from the fountain? Are you sure?") == 'y') {
            drinkfountain();
            return 1;
        }
    }
    /* Or a kitchen sink? */
    if (IS_SINK(levl[u.ux][u.uy].typ)
        /* not as low as floor level but similar restrictions apply */
        && can_reach_floor(FALSE)) {
        if (yn("Drink from the sink?") == 'y') {
            drinksink();
            return 1;
        }
    }
    /* Or a furnace? */
    if (IS_FURNACE(levl[u.ux][u.uy].typ)
        /* not as low as floor level but similar restrictions apply */
        && can_reach_floor(FALSE)) {
        if (yn("Drink from the furnace?") == 'y') {
            drinkfurnace();
            return 1;
        }
    }
    /* Or are you surrounded by water? */
    if (Underwater && !u.uswallow) {
        if (yn("Drink the water around you?") == 'y') {
            pline("Do you know what lives in this water?");
            return 1;
        }
    }

    otmp = getobj(beverages, "drink");
    if (!otmp)
        return 0;

    /* quan > 1 used to be left to useup(), but we need to force
       the current potion to be unworn, and don't want to do
       that for the entire stack when starting with more than 1.
       [Drinking a wielded potion of polymorph can trigger a shape
       change which causes hero's weapon to be dropped.  In 3.4.x,
       that led to an "object lost" panic since subsequent useup()
       was no longer dealing with an inventory item.  Unwearing
       the current potion is intended to keep it in inventory.] */
    if (otmp->quan > 1L) {
        otmp = splitobj(otmp, 1L);
        otmp->owornmask = 0L; /* rest of original stuck unaffected */
    } else if (otmp->owornmask) {
        remove_worn_item(otmp, FALSE);
    }
    otmp->in_use = TRUE; /* you've opened the stopper */

    if (otmp->otyp == KEG) {
        if (yn("Really drink the entire keg at once?") == 'n') {
            pline("Perhaps not.");
            return 0;
        }
        if (otmp->cobj) {
            struct obj *obj;
            int quan = 0;
            u.uconduct.alcohol++;
            while (otmp->cobj) {
                quan++;
                for (obj = otmp->cobj; obj; obj = obj->nobj) {
                    if (obj->otyp == POT_BOOZE)
                        break;
                }
                check_unpaid(obj);
                if (obj->quan > 1L) {
                    obj->quan--;
                    otmp->owt = weight(otmp);
                } else {
                    obj_extract_self(obj);
                    obfree(obj, (struct obj *)0);
                }
                if (!obj->odiluted)
                    healup(1, 0, FALSE, FALSE);
                if (!obj->blessed)
                    make_confused(itimeout_incr(HConfusion, d(3, 8)), FALSE);
                u.uhunger += 10 * (2 + bcsign(obj));
            }
            You("down the entire keg! You are incredibly drunk!");
            if (quan > 5 && !maybe_polyd(is_dwarf(youmonst.data) || is_giant(youmonst.data), 
                Race_if(PM_DWARF))) {
                u.uhp = 0;
                losehp(1, "alcohol poisoning", KILLED_BY);
            }
        } else {
            pline("Unfortunately, your keg is dry as a desert.");
            return 0;
        }
        return 1;
    } else if (otmp->oclass != POTION_CLASS) {
        pline(silly_thing_to, "drink");
        return 0;
    }

    potion_descr = OBJ_DESCR(objects[otmp->otyp]);
    if (potion_descr) {
        if (!strcmp(potion_descr, "milky")
            && !(mvitals[PM_GHOST].mvflags & G_GONE)
            && !rn2(POTION_OCCUPANT_CHANCE(mvitals[PM_GHOST].born))) {
            ghost_from_bottle();
            useup(otmp);
            return 1;
        } else if (!strcmp(potion_descr, "smoky")
                   && !(mvitals[PM_DJINNI].mvflags & G_GONE)
                   && !rn2(POTION_OCCUPANT_CHANCE(mvitals[PM_DJINNI].born))) {
            djinni_from_bottle(otmp);
            useup(otmp);
            return 1;
        } else {
            /* It's possible for the potion to slip out of your hands.
             * In this case it will hit the floor and possibly smash - dependent on Luck.
             * The potion has a chance of slipping in any of the following situations:
             *
             * It is cursed (for the same reason that throwing a cursed knife might slip)
             * It is greased (for completeness - there's no good reason why you would...)
             * You have slippery fingers
             * You are Fumbling or impaired
             * You have very low Dexterity
             *
             * The chances combine if more than one applies.
             * Blessing a potion helps protect it from slipping, though.
             * So does DEX > 8.
             *
             * The numbers are intended to make DEX of 18 reduce the chance of failure due
             * to a curse or single impairment by several times (to 1%) but not to help much
             * against multiple impairments or glib/fumbling/greased.
             */
             int chance = 0;
             if (otmp->cursed) chance += 60;
             if (otmp->blessed) chance -= 25;
             if (otmp->greased) chance += 250; /* muppet */
             if (Glib) chance += 250;
             if (Fumbling) chance += 250;
             if (Blind) chance += 85;
             if (Confusion) chance += 85;
             if (Stunned) chance += 85;
             if (Hallucination) chance += 85;
             switch(ACURR(A_DEX)) {
                case 3:
                    chance += 100;
                    break;
                case 4:
                    chance += 50;
                    break;
                case 5:
                    chance += 20;
                    break;
                default:
                    chance -= (5 * (ACURR(A_DEX) - 8));
            }
            if (chance > 950) chance = 950;
            if (rn2(1000) < chance) {
                /* Butter fingers */
                pline("The %s%s slips from your %s%s!",
                    (otmp->greased) ? "greased " : "",
                    bottlename(),
                    (Glib) ? "slippery " : (Fumbling ? "fumbling " : ""),
                    makeplural(body_part(FINGER)));
                otmp->in_use = FALSE;

                extract_nobj(otmp, &invent);
                mayhitfloor(otmp, (rn2(50) > Luck+20), u.ux, u.uy, TRUE);
                return 1;
            }
        }
    }
    return dopotion(otmp);
}

int
dopotion(otmp)
register struct obj *otmp;
{
    int retval;

    otmp->in_use = TRUE;
    nothing = unkn = 0;
    if ((retval = peffects(otmp)) >= 0)
        return retval;

    if (nothing) {
        unkn++;
        You("have a %s feeling for a moment, then it passes.",
            Hallucination ? "normal" : "peculiar");
    }
    if (otmp->dknown && !objects[otmp->otyp].oc_name_known) {
        if (!unkn) {
            makeknown(otmp->otyp);
            more_experienced(0, 0, 10);
        } else if (!objects[otmp->otyp].oc_uname)
            docall(otmp);
    }
    useup(otmp);
    return 1;
}

int
peffects(otmp)
register struct obj *otmp;
{
    register int i, ii, lim;

    switch (otmp->otyp) {
    case POT_RESTORE_ABILITY:
    case SPE_RESTORE_ABILITY:
    case SCR_ENCHANT_ARMOR:
        unkn++;
        if (otmp->cursed) {
            pline("Ulch!  This makes you feel mediocre!");
            break;
        } else {
            /* unlike unicorn horn, overrides Fixed_abil */
            if (otmp->otyp == SCR_ENCHANT_ARMOR) {
                pline("You feel %s!", (otmp->blessed)
                          ? (unfixable_trouble_count(TRUE) ? "better" : "as good as new")
                          : "good");
            } else {
                pline("Wow!  This makes you feel %s!",
                      (otmp->blessed)
                          ? (unfixable_trouble_count(FALSE) ? "better" : "great")
                          : "good");
            }
            i = rn2(A_MAX); /* start at a random point */
            for (ii = 0; ii < A_MAX; ii++) {
                lim = AMAX(i);
                /* this used to adjust 'lim' for A_STR when u.uhs was
                   WEAK or worse, but that's handled via ATEMP(A_STR) now */
                if (ABASE(i) < lim) {
                    ABASE(i) = lim;
                    context.botl = 1;
                    /* only first found if not blessed */
                    if (!otmp->blessed)
                        break;
                }
                if (++i >= A_MAX)
                    i = 0;
            }

            /* when using the potion (not the spell) also restore lost levels,
               to make the potion more worth keeping around for players with
               the spell or with a unihorn; this is better than full healing
               in that it can restore all of them, not just half, and a
               blessed potion restores them all at once */

            if (otmp->otyp == POT_RESTORE_ABILITY && u.ulevel < u.ulevelmax) {
                do {
                    pluslvl(FALSE);
                } while (u.ulevel < u.ulevelmax && otmp->blessed);
            }
        }
        break;
    case POT_HALLUCINATION:
        if (Hallucination || Halluc_resistance)
            nothing++;
        (void) make_hallucinated(
            itimeout_incr(HHallucination, rn1(200, 600 - 300 * bcsign(otmp))),
            TRUE, 0L);
        if ((otmp->blessed && !rn2(2)) || (!otmp->cursed && !rn2(5))) {
            You("see a vision...");
            display_nhwindow(WIN_MESSAGE, FALSE);
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
            pline_The("vision fades.");
            exercise(A_WIS, TRUE);
        }
        break;
    case POT_WATER:
        if (!otmp->blessed && !otmp->cursed) {
            pline("This tastes like %s.", hliquid("water"));
            u.uhunger += rnd(10);
            newuhs(FALSE);
            break;
        }
        unkn++;
        if (is_undead(youmonst.data) || is_demon(youmonst.data)
            || Race_if(PM_GHOUL) || u.ualign.type == A_CHAOTIC) {
            if (otmp->blessed) {
                pline("This burns like %s!", hliquid("acid"));
                exercise(A_CON, FALSE);
                if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF)) {
                    Your("affinity to %s disappears!",
                         makeplural(mons[u.ulycn].mname));
                    if (youmonst.data == &mons[u.ulycn])
                        you_unwere(FALSE);
                    set_ulycn(NON_PM); /* cure lycanthropy */
                }
                losehp(Maybe_Half_Phys(d(2, 6)), "potion of holy water",
                       KILLED_BY_AN);
            } else if (otmp->cursed) {
                You_feel("quite proud of yourself.");
                healup(d(2, 6), 0, 0, 0);
                if (u.ulycn >= LOW_PM && !Upolyd)
                    you_were();
                exercise(A_CON, TRUE);
            }
        } else {
            if (otmp->blessed) {
                You_feel("full of awe.");
                make_sick(0L, (char *) 0, TRUE, SICK_ALL);
                exercise(A_WIS, TRUE);
                exercise(A_CON, TRUE);
                if (u.ulycn >= LOW_PM  && !Race_if(PM_HUMAN_WEREWOLF))
                    you_unwere(TRUE); /* "Purified" */
                /* make_confused(0L, TRUE); */
            } else {
                if (u.ualign.type == A_LAWFUL) {
                    pline("This burns like %s!", hliquid("acid"));
                    losehp(Maybe_Half_Phys(d(2, 6)), "potion of unholy water",
                           KILLED_BY_AN);
                } else
                    You_feel("full of dread.");
                if (u.ulycn >= LOW_PM && !Upolyd)
                    you_were();
                exercise(A_CON, FALSE);
            }
        }
        break;
    case POT_BOOZE:
        u.uconduct.alcohol++;
        unkn++;
        pline("Ooph!  This tastes like %s%s!",
              otmp->odiluted ? "watered down " : "",
              Hallucination ? "dandelion wine" : "liquid fire");
        if (!otmp->blessed)
            make_confused(itimeout_incr(HConfusion, d(3, 8)), FALSE);
        /* the whiskey makes us feel better */
        if (!otmp->odiluted)
            healup(1, 0, FALSE, FALSE);
        u.uhunger += 10 * (2 + bcsign(otmp));
        newuhs(FALSE);
        exercise(A_WIS, FALSE);
        if (otmp->cursed) {
            You("pass out.");
            multi = -rnd(15);
            nomovemsg = "You awake with a headache.";
        }
        break;
    case POT_ENLIGHTENMENT:
        if (otmp->cursed) {
            unkn++;
            You("have an uneasy feeling...");
            exercise(A_WIS, FALSE);
        } else {
            if (otmp->blessed) {
                (void) adjattrib(A_INT, 1, FALSE);
                (void) adjattrib(A_WIS, 1, FALSE);
            }
            You_feel("self-knowledgeable...");
            display_nhwindow(WIN_MESSAGE, FALSE);
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
            pline_The("feeling subsides.");
            exercise(A_WIS, TRUE);
        }
        break;
    case SPE_INVISIBILITY:
        /* spell cannot penetrate mummy wrapping */
        if (BInvis && uarmc->otyp == MUMMY_WRAPPING) {
            You_feel("rather itchy under %s.", yname(uarmc));
            break;
        }
        /* FALLTHRU */
    case POT_INVISIBILITY:
        if (Invis || Blind || BInvis) {
            nothing++;
        } else {
            self_invis_message();
        }
        if (otmp->blessed)
            HInvis |= FROMOUTSIDE;
        else
            incr_itimeout(&HInvis, rn1(15, 31));
        newsym(u.ux, u.uy); /* update position */
        if (otmp->cursed) {
            pline("For some reason, you feel your presence is known.");
            aggravate();
        }
        break;
    case POT_SEE_INVISIBLE: /* tastes like fruit juice in Rogue */
    case POT_FRUIT_JUICE: {
        int msg = Invisible && !Blind;

        unkn++;
        if (otmp->cursed)
            pline("Yecch!  This tastes %s.",
                  Hallucination ? "overripe" : "rotten");
        else
            pline(
                Hallucination
                    ? "This tastes like 10%% real %s%s all-natural beverage."
                    : "This tastes like %s%s.",
                otmp->odiluted ? "reconstituted " : "", fruitname(TRUE));
        if (otmp->otyp == POT_FRUIT_JUICE) {
            u.uhunger += (otmp->odiluted ? 5 : 10) * (2 + bcsign(otmp));
            newuhs(FALSE);
            break;
        }
        if (!otmp->cursed) {
            /* Tell them they can see again immediately, which
             * will help them identify the potion...
             */
            make_blinded(0L, TRUE);
        }
        if (otmp->blessed)
            HSee_invisible |= FROMOUTSIDE;
        else
            incr_itimeout(&HSee_invisible, rn1(100, 750));
        set_mimic_blocking(); /* do special mimic handling */
        see_monsters();       /* see invisible monsters */
        newsym(u.ux, u.uy);   /* see yourself! */
        if (msg && !Blind) {  /* Blind possible if polymorphed */
            You("can see through yourself, but you are visible!");
            unkn--;
        }
        break;
    }
    case POT_PARALYSIS:
        if (Free_action) {
            You("stiffen momentarily.");
        } else {
            if (Levitation || Is_airlevel(&u.uz) || Is_waterlevel(&u.uz))
                You("are motionlessly suspended.");
            else if (u.usteed)
                You("are frozen in place!");
            else
                Your("%s are frozen to the %s!", makeplural(body_part(FOOT)),
                     surface(u.ux, u.uy));
            nomul(-(rn1(10, 25 - 12 * bcsign(otmp))));
            multi_reason = "frozen by a potion";
            nomovemsg = You_can_move_again;
            exercise(A_DEX, FALSE);
        }
        break;
    case POT_SLEEPING:
        if (Sleep_resistance || Free_action) {
            You("yawn.");
        } else {
            You("suddenly fall asleep!");
            fall_asleep(-rn1(10, 25 - 12 * bcsign(otmp)), TRUE);
        }
        break;
    case POT_MONSTER_DETECTION:
    case SPE_DETECT_MONSTERS:
        if (otmp->blessed) {
            int x, y;

            if (Detect_monsters)
                nothing++;
            unkn++;
            /* after a while, repeated uses become less effective */
            if ((HDetect_monsters & TIMEOUT) >= 300L)
                i = 1;
            else
                i = rn1(40, 21);
            incr_itimeout(&HDetect_monsters, i);
            for (x = 1; x < COLNO; x++) {
                for (y = 0; y < ROWNO; y++) {
                    if (levl[x][y].glyph == GLYPH_INVISIBLE) {
                        unmap_object(x, y);
                        newsym(x, y);
                    }
                    if (MON_AT(x, y))
                        unkn = 0;
                }
            }
            see_monsters();
            if (unkn)
                You_feel("lonely.");
            break;
        }
        if (monster_detect(otmp, 0))
            return 1; /* nothing detected */
        exercise(A_WIS, TRUE);
        break;
    case POT_OBJECT_DETECTION:
    case SPE_DETECT_TREASURE:
        if (object_detect(otmp, 0))
            return 1; /* nothing detected */
        exercise(A_WIS, TRUE);
        break;
    case POT_SICKNESS:
        pline("Yecch!  This stuff tastes like poison.");
        if (otmp->blessed) {
            pline("(But in fact it was mildly stale %s.)", fruitname(TRUE));
            if (!Role_if(PM_HEALER)) {
                /* NB: blessed otmp->fromsink is not possible */
                losehp(1, "mildly contaminated potion", KILLED_BY_AN);
            }
        } else {
            if (Poison_resistance)
                pline("(But in fact it was biologically contaminated %s.)",
                      fruitname(TRUE));
            if (Role_if(PM_HEALER)) {
                pline("Fortunately, you have been immunized.");
            } else {
                char contaminant[BUFSZ];
                int typ = rn2(A_MAX);

                Sprintf(contaminant, "%s%s",
                        (Poison_resistance) ? "mildly " : "",
                        (otmp->fromsink) ? "contaminated tap water"
                                         : "contaminated potion");
                if (!Fixed_abil) {
                    poisontell(typ, FALSE);
                    (void) adjattrib(typ, Poison_resistance ? -1 : -rn1(4, 3),
                                     1);
                }
                if (!Poison_resistance) {
                    if (otmp->fromsink)
                        losehp(rnd(10) + 5 * !!(otmp->cursed), contaminant,
                               KILLED_BY);
                    else
                        losehp(rnd(10) + 5 * !!(otmp->cursed), contaminant,
                               KILLED_BY_AN);
                } else {
                    /* rnd loss is so that unblessed poorer than blessed */
                    losehp(1 + rn2(2), contaminant,
                           (otmp->fromsink) ? KILLED_BY : KILLED_BY_AN);
                }
                exercise(A_CON, FALSE);
            }
        }
        if (Hallucination) {
            You("are shocked back to your senses!");
            u.uroleplay.hallu = FALSE;
            (void) make_hallucinated(0L, FALSE, 0L);
        }
        break;
    case POT_CONFUSION:
        if (!Confusion) {
            if (Hallucination) {
                pline("What a trippy feeling!");
                unkn++;
            } else
                pline("Huh, What?  Where am I?");
        } else
            nothing++;
        make_confused(itimeout_incr(HConfusion,
                                    rn1(7, 16 - 8 * bcsign(otmp))),
                      FALSE);
        break;
    case POT_GAIN_ABILITY:
        if (otmp->cursed) {
            pline("Ulch!  That potion tasted foul!");
            unkn++;
        } else if (Fixed_abil) {
            nothing++;
        } else {      /* If blessed, increase all; if not, try up to */
            int itmp; /* 6 times to find one which can be increased. */

            i = -1;   /* increment to 0 */
            for (ii = A_MAX; ii > 0; ii--) {
                i = (otmp->blessed ? i + 1 : rn2(A_MAX));
                /* only give "your X is already as high as it can get"
                   message on last attempt (except blessed potions) */
                itmp = (otmp->blessed || ii == 1) ? 0 : -1;
                if (i == A_STR) {
                    if (gainstr(otmp, 1, (itmp == 0)) && !otmp->blessed)
                        break;
                }
                else {
                    if (adjattrib(i, 1, itmp) && !otmp->blessed)
                        break;
                }
            }
        }
        break;
    case POT_REFLECTION:
        if (otmp->cursed) {
            pline("It\'s like drinking glue!");
            unkn++;
        } else {
            pline("You are covered in a mirror-like sheen!");
            if (otmp->blessed) {
                set_itimeout(&HReflecting, rn1(50, 250));
            } else {
                set_itimeout(&HReflecting, rn1(10, 20));
            }
        }
        break;
    case POT_REGENERATION:
        if (otmp->cursed) {
            You("begin to wither away!");
            incr_itimeout(&HWithering, rn1(10, 20));
            unkn++;
            context.botl = TRUE;
        } else {
            You("metabolism kicks into overdrive!");
            if (otmp->blessed) {
                set_itimeout(&HRegeneration, rn1(100, 100));
            } else {
                set_itimeout(&HRegeneration, rn1(50, 50));
            }
        }
        break;
    case POT_SPEED:
        /* skip when mounted; heal_legs() would heal steed's legs */
        if (Wounded_legs && !otmp->cursed && !u.usteed) {
            heal_legs(0);
            unkn++;
            break;
        }
        /* FALLTHRU */
    case SPE_HASTE_SELF:
        if (!Very_fast) { /* wwf@doe.carleton.ca */
            You("are suddenly moving %sfaster.", Fast ? "" : "much ");
        } else {
            Your("%s get new energy.", makeplural(body_part(LEG)));
            unkn++;
        }
        exercise(A_DEX, TRUE);
        incr_itimeout(&HFast, rn1(10, 100 + 60 * bcsign(otmp)));
        break;
    case POT_BLINDNESS:
        if (Blind)
            nothing++;
        make_blinded(itimeout_incr(Blinded,
                                   rn1(200, 250 - 125 * bcsign(otmp))),
                     (boolean) !Blind);
        break;
    case POT_GAIN_LEVEL:
        if (otmp->cursed) {
            unkn++;
            /* they went up a level */
            if ((ledger_no(&u.uz) == 1 && u.uhave.amulet)
                || Can_rise_up(u.ux, u.uy, &u.uz)) {
                const char *riseup = "rise up, through the %s!";

                if (ledger_no(&u.uz) == 1) {
                    You(riseup, ceiling(u.ux, u.uy));
                    goto_level(&earth_level, FALSE, FALSE, FALSE, FALSE);
                } else {
                    register int newlev = depth(&u.uz) - 1;
                    d_level newlevel;

                    get_level(&newlevel, newlev);
                    if (on_level(&newlevel, &u.uz)) {
                        pline("It tasted bad.");
                        break;
                    } else
                        You(riseup, ceiling(u.ux, u.uy));
                    goto_level(&newlevel, FALSE, FALSE, FALSE, FALSE);
                }
            } else
                You("have an uneasy feeling.");
            break;
        }
        pluslvl(FALSE);
        /* blessed potions place you at a random spot in the
           middle of the new level instead of the low point */
        if (otmp->blessed)
            u.uexp = rndexp(TRUE);
        break;
    case POT_HEALING:
        You_feel("%s.", u.uroleplay.marathon ? "refreshed" : "better");
        healup(d(6 + 2 * bcsign(otmp), 4), !otmp->cursed ? 1 : 0,
               !!otmp->blessed, !otmp->cursed);
        exercise(A_CON, TRUE);
        break;
    case POT_EXTRA_HEALING:
        You_feel("much %s.", u.uroleplay.marathon ? "more refreshed" : "better");
        healup(d(6 + 2 * bcsign(otmp), 8),
               otmp->blessed ? 5 : !otmp->cursed ? 2 : 0, !otmp->cursed,
               TRUE);
        (void) make_hallucinated(0L, TRUE, 0L);
        exercise(A_CON, TRUE);
        exercise(A_STR, TRUE);
        break;
    case POT_FULL_HEALING:
        You_feel("completely %s.", u.uroleplay.marathon ? "refreshed" : "healed");
        healup(400, 4 + 4 * bcsign(otmp), !otmp->cursed, TRUE);
        /* Restore one lost level if blessed */
        if (otmp->blessed && u.ulevel < u.ulevelmax) {
            /* when multiple levels have been lost, drinking
               multiple potions will only get half of them back */
            u.ulevelmax -= 1;
            pluslvl(FALSE);
        }
        (void) make_hallucinated(0L, TRUE, 0L);
        exercise(A_STR, TRUE);
        exercise(A_CON, TRUE);
        break;
    case POT_LEVITATION:
    case SPE_LEVITATION:
        /*
         * BLevitation will be set if levitation is blocked due to being
         * inside rock (currently or formerly in phazing xorn form, perhaps)
         * but it doesn't prevent setting or incrementing Levitation timeout
         * (which will take effect after escaping from the rock if it hasn't
         * expired by then).
         */
        if (!Levitation && !BLevitation) {
            /* kludge to ensure proper operation of float_up() */
            set_itimeout(&HLevitation, 1L);
            float_up();
            /* This used to set timeout back to 0, then increment it below
               for blessed and uncursed effects.  But now we leave it so
               that cursed effect yields "you float down" on next turn.
               Blessed and uncursed get one extra turn duration. */
        } else /* already levitating, or can't levitate */
            nothing++;

        if (otmp->cursed) {
            /* 'already levitating' used to block the cursed effect(s)
               aside from ~I_SPECIAL; it was not clear whether that was
               intentional; either way, it no longer does (as of 3.6.1) */
            HLevitation &= ~I_SPECIAL; /* can't descend upon demand */
            if (BLevitation) {
                ; /* rising via levitation is blocked */
            } else if ((u.ux == xupstair && u.uy == yupstair)
                    || (sstairs.up && u.ux == sstairs.sx && u.uy == sstairs.sy)
                    || (xupladder && u.ux == xupladder && u.uy == yupladder)) {
                (void) doup();
                /* in case we're already Levitating, which would have
                   resulted in incrementing 'nothing' */
                nothing = 0; /* not nothing after all */
            } else if (has_ceiling(&u.uz)) {
                int dmg = rnd(!uarmh ? 10 : !is_metallic(uarmh) ? 6 : 3);

                You("hit your %s on the %s.", body_part(HEAD),
                    ceiling(u.ux, u.uy));
                losehp(Maybe_Half_Phys(dmg), "colliding with the ceiling",
                       KILLED_BY);
                nothing = 0; /* not nothing after all */
            }
        } else if (otmp->blessed) {
            /* at this point, timeout is already at least 1 */
            incr_itimeout(&HLevitation, rn1(50, 250));
            /* can descend at will (stop levitating via '>') provided timeout
               is the only factor (ie, not also wearing Lev ring or boots) */
            HLevitation |= I_SPECIAL;
        } else /* timeout is already at least 1 */
            incr_itimeout(&HLevitation, rn1(140, 10));

        if (Levitation && IS_SINK(levl[u.ux][u.uy].typ))
            spoteffects(FALSE);
        /* levitating blocks flying */
        float_vs_flight();
        break;
    case POT_GAIN_ENERGY: { /* M. Stephenson */
        int num;

        if (otmp->cursed)
            You_feel("lackluster.");
        else
            pline("Magical energies course through your body.");

        /* old: num = rnd(5) + 5 * otmp->blessed + 1;
         *      blessed:  +7..11 max & current (+9 avg)
         *      uncursed: +2.. 6 max & current (+4 avg)
         *      cursed:   -2.. 6 max & current (-4 avg)
         * new: (3.6.0)
         *      blessed:  +3..18 max (+10.5 avg), +9..54 current (+31.5 avg)
         *      uncursed: +2..12 max (+ 7   avg), +6..36 current (+21   avg)
         *      cursed:   -1.. 6 max (- 3.5 avg), -3..18 current (-10.5 avg)
         */
        num = d(otmp->blessed ? 3 : !otmp->cursed ? 2 : 1, 6);
        if (otmp->cursed)
            num = -num; /* subtract instead of add when cursed */
        u.uenmax += num;
        if (u.uenmax <= 0)
            u.uenmax = 0;
        u.uen += 3 * num;
        if (u.uen > u.uenmax)
            u.uen = u.uenmax;
        else if (u.uen <= 0)
            u.uen = 0;
        context.botl = 1;
        exercise(A_WIS, TRUE);
        break;
    }
    case POT_OIL: { /* P. Winner */
        boolean good_for_you = FALSE;

        if (otmp->lamplit) {
            if (likes_fire(youmonst.data)) {
                pline("Ahh, a refreshing drink.");
                good_for_you = TRUE;
            } else {
                You("burn your %s.", body_part(FACE));
                /* fire damage */
                losehp(d(Fire_resistance ? 1 : 3, 4), "burning potion of oil",
                       KILLED_BY_AN);
            }
        } else if (otmp->cursed)
            pline("This tastes like castor oil.");
        else
            pline("That was smooth!");
        exercise(A_WIS, good_for_you);
        break;
    }
    case POT_ACID:
        if (Acid_resistance) {
            /* Not necessarily a creature who _likes_ acid */
            pline("This tastes %s.", Hallucination ? "tangy" : "sour");
        } else {
            int dmg;

            pline("This burns%s!",
                  otmp->blessed ? " a little" : otmp->cursed ? " a lot"
                                                             : " like acid");
            dmg = d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8);
            losehp(Maybe_Half_Phys(dmg), "potion of acid", KILLED_BY_AN);
            exercise(A_CON, FALSE);
        }
        if (Stoned)
            fix_petrification();
        unkn++; /* holy/unholy water can burn like acid too */
        break;
    case POT_POLYMORPH:
        You_feel("a little %s.", Hallucination ? "normal" : "strange");
        if (!Unchanging)
            polyself(0);
        break;
    case POT_BLOOD:
  	case POT_VAMPIRE_BLOOD:
    		unkn++;
    		u.uconduct.unvegan++;
    		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE))) {
    		    violated_vegetarian();
    		    if (otmp->cursed)
    			      pline("Yecch!  This %s.", Hallucination ?
      			"liquid could do with a good stir" : "blood has congealed");
      		    else pline(Hallucination ?
      		      "The %s liquid stirs memories of home." :
      		      "The %s blood tastes delicious.",
      			  otmp->odiluted ? "watery" : "thick");
    		    if (!otmp->cursed)
          			lesshungry((otmp->odiluted ? 1 : 2) *
          			  (otmp->otyp == POT_VAMPIRE_BLOOD ? 400 :
          			  otmp->blessed ? 15 : 10));
    		    if (otmp->otyp == POT_VAMPIRE_BLOOD && otmp->blessed) {
          			int num = newhp();
          			if (Upolyd) {
          			    u.mhmax += num;
          			    u.mh += num;
          			} else {
          			    u.uhpmax += num;
          			    u.uhp += num;
          			}
    		    }
    		} else if (otmp->otyp == POT_VAMPIRE_BLOOD) {
    		    /* [CWC] fix conducts for potions of (vampire) blood -
    		       doesn't use violated_vegetarian() to prevent
    		       duplicated "you feel guilty" messages */
    		    u.uconduct.unvegetarian++;
    		    if (u.ualign.type == A_LAWFUL || Role_if(PM_MONK)) {
          			You_feel("%sguilty about drinking such a vile liquid.",
          				Role_if(PM_MONK) ? "especially " : "");
          			u.ugangr++;
          			adjalign(-15);
    		    } else if (u.ualign.type == A_NEUTRAL)
    			      adjalign(-3);
    		    exercise(A_CON, FALSE);
    		    if (!Unchanging && polymon(PM_VAMPIRE))
    			  u.mtimedone = 0;	/* "Permament" change */
    		} else {
    		    violated_vegetarian();
    		    pline("Ugh.  That was vile.");
    		    make_vomiting(Vomiting+d(10,8), TRUE);
    		}
    		break;
    default:
        impossible("What a funny potion! (%u)", otmp->otyp);
        return 0;
    }
    return -1;
}

void
healup(nhp, nxtra, curesick, cureblind)
int nhp, nxtra;
register boolean curesick, cureblind;
{
    if (nhp && !u.uroleplay.marathon) {
        if (Upolyd) {
            u.mh += nhp;
            if (u.mh > u.mhmax)
                u.mh = (u.mhmax += nxtra);
        } else {
            u.uhp += nhp;
            if (u.uhp > u.uhpmax)
                u.uhp = (u.uhpmax += nxtra);
        }
    }
    if (cureblind) {
        /* 3.6.1: it's debatible whether healing magic should clean off
           mundane 'dirt', but if it doesn't, blindness isn't cured */
        u.ucreamed = 0;
        make_blinded(0L, TRUE);
        /* heal deafness too */
        make_deaf(0L, TRUE);
    }
    if (curesick) {
        make_vomiting(0L, TRUE);
        make_sick(0L, (char *) 0, TRUE, SICK_ALL);
    }
    context.botl = 1;
    return;
}

void
strange_feeling(obj, txt)
struct obj *obj;
const char *txt;
{
    if (flags.beginner || !txt)
        You("have a %s feeling for a moment, then it passes.",
            Hallucination ? "normal" : "strange");
    else
        pline1(txt);

    if (!obj) /* e.g., crystal ball finds no traps */
        return;

    if (obj->dknown && !objects[obj->otyp].oc_name_known
        && !objects[obj->otyp].oc_uname)
        docall(obj);

    if(!(obj->oartifact)) useup(obj);
}

const char *bottlenames[] = { "bottle", "phial", "flagon", "carafe",
                              "flask",  "jar",   "vial" };

const char *hbottlenames[] = {
    "jug", "pitcher", "barrel", "tin", "bag", "box", "glass", "beaker", "tumbler", "vase", "flowerpot", "pan",
    "thingy", "mug", "teacup", "teapot", "keg", "bucket", "thermos", "amphora", "wineskin", "parcel", "bowl", "ampoule"
};

const char *
bottlename()
{
    return (Hallucination) ? hbottlenames[rn2(SIZE(hbottlenames))] : bottlenames[rn2(SIZE(bottlenames))];
}

/* handle item dipped into water potion or steed saddle splashed by same */
STATIC_OVL boolean
H2Opotion_dip(potion, targobj, useeit, objphrase)
struct obj *potion, *targobj;
boolean useeit;
const char *objphrase; /* "Your widget glows" or "Steed's saddle glows" */
{
    void FDECL((*func), (OBJ_P)) = 0;
    const char *glowcolor = 0;
#define COST_alter (-2)
#define COST_none (-1)
    int costchange = COST_none;
    boolean altfmt = FALSE, res = FALSE;

    if (!potion || potion->otyp != POT_WATER)
        return FALSE;

    if (potion->blessed) {
        if (targobj->cursed) {
            func = uncurse;
            glowcolor = NH_AMBER;
            costchange = COST_UNCURS;
        } else if (!targobj->blessed) {
            func = bless;
            glowcolor = NH_LIGHT_BLUE;
            costchange = COST_alter;
            altfmt = TRUE; /* "with a <color> aura" */
        }
    } else if (potion->cursed) {
        if (targobj->blessed) {
            func = unbless;
            glowcolor = "brown";
            costchange = COST_UNBLSS;
        } else if (!targobj->cursed) {
            func = curse;
            glowcolor = NH_BLACK;
            costchange = COST_alter;
            altfmt = TRUE;
        }
    } else {
        /* dipping into uncursed water; carried() check skips steed saddle */
        if (carried(targobj)) {
            if (water_damage(targobj, 0, TRUE) != ER_NOTHING)
                res = TRUE;
        }
    }
    if (func) {
        /* give feedback before altering the target object;
           this used to set obj->bknown even when not seeing
           the effect; now hero has to see the glow, and bknown
           is cleared instead of set if perception is distorted */
        if (useeit) {
            glowcolor = hcolor(glowcolor);
            if (altfmt)
                pline("%s with %s aura.", objphrase, an(glowcolor));
            else
                pline("%s %s.", objphrase, glowcolor);
            iflags.last_msg = PLNMSG_OBJ_GLOWS;
            targobj->bknown = !Hallucination;
        } else {
            /* didn't see what happened:  forget the BUC state if that was
               known unless the bless/curse state of the water is known;
               without this, hero would know the new state even without
               seeing the glow; priest[ess] will immediately relearn it */
            if (!potion->bknown || !potion->dknown)
                targobj->bknown = 0;
            /* [should the bknown+dknown exception require that water
               be discovered or at least named?] */
        }
        /* potions of water are the only shop goods whose price depends
           on their curse/bless state */
        if (targobj->unpaid && targobj->otyp == POT_WATER) {
            if (costchange == COST_alter)
                /* added blessing or cursing; update shop
                   bill to reflect item's new higher price */
                alter_cost(targobj, 0L);
            else if (costchange != COST_none)
                /* removed blessing or cursing; you
                   degraded it, now you'll have to buy it... */
                costly_alteration(targobj, costchange);
        }
        /* finally, change curse/bless state */
        (*func)(targobj);
        res = TRUE;
    }
    return res;
}

/* potion obj hits monster mon, which might be youmonst; obj always used up */
void
potionhit(mon, obj, how)
struct monst *mon;
struct obj *obj;
int how;
{
    const char *botlnam = bottlename();
    boolean isyou = (mon == &youmonst);
    int distance, tx, ty;
    struct obj *saddle = (struct obj *) 0;
    boolean hit_saddle = FALSE, your_fault = (how <= POTHIT_HERO_THROW);

    if (isyou) {
        tx = u.ux, ty = u.uy;
        distance = 0;
        if ((obj->otyp == POT_ACID) && (obj->ovar1)) {
            pline_The("%s crashes on your %s and explodes!", botlnam, body_part(HEAD));
            killer.format = KILLED_BY_AN;
            strcpy(killer.name, "alchemic blast");
            explode(tx, ty, 10, d(obj->quan, 9)+3, /* not physical damage */
                   POTION_CLASS, EXPL_MAGICAL);
        } else {
            pline_The("%s crashes on your %s and breaks into shards.", botlnam,
                      body_part(HEAD));
            losehp(Maybe_Half_Phys(rnd(2)), (how == POTHIT_OTHER_THROW) ? "propelled potion" /* scatter */
                : "thrown potion", KILLED_BY_AN);
        }
    } else {
        tx = mon->mx, ty = mon->my;
        /* sometimes it hits the saddle */
        if (((mon->misc_worn_check & W_SADDLE)
             && (saddle = which_armor(mon, W_SADDLE)))
            && (!rn2(10)
                || (obj->otyp == POT_WATER
                    && ((rnl(10) > 7 && obj->cursed)
                        || (rnl(10) < 4 && obj->blessed) || !rn2(3)))))
            hit_saddle = TRUE;
        distance = distu(tx, ty);

        if ((obj->otyp == POT_ACID) && (obj->ovar1)) {
            if (!cansee(tx, ty)) {
                pline("BOOM!");
            } else {
                pline_The("%s hits %s and explodes!", botlnam, mon_nam(mon));
            }
            killer.format = KILLED_BY_AN;
            strcpy(killer.name, "alchemic blast");
            explode(tx, ty, 10, d(obj->quan, 9)+3, /* not physical damage */
                   POTION_CLASS, EXPL_MAGICAL);
            return;
        }

        if (!cansee(tx, ty)) {
            pline("Crash!");
        } else {
            char *mnam = mon_nam(mon);
            char buf[BUFSZ];

            if (hit_saddle && saddle) {
                Sprintf(buf, "%s saddle",
                        s_suffix(x_monnam(mon, ARTICLE_THE, (char *) 0,
                                          (SUPPRESS_IT | SUPPRESS_SADDLE),
                                          FALSE)));
            } else if (has_head(mon->data)) {
                Sprintf(buf, "%s %s", s_suffix(mnam),
                        (notonhead ? "body" : "head"));
            } else {
                Strcpy(buf, mnam);
            }
            pline_The("%s crashes on %s and breaks into shards.", botlnam,
                      buf);
        }
        if (rn2(5) && mon->mhp > 1 && !hit_saddle)
            mon->mhp--;
    }

    /* oil doesn't instantly evaporate; Neither does a saddle hit */
    if (obj->otyp != POT_OIL && !hit_saddle && cansee(tx, ty))
        pline("%s.", Tobjnam(obj, "evaporate"));

    if (isyou) {
        switch (obj->otyp) {
        case POT_OIL:
            if (obj->lamplit)
                explode_oil(obj, u.ux, u.uy);
            break;
        case POT_POLYMORPH:
            You_feel("a little %s.", Hallucination ? "normal" : "strange");
            if (!Unchanging && !Antimagic)
                polyself(0);
            break;
        case POT_BLOOD:
        case POT_VAMPIRE_BLOOD:
            if (Blind)
                You_feel("sticky!");
            else if (Race_if(PM_INFERNAL)) {
                pline("Blood drips down your form.");
                exercise(A_CHA, TRUE);
            }
            else {
                You("are covered in blood! How disgusting!");
                exercise(A_CHA, FALSE);
            }

            break;
        case POT_ACID:
            if (!Acid_resistance) {
                int dmg;

                pline("This burns%s!",
                      obj->blessed ? " a little"
                                   : obj->cursed ? " a lot" : "");
                dmg = d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
                losehp(Maybe_Half_Phys(dmg), "potion of acid", KILLED_BY_AN);
            }
            break;
        }
    } else if (hit_saddle && saddle) {
        char *mnam, buf[BUFSZ], saddle_glows[BUFSZ];
        boolean affected = FALSE;
        boolean useeit = !Blind && canseemon(mon) && cansee(tx, ty);

        mnam = x_monnam(mon, ARTICLE_THE, (char *) 0,
                        (SUPPRESS_IT | SUPPRESS_SADDLE), FALSE);
        Sprintf(buf, "%s", upstart(s_suffix(mnam)));

        switch (obj->otyp) {
        case POT_WATER:
            Sprintf(saddle_glows, "%s %s", buf, aobjnam(saddle, "glow"));
            affected = H2Opotion_dip(obj, saddle, useeit, saddle_glows);
            break;
        case POT_POLYMORPH:
            /* Do we allow the saddle to polymorph? */
            break;
        }
        if (useeit && !affected)
            pline("%s %s wet.", buf, aobjnam(saddle, "get"));
    } else {
        boolean angermon = your_fault, cureblind = FALSE;

        switch (obj->otyp) {
        case POT_FULL_HEALING:
            cureblind = TRUE;
            /*FALLTHRU*/
        case POT_EXTRA_HEALING:
            if (!obj->cursed)
                cureblind = TRUE;
            /*FALLTHRU*/
        case POT_HEALING:
            if (obj->blessed)
                cureblind = TRUE;
            if (mon->data == &mons[PM_PESTILENCE])
                goto do_illness;
            /*FALLTHRU*/
        case POT_RESTORE_ABILITY:
        case POT_GAIN_ABILITY:
 do_healing:
            angermon = FALSE;
            if (mon->mhp < mon->mhpmax) {
                mon->mhp = mon->mhpmax;
                if (canseemon(mon))
                    pline("%s looks sound and hale again.", Monnam(mon));
            }
            if (cureblind)
                mcureblindness(mon, canseemon(mon));
            break;
        case POT_SICKNESS:
            if (mon->data == &mons[PM_PESTILENCE])
                goto do_healing;
            if (dmgtype(mon->data, AD_DISE)
                /* won't happen, see prior goto */
                || dmgtype(mon->data, AD_PEST)
                /* most common case */
                || resists_poison(mon)) {
                if (canseemon(mon))
                    pline("%s looks unharmed.", Monnam(mon));
                break;
            }
 do_illness:
            if ((mon->mhpmax > 3) && !resist(mon, POTION_CLASS, 0, NOTELL))
                mon->mhpmax /= 2;
            if ((mon->mhp > 2) && !resist(mon, POTION_CLASS, 0, NOTELL))
                mon->mhp /= 2;
            if (mon->mhp > mon->mhpmax)
                mon->mhp = mon->mhpmax;
            if (canseemon(mon))
                pline("%s looks rather ill.", Monnam(mon));
            break;
        case POT_CONFUSION:
        case POT_BOOZE:
            if (!resist(mon, POTION_CLASS, 0, NOTELL))
                mon->mconf = TRUE;
            break;
        case POT_INVISIBILITY: {
            boolean sawit = canspotmon(mon);

            angermon = FALSE;
            mon_set_minvis(mon);
            if (sawit && !canspotmon(mon) && cansee(mon->mx, mon->my))
                map_invisible(mon->mx, mon->my);
            break;
        }
        case POT_SLEEPING:
            /* wakeup() doesn't rouse victims of temporary sleep */
            if (sleep_monst(mon, rnd(12), POTION_CLASS)) {
                pline("%s falls asleep.", Monnam(mon));
                slept_monst(mon);
            }
            break;
        case POT_PARALYSIS:
            if (mon->mcanmove) {
                /* really should be rnd(5) for consistency with players
                 * breathing potions, but...
                 */
                paralyze_monst(mon, rnd(25));
            }
            break;
        case POT_BLOOD:
        case POT_VAMPIRE_BLOOD:
            pline("%s is covered in blood!", Monnam(mon));
            if (is_vampire(mon->data))
                pline("%s seems to enjoy the blood bath.", Monnam(mon));
            break;
        case POT_SPEED:
            angermon = FALSE;
            mon_adjust_speed(mon, 1, obj);
            break;
        case POT_BLINDNESS:
            if (haseyes(mon->data)) {
                int btmp = 64 + rn2(32)
                            + rn2(32) * !resist(mon, POTION_CLASS, 0, NOTELL);

                btmp += mon->mblinded;
                mon->mblinded = min(btmp, 127);
                mon->mcansee = 0;
            }
            break;
        case POT_WATER:
            if (is_undead(mon->data) || is_demon(mon->data)
                || is_were(mon->data) || is_vampshifter(mon)) {
                if (obj->blessed) {
                    pline("%s %s in pain!", Monnam(mon),
                          is_silent(mon->data) ? "writhes" : "shrieks");
                    if (!is_silent(mon->data))
                        wake_nearto(tx, ty, mon->data->mlevel * 10);
                    mon->mhp -= d(2, 6);
                    /* should only be by you */
                    if (DEADMONSTER(mon))
                        killed(mon);
                    else if (is_were(mon->data) && !is_human(mon->data))
                        new_were(mon); /* revert to human */
                } else if (obj->cursed) {
                    angermon = FALSE;
                    if (canseemon(mon))
                        pline("%s looks healthier.", Monnam(mon));
                    mon->mhp += d(2, 6);
                    if (mon->mhp > mon->mhpmax)
                        mon->mhp = mon->mhpmax;
                    if (is_were(mon->data) && is_human(mon->data)
                        && !Protection_from_shape_changers)
                        new_were(mon); /* transform into beast */
                }
            } else if (mon->data == &mons[PM_GREMLIN]) {
                angermon = FALSE;
                (void) split_mon(mon, (struct monst *) 0);
            } else if (mon->data == &mons[PM_IRON_GOLEM]) {
                if (canseemon(mon))
                    pline("%s rusts.", Monnam(mon));
                mon->mhp -= d(1, 6);
                /* should only be by you */
                if (DEADMONSTER(mon))
                    killed(mon);
            }
            break;
        case POT_OIL:
            if (obj->lamplit)
                explode_oil(obj, tx, ty);
            break;
        case POT_ACID:
            if (!resists_acid(mon) && !resist(mon, POTION_CLASS, 0, NOTELL)) {
                pline("%s %s in pain!", Monnam(mon),
                      is_silent(mon->data) ? "writhes" : "shrieks");
                if (!is_silent(mon->data))
                    wake_nearto(tx, ty, mon->data->mlevel * 10);
                mon->mhp -= d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
                if (DEADMONSTER(mon)) {
                    if (your_fault)
                        killed(mon);
                    else
                        monkilled(mon, NULL, "", AD_ACID);
                }
            }
            break;
        case POT_REGENERATION:
            if (obj->cursed) {
                pline("%s begins to wither!", Monnam(mon));
                mon->mwither = 1;
            }
            break;
        case POT_POLYMORPH:
            (void) bhitm(mon, obj);
            break;
        /*
        case POT_GAIN_LEVEL:
        case POT_LEVITATION:
        case POT_FRUIT_JUICE:
        case POT_MONSTER_DETECTION:
        case POT_OBJECT_DETECTION:
            break;
        */
        }
        /* target might have been killed */
        if (!DEADMONSTER(mon)) {
            if (angermon)
                wakeup(mon, TRUE);
            else
                mon->msleeping = 0;
        }
    }

    /* Note: potionbreathe() does its own docall() */
    if ((distance == 0 || (distance < 3 && rn2(5)))
        && (!breathless(youmonst.data) || haseyes(youmonst.data)))
        potionbreathe(obj);
    else if (obj->dknown && !objects[obj->otyp].oc_name_known
             && !objects[obj->otyp].oc_uname && cansee(tx, ty))
        docall(obj);

    if (*u.ushops && obj->unpaid) {
        struct monst *shkp = shop_keeper(*in_rooms(u.ux, u.uy, SHOPBASE));

        /* neither of the first two cases should be able to happen;
           only the hero should ever have an unpaid item, and only
           when inside a tended shop */
        if (!shkp) /* if shkp was killed, unpaid ought to cleared already */
            obj->unpaid = 0;
        else if (context.mon_moving) /* obj thrown by monster */
            subfrombill(obj, shkp);
        else /* obj thrown by hero */
            (void) stolen_value(obj, u.ux, u.uy, (boolean) shkp->mpeaceful,
                                FALSE);
    }
    obfree(obj, (struct obj *) 0);
}

/* vapors are inhaled or get in your eyes */
void
potionbreathe(obj)
register struct obj *obj;
{
    int i, ii, isdone, kn = 0;
    boolean cureblind = FALSE;

    /* potion of unholy water might be wielded; prevent
       you_were() -> drop_weapon() from dropping it so that it
       remains in inventory where our caller expects it to be */
    obj->in_use = 1;

    switch (obj->otyp) {
    case POT_RESTORE_ABILITY:
    case POT_GAIN_ABILITY:
        if (obj->cursed) {
            if (!breathless(youmonst.data))
                pline("Ulch!  That potion smells terrible!");
            else if (haseyes(youmonst.data)) {
                const char *eyes = body_part(EYE);

                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your("%s %s!", eyes, vtense(eyes, "sting"));
            }
            break;
        } else {
            i = rn2(A_MAX); /* start at a random point */
            for (isdone = ii = 0; !isdone && ii < A_MAX; ii++) {
                if (ABASE(i) < AMAX(i)) {
                    ABASE(i)++;
                    /* only first found if not blessed */
                    isdone = !(obj->blessed);
                    context.botl = 1;
                }
                if (++i >= A_MAX)
                    i = 0;
            }
        }
        break;
    case POT_FULL_HEALING:
        if (Upolyd && u.mh < u.mhmax)
            u.mh++, context.botl = 1;
        if (u.uhp < u.uhpmax)
            u.uhp++, context.botl = 1;
        cureblind = TRUE;
        /*FALLTHRU*/
    case POT_EXTRA_HEALING:
        if (Upolyd && u.mh < u.mhmax)
            u.mh++, context.botl = 1;
        if (u.uhp < u.uhpmax)
            u.uhp++, context.botl = 1;
        if (!obj->cursed)
            cureblind = TRUE;
        /*FALLTHRU*/
    case POT_HEALING:
        if (Upolyd && u.mh < u.mhmax)
            u.mh++, context.botl = 1;
        if (u.uhp < u.uhpmax)
            u.uhp++, context.botl = 1;
        if (obj->blessed)
            cureblind = TRUE;
        if (cureblind) {
            make_blinded(0L, !u.ucreamed);
            make_deaf(0L, TRUE);
        }
        exercise(A_CON, TRUE);
        break;
    case POT_REGENERATION:
        if (obj->cursed) {
            incr_itimeout(&HWithering, rn1(5, 5));
            exercise(A_CON, FALSE);
            You("start to shrivel up!");
        } else {
            You("feel a tiny bit better.");
            set_itimeout(&HRegeneration, rn1(5, 5));
            kn++;
        }
        context.botl = TRUE;
        break;
    case POT_SICKNESS:
        if (!Role_if(PM_HEALER)) {
            if (Upolyd) {
                if (u.mh <= 5)
                    u.mh = 1;
                else
                    u.mh -= 5;
            } else {
                if (u.uhp <= 5)
                    u.uhp = 1;
                else
                    u.uhp -= 5;
            }
            context.botl = 1;
            exercise(A_CON, FALSE);
        }
        break;
    case POT_HALLUCINATION:
        You("have a momentary vision.");
        break;
    case POT_CONFUSION:
    case POT_BOOZE:
        if (!Confusion)
            You_feel("somewhat dizzy.");
        make_confused(itimeout_incr(HConfusion, rnd(5)), FALSE);
        break;
    case POT_INVISIBILITY:
        if (!Blind && !Invis) {
            kn++;
            pline("For an instant you %s!",
                  See_invisible ? "could see right through yourself"
                                : "couldn't see yourself");
        }
        break;
    case POT_PARALYSIS:
        kn++;
        if (!Free_action) {
            pline("%s seems to be holding you.", Something);
            nomul(-rnd(5));
            multi_reason = "frozen by a potion";
            nomovemsg = You_can_move_again;
            exercise(A_DEX, FALSE);
        } else
            You("stiffen momentarily.");
        break;
    case POT_SLEEPING:
        kn++;
        if (!Free_action && !Sleep_resistance) {
            You_feel("rather tired.");
            nomul(-rnd(5));
            multi_reason = "sleeping off a magical draught";
            nomovemsg = You_can_move_again;
            exercise(A_DEX, FALSE);
        } else
            You("yawn.");
        break;
    case POT_SPEED:
        if (!Fast)
            Your("knees seem more flexible now.");
        incr_itimeout(&HFast, rnd(5));
        exercise(A_DEX, TRUE);
        break;
    case POT_BLINDNESS:
        if (!Blind && !Unaware) {
            kn++;
            pline("It suddenly gets dark.");
        }
        make_blinded(itimeout_incr(Blinded, rnd(5)), FALSE);
        if (!Blind && !Unaware)
            Your1(vision_clears);
        break;
    case POT_WATER:
        if (u.umonnum == PM_GREMLIN) {
            (void) split_mon(&youmonst, (struct monst *) 0);
        } else if (u.ulycn >= LOW_PM) {
            /* vapor from [un]holy water will trigger
               transformation but won't cure lycanthropy */
            if (obj->blessed && youmonst.data == &mons[u.ulycn])
                you_unwere(FALSE);
            else if (obj->cursed && !Upolyd)
                you_were();
        }
        break;
    case POT_ACID:
    case POT_POLYMORPH:
        exercise(A_CON, FALSE);
        break;
    case POT_BLOOD:
  	case POT_VAMPIRE_BLOOD:
  		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE))) {
  		    exercise(A_WIS, FALSE);
  		    You_feel("a %ssense of loss.",
  		      obj->otyp == POT_VAMPIRE_BLOOD ? "terrible " : "");
  		} else
  		    exercise(A_CON, FALSE);
      break;
    /*
    case POT_GAIN_LEVEL:
    case POT_LEVITATION:
    case POT_FRUIT_JUICE:
    case POT_MONSTER_DETECTION:
    case POT_OBJECT_DETECTION:
    case POT_OIL:
        break;
     */
    }
    /* note: no obfree() -- that's our caller's responsibility */
    if (obj->dknown) {
        if (kn)
            makeknown(obj->otyp);
        else if (!objects[obj->otyp].oc_name_known
                 && !objects[obj->otyp].oc_uname)
            docall(obj);
    }
}

/* returns the potion type when o1 is dipped in o2 */
STATIC_OVL short
mixtype(o1, o2)
register struct obj *o1, *o2;
{
    int o1typ = o1->otyp, o2typ = o2->otyp;

    /* cut down on the number of cases below */
    if (o1->oclass == POTION_CLASS
        && (o2typ == POT_GAIN_LEVEL || o2typ == POT_GAIN_ENERGY
            || o2typ == POT_HEALING || o2typ == POT_EXTRA_HEALING
            || o2typ == POT_FULL_HEALING || o2typ == POT_ENLIGHTENMENT
            || o2typ == POT_FRUIT_JUICE)) {
        /* swap o1 and o2 */
        o1typ = o2->otyp;
        o2typ = o1->otyp;
    }

    switch (o1typ) {
    case POT_HEALING:
        if (o2typ == POT_SPEED) {
            return POT_EXTRA_HEALING;
        }
        break;
    case POT_EXTRA_HEALING:
        switch (o2->otyp) {
        case POT_GAIN_LEVEL:
        case POT_GAIN_ENERGY:
            return POT_FULL_HEALING;
        }
        break;
    case POT_FULL_HEALING:
        switch (o2->otyp) {
        case POT_GAIN_LEVEL:
        case POT_GAIN_ENERGY:
            return POT_GAIN_ABILITY;
        }
        break;
    case UNICORN_HORN:
        switch (o2typ) {
        case POT_SICKNESS:
            return POT_FRUIT_JUICE;
        case POT_HALLUCINATION:
        case POT_BLINDNESS:
        case POT_CONFUSION:
        case POT_BLOOD:
        case POT_VAMPIRE_BLOOD:
            return POT_WATER;
        }
        break;
    case AMETHYST: /* "a-methyst" == "not intoxicated" */
        if (o2typ == POT_BOOZE)
            return POT_FRUIT_JUICE;
        break;
    case POT_GAIN_LEVEL:
    case POT_GAIN_ENERGY:
        switch (o2typ) {
        case POT_CONFUSION:
            return (rn2(3) ? POT_BOOZE : POT_ENLIGHTENMENT);
        case POT_HEALING:
            return POT_EXTRA_HEALING;
        case POT_EXTRA_HEALING:
            return POT_FULL_HEALING;
        case POT_FULL_HEALING:
            return POT_GAIN_ABILITY;
        case POT_FRUIT_JUICE:
            return POT_SEE_INVISIBLE;
        case POT_BOOZE:
            return POT_HALLUCINATION;
        }
        break;
    case POT_FRUIT_JUICE:
        switch (o2typ) {
        case POT_BLOOD:
            return POT_BLOOD;
        case POT_VAMPIRE_BLOOD:
            return POT_VAMPIRE_BLOOD;
        case POT_SICKNESS:
            return POT_SICKNESS;
        case POT_ENLIGHTENMENT:
        case POT_SPEED:
            return POT_BOOZE;
        case POT_GAIN_LEVEL:
        case POT_GAIN_ENERGY:
            return POT_SEE_INVISIBLE;
        }
        break;
    case POT_ENLIGHTENMENT:
        switch (o2typ) {
        case POT_LEVITATION:
            if (rn2(3))
                return POT_GAIN_LEVEL;
            break;
        case POT_FRUIT_JUICE:
            return POT_BOOZE;
        case POT_BOOZE:
            return POT_CONFUSION;
        }
        break;
    }

    return STRANGE_OBJECT;
}

/* Determine the chance (parts in 1000) that recipe-alchemy will fail (BOOM).
 * It is based most strongly on the difficulty of the recipe and your skill.
 * Luck, intelligence and experience also play a part, though.
 */
int alchemy_failrate(int otyp)
{
    static const int divi[] = { 1, 3, 10, 40, 160, 800, 4000 };
    struct recipe *r = u.alchemy + (otyp - POT_GAIN_ABILITY);
    int difficulty = r->difficulty; // %
    int min = u.ulevel+(2*ACURR(A_INT))+(3*Luck);
    
    /* If it's 0 then this is not alchemizable. */
    if (difficulty == 0) return 1000;

    /* Determine the minimum failure rate from int, luck, XL */
    min += 20;
    if (min <= 4) min = 4;
    min = 350000 / min; // 87.5% minimum fail worst case. Typ (xl10,int16,luck0) 5.64% 2.89% best.

    /* Determine the skill-vs-difficulty based failure rate */
    difficulty *= 10000;
    difficulty /= divi[P_SKILL(P_ALCHEMY)]; // per 1e6
    
    /* Use whichever is worse */
    if (difficulty < min) difficulty = min;
    
    /* Convert back, bound, return. */
    difficulty /= 1000;
    if (difficulty > 1000) difficulty = 1000;
    return difficulty;
}

/* Does something */
const char *alchemy_vague_news(int failrate)
{
    if (failrate < 50)      return "Very Easy";
    if (failrate < 80)      return "Easy";
    if (failrate < 125)     return "Fairly Easy";
    if (failrate < 200)     return "Tricky";
    if (failrate < 350)     return "Fairly Difficult";
    if (failrate < 550)     return "Difficult";
    if (failrate < 750)     return "Very Difficult";
    if (failrate < 950)     return "Extremely Hard";
    if (failrate < 1000)    return "Almost Impossible";
    return "Impossible";
};
    
extern const int monstr[];

/* Convert an object into an alchemy ID and return the total number of monsters and IDs.
 * This will return ALC_UNKNOWN (1) if it's not a valid alchemy ingredient.
 */
int
do_alchemy_id(struct obj * obj, int *monc, int *total, unsigned char *value, boolean ret_mtyp, int ret_id)
{
    boolean ok;
    int valid;
    int oclass, otyp, baseid;
    int mtyp;
    int id = ALC_UNKNOWN;
    *monc = ALC_UNKNOWN;
    *total = ALC_UNKNOWN;
    
    /* Search for monsters matching the tin or corpse's monster ID */
    for(mtyp = 0; mons[mtyp].mname[0]; mtyp++) {
        int f = corpse_frequency(mtyp);
        if (mons[mtyp].geno & (G_UNIQ|G_HELL|G_NOGEN|G_NOCORPSE)) continue;
        if ((mons[mtyp].geno & G_FREQ) == 0) continue;
        if (mons[mtyp].mflags2 & (M2_PEACEFUL|M2_HUMAN|M2_WERE|M2_UNDEAD|M2_NOPOLY|M2_ELF|M2_DWARF|M2_ORC|M2_MERC|M2_LORD|M2_PRINCE|M2_MINION|M2_DOMESTIC)) continue;
        
        /* Never drops a corpse - don't use this ID */
        if (f <= 0) continue;

        id++;
        (*monc)++; (*total)++;
        
        /* The value of a monster corpse is based on its difficulty from monstr[],
         * filled by mstrength(). However this is intended to be "difficulty of
         * obtaining a corpse", so there are some changes - creation in groups
         * makes it easier. Size, etc is handled by corpse_frequency, but being
         * heavy (and so hard to dip, possibly requiring a tinning kit) makes it
         * difficult. Generation frequency also makes a difference.
         **/
        if (value) {
            int v = mons[mtyp].difficulty;
            int f = corpse_frequency(mtyp);
            int w = mons[mtyp].cwt;
            int spill;

            v -= (!!(mons[mtyp].geno & G_SGROUP)) << 1;
            v -= (!!(mons[mtyp].geno & G_LGROUP)) << 2;
            
            /* Heavy corpse so more difficult */
            v += min(w, 3000) / 500;
            
            if (v < 1) v = 1;
            
            /* This is 1..4 and includes a +1 for G_FREQ == 1.
             * Multiply by a arbitrary scaling factor to keep everything in 0..255. */
            v *= f * 6;

            /* This is 1..7. 1 = very rare, 7 = very common, and is linear.
             * It doesn't take account of special generation, though...
             **/
            v *= 10;
            v /= gen_frequency(&mons[mtyp]);

            /* Bound, with soft clipping above 224 */
            if (v < 1) v = 1;
            spill = 0;
            while (v > 224) {
                v -= (v >> 5)+1;
                spill++;
            }
            v = 224+spill;
            if (v > 255) v = 255;
            value[id] = v;
        }
        
        if (obj) {
            if ((((obj->otyp == TIN) && (obj->spe <= 0)) || (obj->otyp == CORPSE)) && (obj->corpsenm == mtyp))
                return id;
        } else {
            if ((ret_mtyp) && (ret_id == id))
                return mtyp;
        }
    }
    
    /* Otherwise check for a matching object type */
    for(otyp = 0; otyp < NUM_OBJECTS; otyp++) {
        ok = FALSE;
        valid = -1;
        oclass = objects[otyp].oc_class;
        
        if (!(OBJ_NAME(objects[otyp]))) continue;
    
        switch(oclass) {
            case ARMOR_CLASS:
            /* Dragon scales are valid. So is mail (which is equivalent) */
            if (objects[otyp].oc_material == DRAGON_HIDE) {
                int scales = armor_scales(otyp);
                if (scales != VOID_DRAGON_SCALES) {
                    if (otyp >= GRAY_DRAGON_SCALES)
                        ok = TRUE;
                    else
                        valid = (otyp - scales) + baseid;
                }
            }
            break;
            
            case POTION_CLASS:
            /* Most potions are valid */
            if ((otyp != POT_ACID) && (otyp != POT_WATER)) ok = TRUE;
            break;
            
            case TOOL_CLASS:
            /* Some tools are valid. Some form equivalence groups */
            if ((otyp == UNICORN_HORN) || (otyp == CAN_OF_GREASE) || (otyp == TALLOW_CANDLE) || (otyp == WAX_CANDLE))
                ok = TRUE;
            if (otyp == TOOLED_HORN) {
                baseid = id + 1;
                ok = TRUE;
            }
            if ((otyp == HORN_OF_PLENTY) || (otyp == FROST_HORN) || (otyp == FIRE_HORN))
                valid = baseid;
            break;
            
            case FOOD_CLASS:
            /* Some foodstuffs are valid. This includes some corpses - defer to a later pass? */
            if ((otyp == GLOB_OF_GRAY_OOZE) || (otyp == GLOB_OF_BROWN_PUDDING) || (otyp == GLOB_OF_GREEN_SLIME) || (otyp == GLOB_OF_BLACK_PUDDING)) ok = TRUE;
            if ((otyp == KELP_FROND) || (otyp == EUCALYPTUS_LEAF) || (otyp == PINCH_OF_CATNIP) || (otyp == SPRIG_OF_WOLFSBANE) || (otyp == CLOVE_OF_GARLIC) || (otyp == LUMP_OF_ROYAL_JELLY)) ok = TRUE;
            
            /* Tins are acceptable as a corpse alternative. Tins of spinach are this separate ID. */
            if (otyp == TIN) {
                ok = TRUE;
                if (obj) {
                    if ((obj->otyp == TIN) && (obj->spe > 0))
                        return ++id;
                } else {
                    if ((!ret_mtyp) && (ret_id == id)) return otyp;
                }
            }
            break;
            
            case WAND_CLASS:
            /* Most wands are valid, unless useless or too powerful
             * ...should spent wands still be usable?
             **/
            if ((otyp != WAN_WISHING) && (otyp != WAN_NOTHING)) ok = TRUE;
            break;
            
            case GEM_CLASS:
            /* Valuable gems are valid */
            if (objects[otyp].oc_material == GEMSTONE) ok = TRUE;
            break;
            
            case ROCK_CLASS:
            /* Flint, luck, touch are valid */
            if ((otyp == FLINT) || (otyp == LUCKSTONE) || (otyp == TOUCHSTONE)) ok = TRUE;
            break;
        }
        if (ok) {
            id++;
            (*total)++;
            if (obj) {
                if (obj->otyp == otyp) return id;
            } else {
                if ((!ret_mtyp) && (ret_id == id)) return otyp;
            }
        }

        if (valid >= 0) {
            if (obj) {
                if (obj->otyp == otyp) return valid;
            } else {
                if ((!ret_mtyp) && (ret_id == id)) return otyp;
            }
        }

        if (value && (ok || (valid >= 0)))
        {
            if (valid < 0) valid = id;
            int v = 1;
            switch(oclass) {
                /* Dragons always drop a corpse, but don't always drop scales.
                 * Part of the reason for the high value of a dragon corpse is the
                 * weight, but even so this should have a higher value than a dragon corpse.
                 * Especially as it is such a useful item.
                 * Corpses are 156 for most, 210 for razor, 222 for filth/hex/ooze.
                 * Use "valid" to avoid needing to check both scales and mail
                 */
                case ARMOR_CLASS:
                    switch(otyp) {
                        case FILTH_DRAGON_SCALE_MAIL:
                        case OOZE_DRAGON_SCALE_MAIL:
                        case HEX_DRAGON_SCALE_MAIL:
                            v = 250;
                            break;
                        case RAZOR_DRAGON_SCALE_MAIL:
                            v = 240;
                            break;
                        case GRAY_DRAGON_SCALE_MAIL:
                        case SILVER_DRAGON_SCALE_MAIL:
                            v = 230;
                            break;
                        case SHIMMERING_DRAGON_SCALE_MAIL:
                        case BLACK_DRAGON_SCALE_MAIL:
                        case PURPLE_DRAGON_SCALE_MAIL:
                            v = 220;
                            break;
                        default:
                            v = 210;
                    }
                    break;
                
                /* Potions vary from very high value (gain ability) to near worthless.
                 * In particular the requirement for a high-value potion can make a
                 * recipe useless, especially when compared to conventional alchemy...
                 * However this is not just a copy of the "difficulty to make", as it
                 * offers something to do with otherwise useless or situational potions.
                 * So only potions that nobody wants to give up score highly.
                 */
                case POTION_CLASS:
                    switch(otyp) {
                        case POT_GAIN_ABILITY:
                            v = 180;
                            break;
                        case POT_GAIN_LEVEL:
                        case POT_REFLECTION:
                        case POT_VAMPIRE_BLOOD:
                            v = 170;
                            break;
                        case POT_FULL_HEALING:
                        case POT_GAIN_ENERGY:
                            v = 160;
                            break;
                        case POT_BLOOD:
                            v = 120;
                            break;
                        case POT_POLYMORPH:
                        case POT_SPEED:
                        case POT_EXTRA_HEALING:
                            v = 80;
                            break;
                        case POT_PARALYSIS:
                        case POT_OIL:
                        case POT_ENLIGHTENMENT:
                        case POT_HEALING:
                            v = 40;
                            break;
                        case POT_LEVITATION:
                        case POT_INVISIBILITY:
                        case POT_MONSTER_DETECTION:
                        case POT_RESTORE_ABILITY:
                        case POT_SICKNESS:
                            v = 20;
                            break;
                        case POT_SEE_INVISIBLE:
                        case POT_CONFUSION:
                        case POT_BLINDNESS:
                        case POT_SLEEPING:
                        case POT_HALLUCINATION:
                        case POT_OBJECT_DETECTION:
                            v = 10;
                            break;
                        case POT_BOOZE:
                        case POT_FRUIT_JUICE:
                            v = 5;
                            break;
                    }
                    break;

                /* Tools vary in value.
                 * Unicorn horn - you need one but commonly have a few more. But not more than that.
                 * Other horns - can be polymorphed for, but are not that common otherwise.
                 * Grease - you probably don't have a spare. There's no source like unihorns, and they have limited uses. 
                 * Candles - you may have a lot more than the 7 you need.
                 */
                case TOOL_CLASS:
                    switch(otyp) {
                        case CAN_OF_GREASE:
                            v = 100;
                            break;
                        case UNICORN_HORN:
                            v = 50;
                            break;
                        case TOOLED_HORN:
                        case FIRE_HORN:
                        case FROST_HORN:
                        case HORN_OF_PLENTY:
                            v = 30;
                            break;
                        case WAX_CANDLE:
                            v = 15;
                            break;
                        case TALLOW_CANDLE:
                            v = 5;
                            break;
                    }
                    break;
                
                /* Food - other than meat.
                 * Globs are based on the difficulty and rarity of the monster.
                 * Other items from item rarity (taking into account special generation).
                 **/
                case FOOD_CLASS:
                    switch(otyp) {
                        case GLOB_OF_GRAY_OOZE:
                            v = 15;
                            break;
                        case GLOB_OF_BROWN_PUDDING:
                            v = 25;
                            break;
                        case GLOB_OF_BLACK_PUDDING:
                            v = 70;
                            break;
                        case GLOB_OF_GREEN_SLIME:   /* only generated in hell, or specially (nasties) */
                            v = 200;
                            break;
                        case EUCALYPTUS_LEAF:       /* mostly from trees */
                        case CLOVE_OF_GARLIC:
                            v = 12;
                            break;
                        case KELP_FROND:            /* only from special generation */
                            v = 150;
                            break;
                        case PINCH_OF_CATNIP:
                            v = 33;
                            break;
                        case SPRIG_OF_WOLFSBANE:
                            v = 25;
                            break;
                        case LUMP_OF_ROYAL_JELLY:   /* only from special generation */
                            v = 80;
                            break;
                        case TIN:                   /* of spinach */
                            v = 60;
                            break;
                    }
                    break;
                
                /* Wands' rarity is a better model of usefulness to you than their cash value */
                case WAND_CLASS:
                    v = 12500 / get_oc_prob(otyp);
                    break;
                
                /* Stones' rarity can't also be used as it is reset by setgemprobs per level.
                 * Cash value is a reasonable alternative.
                 **/
                case GEM_CLASS:
                    v = objects[otyp].oc_cost / 20;
                    break;
                    
                case ROCK_CLASS:
                    if (otyp == FLINT)
                        v = 4;
                    else
                        v = 30;  
                    break;
            }
            assert(v != 1);
            value[id] = v;
        }
    }
    return obj ? ALC_UNKNOWN : -1;
}

/* Convert an alchemy ID into an mtyp.
 * This will return -1 if it's not an mtyp known to alchemy.
 */
int
alchemy_mtyp(int id)
{
    int monc, total;
    return do_alchemy_id(NULL, &monc, &total, NULL, TRUE, id);
}

/* Convert an alchemy ID into an otyp.
 * This will return -1 if it's not an otyp known to alchemy.
 */
int
alchemy_otyp(int id)
{
    int monc, total;
    return do_alchemy_id(NULL, &monc, &total, NULL, FALSE, id);
}

/* Convert an object into an alchemy ID.
 * This will return ALC_UNKNOWN (1) if it's not a valid alchemy ingredient.
 */
int
alchemy_id(struct obj * obj)
{
    int monc, total;
    return do_alchemy_id(obj, &monc, &total, NULL, FALSE, 0);
}

/* Convert an otyp into an alchemy ID.
 * This will return ALC_UNKNOWN (1) if it's not a valid alchemy ingredient.
 */
int
alchemy_otyp_id(int otyp)
{
    struct obj o;
    o.oclass = objects[otyp].oc_class;
    o.otyp = otyp;
    int monc, total;
    return do_alchemy_id(&o, &monc, &total, NULL, FALSE, 0);
}

/* Obtain counts of monsters and total IDs.
 */
void
alchemy_max_id(int *monc, int *total)
{
    struct obj o;
    
    // Dummy object
    o.oclass = ROCK_CLASS;
    o.otyp = ROCK;
    do_alchemy_id(&o, monc, total, NULL, FALSE, 0);
}

/* Return an array in allocated memory of rarity values per ID */
unsigned char *
alchemy_rarities(void)
{
    struct obj o;
    int monc, total;
    unsigned char *value;
    
    // Dummy object
    o.oclass = ROCK_CLASS;
    o.otyp = ROCK;
    
    // Get the number of items 
    do_alchemy_id(&o, &monc, &total, NULL, FALSE, 0);
    value = malloc(total);

    // Now fill in the table
    do_alchemy_id(&o, &monc, &total, value, FALSE, 0);
    
    return value;
}

static int alchemy_compar(const void *v1, const void *v2)
{
    const unsigned short *s1 = v1;
    const unsigned short *s2 = v2;
    int i1 = *s1;
    int i2 = *s2;
    return (i1 - i2);
}

/* Reverse alchemy_pack. Returns the number of IDs, which are returned
 * in *out. This always fills in all 4 possible IDs, even when returning
 * a shorter array - the remaining IDs are zero filled (ALC_NONE).
 * Also optionally returns the number of monsters.
 **/
int alchemy_unpack(out, in, monsters)
unsigned short *out;
long in;
int *monsters;
{
    static int nmons;
    static int nobjs = -1;
    unsigned long packed = (unsigned long)in;
    int i;

    /* Determine maxima */
    if (nobjs < 0) {
        alchemy_max_id(&nmons, &nobjs);
        nobjs -= nmons;
    }

    /* Unpack the 4 fields */
    out[0] = packed % nmons;
    packed /= nmons;
    out[1] = packed % nmons;
    packed /= nmons;
    out[2] = packed % nobjs;
    packed /= nobjs;
    out[3] = packed % nobjs;
    packed /= nobjs;
    
    /* Packed now contains the number of monsters, so adjust IDs */
    for(i=0;i<4;i++) {
        if (out[i] == 0) break;
        if (i >= (int)packed) out[i] += nmons-1;
    }

    assert(i);
    if (monsters) *monsters = (int)packed;
    return i;
}

/* Faff about with arithmetic coding to fit all IDs needed for a recipe
 * into a long... which must be unique, so they are also sorted.
 * Of these 0, 1 or 2 may be monsters and the number of monsters is also
 * part of the encoding.
 * 
 * Spaces beyond the number of IDs contain zero (ALC_NONE) which can be used
 * as a sentinel to determine the number of IDs
 **/
long
alchemy_pack(cid, ids)
const unsigned short *cid;
int ids;
{
    unsigned short id[4];
    int monsters = 0;
    int i;
    static int nmons;
    static int nobjs = -1;
    unsigned long packed = 0;
    assert(ids <= 4);
    assert(ids > 0);
    
    /* Determine maxima */
    if (nobjs < 0) {
        alchemy_max_id(&nmons, &nobjs);
        nobjs -= nmons;
        
        /* Ensure that object and monster lists haven't grown too big,
         * by comparing with 2^32 / 3
         */
        assert(nobjs*nobjs*nmons*nmons < 1431655765);
    }
    
    /* Copy in and sort into ascending order, i.e. monsters first */
    memcpy(id, cid, sizeof(*id)*ids);
    qsort(id, ids, sizeof(id[0]), alchemy_compar);
    
    /* Determine the number of monsters, and get all IDs into range */
    for(i=0;i<ids;i++) {
        assert(id[i]);
        if (id[i] < nmons)
            monsters++;
        else
            id[i] -= (nmons-1);
        assert(id[i]);
    }
    assert(monsters <= 2);
    
    /* Pack */
    packed = id[0];
    if (ids > 1)
        packed += (id[1] * nmons);
    if (ids > 2)
        packed += (id[2] * nmons * nmons);
    if (ids > 3)
        packed += (id[3] * nmons * nmons * nobjs);
    packed += (monsters * nmons * nmons * nobjs * nobjs);
    
    return (long)packed;
}

/*
 * Start an explode timeout on the given object. There had better not
 * be an explode timer already running on the object.
 *
 * age = # of turns until explosion
 *
 * This is a "silent" routine - it should not print anything out.
 */
void
begin_explode(obj, already_lit)
struct obj *obj;
boolean already_lit;
{
    if (obj->age == 0)
        return;

    if (start_timer(1, TIMER_OBJECT, EXPLODE_OBJECT, obj_to_any(obj))) {
        if (carried(obj) && !already_lit)
            update_inventory();
    }
}

/*
 * Stop an explode timeout on the given object if timer attached.
 */
void
end_explode(obj, timer_attached)
struct obj *obj;
boolean timer_attached;
{
    if (!timer_attached) {
        if (obj->where == OBJ_INVENT)
            update_inventory();
    } else if (!stop_timer(EXPLODE_OBJECT, obj_to_any(obj)))
        impossible("end_explode: obj %s not timed!", xname(obj));
}

/*
 * Called when a potion (fuming acid) explodes.
 */
void
explode_potion(arg, timeout)
anything *arg;
long timeout;
{
    struct obj *obj = arg->a_obj;
    boolean canseeit, many, need_newsym, need_invupdate;
    xchar x, y;
    char whose[BUFSZ];
    char line[BUFSZ];
    const char *name;

    many = obj->quan > 1L;

    /* timeout while away */
    if (timeout != monstermoves) {
        long how_long = monstermoves - timeout;

        if (how_long >= obj->age) {
            obj->age = 0;
            end_explode(obj, FALSE);
            obj_extract_self(obj);
            obfree(obj, (struct obj *) 0);
            obj = (struct obj *) 0;
        } else {
            obj->age -= how_long;
            begin_explode(obj, TRUE);
        }
        return;
    }
    
    name = (obj->where == OBJ_FLOOR) ? doname(obj) : cxname(obj);

    /* only interested in INVENT, FLOOR, and MINVENT */
    if (get_obj_location(obj, &x, &y, 0)) {
        canseeit = !Blind && cansee(x, y);
        /* set `whose[]' to be "Your " or "Fred's " or "The goblin's " */
        (void) Shk_Your(whose, obj);
    } else {
        canseeit = FALSE;
    }
    need_newsym = need_invupdate = FALSE;

    if ((int)obj->age <= 1) {
        obj->age = 0;
        if (canseeit || obj->where == OBJ_INVENT) {
            switch (obj->where) {
            case OBJ_INVENT:
                need_invupdate = TRUE;
                /*FALLTHRU*/
            case OBJ_MINVENT:
                pline("%s%s explode%s!", whose, name, many ? "" : "s");
                need_newsym = TRUE;
                break;
            case OBJ_FLOOR:
                You_see("%s explode!", name);
                need_newsym = TRUE;
                break;
            default:
                You_see("%s explode!", name);
                need_newsym = TRUE;
                break;
            }
        }
        end_explode(obj, FALSE);
        {
            int ouch = d(obj->quan, 9)+3;
            /* it would be better to use up the whole stack in advance
               of the message, but we can't because we need to keep it
               around for potionbreathe() [and we can't set obj->in_use
               to 'amt' because that's not implemented] */
            obj->in_use = 1;
            wake_nearto(x, y, (BOLT_LIM + 1) * (BOLT_LIM + 1));
            
            if ((u.ux == x) && (u.uy == y)) {
                if (!breathless(youmonst.data) || haseyes(youmonst.data))
                    potionbreathe(obj);
                exercise(A_STR, FALSE);
                exercise(A_WIS, FALSE);
            }

            killer.format = KILLED_BY_AN;
            strcpy(killer.name, "alchemic blast");
            explode(x, y, 10, ouch, /* not physical damage */
                   POTION_CLASS, EXPL_MAGICAL);
     
            if (obj->where == OBJ_INVENT)
                useupall(obj);
            else {
                obj_extract_self(obj);
                obfree(obj, (struct obj *) 0);
                obj = (struct obj *) 0;
            }
        }
    } else {
        obj->age--;
        /* Give them a warning... */
        if (!u.uroleplay.kaboom && canseeit) {
            static const char *smokes[] = {
                "glow%s", "smoke%s", "boil%s", "froth%s", "bubble%s"
            };
            static const char *hsmokes[] = {
                "sweat%s", "light%s a cigarette", "get%s angry", "turn%s into beer", "blow%s bubbles"
            };
            int r = rn2(obj->age + 50);
            const char *smoke = NULL;

            if (r < (int)(sizeof(smokes)/sizeof(smokes[0])))
                smoke = (Hallucination ? hsmokes : smokes)[r];

            if (smoke) {
                switch (obj->where) {
                    case OBJ_INVENT:
                    case OBJ_MINVENT:
                        sprintf(line, "%s%s %s!", whose, name, smoke);
                        pline(line, many ? "" : "s");
                        break;
                    case OBJ_FLOOR:
                        sprintf(line, "You see %s %s!", name, smoke);
                        pline(line, "");
                        break;
                }
            }
        }
    }
    
    if (obj && obj->age)
        begin_explode(obj, TRUE);
    if (need_newsym)
        newsym(x, y);
    if (need_invupdate)
        update_inventory();
}

/* #dip command */
int
dodip()
{
    static const char Dip_[] = "Dip ";
    register struct obj *potion, *obj;
    struct obj *singlepotion;
    uchar here;
    char allowall[2];
    short mixture;
    char qbuf[QBUFSZ], obuf[QBUFSZ];
    const char *shortestname; /* last resort obj name for prompt */

    allowall[0] = ALL_CLASSES;
    allowall[1] = '\0';
    if (!(obj = getobj(allowall, "dip")))
        return 0;
    if (inaccessible_equipment(obj, "dip", FALSE))
        return 0;

    shortestname = (is_plural(obj) || pair_of(obj)) ? "them" : "it";
    /*
     * Bypass safe_qbuf() since it doesn't handle varying suffix without
     * an awful lot of support work.  Format the object once, even though
     * the fountain and pool prompts offer a lot more room for it.
     * 3.6.0 used thesimpleoname() unconditionally, which posed no risk
     * of buffer overflow but drew bug reports because it omits user-
     * supplied type name.
     * getobj: "What do you want to dip <the object> into? [xyz or ?*] "
     */
    Strcpy(obuf, short_oname(obj, doname, thesimpleoname,
                             /* 128 - (24 + 54 + 1) leaves 49 for <object> */
                             QBUFSZ - sizeof "What do you want to dip \
 into? [abdeghjkmnpqstvwyzBCEFHIKLNOQRTUWXZ#-# or ?*] "));

    here = levl[u.ux][u.uy].typ;
    /* Is there a fountain to dip into here? */
    if (IS_FOUNTAIN(here)) {
        Sprintf(qbuf, "%s%s into the fountain?", Dip_,
                flags.verbose ? obuf : shortestname);
        /* "Dip <the object> into the fountain?" */
        if (yn(qbuf) == 'y') {
            dipfountain(obj);
            return 1;
        }
    } else if (IS_FURNACE(here)) {
        Sprintf(qbuf, "%s%s into the furnace?", Dip_,
                flags.verbose ? obuf : shortestname);
        /* "Dip <the object> into the fountain?" */
        if (yn(qbuf) == 'y') {
            dipfurnace(obj);
            return 1;
        }
    } else if (is_pool(u.ux, u.uy)) {
        const char *pooltype = waterbody_name(u.ux, u.uy);

        Sprintf(qbuf, "%s%s into the %s?", Dip_,
                flags.verbose ? obuf : shortestname, pooltype);
        /* "Dip <the object> into the {pool, moat, &c}?" */
        if (yn(qbuf) == 'y') {
            if (Levitation) {
                floating_above(pooltype);
            } else if (u.usteed && !is_swimmer(u.usteed->data)
                       && P_SKILL(P_RIDING) < P_BASIC) {
                rider_cant_reach(); /* not skilled enough to reach */
            } else {
                if (obj->otyp == POT_ACID)
                    obj->in_use = 1;
                if (water_damage(obj, 0, TRUE) != ER_DESTROYED && obj->in_use)
                    useup(obj);
            }
            return 1;
        }
    }

    /* "What do you want to dip <the object> into? [xyz or ?*] " */
    Sprintf(qbuf, "dip %s into", flags.verbose ? obuf : shortestname);
    potion = getobj(dippables, qbuf);
    if (!potion)
        return 0;
    if (potion == obj && potion->quan == 1L) {
        pline("That is a potion bottle, not a Klein bottle!");
        return 0;
    }
    potion->in_use = TRUE; /* assume it will be used up */
    if (potion->otyp == POT_WATER) {
        boolean useeit = !Blind || (obj == ublindf && Blindfolded_only);
        const char *obj_glows = Yobjnam2(obj, "glow");

        if (H2Opotion_dip(potion, obj, useeit, obj_glows))
            goto poof;
    } else if (obj->otyp == POT_POLYMORPH || potion->otyp == POT_POLYMORPH) {
        /* some objects can't be polymorphed */
        if (obj->otyp == potion->otyp /* both POT_POLY */
            || obj->otyp == WAN_POLYMORPH || obj->otyp == SPE_POLYMORPH
            || obj == uball || obj == uskin
            || obj_resists(obj->otyp == POT_POLYMORPH ? potion : obj,
                           5, 95)) {
            pline1(nothing_happens);
        } else {
            short save_otyp = obj->otyp;

            /* KMH, conduct */
            if(!u.uconduct.polypiles++)
                livelog_printf(LL_CONDUCT, "polymorphed %s first item", uhis());

            obj = poly_obj(obj, STRANGE_OBJECT);

            /*
             * obj might be gone:
             *  poly_obj() -> set_wear() -> Amulet_on() -> useup()
             * if obj->otyp is worn amulet and becomes AMULET_OF_CHANGE.
             */
            if (!obj) {
                makeknown(POT_POLYMORPH);
                use_skill(P_ALCHEMY, 1);
                return 1;
            } else if (obj->otyp != save_otyp) {
                makeknown(POT_POLYMORPH);
                use_skill(P_ALCHEMY, 1);
                useup(potion);
                prinv((char *) 0, obj, 0L);
                return 1;
            } else {
                pline("Nothing seems to happen.");
                goto poof;
            }
        }
        potion->in_use = FALSE; /* didn't go poof */
        return 1;
    } else if ((potion->otyp == POT_ACID) && Subrole_if(SR_ALCHEMIST)) {
        unsigned short id = alchemy_id(obj);
        boolean kaboom = FALSE;

        if (id > 1) {
            /* Chance of explosion per ingredient added.
             * Larger stacks of acid are more likely to fail.
             * If either the ingredient or the acid is cursed, also.
             * If you are impaired you are also likely to fail here.
             * (Conventional alchemy doesn't have such a check. But
             *  then, it doesn't require the Alchemist's special
             * skill...)
             **/
            int chance = obj->quan + 13 + u.ulevel + Luck;
            if (obj->cursed) chance = (chance / 2) + 1;
            if (obj->blessed) chance += 3;
            if (potion->cursed) chance = (chance / 2) + 1;
            if (potion->blessed) chance += 3;
            if (Blind) {
                chance = 1;
                pline("You are unable to see what you are doing, and so...");
            } else if (Hallucination) {
                chance = 1;
                pline("Being so trippy, you screw up and...");
            } else if (Confusion) {
                chance = 1;
                pline("Being so confused, you do something very wrong and...");
            } else if (Glib || Fumbling) {
                chance = 1;
                pline("The bottle slips from your %s fingers and falls...", (Glib ? "greasy" : "clumsy"));
            }
            if (rn2(chance) < obj->quan + 1)
                kaboom = TRUE;
            else {
                unsigned short ids[4] = { 0 };
                int monsters, i;
                int nids = 0;
                
                if (potion->ovar1)
                    nids = alchemy_unpack(ids, potion->ovar1, &monsters);

                /* Are there already 4 ingredients, or 2 monsters, in here? */
                if ((nids >= 4) || (monsters >= 2))
                    kaboom = TRUE;
                else {
                    int failrate = 0;
                    int success = -1;
                    /* Add the ingredient */
                    ids[nids] = id;
                    nids++;
                    potion->ovar1 = alchemy_pack(ids, nids);
                    
                    /* Does this produce anything? */
                    for(i = 0; i < (int)(sizeof(u.alchemy) / sizeof(u.alchemy[0])); i++) {
                        if (potion->ovar1 == u.alchemy[i].base) {
                            /* It's matched the recipe. But do you succeed in producing the
                             * potion?
                             */
                            failrate = alchemy_failrate(i + POT_GAIN_ABILITY);
                            if (rn2(1000) < failrate)
                                kaboom = TRUE;
                            else
                                success = i;
                        }
                    }
                    
                    if (success >= 0) {
                        /* Success! */
                        potion->ovar1 = 0;
                        potion->otyp = success + POT_GAIN_ABILITY;
                        pline_The("mixture looks %s.", hcolor(OBJ_DESCR(objects[potion->otyp])));
                        useup(obj);

                        /* Train the skill more for difficult recipes */
                        use_skill(P_ALCHEMY, 1+(failrate / 20));
                        return 1;
                    } else if (!kaboom) {
                        /* Didn't match, but didn't explode either */
                        pline_The("mixture bubbles, then settles.");
                        use_skill(P_ALCHEMY, 1);
                        useup(obj);
                        /* If this is the first ingredient, start an explode timer.
                         * It's based on Luck and BUC of the acid.
                         *
                         * Minimum time in turns. It's not safe to have horrible luck or
                         * for it to be cursed. The last turn is turn 1, so effectively
                         * you get one less than this.
                         * 
                         *              -13     0       13
                         * Blessed      2+d4    19+d15  27+d21
                         * Uncursed     2+d3    15+d12  24+d16
                         * Cursed       1+d2    2+d3    3+d4
                         * 
                         */
                        if (nids == 1) {
                            int min, range;
                            min = Luck + 16;
                            if (min > 16)
                                min -= (Luck / 2);
                            if (potion->cursed)
                                min = (min / 9)+2;
                            if (potion->blessed)
                                min += (min / 4);
                            range = min;
                            if (range > 8)
                                range -= (range - 8) / 2;
                            if (potion->blessed)
                                range++;
                            potion->age = rn2(range) + min;
                            begin_explode(potion, FALSE);
                        }
                        return 1;
                    }
                }
            }
        } else {
            /* If it's not an ingredient, blow it up */
            kaboom = TRUE;
        } 
        if (kaboom) {
            int ouch = d(potion->quan, 9);
        /* it would be better to use up the whole stack in advance
               of the message, but we can't because we need to keep it
               around for potionbreathe() [and we can't set obj->in_use
               to 'amt' because that's not implemented] */
            obj->in_use = 1;
            pline("BOOM!  It explodes!");

            /* Well, now you know what *not* to do :) */
            use_skill(P_ALCHEMY, 1);
            wake_nearto(u.ux, u.uy, (BOLT_LIM + 1) * (BOLT_LIM + 1));
            exercise(A_STR, FALSE);
            if ((obj->cursed) || (potion->cursed))
				exercise(A_WIS, FALSE);
            if (!breathless(youmonst.data) || haseyes(youmonst.data))
                potionbreathe(obj);
            useup(obj);
            useupall(potion);
            killer.format = KILLED_BY_AN;
            strcpy(killer.name, "alchemic blast");
            explode(u.ux, u.uy, 10, ouch, /* not physical damage */
                   POTION_CLASS, EXPL_MAGICAL);
            return 1;
        }
    } else if (obj->oclass == POTION_CLASS && obj->otyp != potion->otyp) {
        int amt = (int) obj->quan;
        boolean magic;

        mixture = mixtype(obj, potion);

        magic = (mixture != STRANGE_OBJECT) ? objects[mixture].oc_magic
            : (objects[obj->otyp].oc_magic || objects[potion->otyp].oc_magic);
        Strcpy(qbuf, "The"); /* assume full stack */
        if (amt > (magic ? 3 : 7)) {
            /* trying to dip multiple potions will usually affect only a
               subset; pick an amount between 3 and 8, inclusive, for magic
               potion result, between 7 and N for non-magic */
            if (magic)
                amt = rnd(min(amt, 8) - (3 - 1)) + (3 - 1); /* 1..6 + 2 */
            else
                amt = rnd(amt - (7 - 1)) + (7 - 1); /* 1..(N-6) + 6 */

            if ((long) amt < obj->quan) {
                obj = splitobj(obj, (long) amt);
                Sprintf(qbuf, "%ld of the", obj->quan);
            }
        }
        /* [N of] the {obj(s)} mix(es) with [one of] {the potion}... */
        pline("%s %s %s with %s%s...", qbuf, simpleonames(obj),
              otense(obj, "mix"), (potion->quan > 1L) ? "one of " : "",
              thesimpleoname(potion));
        /* get rid of 'dippee' before potential perm_invent updates */
        useup(potion); /* now gone */
        /* Mixing potions is dangerous...
           KMH, balance patch -- acid is particularly unstable */
        if (obj->cursed || obj->otyp == POT_ACID || !rn2(10)) {
            int skill;
            /* it would be better to use up the whole stack in advance
               of the message, but we can't because we need to keep it
               around for potionbreathe() [and we can't set obj->in_use
               to 'amt' because that's not implemented] */
            obj->in_use = 1;
            pline("%sThey explode!", !Deaf ? "BOOM!  " : "");
            wake_nearto(u.ux, u.uy, (BOLT_LIM + 1) * (BOLT_LIM + 1));
            exercise(A_STR, FALSE);
            if (obj->cursed || obj->otyp == POT_ACID)
				exercise(A_WIS, FALSE);
            if (!breathless(youmonst.data) || haseyes(youmonst.data))
                potionbreathe(obj);

            /* Gain alchemy skill if the result is useful, especially if
             * the recipe isn't guaranteed to succeed.
             */
            switch(mixture) {
                case POT_GAIN_LEVEL:
                    skill = 3;
                    break;
                case POT_GAIN_ABILITY:
                case POT_FULL_HEALING:
                case POT_ENLIGHTENMENT:
                    skill = 2;
                    break;
                case POT_EXTRA_HEALING:
                case POT_SEE_INVISIBLE:
                    skill = 1;
                    break;
                default:
                    skill = 0;
            }
            if (skill) use_skill(P_ALCHEMY, skill);

            useupall(obj);
            losehp(amt + rnd(9), /* not physical damage */
                   "alchemic blast", KILLED_BY_AN);
            return 1;
        }

        obj->blessed = obj->cursed = obj->bknown = 0;
        if (Blind || Hallucination)
            obj->dknown = 0;

        if (mixture != STRANGE_OBJECT) {
            obj->otyp = mixture;
        } else {
            struct obj *otmp;

            switch (obj->odiluted ? 1 : rnd(8)) {
            case 1:
                obj->otyp = POT_WATER;
                break;
            case 2:
            case 3:
                obj->otyp = POT_SICKNESS;
                break;
            case 4:
                otmp = mkobj(POTION_CLASS, FALSE);
                obj->otyp = otmp->otyp;
                obfree(otmp, (struct obj *) 0);
                break;
            default:
                useupall(obj);
                pline_The("mixture %sevaporates.",
                          !Blind ? "glows brightly and " : "");
                return 1;
            }
        }
        obj->odiluted = ((obj->otyp != POT_WATER) && (obj->otyp != POT_OIL));

        if (obj->otyp == POT_WATER && !Hallucination) {
            pline_The("mixture bubbles%s.", Blind ? "" : ", then clears");
        } else if (!Blind) {
            pline_The("mixture looks %s.",
                      hcolor(OBJ_DESCR(objects[obj->otyp])));
        }

        /* this is required when 'obj' was split off from a bigger stack,
           so that 'obj' will now be assigned its own inventory slot;
           it has a side-effect of merging 'obj' into another compatible
           stack if there is one, so we do it even when no split has
           been made in order to get the merge result for both cases;
           as a consequence, mixing while Fumbling drops the mixture */
        freeinv(obj);
        (void) hold_another_object(obj, "You drop %s!", doname(obj),
                                   (const char *) 0);
        return 1;
    }

    if (potion->otyp == POT_ACID && obj->otyp == CORPSE
        && (obj->corpsenm == PM_LICHEN || obj->corpsenm == PM_LEGENDARY_LICHEN)
        && !Blind) {
        static boolean first = TRUE;
        if (first) {
            first = FALSE;
            use_skill(P_ALCHEMY, 1);
        }
        pline("%s %s %s around the edges.", The(cxname(obj)),
              otense(obj, "turn"), Blind ? "wrinkled"
                                   : potion->odiluted ? hcolor(NH_ORANGE)
                                     : hcolor(NH_RED));
        potion->in_use = FALSE; /* didn't go poof */
        if (potion->dknown
            && !objects[potion->otyp].oc_name_known
            && !objects[potion->otyp].oc_uname)
            docall(potion);
        return 1;
    }

    if (potion->otyp == POT_WATER && obj->otyp == TOWEL) {
        pline_The("towel soaks it up!");
        /* wetting towel already done via water_damage() in H2Opotion_dip */
        goto poof;
    }

    if (is_poisonable(obj) && !obj->oartifact) {
        if (potion->otyp == POT_SICKNESS && !obj->opoisoned) {
            char buf[BUFSZ];

            if (potion->quan > 1L)
                Sprintf(buf, "One of %s", the(xname(potion)));
            else
                Strcpy(buf, The(xname(potion)));
            pline("%s forms a coating on %s.", buf, the(xname(obj)));
            obj->opoisoned = TRUE;
            goto poof;
        } else if (obj->opoisoned && (potion->otyp == POT_HEALING
                                      || potion->otyp == POT_EXTRA_HEALING
                                      || potion->otyp == POT_FULL_HEALING)) {
            pline("A coating wears off %s.", the(xname(obj)));
            obj->opoisoned = 0;
            goto poof;
        }
    }

    if (potion->otyp == POT_ACID) {
        if (erode_obj(obj, 0, ERODE_CORRODE, EF_GREASE) != ER_NOTHING)
            goto poof;
    }

    if (potion->otyp == POT_OIL) {
        boolean wisx = FALSE;

        if (potion->lamplit) { /* burning */
            fire_damage(obj, TRUE, u.ux, u.uy);
        } else if (potion->cursed) {
            pline_The("potion spills and covers your %s with oil.",
                      fingers_or_gloves(TRUE));
            make_glib((int) (Glib & TIMEOUT) + d(2, 10));
        } else if (obj->oclass != WEAPON_CLASS && !is_weptool(obj)) {
            /* the following cases apply only to weapons */
            goto more_dips;
            /* Oil removes rust and corrosion, but doesn't unburn.
             * Arrows, etc are classed as metallic due to arrowhead
             * material, but dipping in oil shouldn't repair them.
             */
        } else if ((!is_rustprone(obj) && !is_corrodeable(obj))
                   || is_ammo(obj) || (!obj->oeroded && !obj->oeroded2)) {
            /* uses up potion, doesn't set obj->greased */
            if (!Blind)
                pline("%s %s with an oily sheen.", Yname2(obj),
                      otense(obj, "gleam"));
            else /*if (!uarmg)*/
                pline("%s %s oily.", Yname2(obj), otense(obj, "feel"));
        } else {
            pline("%s %s less %s.", Yname2(obj),
                  otense(obj, !Blind ? "are" : "feel"),
                  (obj->oeroded && obj->oeroded2)
                      ? "corroded and rusty"
                      : obj->oeroded ? "rusty" : "corroded");
            if (obj->oeroded > 0)
                obj->oeroded--;
            if (obj->oeroded2 > 0)
                obj->oeroded2--;
            wisx = TRUE;
        }
        exercise(A_WIS, wisx);
        if (potion->dknown)
            makeknown(potion->otyp);
        useup(potion);
        return 1;
    }
 more_dips:

    /* Allow filling of MAGIC_LAMPs to prevent identification by player */
    if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP)
        && (potion->otyp == POT_OIL)) {
        /* Turn off engine before fueling, turn off fuel too :-)  */
        if (obj->lamplit || potion->lamplit) {
            useup(potion);
            explode(u.ux, u.uy, 11, d(6, 6), 0, EXPL_FIERY);
            exercise(A_WIS, FALSE);
            return 1;
        }
        /* Adding oil to an empty magic lamp renders it into an oil lamp */
        if ((obj->otyp == MAGIC_LAMP) && obj->spe == 0) {
            obj->otyp = OIL_LAMP;
            obj->age = 0;
        }
        if (obj->age > 1000L) {
            pline("%s %s full.", Yname2(obj), otense(obj, "are"));
            potion->in_use = FALSE; /* didn't go poof */
        } else {
            You("fill %s with oil.", yname(obj));
            check_unpaid(potion);        /* Yendorian Fuel Tax */
            /* burns more efficiently in a lamp than in a bottle;
               diluted potion provides less benefit but we don't attempt
               to track that the lamp now also has some non-oil in it */
            obj->age += (!potion->odiluted ? 4L : 3L) * potion->age / 2L;
            if (obj->age > 1500L)
                obj->age = 1500L;
            useup(potion);
            exercise(A_WIS, TRUE);
        }
        if (potion->dknown)
            makeknown(POT_OIL);
        obj->spe = 1;
        update_inventory();
        return 1;
    }

    potion->in_use = FALSE; /* didn't go poof */
    if ((obj->otyp == UNICORN_HORN || obj->otyp == AMETHYST)
        && (mixture = mixtype(obj, potion)) != STRANGE_OBJECT) {
        char oldbuf[BUFSZ], newbuf[BUFSZ];
        short old_otyp = potion->otyp;
        boolean old_dknown = FALSE;
        boolean more_than_one = potion->quan > 1L;

        oldbuf[0] = '\0';
        if (potion->dknown) {
            old_dknown = TRUE;
            Sprintf(oldbuf, "%s ", hcolor(OBJ_DESCR(objects[potion->otyp])));
        }
        /* with multiple merged potions, split off one and
           just clear it */
        if (potion->quan > 1L) {
            singlepotion = splitobj(potion, 1L);
        } else
            singlepotion = potion;

        costly_alteration(singlepotion, COST_NUTRLZ);
        singlepotion->otyp = mixture;
        singlepotion->blessed = 0;
        if (mixture == POT_WATER)
            singlepotion->cursed = singlepotion->odiluted = 0;
        else
            singlepotion->cursed = obj->cursed; /* odiluted left as-is */
        singlepotion->bknown = FALSE;
        if (Blind) {
            singlepotion->dknown = FALSE;
        } else {
            singlepotion->dknown = !Hallucination;
            *newbuf = '\0';
            if (mixture == POT_WATER && singlepotion->dknown)
                Sprintf(newbuf, "clears");
            else if (!Blind)
                Sprintf(newbuf, "turns %s",
                        hcolor(OBJ_DESCR(objects[mixture])));
            if (*newbuf)
                pline_The("%spotion%s %s.", oldbuf,
                          more_than_one ? " that you dipped into" : "",
                          newbuf);
            else
                pline("Something happens.");

            if (old_dknown
                && !objects[old_otyp].oc_name_known
                && !objects[old_otyp].oc_uname) {
                struct obj fakeobj;

                fakeobj = zeroobj;
                fakeobj.dknown = 1;
                fakeobj.otyp = old_otyp;
                fakeobj.oclass = POTION_CLASS;
                docall(&fakeobj);
            }
        }
        obj_extract_self(singlepotion);
        singlepotion = hold_another_object(singlepotion,
                                           "You juggle and drop %s!",
                                           doname(singlepotion),
                                           (const char *) 0);
        nhUse(singlepotion);
        update_inventory();
        return 1;
    }

    pline("Interesting...");
    return 1;

 poof:
    if (potion->dknown
        && !objects[potion->otyp].oc_name_known
        && !objects[potion->otyp].oc_uname)
        docall(potion);
    useup(potion);
    return 1;
}

/* *monp grants a wish and then leaves the game */
void
mongrantswish(monp)
struct monst **monp;
{
    struct monst *mon = *monp;
    int mx = mon->mx, my = mon->my, glyph = glyph_at(mx, my);

    /* remove the monster first in case wish proves to be fatal
       (blasted by artifact), to keep it out of resulting bones file */
    mongone(mon);
    *monp = 0; /* inform caller that monster is gone */
    /* hide that removal from player--map is visible during wish prompt */
    tmp_at(DISP_ALWAYS, glyph);
    tmp_at(mx, my);
    /* grant the wish */
    makewish();
    /* clean up */
    tmp_at(DISP_END, 0);
}

void
djinni_from_bottle(obj)
struct obj *obj;
{
    struct monst *mtmp;
    int chance;

    if (!(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))) {
        pline("It turns out to be empty.");
        return;
    }

    if (!Blind) {
        pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
        pline("%s speaks.", Monnam(mtmp));
    } else {
        You("smell acrid fumes.");
        pline("%s speaks.", Something);
    }

    chance = rn2(5);
    if (obj->blessed)
        chance = (chance == 4) ? rnd(4) : 0;
    else if (obj->cursed)
        chance = (chance == 0) ? rn2(4) : 4;
    /* 0,1,2,3,4:  b=80%,5,5,5,5; nc=20%,20,20,20,20; c=5%,5,5,5,80 */

    switch (chance) {
    case 0:
        verbalize("I am in your debt.  I will grant one wish!");
        /* give a wish and discard the monster (mtmp set to null) */
        mongrantswish(&mtmp);
        break;
    case 1:
        verbalize("Thank you for freeing me!");
        (void) tamedog(mtmp, (struct obj *) 0);
        break;
    case 2:
        verbalize("You freed me!");
        mtmp->mpeaceful = TRUE;
        set_malign(mtmp);
        break;
    case 3:
        verbalize("It is about time!");
        if (canspotmon(mtmp))
            pline("%s vanishes.", Monnam(mtmp));
        mongone(mtmp);
        break;
    default:
        verbalize("You disturbed me, fool!");
        mtmp->mpeaceful = FALSE;
        set_malign(mtmp);
        break;
    }
}

/* clone a gremlin or mold (2nd arg non-null implies heat as the trigger);
   hit points are cut in half (odd HP stays with original) */
struct monst *
split_mon(mon, mtmp)
struct monst *mon,  /* monster being split */
             *mtmp; /* optional attacker whose heat triggered it */
{
    struct monst *mtmp2;
    char reason[BUFSZ];

    reason[0] = '\0';
    if (mtmp)
        Sprintf(reason, " from %s heat",
                (mtmp == &youmonst) ? the_your[1]
                                    : (const char *) s_suffix(mon_nam(mtmp)));

    if (mon == &youmonst) {
        mtmp2 = cloneu();
        if (mtmp2) {
            mtmp2->mhpmax = u.mhmax / 2;
            u.mhmax -= mtmp2->mhpmax;
            context.botl = 1;
            You("multiply%s!", reason);
        }
    } else {
        mtmp2 = clone_mon(mon, 0, 0);
        if (mtmp2) {
            mtmp2->mhpmax = mon->mhpmax / 2;
            mon->mhpmax -= mtmp2->mhpmax;
            if (canspotmon(mon))
                pline("%s multiplies%s!", Monnam(mon), reason);
        }
    }
    return mtmp2;
}

/*potion.c*/

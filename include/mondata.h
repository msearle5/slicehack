/* NetHack 3.6	mondata.h	$NHDT-Date: 1576626512 2019/12/17 23:48:32 $  $NHDT-Branch: NetHack-3.6 $:$NHDT-Revision: 1.39 $ */
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */
/* Edited 5/12/18 by NullCGT */

#ifndef MONDATA_H
#define MONDATA_H

#define verysmall(ptr) ((ptr)->msize < MZ_SMALL)
#define bigmonst(ptr) ((ptr)->msize >= MZ_LARGE)
#define biggermonst(ptr) ((ptr)->msize > (youmonst.data)->msize)

#define pm_resistance(ptr, typ) (((ptr)->mresists & (typ)) != 0)

#define resists_fire(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_FIRE) != 0)
#define resists_cold(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_COLD) != 0)
#define resists_sleep(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_SLEEP) != 0)
#define resists_disint(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_DISINT) != 0)
#define resists_elec(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_ELEC) != 0)
#define resists_poison(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_POISON) != 0)
#define resists_acid(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_ACID) != 0)
#define resists_ston(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_STONE) != 0)
#define resists_sonic(mon) \
    ((((mon)->data->mresists | (mon)->mextrinsics) & MR_SONIC) != 0)
#define resists_psychic(mon) \
     ((((mon)->data->mresists | (mon)->mextrinsics) & MR_PSYCHIC) != 0)

#define has_telepathy(mon) \
    (telepathic((mon)->data) || ((mon)->mextrinsics & MR2_TELEPATHY) != 0)

#define can_wwalk(mon) (((mon)->mextrinsics & MR2_WATERWALK) != 0)
#define can_jump(mon)  (((mon)->mextrinsics & MR2_JUMPING) != 0)
#define teleporter(mon) (((mon)->mextrinsics & MR2_TELEPORT) != 0 || \
                          (mon->data->mflags1 & M1_TPORT) != 0L)
#define has_displacement(mon) (((mon)->mextrinsics & MR2_DISPLACED) != 0)

#define resists_mgc(ptr) \
    (dmgtype(ptr, AD_MAGM) || ptr == &mons[PM_BABY_GRAY_DRAGON] \
     || dmgtype(ptr, AD_RBRE)) /* Chromatic Dragon */

#define resists_drain(ptr) \
    (is_undead(ptr) || is_demon(ptr) || is_were(ptr) \
     || ptr == &mons[PM_DEATH] || ptr == &mons[PM_GRIM_REAPER])
/* is_were() doesn't handle hero in human form */

#define resists_death(ptr) \
    ((ptr == &mons[PM_MOVANIC_DEVA]) || \
     (ptr == &mons[PM_MONADIC_DEVA]) || \
     (ptr == &mons[PM_ASTRAL_DEVA]) || \
     (ptr == &mons[PM_PLANETAR]) || \
     (ptr == &mons[PM_SOLAR]))

#define is_lminion(mon) \
    (is_minion((mon)->data) && mon_aligntyp(mon) == A_LAWFUL)
#define is_jumper(ptr) (((ptr)->mflags3 & M3_JUMPER) != 0L)
#define is_flyer(ptr) (((ptr)->mflags1 & M1_FLY) != 0L)
#define is_floater(ptr) ((ptr)->mlet == S_EYE || (ptr)->mlet == S_LIGHT)
/* clinger: piercers, mimics, wumpus -- generally don't fall down holes */
#define is_clinger(ptr) (((ptr)->mflags1 & M1_CLING) != 0L)
#define grounded(ptr) (!is_flyer(ptr) && !is_floater(ptr) && !is_clinger(ptr))
#define is_swimmer(ptr) (((ptr)->mflags1 & M1_SWIM) != 0L)
#define is_lemming(ptr)	(((ptr)->mlet == S_LEPRECHAUN) && ((ptr) != &mons[PM_LEPRECHAUN]))
#define breathless(ptr) (((ptr)->mflags1 & M1_BREATHLESS) != 0L)
#define amphibious(ptr) \
    (((ptr)->mflags1 & (M1_AMPHIBIOUS | M1_BREATHLESS)) != 0L)
#define passes_walls(ptr) (((ptr)->mflags1 & M1_WALLWALK) != 0L)
#define amorphous(ptr) (((ptr)->mflags1 & M1_AMORPHOUS) != 0L)
#define noncorporeal(ptr) ((ptr)->mlet == S_GHOST)
#define tunnels(ptr) (((ptr)->mflags1 & M1_TUNNEL) != 0L)
#define needspick(ptr) (((ptr)->mflags1 & M1_NEEDPICK) != 0L)
/* hides_under() requires an object at the location in order to hide */
#define hides_under(ptr) (((ptr)->mflags1 & M1_CONCEAL) != 0L)
/* is_hider() is True for mimics but when hiding they appear as something
   else rather than become mon->mundetected, so use is_hider() with care */
#define is_hider(ptr) (((ptr)->mflags1 & M1_HIDE) != 0L)
/* piercers cling to the ceiling; lurkers above are hiders but they fly
   so aren't classified as clingers; unfortunately mimics are classified
   as both hiders and clingers but have nothing to do with ceilings;
   wumpuses (not wumpi :-) cling but aren't hiders */
#define ceiling_hider(ptr) \
    (is_hider(ptr) && ((is_clinger(ptr) && (ptr)->mlet != S_MIMIC) \
                       || is_flyer(ptr))) /* lurker above */
#define haseyes(ptr) (((ptr)->mflags1 & M1_NOEYES) == 0L)
/* used to decide whether plural applies so no need for 'more than 2' */
#define eyecount(ptr) \
    (!haseyes(ptr) ? 0                                                     \
     : ((ptr) == &mons[PM_CYCLOPS] || (ptr) == &mons[PM_FLOATING_EYE]) ? 1 \
       : 2)
#define nohands(ptr) (((ptr)->mflags1 & M1_NOHANDS) != 0L)
#define nolimbs(ptr) (((ptr)->mflags1 & M1_NOLIMBS) == M1_NOLIMBS)
#define notake(ptr) (((ptr)->mflags1 & M1_NOTAKE) != 0L)
#define has_head(ptr) (((ptr)->mflags1 & M1_NOHEAD) == 0L)
#define has_horns(ptr) (num_horns(ptr) > 0)
#define has_beak(ptr) (is_bird(ptr) || \
                        (ptr) == &mons[PM_TENGU] || \
                        (ptr) == &mons[PM_FELL_BEAST] || \
                        (ptr) == &mons[PM_VROCK])
#define is_whirly(ptr) \
    ((ptr)->mlet == S_VORTEX || (ptr) == &mons[PM_AIR_ELEMENTAL] || \
      (ptr) == &mons[PM_TASMANIAN_DEVIL])
#define flaming(ptr)                                                     \
    ((ptr) == &mons[PM_FIRE_VORTEX] || (ptr) == &mons[PM_FLAMING_SPHERE] \
     || (ptr) == &mons[PM_FIRE_ELEMENTAL] || (ptr) == &mons[PM_SALAMANDER] \
     || (ptr) == &mons[PM_HELLBEAR])
#define is_silent(ptr) ((ptr)->msound == MS_SILENT)
#define mon_flammable(ptr) (((ptr) == &mons[PM_WOOD_GOLEM]) || ((ptr) == &mons[PM_STRAW_GOLEM]) \
    || ((ptr) == &mons[PM_PAPER_GOLEM]) || ((ptr) == &mons[PM_GREEN_SLIME]) \
    || ((ptr) == &mons[PM_WALKING_OAK]) || ((ptr) == &mons[PM_WALKING_BIRCH]) \
    || ((ptr) == &mons[PM_WALKING_WILLOW]))
#define unsolid(ptr) (((ptr)->mflags1 & M1_UNSOLID) != 0L)
#define mindless(ptr) (((ptr)->mflags1 & M1_MINDLESS) != 0L)
#define humanoid(ptr) (((ptr)->mflags1 & M1_HUMANOID) != 0L)
#define is_animal(ptr) (((ptr)->mflags1 & M1_ANIMAL) != 0L)
#define slithy(ptr) (((ptr)->mflags1 & M1_SLITHY) != 0L)
#define is_wooden(ptr) ((ptr) == &mons[PM_WOOD_GOLEM] || (ptr->mlet) == S_PLANT)
#define thick_skinned(ptr) (((ptr)->mflags1 & M1_THICK_HIDE) != 0L)
#define hug_throttles(ptr) ((ptr) == &mons[PM_ROPE_GOLEM])
#define slimeproof(ptr) \
    ((ptr) == &mons[PM_GREEN_SLIME] || flaming(ptr) || noncorporeal(ptr))
#define lays_eggs(ptr) (((ptr)->mflags1 & M1_OVIPAROUS) != 0L)
#define eggs_in_water(ptr) \
    (lays_eggs(ptr) && (ptr)->mlet == S_EEL && is_swimmer(ptr))
#define regenerates(ptr) (((ptr)->mflags1 & M1_REGEN) != 0L)
#define perceives(ptr) (((ptr)->mflags1 & M1_SEE_INVIS) != 0L)
#define can_teleport(ptr) (((ptr)->mflags1 & M1_TPORT) != 0L)
#define control_teleport(ptr) (((ptr)->mflags1 & M1_TPORT_CNTRL) != 0L)
#define telepathic(ptr)                                                \
    ((ptr) == &mons[PM_FLOATING_EYE] || is_mind_flayer(ptr))
#define is_armed(ptr) attacktype(ptr, AT_WEAP)
#define acidic(ptr) (((ptr)->mflags1 & M1_ACID) != 0L)
#define poisonous(ptr) (((ptr)->mflags1 & M1_POIS) != 0L)
#define carnivorous(ptr) (((ptr)->mflags1 & M1_CARNIVORE) != 0L)
#define herbivorous(ptr) (((ptr)->mflags1 & M1_HERBIVORE) != 0L)
#define metallivorous(ptr) (((ptr)->mflags1 & M1_METALLIVORE) != 0L)
#define polyok(ptr) (((ptr)->mflags2 & M2_NOPOLY) == 0L)
#define is_shapeshifter(ptr) (((ptr)->mflags2 & M2_SHAPESHIFTER) != 0L)
#define is_undead(ptr) (((ptr)->mhflags & MH_UNDEAD) != 0L)
#define is_were(ptr) (((ptr)->mhflags & MH_WERE) != 0L)
#define is_summoner(ptr) (((ptr)->mflags4 & M4_SUMMONER) != 0L)
/* return TRUE if the monster tends to revive */
#define is_reviver(ptr) (((ptr)->mflags4 & M4_REVIVE) != 0L)
#define is_elf(ptr) (((ptr)->mhflags & MH_ELF) != 0L)
#define is_dwarf(ptr) (((ptr)->mhflags & MH_DWARF) != 0L)
#define is_gnome(ptr) (((ptr)->mhflags & MH_GNOME) != 0L)
#define is_orc(ptr) (((ptr)->mhflags & MH_ORC) != 0L)
#define is_human(ptr) (((ptr)->mhflags & MH_HUMAN) != 0L)
#define your_race(ptr) (((ptr)->mflags2 & urace.selfmask) != 0L)
#define is_bat(ptr) (((ptr)->mflags3 & M3_BAT) != 0L)
#define is_bird(ptr) ((ptr)->mlet == S_BAT && !is_bat(ptr))
# define is_rat(ptr) ((ptr) == &mons[PM_SEWER_RAT] || \
				 (ptr) == &mons[PM_GIANT_RAT] || \
				 (ptr) == &mons[PM_RABID_RAT] || \
				 (ptr) == &mons[PM_ENORMOUS_RAT] || \
				 (ptr) == &mons[PM_PACKRAT] || \
				 (ptr) == &mons[PM_HELLRAT] || \
				 (ptr) == &mons[PM_RODENT_OF_UNUSUAL_SIZE])
#define is_giant(ptr) (((ptr)->mhflags & MH_GIANT) != 0L)
#define is_marsupial(ptr) ((ptr) == &mons[PM_WALLABY] || \
                          (ptr) == &mons[PM_WALLAROO] || \
                          (ptr) == &mons[PM_KANGAROO])
#define is_golem(ptr) ((ptr)->mlet == S_GOLEM)

#define is_domestic(ptr) ((((ptr)->mflags2 & M2_DOMESTIC) != 0L) \
     || (Role_if(PM_DRAGONMASTER) && (((ptr) >= &mons[PM_BABY_GRAY_DRAGON] && \
                             (ptr) <= &mons[PM_GREEN_DRAGON]) || \
                             (ptr) == &mons[PM_YELLOW_DRAGON])))
#define is_demon(ptr) (((ptr)->mhflags & MH_DEMON) != 0L)

#define is_jumper(ptr) (((ptr)->mflags3 & M3_JUMPER) != 0L)
#define is_mind_flayer(ptr) (((ptr)->mhflags & MH_FLAYER) != 0L)
#define is_ghoul(ptr) (((ptr)->mhflags & MH_GHOUL) != 0L)
#define is_mercenary(ptr) (((ptr)->mflags2 & M2_MERC) != 0L)
#define is_male(ptr) (((ptr)->mflags2 & M2_MALE) != 0L)
#define is_female(ptr) (((ptr)->mflags2 & M2_FEMALE) != 0L)
#define is_neuter(ptr) (((ptr)->mflags2 & M2_NEUTER) != 0L)
#define is_wanderer(ptr) (((ptr)->mflags2 & M2_WANDER) != 0L)
#define always_hostile(ptr) (((ptr)->mflags2 & M2_HOSTILE) != 0L)
#define always_peaceful(ptr) (((ptr)->mflags2 & M2_PEACEFUL) != 0L)
#define race_hostile(ptr) (((ptr)->mhflags & urace.hatemask) != 0L)
#define race_peaceful(ptr) (((ptr)->mhflags & urace.lovemask) != 0L)
#define extra_nasty(ptr) (((ptr)->mflags2 & M2_NASTY) != 0L)
#define strongmonst(ptr) (((ptr)->mflags2 & M2_STRONG) != 0L)
#define can_breathe(ptr) attacktype(ptr, AT_BREA)
#define cantwield(ptr) (nohands(ptr) || verysmall(ptr))
#define could_twoweap(ptr) ((ptr)->mattk[1].aatyp == AT_WEAP)
#define cantweararm(ptr) (breakarm(ptr) || sliparm(ptr))
#define throws_rocks(ptr) (((ptr)->mflags2 & M2_ROCKTHROW) != 0L)
#define type_is_pname(ptr) (((ptr)->mflags2 & M2_PNAME) != 0L)
#define is_dragon(ptr) ((ptr) >= &mons[PM_BABY_GRAY_DRAGON] && \
                        (ptr) <= &mons[PM_VOID_DRAGON])
#define is_baby_dragon(ptr) ((ptr) >= &mons[PM_BABY_GRAY_DRAGON] && \
                        (ptr) <= &mons[PM_BABY_YELLOW_DRAGON])
#define is_feline(ptr) ((ptr)->mlet == S_FELINE)
#define is_lord(ptr) (((ptr)->mflags2 & M2_LORD) != 0L)
#define is_prince(ptr) (((ptr)->mflags2 & M2_PRINCE) != 0L)
#define is_ndemon(ptr) \
    (is_demon(ptr) && (((ptr)->mflags2 & (M2_LORD | M2_PRINCE)) == 0L))
#define is_dlord(ptr) (is_demon(ptr) && is_lord(ptr))
#define is_dprince(ptr) (is_demon(ptr) && is_prince(ptr))
#define is_minion(ptr) (((ptr)->mflags2 & M2_MINION) != 0L)
#define likes_gold(ptr) (((ptr)->mflags2 & M2_GREEDY) != 0L)
#define likes_gems(ptr) (((ptr)->mflags2 & M2_JEWELS) != 0L)
#define likes_objs(ptr) (((ptr)->mflags2 & M2_COLLECT) != 0L || is_armed(ptr))
#define likes_magic(ptr) (((ptr)->mflags2 & M2_MAGIC) != 0L)
#define webmaker(ptr) \
    ((ptr) == &mons[PM_CAVE_SPIDER] || (ptr) == &mons[PM_GIANT_SPIDER])
#define is_unicorn(ptr) ((ptr)->mlet == S_UNICORN && likes_gems(ptr))
#define is_longworm(ptr)                                                   \
    (((ptr) == &mons[PM_BABY_LONG_WORM]) || ((ptr) == &mons[PM_LONG_WORM]) \
     || ((ptr) == &mons[PM_LONG_WORM_TAIL]))
#define is_covetous(ptr) (((ptr)->mflags3 & M3_COVETOUS))
#define infravision(ptr) (((ptr)->mflags3 & M3_INFRAVISION))
#define infravisible(ptr) (((ptr)->mflags3 & M3_INFRAVISIBLE))
#define is_displacer(ptr) (((ptr)->mflags3 & M3_DISPLACES) != 0L)
#define is_displaced(ptr) ((ptr) == &mons[PM_SHIMMERING_DRAGON] || \
    (ptr) == &mons[PM_BABY_SHIMMERING_DRAGON] || \
    (ptr) == &mons[PM_CHESHIRE_CAT])
#define is_organized(ptr)  (((ptr)->mflags3 & M3_ORGANIZED))
#define is_mplayer(ptr) \
    (((ptr) >= &mons[PM_ARCHEOLOGIST]) && ((ptr) <= &mons[PM_WIZARD]))
#define is_watch(ptr) \
    ((ptr) == &mons[PM_WATCHMAN] || (ptr) == &mons[PM_WATCH_CAPTAIN])
#define is_rider(ptr)                                      \
    ((ptr) == &mons[PM_DEATH] || (ptr) == &mons[PM_FAMINE] \
     || (ptr) == &mons[PM_PESTILENCE])
#define is_vampire(ptr)  (((ptr)->mhflags & MH_VAMPIRE) != 0L)
/* Rider corpses are treated as non-rotting so that attempting to eat one
   will be sure to reach the stage of eating where that meal is fatal */
#define nonrotting_corpse(mnum) \
    ((mnum) == PM_LIZARD || (mnum) == PM_LICHEN || \
     (mnum) == PM_LEGENDARY_LICHEN || \
     (mnum) == PM_DEATH_MAGGOT || is_rider(&mons[mnum]))

#define is_silver(ptr) \
    ((ptr) == &mons[PM_SILVER_GOLEM])

#define is_placeholder(ptr) (((ptr)->mflags3 & M3_PLACEHOLDER) != 0L)
/* monsters whose corpses and statues need special handling;
   note that high priests and the Wizard of Yendor are flagged
   as unique even though they really aren't; that's ok here */
#define unique_corpstat(ptr) (((ptr)->geno & G_UNIQ) != 0)

/* this returns the light's range, or 0 if none; if we add more light emitting
   monsters, we'll likely have to add a new light range field to mons[] */
#define emits_light(ptr)                                          \
    (((ptr)->mlet == S_LIGHT || (ptr) == &mons[PM_FLAMING_SPHERE] \
      || (ptr) == &mons[PM_SHOCKING_SPHERE]                       \
      || (ptr) == &mons[PM_FIRE_VORTEX]                          \
      || (ptr) == &mons[PM_GOLD_DRAGON]                          \
      || (ptr) == &mons[PM_BABY_GOLD_DRAGON]                     \
      || (ptr) == &mons[PM_WAX_GOLEM])                           \
         ? 1                                                     \
         : ((ptr) == &mons[PM_FIRE_ELEMENTAL]) ? 1 : 0)
/*	[note: the light ranges above were reduced to 1 for performance...] */
#define likes_lava(ptr) \
    (ptr == &mons[PM_FIRE_ELEMENTAL] || ptr == &mons[PM_SALAMANDER] \
          || ptr == &mons[PM_MAGMA_ELEMENTAL] \
          || ptr == &mons[PM_VENT_WURM])
#define pm_invisible(ptr) \
    ((ptr) == &mons[PM_STALKER] || (ptr) == &mons[PM_BLACK_LIGHT] \
      || (ptr) == &mons[PM_HELLCAT])

/* could probably add more */
#define likes_fire(ptr)                                                  \
    ((ptr) == &mons[PM_FIRE_VORTEX] || (ptr) == &mons[PM_FLAMING_SPHERE] \
     || likes_lava(ptr))

#define touch_petrifies(ptr) \
    ((ptr) == &mons[PM_COCKATRICE] || (ptr) == &mons[PM_CHICKATRICE] \
     || (ptr) == &mons[PM_WERECOCKATRICE] || (ptr) == &mons[PM_HUMAN_WERECOCKATRICE])

/* monster types that cause hero to be turned into stone if eaten */
#define flesh_petrifies(pm) (touch_petrifies(pm) || (pm) == &mons[PM_MEDUSA])

#define is_pirate(ptr) \
    ((ptr) == &mons[PM_PIRATE] || (ptr) == &mons[PM_SKELETAL_PIRATE] \
      || (ptr) == &mons[PM_DAMNED_PIRATE] || (ptr) == &mons[PM_PLANAR_PIRATE] \
      || (ptr) == &mons[PM_MAYOR_CUMMERBUND] || (ptr) == &mons[PM_PIRATE_BROTHER])
#define is_shopkeeper(ptr) \
    ((ptr) == &mons[PM_SHOPKEEPER] || (ptr) == &mons[PM_EXTRAPLANAR_MERCHANT])

#define hates_light(ptr) (((ptr)->mflags4 & M4_HATESLIGHT) != 0L)

/* used to vary a few messages */
#define weirdnonliving(ptr) (is_golem(ptr) || (ptr)->mlet == S_VORTEX)
#define nonliving(ptr) \
    (is_undead(ptr) || (ptr) == &mons[PM_MANES] || weirdnonliving(ptr))

/* no corpse (ie, blank scrolls) if killed by fire */
#define completelyburns(ptr) \
    ((ptr) == &mons[PM_PAPER_GOLEM] || (ptr) == &mons[PM_STRAW_GOLEM] \
      || (ptr) == &mons[PM_WAX_GOLEM])

/* Used for conduct with corpses, tins, and digestion attacks */
/* G_NOCORPSE monsters might still be swallowed as a purple worm */
/* Maybe someday this could be in mflags... */
#define vegan(ptr)                                                 \
    ((ptr)->mlet == S_BLOB || (ptr)->mlet == S_JELLY               \
     || (ptr)->mlet == S_FUNGUS || (ptr)->mlet == S_VORTEX         \
     || (ptr)->mlet == S_LIGHT                                     \
     || ((ptr)->mlet == S_ELEMENTAL && (ptr) != &mons[PM_STALKER]) \
     || ((ptr)->mlet == S_GOLEM && (ptr) != &mons[PM_FLESH_GOLEM]  \
         && (ptr) != &mons[PM_LEATHER_GOLEM]) || noncorporeal(ptr))
#define vegetarian(ptr) \
    (vegan(ptr)         \
     || ((ptr)->mlet == S_PUDDING && (ptr) != &mons[PM_BLACK_PUDDING]))

/* monkeys are tameable via bananas but not pacifiable via food,
   otherwise their theft attack could be nullified too easily;
   dogs and cats can be tamed by anything they like to eat and are
   pacified by any other food;
   horses can be tamed by always-veggy food or lichen corpses but
   not tamed or pacified by other corpses or tins of veggy critters */
#define befriend_with_obj(ptr, obj) \
    (((ptr) == &mons[PM_MONKEY] || (ptr) == &mons[PM_APE])               \
     ? (obj)->otyp == BANANA                                             \
     : (((ptr) == &mons[PM_BABY_PENGUIN]) || ((ptr) == &mons[PM_PENGUIN]) || ((ptr) == &mons[PM_EMPEROR_PENGUIN])) \
      ? (obj)->otyp == find_skate()                                      \
      : ((ptr) == &mons[PM_KOALA])                                       \
      ? (obj)->otyp == EUCALYPTUS_LEAF                                   \
       : ((is_domestic(ptr) ||                                           \
        (is_rat(ptr) && (Role_if(PM_CONVICT) || (obj)->otyp == CHEESE))) \
          && (obj)->oclass == FOOD_CLASS                                 \
          && ((ptr)->mlet != S_UNICORN                                   \
              || obj->material == VEGGY                                  \
              || ((obj)->otyp == CORPSE && ((obj)->corpsenm == PM_LICHEN \
                    || (obj)->corpsenm == PM_LEGENDARY_LICHEN)))))

/* For vampires */
#define has_blood(ptr)	(!vegetarian(ptr) && \
				   (ptr)->mlet != S_GOLEM && \
				   (!is_undead(ptr) || is_vampire(ptr)))

#define is_blkmktstaff(ptr)	(Is_blackmarket(&u.uz) && \
				  (ptr) == &mons[PM_ARMS_DEALER])

#endif /* MONDATA_H */

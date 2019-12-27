/* NetHack 3.6	u_init.c	$NHDT-Date: 1575245094 2019/12/02 00:04:54 $  $NHDT-Branch: NetHack-3.6 $:$NHDT-Revision: 1.60 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2017. */
/* NetHack may be freely redistributed.  See license for details. */

/* Edited on 5/8/18 by NullCGT */

#include "hack.h"
#include <assert.h>

struct trobj {
    short trotyp;
    schar trspe;
    char trclass;
    Bitfield(trquan, 6);
    Bitfield(trbless, 2);
};

STATIC_DCL void FDECL(ini_inv, (struct trobj *));
STATIC_DCL void FDECL(knows_object, (int));
STATIC_DCL void FDECL(knows_class, (CHAR_P));
STATIC_DCL boolean FDECL(restricted_spell_discipline, (int));

#define UNDEF_TYP 0
#define UNDEF_SPE '\177'
#define UNDEF_BLESS 2

/*
 *      Initial inventory for the various roles.
 */

static struct trobj Archeologist[] = {
    /* if adventure has a name...  idea from tan@uvm-gen */
    { BULLWHIP, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
    { JACKET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FEDORA, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
    { PICK_AXE, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
    { TINNING_KIT, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
    { TOUCHSTONE, 0, GEM_CLASS, 1, 0 },
    { SACK, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Barbarian[] = {
#define B_MAJOR 0 /* two-handed sword or battle-axe  */
#define B_MINOR 1 /* matched with axe or short sword */
    { TWO_HANDED_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { AXE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { RING_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Cartomancer[] = {
    { HAWAIIAN_SHIRT, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { DAGGER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SHURIKEN, 0, GEM_CLASS, 60, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 4, UNDEF_BLESS },
    { SCR_CREATE_MONSTER, 0, SCROLL_CLASS, 1, UNDEF_BLESS },
    { SCR_CREATE_MONSTER, 1, SCROLL_CLASS, 1, UNDEF_BLESS },
    { SCR_CREATE_MONSTER, 2, SCROLL_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Cave_man[] = {
#define C_AMMO 2
    { CLUB, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SLING, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
    { FLINT, 0, GEM_CLASS, 15, UNDEF_BLESS }, /* quan is variable */
    { ROCK, 0, GEM_CLASS, 3, 0 },             /* yields 18..33 */
    { LIGHT_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Dragonmaster[] = {
    { BROADSWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SCALE_MAIL, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
    { TRIPE_RATION, 0, FOOD_CLASS, 2, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Convict[] = {
	{ ROCK, 0, GEM_CLASS, 1, 0 },
	{ STRIPED_SHIRT, 0, ARMOR_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Healer[] = {
    { SCALPEL, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { GLOVES, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { STETHOSCOPE, 0, TOOL_CLASS, 1, 0 },
    { MEDICAL_KIT, 0, TOOL_CLASS, 1, 0 },
    { POT_HEALING, 0, POTION_CLASS, 4, UNDEF_BLESS },
    { POT_EXTRA_HEALING, 0, POTION_CLASS, 4, UNDEF_BLESS },
    { WAN_SLEEP, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
    /* always blessed, so it's guaranteed readable */
    { SPE_HEALING, 0, SPBOOK_CLASS, 1, 1 },
    { SPE_EXTRA_HEALING, 0, SPBOOK_CLASS, 1, 1 },
    { SPE_STONE_TO_FLESH, 0, SPBOOK_CLASS, 1, 1 },
    { APPLE, 0, FOOD_CLASS, 5, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Knight[] = {
    { LONG_SWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { LANCE, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { RING_MAIL, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { SMALL_SHIELD, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { APPLE, 0, FOOD_CLASS, 10, 0 },
    { CARROT, 0, FOOD_CLASS, 10, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Knight1[] = {
    { HELMET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Knight2[] = {
    { GAUNTLETS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Monk[] = {
#define M_BOOK 2
    { GLOVES, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
    { MYSTIC_ROBE, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
    { POT_HEALING, 0, POTION_CLASS, 3, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
    { APPLE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
    { ORANGE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
    /* Yes, we know fortune cookies aren't really from China.  They were
     * invented by George Jung in Los Angeles, California, USA in 1916.
     */
    { FORTUNE_COOKIE, 0, FOOD_CLASS, 3, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Pirate[] = {
#define PIR_KNIVES	1
#define PIR_SNACK 5
#define PIR_JEWELRY 7
#define PIR_TOOL 8
	{ SCIMITAR, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ KNIFE, 1, WEAPON_CLASS, 2, 0 },
	{ JACKET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ HIGH_BOOTS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ CRAM_RATION, 0, FOOD_CLASS, 2, UNDEF_BLESS },
	{ BANANA, 0, FOOD_CLASS, 3, 0 },
	{ POT_BOOZE, 0, POTION_CLASS, 3, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ OILSKIN_SACK, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Priest[] = {
    { MACE, 1, WEAPON_CLASS, 1, 1 },
    { MYSTIC_ROBE, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { SMALL_SHIELD, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { POT_WATER, 0, POTION_CLASS, 4, 1 }, /* holy water */
    { CLOVE_OF_GARLIC, 0, FOOD_CLASS, 1, 0 },
    { SPRIG_OF_WOLFSBANE, 0, FOOD_CLASS, 1, 0 },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 2, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Ranger[] = {
#define RAN_BOW 1
#define RAN_TWO_ARROWS 2
#define RAN_ZERO_ARROWS 3
    { DAGGER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { BOW, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { ARROW, 2, WEAPON_CLASS, 50, UNDEF_BLESS },
    { ARROW, 0, WEAPON_CLASS, 30, UNDEF_BLESS },
    { CLOAK_OF_DISPLACEMENT, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
    { CRAM_RATION, 0, FOOD_CLASS, 4, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Rogue[] = {
#define R_DAGGERS 1
    { SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { DAGGER, 0, WEAPON_CLASS, 10, 0 }, /* quan is variable */
    { LIGHT_ARMOR, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { POT_SICKNESS, 0, POTION_CLASS, 1, 0 },
    { LOCK_PICK, 0, TOOL_CLASS, 1, 0 },
    { SACK, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Samurai[] = {
#define S_ARROWS 3
    { KATANA, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS }, /* wakizashi */
    { YUMI, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { YA, 0, WEAPON_CLASS, 25, UNDEF_BLESS }, /* variable quan */
    { SPLINT_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Tourist[] = {
#define T_DARTS 0
    { DART, 2, WEAPON_CLASS, 25, UNDEF_BLESS }, /* quan is variable */
    { UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 10, 0 },
    { POT_EXTRA_HEALING, 0, POTION_CLASS, 2, UNDEF_BLESS },
    { SCR_MAGIC_MAPPING, 0, SCROLL_CLASS, 4, UNDEF_BLESS },
    { HAWAIIAN_SHIRT, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { EXPENSIVE_CAMERA, UNDEF_SPE, TOOL_CLASS, 1, 0 },
    { CREDIT_CARD, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Valkyrie[] = {
    { LONG_SWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { DAGGER, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SMALL_SHIELD, 3, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Wizard[] = {
    { WIZARDSTAFF, 1, WEAPON_CLASS, 1, 1 },
    { CLOAK_OF_MAGIC_RESISTANCE, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, RING_CLASS, 2, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 3, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 3, UNDEF_BLESS },
    { SPE_FORCE_BOLT, 0, SPBOOK_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Alchemist[] = {
#define A_POTION 2
#define A_ACID 3
    { QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },
    { ALCHEMY_SMOCK, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
    { POT_ACID, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
    { SPE_FORCE_BOLT, 0, SPBOOK_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};

/*
 *      Optional extra inventory items.
 */

static struct trobj Tinopener[] = { { TIN_OPENER, 0, TOOL_CLASS, 1, 0 },
                                    { 0, 0, 0, 0, 0 } };
static struct trobj Magicmarker[] = { { MAGIC_MARKER, UNDEF_SPE, TOOL_CLASS,
                                        1, 0 },
                                      { 0, 0, 0, 0, 0 } };
static struct trobj Lamp[] = { { OIL_LAMP, 1, TOOL_CLASS, 1, 0 },
                               { 0, 0, 0, 0, 0 } };
static struct trobj Wand[] = { { UNDEF_TYP, UNDEF_SPE, WAND_CLASS, 1, 0 },
                               { 0, 0, 0, 0, 0 } };
static struct trobj Ring[] = { { UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, 0 },
                               { 0, 0, 0, 0, 0 } };
static struct trobj Scroll[] = { { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, 0 },
                               { 0, 0, 0, 0, 0 } };
static struct trobj Blindfold[] = { { BLINDFOLD, 0, TOOL_CLASS, 1, 0 },
                                    { 0, 0, 0, 0, 0 } };
static struct trobj Instrument[] = { { FLUTE, 0, TOOL_CLASS, 1, 0 },
                                     { 0, 0, 0, 0, 0 } };
static struct trobj Xtra_food[] = { { UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 2, 0 },
                                    { 0, 0, 0, 0, 0 } };
static struct trobj Leash[] = { { LEASH, 0, TOOL_CLASS, 1, 0 },
                                { 0, 0, 0, 0, 0 } };
static struct trobj Towel[] = { { TOWEL, 0, TOOL_CLASS, 1, 0 },
                                { 0, 0, 0, 0, 0 } };
static struct trobj Wishing[] = { { WAN_WISHING, 3, WAND_CLASS, 1, 0 },
                                  { 0, 0, 0, 0, 0 } };
static struct trobj Money[] = { { GOLD_PIECE, 0, COIN_CLASS, 1, 0 },
                                { 0, 0, 0, 0, 0 } };
static struct trobj Gem[] = { { UNDEF_TYP, 0, GEM_CLASS, 1, 0 },
                                { 0, 0, 0, 0, 0 } };

/* race-based substitutions for initial inventory;
   the weaker cloak for elven rangers is intentional--they shoot better */
static struct inv_sub {
    short race_pm, item_otyp, subs_otyp;
} inv_subs[] = {
    { PM_ELF, DAGGER, ELVEN_DAGGER },
    { PM_ELF, SPEAR, ELVEN_SPEAR },
    { PM_ELF, SHORT_SWORD, ELVEN_SHORT_SWORD },
    { PM_ELF, BOW, ELVEN_BOW },
    { PM_ELF, ARROW, ELVEN_ARROW },
    { PM_ELF, HELMET, ELVEN_HELM },
    /* { PM_ELF, SMALL_SHIELD, ELVEN_SHIELD }, */
    { PM_ELF, CLOAK_OF_DISPLACEMENT, ELVEN_CLOAK },
    { PM_ELF, CRAM_RATION, LEMBAS_WAFER },
    { PM_MERFOLK, SACK, OILSKIN_SACK },
    { PM_MERFOLK, SPEAR, TRIDENT },
    { PM_MERFOLK, MACE, TRIDENT },
    { PM_MERFOLK, BOW, CROSSBOW },
    { PM_MERFOLK, ARROW, CROSSBOW_BOLT },
    { PM_ORC, DAGGER, ORCISH_DAGGER },
    { PM_ORC, SPEAR, ORCISH_SPEAR },
    { PM_ORC, SHORT_SWORD, ORCISH_SHORT_SWORD },
    { PM_ORC, BOW, ORCISH_BOW },
    { PM_ORC, ARROW, ORCISH_ARROW },
    { PM_ORC, HELMET, ORCISH_HELM },
    { PM_ORC, SMALL_SHIELD, ORCISH_SHIELD },
    { PM_ORC, RING_MAIL, ORCISH_RING_MAIL },
    { PM_ORC, CRAM_RATION, TRIPE_RATION },
    { PM_ORC, LEMBAS_WAFER, TRIPE_RATION },
    { PM_DWARF, SPEAR, DWARVISH_SPEAR },
    { PM_DWARF, SHORT_SWORD, DWARVISH_SHORT_SWORD },
    { PM_DWARF, HELMET, DWARVISH_HELM },
    /* { PM_DWARF, SMALL_SHIELD, DWARVISH_ROUNDSHIELD }, */
    /* { PM_DWARF, PICK_AXE, DWARVISH_MATTOCK }, */
    { PM_DWARF, LEMBAS_WAFER, CRAM_RATION },
    { PM_GNOME, BOW, CROSSBOW },
    { PM_GNOME, ARROW, CROSSBOW_BOLT },
    { PM_GIANT, HAWAIIAN_SHIRT, LOW_BOOTS },
    { PM_GIANT, MYSTIC_ROBE, LOW_BOOTS },
    { PM_GIANT, RING_MAIL, HIGH_BOOTS },
    { PM_GIANT, LIGHT_ARMOR, LOW_BOOTS },
    { PM_DROW, DAGGER, DARK_ELVEN_DAGGER },
    { PM_DROW, SHORT_SWORD, DARK_ELVEN_SHORT_SWORD },
    { PM_DROW, BOW, DARK_ELVEN_BOW },
    { PM_DROW, ARROW, DARK_ELVEN_ARROW },
    { PM_VAMPIRE, POT_FRUIT_JUICE, POT_BLOOD },
    { NON_PM, STRANGE_OBJECT, STRANGE_OBJECT }
};

static const struct def_skill Skill_A[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_BASIC },
    { P_PICK_AXE, P_EXPERT },
    { P_SHORT_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_EXPERT },
    { P_CLUB, P_SKILLED },
    { P_QUARTERSTAFF, P_SKILLED },
    { P_SLING, P_SKILLED },
    { P_DART, P_BASIC },
    { P_BOOMERANG, P_EXPERT },
    { P_WHIP, P_EXPERT },
    { P_UNICORN_HORN, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_BASIC },
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_COOKING, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_B[] = {
    { P_DAGGER, P_BASIC },
    { P_AXE, P_EXPERT },
    { P_PICK_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },
    { P_TWO_HANDED_SWORD, P_EXPERT },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_BASIC },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_BASIC },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_SKILLED },
    { P_BOW, P_BASIC },
    { P_ATTACK_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_BASIC }, /* special spell is haste self */
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Car[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_BASIC },
    { P_SHORT_SWORD, P_BASIC },
    { P_CLUB, P_BASIC },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_EXPERT },
    { P_ATTACK_SPELL, P_EXPERT },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
    { P_RIDING, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_COOKING, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_C[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },
    { P_PICK_AXE, P_BASIC },
    { P_CLUB, P_EXPERT },
    { P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_SKILLED },
    { P_HAMMER, P_SKILLED },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_EXPERT },
    { P_TRIDENT, P_SKILLED },
    { P_BOW, P_SKILLED },
    { P_SLING, P_EXPERT },
    { P_ATTACK_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_SKILLED },
    { P_BOOMERANG, P_EXPERT },
    { P_UNICORN_HORN, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Con[] = {
    { P_DAGGER, P_SKILLED },		
    { P_KNIFE,  P_EXPERT },
    { P_HAMMER, P_SKILLED },		
    { P_PICK_AXE, P_EXPERT },
    { P_CLUB, P_EXPERT },		    
    { P_MACE, P_BASIC },
    { P_DART, P_SKILLED },		    
    { P_FLAIL, P_EXPERT },
    { P_SHORT_SWORD, P_BASIC },		
    { P_SLING, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },	
    { P_ESCAPE_SPELL, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_COOKING, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_D[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_BASIC },
    { P_AXE, P_BASIC },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_BASIC },
    { P_BROAD_SWORD, P_EXPERT },
    { P_LONG_SWORD, P_BASIC },
    { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_SKILLED },
    { P_CLUB, P_BASIC },
    { P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_EXPERT },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_EXPERT },
    { P_SPEAR, P_EXPERT },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_EXPERT },
    { P_BOW, P_BASIC },
    { P_CROSSBOW, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_BASIC },
    { P_RIDING, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_COOKING, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_H[] = {
    { P_DAGGER, P_SKILLED },
    { P_KNIFE, P_EXPERT },
    { P_SHORT_SWORD, P_SKILLED },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_SKILLED },
    { P_UNICORN_HORN, P_EXPERT },
    { P_HEALING_SPELL, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_COOKING, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_K[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_BASIC },
    { P_AXE, P_SKILLED },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_SKILLED },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_SKILLED },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_SKILLED },
    { P_CLUB, P_BASIC },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_EXPERT },
    { P_BOW, P_BASIC },
    { P_CROSSBOW, P_SKILLED },
    { P_ATTACK_SPELL, P_SKILLED },
    { P_HEALING_SPELL, P_SKILLED },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_RIDING, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_COOKING, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Mon[] = {
    { P_QUARTERSTAFF, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_CROSSBOW, P_BASIC },
    { P_SHURIKEN, P_BASIC },
    { P_ATTACK_SPELL, P_BASIC },
    { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_BASIC },
    { P_MARTIAL_ARTS, P_GRAND_MASTER },
    { P_COOKING, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_P[] = {
    { P_CLUB, P_EXPERT },
    { P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_EXPERT },
    { P_FLAIL, P_EXPERT },
    { P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_SKILLED },
    { P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },
    { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },
    { P_DART, P_BASIC },
    { P_SHURIKEN, P_BASIC },
    { P_BOOMERANG, P_BASIC },
    { P_UNICORN_HORN, P_SKILLED },
    { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_CLERIC_SPELL, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_COOKING, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Pir[] = {
    { P_DAGGER, P_SKILLED },
    { P_KNIFE,  P_EXPERT },
    { P_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_BASIC },
   	{ P_BROAD_SWORD, P_EXPERT },
    { P_LONG_SWORD, P_BASIC },
   	{ P_SCIMITAR, P_EXPERT },
    { P_SABER, P_EXPERT },
   	{ P_CLUB, P_BASIC },
    { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_EXPERT },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_EXPERT },
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_SKILLED },
    { P_WHIP, P_SKILLED },
    { P_UNICORN_HORN, P_BASIC },
   	{ P_ATTACK_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_BASIC },
   	{ P_ENCHANTMENT_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_COOKING, P_EXPERT }, /* pirate chefs */
    { P_NONE, 0 }
};
static const struct def_skill Skill_R[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },
    { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_SKILLED },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_SKILLED },
    { P_DIVINATION_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_COOKING, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Ran[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_BASIC },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_SKILLED },
    { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_EXPERT },
    { P_TRIDENT, P_BASIC },
    { P_BOW, P_EXPERT },
    { P_SLING, P_EXPERT },
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_SKILLED },
    { P_BOOMERANG, P_EXPERT },
    { P_WHIP, P_BASIC },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_ESCAPE_SPELL, P_BASIC },
    { P_RIDING, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_COOKING, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_S[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_EXPERT },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_FLAIL, P_SKILLED },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_LANCE, P_SKILLED },
    { P_BOW, P_EXPERT },
    { P_SHURIKEN, P_EXPERT },
    { P_ATTACK_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_BASIC }, /* special spell is clairvoyance */
    { P_CLERIC_SPELL, P_SKILLED },
    { P_RIDING, P_SKILLED },
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_MARTIAL_ARTS, P_MASTER },
    { P_COOKING, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_T[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_BASIC },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_BASIC },
    { P_LONG_SWORD, P_BASIC },
    { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },
    { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },
    { P_BOOMERANG, P_BASIC },
    { P_WHIP, P_BASIC },
    { P_UNICORN_HORN, P_SKILLED },
    { P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_COOKING, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_V[] = {
    { P_DAGGER, P_EXPERT },
    { P_AXE, P_EXPERT },
    { P_PICK_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_SKILLED },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_EXPERT },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_SKILLED },
    { P_SLING, P_BASIC },
    { P_ATTACK_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_BASIC },
    { P_RIDING, P_SKILLED },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_COOKING, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_W[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_BASIC },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },
    { P_ATTACK_SPELL, P_EXPERT },
    { P_HEALING_SPELL, P_SKILLED },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
    { P_RIDING, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_COOKING, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Alchemist[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_BASIC },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },
    { P_ATTACK_SPELL, P_EXPERT },
    { P_HEALING_SPELL, P_SKILLED },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
    { P_RIDING, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_ALCHEMY, P_GRAND_MASTER },
    { P_NONE, 0 }
};

STATIC_OVL void
knows_object(obj)
register int obj;
{
    discover_object(obj, TRUE, FALSE);
    objects[obj].oc_pre_discovered = 1; /* not a "discovery" */
}

/* Know ordinary (non-magical) objects of a certain class,
 * like all gems except the loadstone and luckstone.
 */
STATIC_OVL void
knows_class(sym)
register char sym;
{
    register int ct;
    for (ct = 1; ct < NUM_OBJECTS; ct++)
        if (objects[ct].oc_class == sym && !objects[ct].oc_magic)
            knows_object(ct);
}

#define BASIC(X)        (X*1)
#define SKILLED(X)      (X*4)
#define EXPERT(X)       (X*16)
#define MASTER(X)       (X*80)
#define GRAND(X)        (X*400)

static const unsigned char *alchemy_val;

/* Initialize alchemy for one ingredient */
STATIC_OVL int
ini_ingredient(difficulty, targetid, others)
int difficulty;
int targetid;
unsigned short *others;
{
    static int nmons;
    static int nids = -1;
    static int id_gain_level;
    static int id_extra_healing;
    static int id_full_healing;
    static int id_gain_ability;
    static int id_confusion;
    static int id_hallucination;
    static int id_booze;
    static int id_enlightenment;
    static int id_see_invisible;
    static int id_fruit_juice;
    static int id_healing;
    int id = -1;
    
    /* First time, get the number of IDs */
    if (nids == -1) {
        alchemy_max_id(&nmons, &nids);
        id_gain_level = alchemy_otyp_id(POT_GAIN_LEVEL);
        id_extra_healing = alchemy_otyp_id(POT_EXTRA_HEALING);
        id_full_healing = alchemy_otyp_id(POT_FULL_HEALING);
        id_gain_ability = alchemy_otyp_id(POT_GAIN_ABILITY);
        id_confusion = alchemy_otyp_id(POT_CONFUSION);
        id_fruit_juice = alchemy_otyp_id(POT_FRUIT_JUICE);
        id_hallucination = alchemy_otyp_id(POT_HALLUCINATION);
        id_booze = alchemy_otyp_id(POT_BOOZE);
        id_enlightenment = alchemy_otyp_id(POT_ENLIGHTENMENT);
        id_see_invisible = alchemy_otyp_id(POT_SEE_INVISIBLE);
        id_healing = alchemy_otyp_id(POT_HEALING);
    }
    
    /* A random ingredient is picked by ID.
     * It is checked for bad things (same as others,
     * same as common alchemy, same as the target) and if it passes there
     * is a rarity roll which favors common items in low-level potions
     * and exotic items in high-level potions. (Including first going by
     * monster or not?)
     * 
     * Genocided monsters can't be checked for, since this is a one-time set up.
     * 
     * A recipe being the same as common alchemy is best checked here, not in
     * ini_potion, because of all the possible combinations (the same as common
     * alchemy plus an X is just as bad). It's OK instead to rule out all potions
     * that could contribute.
     * 
     * Checking for being the same as another recipe is done in ini_potion.
     */
    do {
        id = rn2(nids-2)+2;
        if (id == targetid) id = -1;            /* Don't require an X to make an X */
        for(int i=0;i<4;i++)
            if (id == others[i]) id = -1;       /* Don't require 2 X's to make a Y */

        /* Avoid common alchemy duplication. as are the usually useless ones and the effects of dipping
         * a unihorn or amethyst - as there is no recipe for water or juice.
         **/
        if (((targetid == id_extra_healing) && (id == id_healing)) ||
            ((targetid == id_full_healing) && (id == id_extra_healing)) ||
            ((targetid == id_gain_ability) && (id == id_full_healing)) ||
            ((targetid == id_enlightenment) && (id == id_confusion)) ||
            ((targetid == id_gain_level) && (id == id_enlightenment)) ||
            ((targetid == id_see_invisible) && (id == id_fruit_juice)) ||
            ((targetid == id_booze) && (id == id_fruit_juice)) ||
            ((targetid == id_confusion) && (id == id_enlightenment)) ||
            ((targetid == id_hallucination) && (id == id_booze)))
                id = -1;
        
        /* Retry rare items */
        if ((id >= 0) && rn2(alchemy_val[id] + difficulty) < alchemy_val[id])
            id = -1;
    } while (id < 0);

    assert(id);
    return id;
}

/* Initialize alchemy for one potion */
STATIC_OVL void
ini_potion(target, difficulty)
int target;
int difficulty;
{
    struct recipe *r = u.alchemy + (target - POT_GAIN_ABILITY);
    unsigned short id[5];
    int objects, rarity, monsters, bound, targetid;
    static int nmons = -1;
    int i;
    long packed;
    boolean retry;
    int init_difficulty, limit;
    r->difficulty = difficulty;
    
    /* First time, get the number of monsters */
    if (nmons == -1) {
        int nids;
        alchemy_max_id(&nmons, &nids);
    }
    
    /* Number of items required is loosely based on the difficulty.
     * Most potions are 2 or 3. This should be based on the rarity of the
     * items, though, not calculated up-front.
     **/

    /* Transform difficulty into 'sum of rarities' */
    rarity = difficulty;
    rarity *= 40;
    rarity = isqrt(rarity);
    rarity += 30;
    targetid = alchemy_otyp_id(target);
    init_difficulty = rarity;

     /* Loop until acceptable, each time increasing the acceptance window */
    bound = rarity / 20;
    do {
        memset(id, 0, sizeof(id));
        objects = 0;
        monsters = 0;
        difficulty = rarity;
        bound++;
        limit = 40;
        retry = FALSE;

        /* Loop until out of space or difficulty OK or too many monsters */
        do {
            /* Add an ingredient */
            id[objects] = ini_ingredient(difficulty, targetid, &id);
            if (id[objects] <= nmons) {
                if (monsters >= 2)
                    continue;
                monsters++;
            }
            limit++;
            /* Avoid any one item being too much of the value of the potion */
            if (alchemy_val[id[objects]] > ((init_difficulty * limit) / 100))
                continue;
            difficulty -= alchemy_val[id[objects]];
            objects++;
        } while ((difficulty > bound) && (objects < 4));
        if (monsters >= 2) retry = TRUE;
        if ((difficulty < -bound) || (difficulty > bound)) retry = TRUE;
        if (objects < 1) retry = TRUE;
    
        if (retry == FALSE) {
            /* Convert them to recipe form, and make sure it's not the same as a previous recipe */

            packed = alchemy_pack(id, objects);
            for(i = 0; i < (int)(sizeof(u.alchemy)/sizeof(u.alchemy[0])); i++)
                if (u.alchemy[i].base == packed)
                    retry = TRUE;

            r->base = packed;
        }
    } while (retry);
}

/* Initialize alchemy - roll up a new set of recipes */
STATIC_OVL void
ini_alchemy()
{
    int i, j;
    boolean retry = FALSE;

    /* Fill in the difficulty table */
    if (alchemy_val) free((void *)alchemy_val);
    alchemy_val = alchemy_rarities();
    
    do
    {
        /* Clear the alchemy information to 'unalchemizable' (difficulty 0) */
        memset(u.alchemy, 0, sizeof(u.alchemy));

        /* Potions in order of difficulty, from easiest to hardest.
         * Typical potions have around 10% chance of fail at their nominal skill level,
         * but you can try to go for higher ones with a multiplier (x4..5?)
         */

        /* Basic */
        ini_potion(POT_SICKNESS,            BASIC(5));
        ini_potion(POT_BOOZE,               BASIC(10));
        ini_potion(POT_SEE_INVISIBLE,       BASIC(13));
        ini_potion(POT_HEALING,             BASIC(15));

        /* Skilled */
        ini_potion(POT_CONFUSION,           SKILLED(5));
        ini_potion(POT_HALLUCINATION,       SKILLED(7));
        ini_potion(POT_SLEEPING,            SKILLED(8));
        ini_potion(POT_RESTORE_ABILITY,     SKILLED(13));
        ini_potion(POT_EXTRA_HEALING,       SKILLED(15));

        /* Expert */
        ini_potion(POT_BLINDNESS,           EXPERT(5));
        ini_potion(POT_OBJECT_DETECTION,    EXPERT(8));
        ini_potion(POT_MONSTER_DETECTION,   EXPERT(10));
        ini_potion(POT_INVISIBILITY,        EXPERT(16));
        ini_potion(POT_LEVITATION,          EXPERT(18));

        /* Master */
        ini_potion(POT_OIL,                 MASTER(5));
        ini_potion(POT_ENLIGHTENMENT,       MASTER(7));
        ini_potion(POT_SPEED,               MASTER(9));
        ini_potion(POT_GAIN_ENERGY,         MASTER(12));
        ini_potion(POT_POLYMORPH,           MASTER(17));
        ini_potion(POT_FULL_HEALING,        MASTER(20));

        /* Grand Master */
        ini_potion(POT_PARALYSIS,           GRAND(5));
        ini_potion(POT_REFLECTION,          GRAND(12));
        ini_potion(POT_GAIN_LEVEL,          GRAND(15));
        ini_potion(POT_GAIN_ABILITY,        GRAND(20));

        /* Ensure that no potions are unbrewable due to there being no way
         * to add the listed ingredients without passing through another
         * recipe first.
         */
        for(i=0;i<(int)(sizeof(u.alchemy)/sizeof(u.alchemy[0]));i++) {
            unsigned short id[5] = { 0 };
            int monsters;
            boolean skip = FALSE;
            if (u.alchemy[i].difficulty) {
                int nids = alchemy_unpack(id, u.alchemy[i].base, &monsters);
                if (nids > 1) {
                    static const short order2[] = { 0, -1, 1, -1 };
                    static const short order3[] = { 0, 1, -1, 0, 2, -1,
                                                    1, 0, -1, 1, 2, -1,
                                                    2, 0, -1, 2, 1, -1 };
                    static const short order4[] = { 3, 0, 1, -1, 3, 0, 2, -1,
                                                    3, 1, 0, -1, 3, 1, 2, -1,
                                                    3, 2, 0, -1, 3, 2, 1, -1,
                                                    0, 3, 1, -1, 0, 3, 2, -1,
                                                    1, 3, 0, -1, 1, 3, 2, -1,
                                                    2, 3, 0, -1, 2, 3, 1, -1,
                                                    0, 1, 3, -1, 0, 2, 3, -1,
                                                    1, 0, 3, -1, 1, 2, 3, -1,
                                                    2, 0, 3, -1, 2, 1, 3, -1,
                                                    0, 1, 2, -1, 0, 2, 1, -1,
                                                    1, 0, 2, -1, 1, 2, 0, -1,
                                                    2, 0, 1, -1, 2, 1, 0, -1 };
                    static const short *orders[] = { NULL, NULL, order2, order3, order4 };
                    static const int norders[] = { 0, 0, 2, 6, 24 };
                    const short *order = orders[nids];
                    int norder = norders[nids];

                    do {
                        int current_id = 0;
                        unsigned short pack_id[5] = { 0 };
                        long recipe;

                        skip = FALSE;

                        /* Put the IDs in an order */
                        do {
                            pack_id[current_id++] = id[*order++];

                            /* Pack the partial and test it against all recipes */
                            recipe = alchemy_pack(pack_id, current_id);
                            for(j = 0; j < (int)(sizeof(u.alchemy)/sizeof(u.alchemy[0])); j++)
                                if (u.alchemy[j].base == recipe)
                                    skip = TRUE;

                        } while (*order >= 0);

                        /* If it has got to this point with skip FALSE, then
                         * this order is OK - so can stop now.
                         */
                        if (!skip) break;

                        order++;
                        norder--;
                    } while (norder > 0);

                    /* If it has got to this point by breaking out, then there is
                     * an order and this recipe is OK.
                     */
                    retry = (norder == 0);
                }
            }
        }
    } while (retry);
}

#undef BASIC
#undef SKILLED
#undef EXPERT
#undef MASTER
#undef GRAND

void
u_init()
{
    register int i;
    struct u_roleplay tmpuroleplay = u.uroleplay; /* set by rcfile options */
    struct permonst* shambler = &mons[PM_SHAMBLING_HORROR];
    struct attack* attkptr;

    flags.gender = flags.initgend;
    flags.beginner = 1;

    /* WAC -- Clear Tech List since adjabil will init the 1st level techs*/
  	for (i = 0; i <= MAXTECH; i++) tech_list[i].t_id = NO_TECH;

    /* zero u, including pointer values --
     * necessary when aborting from a failed restore */
    (void) memset((genericptr_t) &u, 0, sizeof(u));
    u.ustuck = (struct monst *) 0;
    (void) memset((genericptr_t) &ubirthday, 0, sizeof(ubirthday));
    (void) memset((genericptr_t) &urealtime, 0, sizeof(urealtime));

    u.uroleplay = tmpuroleplay; /* restore options set via rcfile */

#if 0  /* documentation of more zero values as desirable */
    u.usick_cause[0] = 0;
    u.uluck  = u.moreluck = 0;
    uarmu = 0;
    uarm = uarmc = uarmh = uarms = uarmg = uarmf = 0;
    uwep = uball = uchain = uleft = uright = 0;
    uswapwep = uquiver = 0;
    u.twoweap = 0;
    u.ublessed = 0;                     /* not worthy yet */
    u.ugangr   = 0;                     /* gods not angry */
    u.ugifts   = 0;                     /* no divine gifts bestowed */
    u.uevent.uhand_of_elbereth = 0;
    u.uevent.uheard_tune = 0;
    u.uevent.uopened_dbridge = 0;
    u.uevent.udemigod = 0;              /* not a demi-god yet... */
    u.udg_cnt = 0;
    u.mh = u.mhmax = u.mtimedone = 0;
    u.uz.dnum = u.uz0.dnum = 0;
    u.utotype = 0;
#endif /* 0 */

    u.uz.dlevel = 1;
    u.uz0.dlevel = 0;
    u.utolev = u.uz;

    u.umoved = FALSE;
    u.umortality = 0;
    u.ugrave_arise = Role_if(PM_PIRATE) ? PM_SKELETAL_PIRATE : NON_PM;

    u.ukinghill = 0;
    u.protean = 0;

    u.umonnum = u.umonster = monnum_gender(flags.gender, FALSE);
    u.ulycn = NON_PM;
    set_uasmon();

    u.ulevel = 0; /* set up some of the initial attributes */
    u.uconhp = 0;
    u.uhp = u.uhpmax = newhp();
    u.uen = u.uenmax = newpw();
    u.uspellprot = 0;
    adjabil(0, 1);
    u.ulevel = u.ulevelmax = 1;

    init_uhunger();
    for (i = 0; i <= MAXSPELL; i++)
        spl_book[i].sp_id = NO_SPELL;
    u.ublesscnt = 300; /* no prayers just yet */
    u.ublesstim = 0;
    u.ualignbase[A_CURRENT] = u.ualignbase[A_ORIGINAL] = u.ualign.type =
        aligns[flags.initalign].value;

#if defined(BSD) && !defined(POSIX_TYPES)
    (void) time((long *) &ubirthday);
#else
    (void) time(&ubirthday);
#endif
    /*
     *  For now, everyone starts out with a night vision range of 1 and
     *  their xray range disabled.
     */
    u.nv_range = 1;
    u.xray_range = -1;

    /*** Role-specific initializations ***/
    switch (Role_switch) {
    /* rn2(100) > 50 necessary for some choices because some
     * random number generators are bad enough to seriously
     * skew the results if we use rn2(2)...  --KAA
     */
    case PM_ARCHEOLOGIST:
        ini_inv(Archeologist);
        if (!rn2(10))
            ini_inv(Tinopener);
        else if (!rn2(4))
            ini_inv(Lamp);
        else if (!rn2(10))
            ini_inv(Magicmarker);
        knows_object(SACK);
        knows_object(TOUCHSTONE);
        skill_init(Skill_A);
        break;
    case PM_BARBARIAN:
        if (rn2(100) >= 50) { /* see above comment */
            Barbarian[B_MAJOR].trotyp = BATTLE_AXE;
            Barbarian[B_MINOR].trotyp = SHORT_SWORD;
        }
        ini_inv(Barbarian);
        if (!rn2(6))
            ini_inv(Lamp);
        if (Race_if(PM_GIANT)) {
            struct trobj RandomGem = Gem[0];
            while (!rn2(3)) {
                int gem = rnd_class(TOPAZ, JADE);
                Gem[0] = RandomGem;
                Gem[0].trotyp = gem;
                ini_inv(Gem);
                knows_object(gem);
            }
        }
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_B);
        break;
    case PM_CAVEMAN:
        Cave_man[C_AMMO].trquan = rn1(11, 10); /* 10..20 */
        ini_inv(Cave_man);
        if (Race_if(PM_GIANT)) {
            struct trobj RandomGem = Gem[0];
            while (!rn2(8)) {
                int gem = rnd_class(TOPAZ, JADE);
                Gem[0] = RandomGem;
                Gem[0].trotyp = gem;
                ini_inv(Gem);
                knows_object(gem);
            }
        }
        skill_init(Skill_C);
        break;
    case PM_CARTOMANCER:
        ini_inv(Cartomancer);
        skill_init(Skill_Car);
        break;
    case PM_DRAGONMASTER:
        ini_inv(Dragonmaster);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_D);
        break;
    case PM_CONVICT:
        ini_inv(Convict);
        knows_object(SKELETON_KEY);
        knows_object(GRAPPLING_HOOK);
        skill_init(Skill_Con);
        u.uhunger = 200;  /* On the verge of hungry */
    	u.ualignbase[A_CURRENT] = u.ualignbase[A_ORIGINAL] =
        u.ualign.type = A_CHAOTIC; /* Override racial alignment */
        urace.hatemask |= urace.lovemask;   /* Hated by the race's allies */
        urace.lovemask = 0; /* Convicts are pariahs of their race */
        break;
    case PM_HEALER:
        u.umoney0 = rn1(1000, 1001);
        ini_inv(Healer);
        if (!rn2(25))
            ini_inv(Lamp);
        knows_object(POT_FULL_HEALING);
        skill_init(Skill_H);
        break;
    case PM_KNIGHT:
        ini_inv(Knight);
        if(rn2(2))
            ini_inv(Knight1);
        else
            ini_inv(Knight2);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        /* give knights chess-like mobility--idea from wooledge@..cwru.edu */
        HJumping |= FROMOUTSIDE;
        skill_init(Skill_K);
        break;
    case PM_MONK: {
        static short M_spell[] = { SPE_HEALING, SPE_PROTECTION, SPE_SLEEP };

        Monk[M_BOOK].trotyp = M_spell[rn2(90) / 30]; /* [0..2] */
        ini_inv(Monk);
        if (!rn2(5))
            ini_inv(Magicmarker);
        else if (!rn2(10))
            ini_inv(Lamp);
        knows_class(ARMOR_CLASS);
        /* sufficiently martial-arts oriented item to ignore language issue */
        knows_object(SHURIKEN);
        skill_init(Skill_Mon);
        break;
    }
    case PM_PIRATE:
        u.umoney0 = rnd(300);
     		Pirate[PIR_KNIVES].trquan = rn1(2, 2);
     		if(!rn2(4)) Pirate[PIR_SNACK].trotyp = KELP_FROND;
     		Pirate[PIR_SNACK].trquan += rn2(4);
     		if(rn2(100)<50)	Pirate[PIR_JEWELRY].trotyp = RIN_ADORNMENT;
     		if(rn2(100)<50)	Pirate[PIR_TOOL].trotyp = GRAPPLING_HOOK;
     		ini_inv(Pirate);
     		knows_object(OILSKIN_SACK);
     		knows_object(OILSKIN_CLOAK);
     		knows_object(GRAPPLING_HOOK);
     		skill_init(Skill_Pir);
     		break;
    case PM_PRIEST:
        ini_inv(Priest);
        if (!rn2(10))
            ini_inv(Magicmarker);
        else if (!rn2(10))
            ini_inv(Lamp);
        knows_object(POT_WATER);
        skill_init(Skill_P);
        /* KMH, conduct --
         * Some may claim that this isn't agnostic, since they
         * are literally "priests" and they have holy water.
         * But we don't count it as such.  Purists can always
         * avoid playing priests and/or confirm another player's
         * role in their YAAP.
         */
        break;
    case PM_RANGER:
        Ranger[RAN_TWO_ARROWS].trquan = rn1(10, 50);
        Ranger[RAN_ZERO_ARROWS].trquan = rn1(10, 30);
        ini_inv(Ranger);
        skill_init(Skill_Ran);
        break;
    case PM_ROGUE:
        Rogue[R_DAGGERS].trquan = rn1(10, 6);
        u.umoney0 = 0;
        ini_inv(Rogue);
        if (!rn2(5))
            ini_inv(Blindfold);
        knows_object(SACK);
        skill_init(Skill_R);
        break;
    case PM_SAMURAI:
        Samurai[S_ARROWS].trquan = rn1(20, 26);
        ini_inv(Samurai);
        if (!rn2(5))
            ini_inv(Blindfold);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_S);
        break;
    case PM_TOURIST:
        Tourist[T_DARTS].trquan = rn1(20, 21);
        u.umoney0 = rnd(1000);
        /* Giants get gems instead - worth a bit more, because of being harder to use */
        if (Race_if(PM_GIANT)) {
            u.umoney0 += 250;
            struct trobj RandomGem = Gem[0];
            do {
                int gem = rnd_class(TOPAZ, JADE);
                u.umoney0 -= objects[gem].oc_cost;
                Gem[0] = RandomGem;
                Gem[0].trotyp = gem;
                ini_inv(Gem);
                knows_object(gem);
            } while (u.umoney0 >= 0);
            u.umoney0 = 0;
        }
        ini_inv(Tourist);
        if (!rn2(25))
            ini_inv(Tinopener);
        else if (!rn2(25))
            ini_inv(Leash);
        else if (!rn2(25))
            ini_inv(Towel);
        else if (!rn2(25))
            ini_inv(Magicmarker);
        skill_init(Skill_T);
        break;
    case PM_VALKYRIE:
        ini_inv(Valkyrie);
        if (!rn2(6))
            ini_inv(Lamp);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_V);
        break;
    case PM_WIZARD:
        switch Subrole_switch {
            case SR_WIZARD:
                ini_inv(Wizard);
                if (!rn2(5))
                    ini_inv(Magicmarker);
                if (!rn2(5))
                    ini_inv(Blindfold);
                skill_init(Skill_W);
                break;
            case SR_ALCHEMIST:
                {
                    int acid = d(10,2) - 9;
                    int potion = 12-acid;
                    Alchemist[A_ACID].trquan = acid;
                    Alchemist[A_POTION].trquan = potion;
                }
                ini_inv(Alchemist);
                ini_alchemy();
                skill_init(Skill_W);
                if (!rn2(3)) ini_inv(Wand);
                else if (!rn2(2)) ini_inv(Ring);
                else ini_inv(Scroll);
                if (!rn2(5))
                    ini_inv(Lamp);
                if (!rn2(5))
                    ini_inv(Blindfold);
                skill_init(Skill_Alchemist);
                break;
            default: /* impossible */
                impossible("Unknown sub-role");
                break;
        }
        break;

    default: /* impossible */
        break;
    }

    /*** Race-specific initializations ***/
    switch (Race_switch) {
    case PM_HUMAN:
        /* Nothing special */
        break;

    case PM_ELF:
        /*
         * Elves are people of music and song, or they are warriors.
         * Non-warriors get an instrument.  We use a kludge to
         * get only non-magic instruments.
         */
        if (Role_if(PM_PRIEST) || Role_if(PM_CARTOMANCER)
            || Role_if(PM_WIZARD)) {
            static int trotyp[] = { FLUTE,  TOOLED_HORN,       HARP,
                                    BELL,         BUGLE,       LEATHER_DRUM };
            Instrument[0].trotyp = trotyp[rn2(SIZE(trotyp))];
            ini_inv(Instrument);
        }

        /* Elves can recognize all elvish objects */
        knows_object(ELVEN_SHORT_SWORD);
        knows_object(ELVEN_ARROW);
        knows_object(ELVEN_BOW);
        knows_object(ELVEN_SPEAR);
        knows_object(ELVEN_DAGGER);
        knows_object(ELVEN_BROADSWORD);
        knows_object(ELVEN_RING_MAIL);
        knows_object(ELVEN_HELM);
        knows_object(ELVEN_SHIELD);
        knows_object(ELVEN_BOOTS);
        knows_object(ELVEN_CLOAK);
        break;
    case PM_DROW:
  	    /* Drows can recognize all droven objects */
  	    knows_object(DARK_ELVEN_SHORT_SWORD);
  	    knows_object(DARK_ELVEN_ARROW);
  	    knows_object(DARK_ELVEN_BOW);
  	    knows_object(DARK_ELVEN_DAGGER);
  	    knows_object(DARK_ELVEN_RING_MAIL);
  	    break;

    case PM_DWARF:
        /* Dwarves can recognize all dwarvish objects */
        knows_object(DWARVISH_SPEAR);
        knows_object(DWARVISH_SHORT_SWORD);
        knows_object(DWARVISH_MATTOCK);
        knows_object(DWARVISH_HELM);
        knows_object(DWARVISH_RING_MAIL);
        knows_object(DWARVISH_CLOAK);
        knows_object(DWARVISH_ROUNDSHIELD);
        break;

    case PM_MERFOLK:
    case PM_GNOME:
        break;

    case PM_GIANT:
        /* you need more food */
        if (!Role_if(PM_WIZARD))
            ini_inv(Xtra_food);
        /* Giants know valuable gems from glass, and may recognize a few types of valuable gem. */
        for(i=DILITHIUM_CRYSTAL; i<=LUCKSTONE; i++)
            if ((objects[i].oc_cost <= 1) || (rn2(100) < 5+ACURR(A_INT)))
                knows_object(i);
        break;

    case PM_ORC:
        /* compensate for generally inferior equipment */
        if (!Role_if(PM_WIZARD) && !Role_if(PM_CONVICT))
            ini_inv(Xtra_food);
        /* Orcs can recognize all orcish objects */
        knows_object(ORCISH_SHORT_SWORD);
        knows_object(ORCISH_ARROW);
        knows_object(ORCISH_BOW);
        knows_object(ORCISH_SPEAR);
        knows_object(ORCISH_DAGGER);
        knows_object(ORCISH_RING_MAIL);
        knows_object(ORCISH_HELM);
        knows_object(ORCISH_SHIELD);
        knows_object(URUK_HAI_SHIELD);
        knows_object(ORCISH_CLOAK);
        break;

    case PM_VAMPIRE:
        knows_object(POT_VAMPIRE_BLOOD);
        knows_object(POT_BLOOD);
	    /* Vampires start off with gods not as pleased, luck penalty */
	    adjalign(-5); 
	    change_luck(-1);
	    break;

    case PM_HUMAN_WEREWOLF:
        set_ulycn(PM_WEREWOLF);
        break;

    default: /* impossible */
        break;
    }

    if (discover)
        ini_inv(Wishing);

    if (wizard)
        read_wizkit();

    if (u.umoney0)
        ini_inv(Money);
    u.umoney0 += hidden_gold(); /* in case sack has gold in it */

    /* Ensure that Monks don't start with meat. (Tripe is OK, as it's
     * meant as pet food.)
     **/
    if (Role_if(PM_MONK)) {
        struct obj *otmp;
        for(otmp = invent; otmp; otmp = otmp->nobj) {
            if ((otmp->otyp == TIN) && (!vegetarian(&mons[otmp->corpsenm]))) {
                if (rn2(2)) {
                    otmp->spe = 1;
                    otmp->corpsenm = NON_PM;
                } else {
                    otmp->corpsenm = PM_LICHEN;
                }
            }
        }
    }

    /* Ensure that you don't start with poisonous meat, unless you resist.
     **/
    if (!Role_if(PM_BARBARIAN) && !Role_if(PM_HEALER) &&
		!Race_if(PM_ORC) && !Race_if(PM_HUMAN_WEREWOLF)) {
        struct obj *otmp;
        for(otmp = invent; otmp; otmp = otmp->nobj) {
            if ((otmp->otyp == TIN) && (poisonous(&mons[otmp->corpsenm]))) {
                otmp->corpsenm = PM_LICHEN;
            }
        }
    }

    find_ac();     /* get initial ac value */
    init_attr(75); /* init attribute values */
    max_rank_sz(); /* set max str size for class ranks */
    /*
     *  Do we really need this?
     */
    for (i = 0; i < A_MAX; i++)
        if (!rn2(20)) {
            register int xd = rn2(7) - 2; /* biased variation */

            (void) adjattrib(i, xd, TRUE);
            if (ABASE(i) < AMAX(i))
                AMAX(i) = ABASE(i);
        }

    /* make sure you can carry all you have - especially for Tourists */
    while (inv_weight() > 0) {
        if (adjattrib(A_STR, 1, TRUE))
            continue;
        if (adjattrib(A_CON, 1, TRUE))
            continue;
        /* only get here when didn't boost strength or constitution */
        break;
    }

    /* what a horrible night to have a curse */
  	shambler->mlevel += rnd(12)-3;				/* shuffle level */
  	shambler->mmove = rn2(10)+9;				/* slow to very fast */
  	shambler->ac = rn2(21)-10;				/* any AC */
  	shambler->mr = rn2(5)*25;				/* varying amounts of MR */
  	shambler->maligntyp = rn2(21)-10;			/* any alignment */
  	/* attacks...?  */
  	for (i = 0; i < rnd(4); i++) {
  		attkptr = &shambler->mattk[i];
  		/* restrict it to certain types of attacks */
  		attkptr->aatyp = 0;
  		while (attkptr->aatyp == 0 || attkptr->aatyp == AT_ENGL || attkptr->aatyp == AT_SPIT ||
  					attkptr->aatyp == AT_BREA || attkptr->aatyp == AT_EXPL ||
  					attkptr->aatyp == AT_BOOM || attkptr->aatyp == AT_GAZE ||
            attkptr->aatyp == AT_HUGS) {
  			attkptr->aatyp = rn2(AT_TENT);
  		}
  		attkptr->adtyp = 0;
  		while (attkptr->adtyp == 0 || attkptr->adtyp == AD_DETH || attkptr->adtyp == AD_TLPT ||
  					attkptr->adtyp == AD_SLIM || attkptr->adtyp == AD_VOID ||
  					attkptr->adtyp == AD_ENCH || attkptr->adtyp == AD_DISN ||
  					attkptr->adtyp == AD_PEST || attkptr->adtyp == AD_FAMN ||
            attkptr->adtyp == AD_HYDR || attkptr->adtyp == AD_QUIL ||
            attkptr->adtyp == AD_LUCK || attkptr->adtyp == AD_SKEL) {
  			attkptr->adtyp = rn2(AD_HNGY);
  		}
  		attkptr->damn = 2;				/* we're almost sure to get this wrong first time */
  		attkptr->damd = 10;				/* either too high or too low */
  	}
  	shambler->msize = rn2(MZ_GIGANTIC+1);			/* any size */
  	shambler->cwt = 20;					/* fortunately moot as it's flagged NOCORPSE */
  	shambler->cnutrit = 20;					/* see above */
  	shambler->msound = rn2(MS_HUMANOID);			/* any but the specials */
  	shambler->mresists = 0;
  	for (i = 0; i < rnd(6); i++) {
  		shambler->mresists |= (1 << rn2(8));		/* physical resistances... */
  	}
  	for (i = 0; i < rnd(5); i++) {
  		shambler->mresists |= (0x100 << rn2(7));	/* 'different' resistances, even clumsy */
  	}
  	shambler->mconveys = 0;					/* flagged NOCORPSE */
  	/*
  	 * now time for the random flags.  this will likely produce
  	 * a number of complete trainwreck monsters at first, but
  	 * every so often something will dial up nasty stuff
  	 */
  	shambler->mflags1 = 0;
  	for (i = 0; i < rnd(17); i++) {
  		shambler->mflags1 |= (1 << rn2(33));		/* trainwreck this way :D */
  	}
  	shambler->mflags1 &= ~M1_UNSOLID;			/* no ghosts */
  	shambler->mflags1 &= ~M1_WALLWALK;			/* no wall-walkers */

  	shambler->mflags2 = M2_NOPOLY | M2_HOSTILE;		/* Don't let the player be one of these yet. */
  	for (i = 0; i < rnd(17); i++) {
  		shambler->mflags2 |= (1 << rn2(31));
  	}
  	shambler->mflags2 &= ~M2_MERC;				/* no guards */
  	shambler->mflags2 &= ~M2_PEACEFUL;			/* no peacefuls */
  	/* shambler->mflags2 &= ~M2_WERE; */
  	shambler->mflags2 &= ~M2_PNAME;				/* not a proper name */

    return;
}

/* skills aren't initialized, so we use the role-specific skill lists */
STATIC_OVL boolean
restricted_spell_discipline(otyp)
int otyp;
{
    const struct def_skill *skills;
    int this_skill = spell_skilltype(otyp);

    switch (Role_switch) {
    case PM_ARCHEOLOGIST:
        skills = Skill_A;
        break;
    case PM_BARBARIAN:
        skills = Skill_B;
        break;
    case PM_CAVEMAN:
        skills = Skill_C;
        break;
    case PM_CARTOMANCER:
        skills = Skill_Car;
        break;
    case PM_DRAGONMASTER:
        skills = Skill_D;
        break;
    case PM_CONVICT:
        skills = Skill_Con;
        break;
    case PM_HEALER:
        skills = Skill_H;
        break;
    case PM_KNIGHT:
        skills = Skill_K;
        break;
    case PM_MONK:
        skills = Skill_Mon;
        break;
    case PM_PIRATE:
        skills = Skill_Pir;
        break;
    case PM_PRIEST:
        skills = Skill_P;
        break;
    case PM_RANGER:
        skills = Skill_Ran;
        break;
    case PM_ROGUE:
        skills = Skill_R;
        break;
    case PM_SAMURAI:
        skills = Skill_S;
        break;
    case PM_TOURIST:
        skills = Skill_T;
        break;
    case PM_VALKYRIE:
        skills = Skill_V;
        break;
    case PM_WIZARD:
        skills = Skill_W;
        break;
    default:
        skills = 0; /* lint suppression */
        break;
    }

    while (skills && skills->skill != P_NONE) {
        if (skills->skill == this_skill)
            return FALSE;
        ++skills;
    }
    return TRUE;
}

/* substitute race-specific items; this used to be in
   the 'if (otyp != UNDEF_TYP) { }' block below, but then
   substitutions didn't occur for randomly generated items
   (particularly food) which have racial substitutes - while
   putting it in the later position only produced partially
   substituted items such as iron elven arrows.
   */
STATIC_OVL int
race_trans(obj, trop, otyp)
struct obj *obj;
register struct trobj *trop;
int otyp;
{
    int i;
    if (urace.malenum != PM_HUMAN) {
        for (i = 0; inv_subs[i].race_pm != NON_PM; ++i)
            if (inv_subs[i].race_pm == urace.malenum
                && otyp == inv_subs[i].item_otyp) {
                debugpline3("race_trans: substituting %s for %s%s",
                            OBJ_NAME(objects[inv_subs[i].subs_otyp]),
                            (trop->trotyp == UNDEF_TYP) ? "random " : "",
                            OBJ_NAME(objects[otyp]));
                otyp = obj->otyp = inv_subs[i].subs_otyp;
                break;
            }
    }
    return otyp;
}

STATIC_OVL void
ini_inv(trop)
register struct trobj *trop;
{
    struct obj *obj;
    int otyp, i;
    int corpses = 0;

    /* This will fail if the same list is used more than once */
    assert(trop->trquan > 0);

    while (trop->trclass) {
        otyp = (int) trop->trotyp;
        if (otyp != UNDEF_TYP) {
            obj = mksobj(otyp, TRUE, FALSE);
            otyp = race_trans(obj, trop, otyp);

            /* Don't allow materials to be start scummed for */
            obj->material = objects[obj->otyp].oc_material;
            /* Don't allow weapons to roll high enchantment and get an oname
             * when they'll then have their enchantment set after this */
            if (Hate_material(SILVER) && obj->material == SILVER)
                obj->material = IRON;
            free_oname(obj);

        } else { /* UNDEF_TYP */
            static NEARDATA short nocreate = STRANGE_OBJECT;
            static NEARDATA short nocreate2 = STRANGE_OBJECT;
            static NEARDATA short nocreate3 = STRANGE_OBJECT;
            static NEARDATA short nocreate4 = STRANGE_OBJECT;
            /*
             * For random objects, do not create certain overly powerful
             * items: wand of wishing, ring of levitation, or the
             * polymorph/polymorph control combination.  Specific objects,
             * i.e. the discovery wishing, are still OK.
             * Also, don't get a couple of really useless items.  (Note:
             * punishment isn't "useless".  Some players who start out with
             * one will immediately read it and use the iron ball as a
             * weapon.)
             */
            obj = mkobj(trop->trclass, FALSE);
            otyp = obj->otyp;
            while (otyp == WAN_WISHING || otyp == nocreate
                   || otyp == nocreate2 || otyp == nocreate3
                   || otyp == nocreate4 || otyp == RIN_LEVITATION
                   || otyp == SCR_CLONING
                   /* 'useless' items */
                   || otyp == POT_HALLUCINATION
                   || otyp == POT_ACID
                   || ((Subrole_if(SR_ALCHEMIST)) && (otyp == POT_WATER))
                   || ((Subrole_if(SR_ALCHEMIST)) && (otyp == POT_FRUIT_JUICE))
                   || otyp == SCR_AMNESIA
                   || otyp == SCR_FIRE
                   || otyp == SCR_BLANK_PAPER
                   || otyp == SPE_BLANK_PAPER
                   || otyp == RIN_AGGRAVATE_MONSTER
                   || otyp == RIN_HUNGER
                   || otyp == WAN_NOTHING
                   /* starting with pumpkins can give better AC */
                   || otyp == PUMPKIN
                   /* orcs start with poison resistance */
                   || (otyp == RIN_POISON_RESISTANCE && Race_if(PM_ORC))
                   /* Monks don't use weapons */
                   || (otyp == SCR_ENCHANT_WEAPON && Role_if(PM_MONK))
                   /* Infernals already have regeneration and hate silver */
                   || (otyp == RIN_REGENERATION && Role_if(PM_INFERNAL))
                   || (objects[obj->otyp].oc_material == SILVER && Role_if(PM_INFERNAL))
                   /* wizard patch -- they already have one */
                   || (otyp == SPE_FORCE_BOLT && Role_if(PM_WIZARD))
                   /* powerful spells are either useless to
                      low level players or unbalancing; also
                      spells in restricted skill categories */
                   || (obj->oclass == SPBOOK_CLASS
                       && (objects[otyp].oc_level > 3
                           || restricted_spell_discipline(otyp)))) {
                dealloc_obj(obj);
                obj = mkobj(trop->trclass, FALSE);
                otyp = obj->otyp;
            }
            /* Don't allow materials to be start scummed for */
            set_material(obj, objects[obj->otyp].oc_material);
            /* Don't start with +0 or negative rings */
            if (objects[otyp].oc_charged && obj->spe <= 0)
                obj->spe = rnew(0, FALSE, TRUE);

            /* Heavily relies on the fact that 1) we create wands
             * before rings, 2) that we create rings before
             * spellbooks, and that 3) not more than 1 object of a
             * particular symbol is to be prohibited.  (For more
             * objects, we need more nocreate variables...)
             */
            switch (otyp) {
            case WAN_POLYMORPH:
            case RIN_POLYMORPH:
            case POT_POLYMORPH:
                nocreate = RIN_POLYMORPH_CONTROL;
                break;
            case RIN_POLYMORPH_CONTROL:
                nocreate = RIN_POLYMORPH;
                nocreate2 = SPE_POLYMORPH;
                nocreate3 = POT_POLYMORPH;
            }
            /* Don't have 2 of the same ring or spellbook */
            if (obj->oclass == RING_CLASS || obj->oclass == SPBOOK_CLASS)
                nocreate4 = otyp;
        }

        otyp = race_trans(obj, trop, otyp);

        /* Create ghoul corpses */
        if (urace.malenum == PM_GHOUL && obj->oclass == FOOD_CLASS) {
            dealloc_obj(obj);
            if (corpses <= 2) {
                obj = mksobj(CORPSE, TRUE, FALSE);
                obj->corpsenm = PM_DEATH_MAGGOT;
                obj->age = -100;
                corpses++;
            } else {
                trop++;
                continue;
            }
        }

        /* Set up cartomancer cards */
        if (urole.malenum == PM_CARTOMANCER && obj->otyp == SCR_CREATE_MONSTER) {
            do {
                i = rn2(NUMMONS);
            } while ((type_is_pname(&mons[i]) || (mons[i].geno & G_UNIQ) || (mons[i].geno & G_NOGEN)));
            obj->corpsenm = i;
        }

        /* Create vampire blood */
        if (urace.malenum == PM_VAMPIRE && obj->otyp == FOOD_RATION) {
            dealloc_obj(obj);
            obj = mksobj(POT_VAMPIRE_BLOOD, TRUE, FALSE);
        }

        /* nudist gets no armor */
        if (u.uroleplay.nudist && obj->oclass == ARMOR_CLASS) {
            dealloc_obj(obj);
            trop++;
            continue;
        }

		/* exhibitionist gets nothing opaque */
        if (u.uroleplay.exhibitionist && is_opaque_armor(obj)) {
			dealloc_obj(obj);
            trop++;
            continue;
        }

        /* unarmored gets nothing armored */
		if (u.uroleplay.unarmored && is_protective_armor(obj)) {
			dealloc_obj(obj);
            trop++;
            continue;
        }

        if (trop->trclass == COIN_CLASS) {
            /* no "blessed" or "identified" money */
            obj->quan = u.umoney0;
        } else {
            if (objects[otyp].oc_uses_known)
                obj->known = 1;
            obj->dknown = obj->bknown = obj->rknown = 1;
            if (Is_container(obj) || obj->otyp == STATUE) {
                obj->cknown = obj->lknown = 1;
                obj->otrapped = 0;
            }
            obj->cursed = 0;
            if (obj->opoisoned && u.ualign.type != A_CHAOTIC)
                obj->opoisoned = 0;
            if (obj->oclass == WEAPON_CLASS || obj->oclass == TOOL_CLASS) {
                obj->quan = (long) trop->trquan;
                trop->trquan = 1;
            } else if (obj->oclass == GEM_CLASS && is_graystone(obj)
                       && obj->otyp != FLINT) {
                obj->quan = 1L;
            }
            if (obj->otyp == STRIPED_SHIRT ) {
                obj->cursed = TRUE;
            }
            if (trop->trspe != UNDEF_SPE)
                obj->spe = trop->trspe;
            if (trop->trbless != UNDEF_BLESS)
                obj->blessed = trop->trbless;
        }
        /* defined after setting otyp+quan + blessedness */
        obj->owt = weight(obj);
        obj = addinv(obj);

        /* Make the type known if necessary */
        if (OBJ_DESCR(objects[otyp]) && obj->known)
            discover_object(otyp, TRUE, FALSE);
        if (otyp == OIL_LAMP)
            discover_object(POT_OIL, TRUE, FALSE);

        if (obj->oclass == ARMOR_CLASS) {
            if (is_shield(obj) && !uarms && !(uwep && bimanual(uwep))) {
                setworn(obj, W_ARMS, TRUE);
                /* Prior to 3.6.2 this used to unset uswapwep if it was set, but
                   wearing a shield doesn't prevent having an alternate
                   weapon ready to swap with the primary; just make sure we
                   aren't two-weaponing (academic; no one starts that way) */
                u.twoweap = FALSE;
            } else if (is_helmet(obj) && !uarmh)
                setworn(obj, W_ARMH, TRUE);
            else if (is_gloves(obj) && !uarmg)
                setworn(obj, W_ARMG, TRUE);
            else if (is_shirt(obj) && !uarmu)
                setworn(obj, W_ARMU, TRUE);
            else if (is_cloak(obj) && !uarmc)
                setworn(obj, W_ARMC, TRUE);
            else if (is_boots(obj) && !uarmf)
                setworn(obj, W_ARMF, TRUE);
            else if (is_suit(obj) && !uarm)
                setworn(obj, W_ARM, TRUE);
        }

        if (obj->oclass == WEAPON_CLASS || is_weptool(obj)
            || otyp == TIN_OPENER || otyp == FLINT || otyp == ROCK) {
            if (is_ammo(obj) || is_missile(obj)) {
                if (!uquiver)
                    setuqwep(obj);
            } else if (!uwep && (!uarms || !bimanual(obj))) {
                setuwep(obj);
                staff_set_en(obj);
            }
            else if (!uswapwep) {
                setuswapwep(obj);
            }
        }
        if (obj->oclass == SPBOOK_CLASS && obj->otyp != SPE_BLANK_PAPER)
            initialspell(obj);

#if !defined(PYRAMID_BUG) && !defined(MAC)
        if (--trop->trquan)
            continue; /* make a similar object */
#else
        if (trop->trquan) { /* check if zero first */
            --trop->trquan;
            if (trop->trquan)
                continue; /* make a similar object */
        }
#endif
        trop++;
    }
}

/*u_init.c*/

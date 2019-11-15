/* NetHack 3.6	role.c	$NHDT-Date: 1547086250 2019/01/10 02:10:50 $  $NHDT-Branch: NetHack-3.6.2-beta01 $:$NHDT-Revision: 1.56 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/*-Copyright (c) Robert Patrick Rankin, 2012. */
/* NetHack may be freely redistributed.  See license for details. */

/* Edited on 5/8/18 by NullCGT */

#include "hack.h"

/*** Table of all roles ***/
/* According to AD&D, HD for some classes (ex. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  Thus I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 4 since without one you can't teleport or cast
 * spells. --KAA
 *
 * As the wizard has been updated (wizard patch 5 jun '96) their HD can be
 * brought closer into line with AD&D. This forces wizards to use magic more
 * and distance themselves from their attackers. --LSZ
 *
 * With the introduction of races, some hit points and energy
 * has been reallocated for each race.  The values assigned
 * to the roles has been reduced by the amount allocated to
 * humans.  --KMH
 *
 * God names use a leading underscore to flag goddesses.
 */
const struct Role roles[] = {
    { { "Archeologist", 0, 0 },
      { { "Digger", 0, 0 },
        { "Field Worker", 0, 0 },
        { "Investigator", 0, 0 },
        { "Exhumer", 0, 0 },
        { "Excavator", 0, 0 },
        { "Spelunker", 0, 0 },
        { "Speleologist", 0, 0 },
        { "Collector", 0, 0 },
        { "Curator", 0, 0 } },
      "Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
      "Arc",
      "the College of Archeology",
      "the Tomb of the Toltec Kings",
      PM_ARCHEOLOGIST,
      NON_PM,
      NON_PM,
      NON_PM,
      PM_LORD_CARNARVON,
      PM_STUDENT,
      PM_MINION_OF_HUHETOTL,
      NON_PM,
      PM_HUMAN_MUMMY,
      S_SNAKE,
      S_MUMMY,
      ART_ORB_OF_DETECTION,
      MH_HUMAN | MH_DWARF | MH_GNOME | MH_ANGEL | MH_DEMON | MH_GHOUL | MH_WERE
        | MH_VAMPIRE,
      ROLE_MALE | ROLE_FEMALE |
      ROLE_NEUTER | ROLE_LAWFUL | ROLE_NEUTRAL,
      /* Str Int Wis Dex Con Cha */
      { 7, 10, 10, 7, 7, 7 },
      { 20, 20, 20, 10, 20, 10 },
      /* Init   Lower  Higher */
      { 11, 0, 0, 8, 1, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      14, /* Energy */
      10,
      5,
      0,
      2,
      10,
      A_INT,
      SPE_MAGIC_MAPPING,
      -4 },
    { { "Barbarian", 0, 0 },
      { { "Plunderer", "Plunderess", 0 },
        { "Pillager", 0, 0 },
        { "Bandit", 0, 0 },
        { "Brigand", 0, 0 },
        { "Raider", 0, 0 },
        { "Reaver", 0, 0 },
        { "Slayer", 0, 0 },
        { "Chieftain", "Chieftainess", "Chief" },
        { "Conqueror", "Conqueress", 0} },
      "Mitra", "Crom", "Set", /* Hyborian */
      "Bar",
      "the Camp of the Duali Tribe",
      "the Duali Oasis",
      PM_BARBARIAN,
      NON_PM,
      NON_PM,
      NON_PM,
      PM_PELIAS,
      PM_CHIEFTAIN,
      PM_THOTH_AMON,
      PM_OGRE,
      PM_TROLL,
      S_OGRE,
      S_TROLL,
      ART_HEART_OF_AHRIMAN,

      MH_HUMAN | MH_DWARF | MH_MERFOLK | MH_ORC | MH_DEMON | MH_GIANT | MH_GHOUL | ROLE_MALE | ROLE_FEMALE
          | ROLE_NEUTER | ROLE_NEUTRAL | ROLE_CHAOTIC,

      MH_HUMAN | MH_DWARF | MH_MERFOLK | MH_ORC | MH_DEMON | MH_GHOUL | MH_WERE
        | MH_VAMPIRE,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_NEUTRAL | ROLE_CHAOTIC,
      /* Str Int Wis Dex Con Cha */
      { 16, 7, 7, 15, 16, 6 },
      { 30, 6, 7, 20, 30, 7 },
      /* Init   Lower  Higher */
      { 14, 0, 0, 10, 2, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      10, /* Energy */
      10,
      14,
      0,
      0,
      8,
      A_INT,
      SPE_HASTE_SELF,
      -4 },
    { { "Caveman", "Cavewoman", "Caveperson" },
      { { "Troglodyte", 0, 0 },
        { "Aborigine", 0, 0 },
        { "Wanderer", 0, 0 },
        { "Vagrant", 0, 0 },
        { "Wayfarer", 0, 0 },
        { "Roamer", 0, 0 },
        { "Nomad", 0, 0 },
        { "Rover", 0, 0 },
        { "Pioneer", 0, 0 } },
      "Anu", "_Ishtar", "Anshar", /* Babylonian */
      "Cav",
      "the Caves of the Ancestors",
      "the Dragon's Lair",
      PM_CAVEMAN,
      PM_CAVEWOMAN,
      PM_CAVEPERSON,
      PM_LITTLE_DOG,
      PM_SHAMAN_KARNOV,
      PM_NEANDERTHAL,
      PM_CHROMATIC_DRAGON,
      PM_BUGBEAR,
      PM_HILL_GIANT,
      S_HUMANOID,
      S_GIANT,
      ART_SCEPTRE_OF_MIGHT,

      MH_HUMAN | MH_DWARF | MH_GNOME | MH_GIANT | MH_WERE | MH_VAMPIRE | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER
          | ROLE_LAWFUL | ROLE_NEUTRAL,

      /* Str Int Wis Dex Con Cha */
      { 10, 7, 7, 7, 8, 6 },
      { 30, 6, 7, 20, 30, 7 },
      /* Init   Lower  Higher */
      { 14, 0, 0, 8, 2, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      10, /* Energy */
      0,
      12,
      0,
      1,
      8,
      A_INT,
      SPE_DIG,
      -4 },
    { { "Cartomancer", 0, 0 },
      { { "Learner", 0, 0 },
        { "Shuffler", 0, 0 },
        { "Player", 0, 0 },
        { "Reader", 0, 0 },
        { "Strategist", 0, 0 },
        { "Deck Stacker", 0, 0 },
        { "Card Collector", 0, 0 },
        { "Deck Stacker", 0, 0 },
        { "King of Games", "Queen of Games", "Monarch of Games" } },

      "Johnny", "Spike", "Timmy", /* Card player archetypes */
      "Car",
      "the Card Training School",
      "the Great Card Arena",
      PM_CARTOMANCER,
      NON_PM,
      NON_PM,
      PM_LITTLE_DOG,
      PM_LORD_OF_THE_CARDS,
      PM_STUDENT, /* Reusing students to avoid redundancy */
      PM_DAL_ZETHIRE,
      PM_COBRA,
      PM_GUARDIAN_NAGA,
      S_SNAKE,
      S_NAGA,
      ART_HOLOGRAPHIC_VOID_LILY,
      MH_HUMAN | MH_DWARF | MH_ELF | MH_GNOME | MH_DEMON | MH_GHOUL
        | MH_VAMPIRE,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER |
          ROLE_LAWFUL | ROLE_NEUTRAL | ROLE_CHAOTIC,
      /* Str Int Wis Dex Con Cha */
      { 7, 10, 7, 7, 7, 7 },
      { 10, 30, 10, 20, 20, 10 },
      /* Init   Lower  Higher */
      { 10, 0, 0, 8, 1, 0 }, /* Hit points */
      { 4, 3, 0, 2, 0, 3 },
      12, /* Energy */
      0,
      1,
      0,
      3,
      10,
      A_INT,
      SPE_CREATE_MONSTER,
      -4 },
    { {"Convict", 0, 0}, {
        {"Detainee", 0,     0},
        {"Inmate", 0,   0},
        {"Jail-bird", 0, 0},
        {"Prisoner", 0, 0},
        {"Outlaw", 0,    0},
        {"Crook", 0,   0},
        {"Desperado", 0,     0},
        {"Felon", 0,    0},
        {"Fugitive", 0,  0} },
        "Ilmater", "Grumbar", "_Tymora",	/* Faerunian */
        "Con", "Castle Waterdeep Dungeon", "the Warden's Level",
        PM_CONVICT, 
        NON_PM, 
        NON_PM,
        PM_SEWER_RAT,
        PM_ROBERT_THE_LIFER, 
        PM_INMATE, 
        PM_WARDEN_ARIANNA,
        PM_GIANT_BEETLE, 
        PM_SOLDIER_ANT, 
        S_RODENT, 
        S_SPIDER,
        ART_IRON_BALL_OF_LIBERATION,
        MH_HUMAN | MH_DWARF | MH_GNOME | MH_MERFOLK | MH_ORC | MH_GHOUL | MH_WERE
            | MH_VAMPIRE,  
        ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_CHAOTIC,
        /* Str Int Wis Dex Con Cha */
        {  10,  7,  7,  7, 13,  6 },
        {  20, 20, 10, 20, 20, 10 },
        /* Init   Lower  Higher */
        {  8, 0,  0, 8,  0, 0 },	/* Hit points */
        {  1, 0,  0, 1,  0, 1 },10,	/* Energy */
        -10, 5, 0, 2, 10, 
        A_INT, 
        SPE_TELEPORT_AWAY,   
        -4 },
    { { "Dragonmaster", 0, 0 },
      { { "Servant of Scales", 0, 0 },
        { "Page of Wings", 0, 0 },
        { "Page of Claws", 0, 0 },
        { "Page of Scales", 0, 0 },
        { "Knight of the Skies", 0, 0 },
        { "Knight of Talons", 0, 0 },
        { "Knight of Scales", 0, 0 },
        { "Knight of Power", 0, 0 },
        { "Knight of Breath", 0, 0 } },
      "Enki", "Enlil", "_Ereshkigal", /* Sumerian */
      "Dra",
      "the Sacred Nesting Grounds",
      "the Draconic Boneyard",
      PM_DRAGONMASTER,
      NON_PM,
      NON_PM,
      NON_PM,
      PM_DRAKEMOTHER,
      PM_DRAKE,
      PM_SLIPSCALE_THE_BETRAYER,
      PM_CHIMERA,
      PM_ZOMBIE_DRAGON,
      S_BAT,
      S_QUADRUPED,
      ART_SHARUR,
      MH_HUMAN | MH_ELF,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_LAWFUL | ROLE_NEUTRAL,
      /* Str Int Wis Dex Con Cha */
      { 13, 17, 14, 8, 8, 8 },
      { 30, 15, 15, 10, 20, 10 },
      /* Init   Lower  Higher */
      { 14, 0, 0, 8, 2, 0 }, /* Hit points */
      { 1, 4, 0, 1, 0, 2 },
      10, /* Energy */
      10,
      8,
      -2,
      0,
      9,
      A_INT,
      SPE_FIREBALL,
      -4 },
    { { "Healer", 0, 0 },
      { { "Rhizotomist", 0, 0 },
        { "Empiric", 0, 0 },
        { "Embalmer", 0, 0 },
        { "Dresser", 0, 0 },
        { "Medicus ossium", "Medica ossium", 0 },
        { "Herbalist", 0 },
        { "Magister", "Magistra", "Magistrum" },
        { "Physician", 0, 0 },
        { "Chirurgeon", 0, 0 } },
      "_Athena", "Hermes", "Poseidon", /* Greek */
      "Hea",
      "the Temple of Epidaurus",
      "the Temple of Coeus",
      PM_HEALER,
      NON_PM,
      NON_PM,
      NON_PM,
      PM_HIPPOCRATES,
      PM_ATTENDANT,
      PM_CYCLOPS,
      PM_GIANT_RAT,
      PM_SNAKE,
      S_RODENT,
      S_YETI,
      ART_STAFF_OF_AESCULAPIUS,
      MH_HUMAN | MH_MERFOLK | MH_GNOME | MH_ANGEL | MH_GHOUL | MH_WERE | MH_ELF,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_NEUTRAL,
      /* Str Int Wis Dex Con Cha */
      { 7, 7, 13, 7, 11, 16 },
      { 15, 20, 20, 15, 25, 5 },
      /* Init   Lower  Higher */
      { 11, 0, 0, 8, 1, 0 }, /* Hit points */
      { 1, 4, 0, 1, 0, 2 },
      20, /* Energy */
      10,
      3,
      -3,
      2,
      10,
      A_WIS,
      SPE_CURE_SICKNESS,
      -4 },
    { { "Knight", 0, 0 },
      { { "Gallant", 0, 0 },
        { "Esquire", 0, 0 },
        { "Bachelor", 0, 0 },
        { "Sergeant", 0, 0 },
        { "Knight", 0, 0 },
        { "Banneret", 0, 0 },
        { "Chevalier", "Chevaliere", 0 },
        { "Seignieur", "Dame", "Persone" },
        { "Paladin", 0, 0 } },
      "Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
      "Kni",
      "Camelot Castle",
      "the Isle of Glass",
      PM_KNIGHT,
      NON_PM,
      NON_PM,
      PM_PONY,
      PM_KING_ARTHUR,
      PM_PAGE,
      PM_IXOTH,
      PM_QUASIT,
      PM_OCHRE_JELLY,
      S_IMP,
      S_JELLY,
      ART_MAGIC_MIRROR_OF_MERLIN,
      MH_HUMAN | MH_DWARF | MH_ANGEL,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_LAWFUL,
      /* Str Int Wis Dex Con Cha */
      { 13, 7, 14, 8, 10, 17 },
      { 30, 15, 15, 10, 20, 10 },
      /* Init   Lower  Higher */
      { 14, 0, 0, 8, 2, 0 }, /* Hit points */
      { 1, 4, 0, 1, 0, 2 },
      10, /* Energy */
      10,
      8,
      -2,
      0,
      9,
      A_WIS,
      SPE_TURN_UNDEAD,
      -4 },
    { { "Monk", 0, 0 },
      { { "Candidate", 0 },
        { "Novice", 0, 0 },
        { "Initiate", 0, 0 },
        { "Student of Stones", 0, 0 },
        { "Student of Waters", 0, 0 },
        { "Student of Metals", 0, 0 },
        { "Student of Winds", 0, 0 },
        { "Student of Fire", 0, 0 },
        { "Master", 0, 0 } },
      "Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
      "Mon",
      "the Monastery of Chan-Sune",
      "the Monastery of the Earth-Lord",
      PM_MONK,
      NON_PM,
      NON_PM,
      NON_PM,
      PM_GRAND_MASTER,
      PM_ABBOT,
      PM_MASTER_KAEN,
      PM_EARTH_ELEMENTAL,
      PM_XORN,
      S_ELEMENTAL,
      S_XORN,
      ART_EYES_OF_THE_OVERWORLD,

      MH_HUMAN | MH_DWARF | MH_MERFOLK | MH_GIANT | MH_GHOUL | MH_DEMON | MH_ORC | ROLE_MALE | ROLE_FEMALE| ROLE_NEUTER | ROLE_LAWFUL | ROLE_NEUTRAL
          | ROLE_CHAOTIC,

      /* Str Int Wis Dex Con Cha */
      { 10, 7, 8, 8, 7, 7 },
      { 25, 10, 20, 20, 15, 10 },
      /* Init   Lower  Higher */
      { 12, 0, 0, 8, 1, 0 }, /* Hit points */
      { 2, 2, 0, 2, 0, 2 },
      10, /* Energy */
      10,
      8,
      -2,
      2,
      20,
      A_WIS,
      SPE_RESTORE_ABILITY,
      -4 },
    { { "Priest", "Priestess", "Minister" },
      { { "Aspirant", 0, 0 },
        { "Acolyte", 0, 0 },
        { "Adept", 0, 0 },
        { "Priest", "Priestess", "Minister" },
        { "Curate", 0, 0 },
        { "Canon", "Canoness", "Canonite" },
        { "Lama", 0, 0 },
        { "Patriarch", "Matriarch", "Family Head" },
        { "High Priest", "High Priestess", "High Minister" } },
      0, 0, 0, /* deities from a randomly chosen other role will be used */
      "Pri",
      "the Great Temple",
      "the Temple of Nalzok",
      PM_PRIEST,
      PM_PRIESTESS,
      PM_MINISTER,
      NON_PM,
      PM_ARCH_PRIEST,
      PM_ACOLYTE,
      PM_NALZOK,
      PM_HUMAN_ZOMBIE,
      PM_WRAITH,
      S_ZOMBIE,
      S_WRAITH,
      ART_MITRE_OF_HOLINESS,
      MH_HUMAN | MH_DWARF | MH_MERFOLK | MH_GNOME | MH_ELF | MH_DEMON | MH_GHOUL | MH_ANGEL | MH_WERE | MH_ORC
        | MH_VAMPIRE,
      ROLE_MALE | ROLE_FEMALE |
          ROLE_NEUTER | ROLE_LAWFUL | ROLE_NEUTRAL | ROLE_CHAOTIC,
      /* Str Int Wis Dex Con Cha */
      { 7, 7, 10, 7, 7, 7 },
      { 15, 10, 30, 15, 20, 10 },
      /* Init   Lower  Higher */
      { 12, 0, 0, 8, 1, 0 }, /* Hit points */
      { 4, 3, 0, 2, 0, 2 },
      10, /* Energy */
      0,
      3,
      -2,
      2,
      10,
      A_WIS,
      SPE_REMOVE_CURSE,
      -4 },
    {	{"Pirate", 0, 0}, {
     	{"Landlubber",  0, 0},
     	{"Swabbie",        	0, 0},
     	{"Cutthroat",   0, 0},
     	{"Bosun",      		0, 0},
     	{"Second Mate",     0, 0},
     	{"First Mate",      0, 0},
     	{"Captain",			0, 0},
     	{"Pirate Lord",   	0, 0},
     	{"Dread Pirate",  	0, 0} },
     	"Davy Jones", "_the deep blue sea", "_the storm",
      /* Davy Jones would be chaotic, but we want something all pirates are
        opposed to. */
     	"Pir", "Tortuga", "Shipwreck Island",
     	PM_PIRATE,
      NON_PM,
      NON_PM,
      NON_PM,
     	PM_MAYOR_CUMMERBUND,
      PM_PIRATE_BROTHER,
      PM_BLACKBEARD_S_GHOST,
     	PM_SKELETAL_PIRATE,
      PM_SOLDIER,
      S_RODENT,
      S_ELEMENTAL, /* Ghost pirates, soldiers, rats in the food stores, and the occasional storm*/
     	ART_TREASURY_OF_PROTEUS,
     	MH_HUMAN | MH_DEMON | MH_MERFOLK | MH_GHOUL | MH_WERE | MH_GNOME | MH_ORC
            | MH_VAMPIRE,
          ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_CHAOTIC | ROLE_NEUTRAL,
     	/* Str Int Wis Dex Con Cha */
     	{   8,  7, 7,  8,  8,  7 },
     	{  20, 15, 10, 20, 25, 10 },
     	/* Init   Lower  Higher */
     	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
     	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
     	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -4
    },
    /* Note:  Rogue precedes Ranger so that use of `-R' on the command line
       retains its traditional meaning. */
    { { "Rogue", 0, 0},
      { { "Footpad", 0, 0 },
        { "Cutpurse", 0, 0 },
        { "Rogue", 0, 0 },
        { "Pilferer", 0, 0 },
        { "Robber", 0, 0 },
        { "Burglar", 0, 0 },
        { "Filcher", 0, 0 },
        { "Magsman", "Magswoman", "Magsperson" },
        { "Thief", 0, 0 } },
      "Issek", "Mog", "Kos", /* Nehwon */
      "Rog",
      "the Thieves' Guild Hall",
      "the Assassins' Guild Hall",
      PM_ROGUE,
      NON_PM,
      NON_PM,
      NON_PM,
      PM_MASTER_OF_THIEVES,
      PM_THUG,
      PM_MASTER_ASSASSIN,
      PM_LEPRECHAUN,
      PM_GUARDIAN_NAGA,
      S_NYMPH,
      S_NAGA,
      ART_MASTER_KEY_OF_THIEVERY,
      MH_HUMAN | MH_MERFOLK | MH_DEMON | MH_GHOUL | MH_ORC | MH_WERE | MH_GNOME | MH_ELF
        | MH_VAMPIRE,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_CHAOTIC,
      /* Str Int Wis Dex Con Cha */
      { 7, 7, 7, 10, 7, 6 },
      { 20, 10, 10, 30, 20, 10 },
      /* Init   Lower  Higher */
      { 10, 0, 0, 8, 1, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      11, /* Energy */
      10,
      8,
      0,
      1,
      9,
      A_INT,
      SPE_DETECT_TREASURE,
      -4 },
    { { "Ranger", 0, 0 },
      {
#if 0 /* OBSOLETE */
        {"Edhel",   "Elleth"},
        {"Edhel",   "Elleth"},         /* elf-maid */
        {"Ohtar",   "Ohtie"},          /* warrior */
        {"Kano",    "Kanie"},          /* commander (Q.) ['a] educated guess,
                                          until further research- SAC */
        {"Arandur"," Aranduriel"}, /* king's servant, minister (Q.) - guess */
        {"Hir",         "Hiril"},      /* lord, lady (S.) ['ir] */
        {"Aredhel",     "Arwen"},      /* noble elf, maiden (S.) */
        {"Ernil",       "Elentariel"}, /* prince (S.), elf-maiden (Q.) */
        {"Elentar",     "Elentari"},   /* Star-king, -queen (Q.) */
        "Solonor Thelandira", "Aerdrie Faenya", "Lolth", /* Elven */
#endif
        { "Tenderfoot", 0, 0 },
        { "Lookout", 0, 0 },
        { "Trailblazer", 0, 0 },
        { "Reconnoiterer", "Reconnoiteress", 0 },
        { "Scout", 0, 0 },
        { "Arbalester", 0, 0 }, /* One skilled at crossbows */
        { "Archer", 0, 0 },
        { "Sharpshooter", 0, 0 },
        { "Marksman", "Markswoman", "Marksperson" } },
      "Mercury", "_Venus", "Mars", /* Roman/planets */
      "Ran",
      "Orion's camp",
      "the cave of the wumpus",
      PM_RANGER,
      NON_PM,
      NON_PM,
      PM_LITTLE_DOG /* Orion & canis major */,
      PM_ORION,
      PM_HUNTER,
      PM_SCORPIUS,
      PM_FOREST_CENTAUR,
      PM_SCORPION,
      S_CENTAUR,
      S_SPIDER,
      ART_LONGBOW_OF_DIANA,
      MH_HUMAN | MH_MERFOLK | MH_ELF | MH_GNOME | MH_ORC | MH_ANGEL |
          MH_DEMON | MH_GHOUL | MH_WERE | MH_DWARF,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_NEUTRAL | ROLE_CHAOTIC,
      /* Str Int Wis Dex Con Cha */
      { 13, 13, 13, 9, 13, 7 },
      { 30, 10, 10, 20, 20, 10 },
      /* Init   Lower  Higher */
      { 13, 0, 0, 6, 1, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      12, /* Energy */
      10,
      9,
      2,
      1,
      10,
      A_INT,
      SPE_INVISIBILITY,
      -4 },
    { { "Samurai", 0, 0 },
      { { "Hatamoto", 0, 0 },       /* Banner Knight */
        { "Ronin", 0, 0 },          /* no allegiance */
        { "Ninja", "Kunoichi", "Sohei" }, /* secret society */
        { "Joshu", 0, 0 },          /* heads a castle */
        { "Ryoshu", 0, 0 },         /* has a territory */
        { "Kokushu", 0, 0 },        /* heads a province */
        { "Daimyo", 0, 0 },         /* a samurai lord */
        { "Kuge", 0, 0 },           /* Noble of the Court */
        { "Shogun", 0, 0 } },       /* supreme commander, warlord */
      "_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
      "Sam",
      "the Castle of the Taro Clan",
      "the Shogun's Castle",
      PM_SAMURAI,
      NON_PM,
      NON_PM,
      PM_LITTLE_DOG,
      PM_LORD_SATO,
      PM_ROSHI,
      PM_ASHIKAGA_TAKAUJI,
      PM_WOLF,
      PM_STALKER,
      S_DOG,
      S_ELEMENTAL,
      ART_TSURUGI_OF_MURAMASA,
      MH_HUMAN | MH_GHOUL,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_LAWFUL,
      /* Str Int Wis Dex Con Cha */
      { 10, 8, 7, 10, 17, 6 },
      { 30, 10, 8, 30, 14, 8 },
      /* Init   Lower  Higher */
      { 13, 0, 0, 8, 1, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      11, /* Energy */
      10,
      10,
      0,
      0,
      8,
      A_INT,
      SPE_CLAIRVOYANCE,
      -4 },
    { { "Tourist", 0, 0 },
      { { "Rambler", 0, 0 },
        { "Sightseer", 0, 0 },
        { "Excursionist", 0, 0 },
        { "Peregrinator", "Peregrinatrix", "Dabbler" },
        { "Traveler", 0, 0 },
        { "Journeyer", 0, 0 },
        { "Voyager", 0, 0 },
        { "Explorer", 0, 0 },
        { "Adventurer", 0, 0 } },
      "Blind Io", "_The Lady", "Offler", /* Discworld */
      "Tou",
      "Ankh-Morpork",
      "the Thieves' Guild Hall",
      PM_TOURIST,
      NON_PM,
      NON_PM,
      NON_PM,
      PM_TWOFLOWER,
      PM_GUIDE,
      PM_MASTER_OF_THIEVES,
      PM_GIANT_SPIDER,
      PM_FOREST_CENTAUR,
      S_SPIDER,
      S_CENTAUR,
      ART_YENDORIAN_EXPRESS_CARD,

      MH_HUMAN | MH_GIANT | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_NEUTRAL,

      /* Str Int Wis Dex Con Cha */
      { 7, 10, 6, 7, 7, 10 },
      { 15, 10, 10, 15, 30, 20 },
      /* Init   Lower  Higher */
      { 8, 0, 0, 8, 0, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      14, /* Energy */
      0,
      5,
      1,
      2,
      10,
      A_INT,
      SPE_CHARM_MONSTER,
      -4 },
    { { "Valkyrie", 0, 0 },
      { { "Stripling", 0, 0 },
        { "Skirmisher", 0, 0 },
        { "Fighter", 0, 0 },
        { "Man-at-arms", "Woman-at-arms", "Person-at-arms" },
        { "Warrior", 0, 0 },
        { "Swashbuckler", 0, 0 },
        { "Hero", "Heroine", "Heroix" },
        { "Champion", 0, 0 },
        { "Lord", "Lady", "Liege" } },
      "Tyr", "Odin", "Loki", /* Norse */
      "Val",
      "the Shrine of Destiny",
      "the cave of Surtur",
      PM_VALKYRIE,
      NON_PM,
      NON_PM,
      NON_PM /*PM_WINTER_WOLF_CUB*/,
      PM_NORN,
      PM_WARRIOR,
      PM_LORD_SURTUR,
      PM_FIRE_ANT,
      PM_FIRE_GIANT,
      S_ANT,
      S_GIANT,
      ART_ORB_OF_FATE,
      MH_HUMAN | MH_DWARF | MH_ANGEL | MH_WERE,
      ROLE_FEMALE | ROLE_NEUTER | ROLE_LAWFUL | ROLE_NEUTRAL,
      /* Str Int Wis Dex Con Cha */
      { 10, 7, 7, 7, 10, 7 },
      { 30, 6, 7, 20, 30, 7 },
      /* Init   Lower  Higher */
      { 14, 0, 0, 8, 2, 0 }, /* Hit points */
      { 1, 0, 0, 1, 0, 1 },
      10, /* Energy */
      0,
      10,
      -2,
      0,
      9,
      A_WIS,
      SPE_CONE_OF_COLD,
      -4 },
    { { "Wizard", 0, 0 },
      { { "Evoker", 0, 0 },
        { "Conjurer", 0, 0 },
        { "Thaumaturge", 0, 0 },
        { "Magician", 0, 0 },
        { "Enchanter", "Enchantress", "Enchantrix" },
        { "Sorcerer", "Sorceress", "Magister" },
        { "Necromancer", 0, 0 },
        { "Wizard", 0, 0 },
        { "Mage", 0, 0 } },
      "Ptah", "Thoth", "Anhur", /* Egyptian */
      "Wiz",
      "the Lonely Tower",
      "the Tower of Darkness",
      PM_WIZARD,
      NON_PM,
      NON_PM,
      PM_KITTEN,
      PM_NEFERET_THE_GREEN,
      PM_APPRENTICE,
      PM_DARK_ONE,
      PM_VAMPIRE_BAT,
      PM_XORN,
      S_BAT,
      S_WRAITH,
      ART_EYE_OF_THE_AETHIOPICA,
      MH_HUMAN | MH_ELF | MH_MERFOLK | MH_GNOME | MH_ORC | MH_DEMON | MH_ANGEL |
          MH_GHOUL | MH_WERE | MH_DWARF | MH_VAMPIRE,
      ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_NEUTRAL | ROLE_CHAOTIC,
      /* Str Int Wis Dex Con Cha */
      { 7, 10, 7, 7, 7, 7 },
      { 10, 30, 10, 20, 20, 10 },
      /* Init   Lower  Higher */
      { 10, 0, 0, 8, 1, 0 }, /* Hit points */
      { 4, 3, 0, 2, 0, 3 },
      12, /* Energy */
      0,
      1,
      0,
      3,
      10,
      A_INT,
      SPE_MAGIC_MISSILE,
      -4 },
    /* Array terminator */
    { }
};

/* The player's role, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Role urole = {
    { "Undefined", 0, 0 },
    { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
      { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
    "L", "N", "C",
    "Xxx", "home", "locate",
    NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
    0, 0, 0, 0, 0,
    /* Str Int Wis Dex Con Cha */
    { 7, 7, 7, 7, 7, 7 },
    { 20, 15, 15, 20, 20, 10 },
    /* Init   Lower  Higher */
    { 10, 0, 0, 8, 1, 0 }, /* Hit points */
    { 2, 0, 0, 2, 0, 3 },
    14, /* Energy */
     0,
    10,
     0,
     0,
     4,
    A_INT,
     0,
    -3
};

/* Macros to simplify subroles, to use where the subrole doesn't make that change.
 * All subroles should contain either SAME_X or SR_X (and a valid section) for all sections.
 *
 * Example (with all SR_X bits and all SAME_ blocks):
 *
 * { { "Wizzard", 0 },
        "Wizard",
        SR_WIZARD,
        SR_ATTRIB | SR_SPELL | SR_ALLOW | SR_QUEST | SR_PET | SR_RANK | SR_GODS | SR_NAME,
      {
        SAME_NAME,
        SAME_RANK,
        SAME_GODS,
        "Wiz",
        SAME_QUEST1,
        PM_WIZARD,
        NON_PM,
        SAME_PET,
        SAME_QUEST2,
        SAME_ALLOW,
        SAME_ATTRIB,
        SAME_SPELL
      }
    },
 **/
#define SAME_ATTRIB { 0 }, { 0 }, { 0 }, { 0 }, 0, 0
#define SAME_SPELL  0,0,0,0,0,0,0
#define SAME_ALLOW  0
#define SAME_PET  0
#define SAME_QUEST1 NULL,NULL
#define SAME_QUEST2 0,0,0,0,0,0,0,0
#define SAME_NAME   { NULL, NULL, NULL }
#define SAME_RANK   { { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 } }
#define SAME_GODS   NULL,NULL,NULL

/* Table of all subroles */
const struct Subrole subroles[] = {

    { { "Wizard", 0 },
        "Wizard",
        "The traditional unspecialized magic-user. You are capable of handling all spell schools "
        "effectively and you will begin with a wide assortment of magical items and spells, including "
        "a wizard's staff.",
        SR_WIZARD,
        0,
      {
        SAME_NAME,
        SAME_RANK,
        SAME_GODS,
        "Wiz",
        SAME_QUEST1,
        PM_WIZARD,
        NON_PM,
        NON_PM,
        SAME_PET,
        SAME_QUEST2,
        SAME_ALLOW,
        SAME_ATTRIB,
        SAME_SPELL
      }
    },

    { { "Alchemist", 0 },
        "Wizard",
        "A wizard who specializes in brewing potions. You'll begin with a stock of potions, an "
        "alchemist's smock, basic skill in alchemy and knowledge of alchemical recipes. Use #alchemy "
        "to list known recipes. Your special spell is polymorph.",
        SR_ALCHEMIST,
        SR_SPELL | SR_NAME,
      {
        { "Alchemist", 0 },
        SAME_RANK,
        SAME_GODS,
        "Wiz",
        SAME_QUEST1,
        PM_WIZARD,
        NON_PM,
        NON_PM,
        SAME_PET,
        SAME_QUEST2,
        SAME_ALLOW,
        SAME_ATTRIB,
        1,
        0,
        3,
        10,
        A_INT,
        SPE_POLYMORPH,
        -4
      }
    },

    /* Terminator */
    { { 0, 0 }, "", "", 0, 0, {} }
};

/* The player's subrole, created at runtime from initial
 * choices.
 */
struct Subrole usubrole;

/* Table of all races */
const struct Race races[] = {
    {
        "human",
        "human",
        "humanity",
        "Hum",
        { "man", "woman", "person" },
        PM_HUMAN,
        NON_PM,
        NON_PM,
        PM_HUMAN_MUMMY,
        PM_HUMAN_ZOMBIE,
        MH_HUMAN | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER
            | ROLE_LAWFUL | ROLE_NEUTRAL | ROLE_CHAOTIC,
        MH_HUMAN,
        0,
        MH_GNOME | MH_ORC,
        /*    Str     Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR18(100), 18, 18, 18, 18, 18 },
        /* Init   Lower  Higher */
        { 2, 0, 0, 2, 1, 0 }, /* Hit points */
        { 1, 0, 2, 0, 2, 0 }  /* Energy */
    },
    {
        "angel",
        "angelic",
        "celestialkind",
        "Ang",
        { 0, 0, 0 },
        PM_MINOR_ANGEL,
        NON_PM,
        NON_PM,
        NON_PM,
        NON_PM,
        MH_ANGEL | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_LAWFUL,
        MH_ANGEL,
        0,
        MH_DEMON,
        /*    Str     Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { 18, 18, 18, 18, 16, 20 },
        /* Init   Lower  Higher */
        { 2, 0, 0, 2, 1, 0 }, /* Hit points */
        { 1, 0, 2, 0, 2, 0 }  /* Energy */
    },
    {
        "elf",
        "elven",
        "elvenkind",
        "Elf",
        { 0, 0, 0 },
        PM_ELF,
        NON_PM,
        NON_PM,
        PM_ELF_MUMMY,
        PM_ELF_ZOMBIE,
        MH_ELF | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_CHAOTIC,
        MH_ELF,
        MH_ELF,
        MH_ORC,
        /*  Str    Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { 18, 20, 20, 18, 16, 18 },
        /* Init   Lower  Higher */
        { 1, 0, 0, 1, 1, 0 }, /* Hit points */
        { 2, 0, 3, 0, 3, 0 }  /* Energy */
    },
    {
        "infernal",
        "infernal",
        "demonkind",
        "Inf",
        { 0, 0, 0 },
        PM_INFERNAL,
        NON_PM,
        NON_PM,
        NON_PM,
        NON_PM,
        MH_DEMON | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER
            | ROLE_LAWFUL | ROLE_CHAOTIC,
        MH_DEMON,
        0,
        MH_GNOME | MH_ORC | MH_DWARF | MH_ELF | MH_ANGEL,
        /*    Str     Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR18(100), 18, 18, 18, 18, 20 },
        /* Init   Lower  Higher */
        { 2, 0, 0, 2, 1, 0 }, /* Hit points */
        { 1, 0, 2, 0, 2, 0 }  /* Energy */
    },
    {
        "merfolk",
        "merfolkish",
        "merfolkkind",
        "Mer",
        { 0, 0, 0 },
        PM_MERFOLK,
        NON_PM,
        NON_PM,
        NON_PM,
        NON_PM,
        MH_MERFOLK | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER
            | ROLE_NEUTRAL | ROLE_LAWFUL,
        MH_MERFOLK,
        MH_MERFOLK,
        MH_ORC,
        /*  Str    Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { 18, 20, 20, 16, 16, 14 },
        /* Init   Lower  Higher */
        { 1, 0, 0, 1, 1, 0 }, /* Hit points */
        { 2, 0, 3, 0, 3, 0 }  /* Energy */
    },
    {
        "drow",
        "droven",
        "drovenkind",
        "Dro",
    	  {0, 0, 0},
    	PM_DROW,
        NON_PM,
        NON_PM,
        PM_ELF_MUMMY,
        PM_ELF_ZOMBIE,
    	MH_ELF | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_CHAOTIC,
    	MH_ELF,
        0,
        MH_ORC | MH_DWARF | MH_GNOME,
      	/*  Str    Int Wis Dex Con Cha */
      	{    3,     3,  3,  3,  3,  3 },
      	{   18,    20, 20, 18, 16, 18 },
      	/* Init   Lower  Higher */
      	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
      	{  2, 0,  3, 0,  3, 0 }		/* Energy */
    },
    {
        "dwarf",
        "dwarven",
        "dwarvenkind",
        "Dwa",
        { 0, 0, 0 },
        PM_DWARF,
        NON_PM,
        NON_PM,
        PM_DWARF_MUMMY,
        PM_DWARF_ZOMBIE,
        MH_DWARF | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_LAWFUL,
        MH_DWARF,
        MH_DWARF | MH_GNOME,
        MH_ORC | MH_GIANT,
        /*    Str     Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR18(100), 16, 16, 20, 20, 16 },
        /* Init   Lower  Higher */
        { 4, 0, 0, 3, 2, 0 }, /* Hit points */
        { 0, 0, 0, 0, 0, 0 }  /* Energy */
    },
    {
        "gnome",
        "gnomish",
        "gnomehood",
        "Gno",
        { 0, 0, 0 },
        PM_GNOME,
        NON_PM,
        NON_PM,
        PM_GNOME_MUMMY,
        PM_GNOME_ZOMBIE,
        MH_GNOME | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_NEUTRAL,
        MH_GNOME,
        MH_DWARF | MH_GNOME,
        MH_HUMAN | MH_GIANT,
        /*  Str    Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR18(50), 19, 18, 18, 18, 18 },
        /* Init   Lower  Higher */
        { 1, 0, 0, 1, 0, 0 }, /* Hit points */
        { 2, 0, 2, 0, 2, 0 }  /* Energy */
    },
    {
        "ghoul",
        "ghoul",
        "ghoulkind",
        "Gul",
        { 0, 0, 0 },
        PM_GHOUL,
        NON_PM,
        NON_PM,
        PM_HUMAN_MUMMY,
        PM_HUMAN_ZOMBIE,
        MH_GHOUL | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER
            | ROLE_LAWFUL | ROLE_NEUTRAL | ROLE_CHAOTIC,
        MH_GHOUL,
        MH_GHOUL,
        MH_GNOME | MH_ELF | MH_DWARF,
        /*    Str     Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR18(50), 20, 16, 16, 18, 16 },
        /* Init   Lower  Higher */
        { 1, 0, 0, 0, 1, 0 }, /* Hit points */
        { 2, 0, 2, 0, 2, 0 }  /* Energy */
    },
    {
        "orc",
        "orcish",
        "orcdom",
        "Orc",
        { 0, 0, 0 },
        PM_ORC,
        NON_PM,
        NON_PM,
        PM_ORC_MUMMY,
        PM_ORC_ZOMBIE,
        MH_ORC | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER | ROLE_CHAOTIC,
        MH_ORC,
        0,
        MH_HUMAN | MH_ELF | MH_DWARF,
        /*  Str    Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR18(50), 16, 16, 18, 18, 16 },
        /* Init   Lower  Higher */
        { 1, 0, 0, 1, 0, 0 }, /* Hit points */
        { 1, 0, 1, 0, 1, 0 }  /* Energy */
    },
    {
        "werewolf",
        "lycanthrope",
        "lycanthrope-kind",
        "Wer",
        { "man", "woman", "person" },
        PM_HUMAN_WEREWOLF,
        NON_PM,
        NON_PM,
        PM_HUMAN_MUMMY,
        PM_HUMAN_ZOMBIE,
        MH_WERE | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER
            | ROLE_CHAOTIC | ROLE_NEUTRAL,
        MH_WERE,
        MH_WERE,
        MH_GNOME | MH_ORC | MH_ELF,
        /*    Str     Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR19(19), 15, 15, 20, 19, 15 },
        /* Init   Lower  Higher */
        { 2, 0, 0, 2, 1, 0 }, /* Hit points */
        { 1, 0, 2, 0, 2, 0 }  /* Energy */
    },
    {
        "giant",
        "giant",
        "giant-kind",
        "Gia",
        { 0, 0 },
        PM_GIANT,
        NON_PM,
        PM_GIANT_MUMMY,
        PM_GIANT_ZOMBIE,
        MH_GIANT | ROLE_MALE | ROLE_FEMALE | ROLE_CHAOTIC | ROLE_NEUTRAL,
        MH_GIANT,
        MH_GIANT,
        0,
        /*  Str    Int Wis Dex Con Cha */
        { 3, 3, 3, 3, 3, 3 },
        { STR19(25), 14, 18, 14, 25, 16 },
        /* Init   Lower  Higher */
        { 5, 0, 1, 3, 3, 0 }, /* Hit points */
        { 1, 0, 1, 0, 1, 0 }  /* Energy */
    },
    {
        "vampire", 
        "vampiric", 
        "vampirehood", 
        "Vam",
        {0, 0, 0},
        PM_VAMPIRE, 
        NON_PM, 
        NON_PM,
        PM_HUMAN_MUMMY, 
        PM_HUMAN_ZOMBIE,
        MH_VAMPIRE | ROLE_MALE | ROLE_FEMALE | ROLE_NEUTER 
            | ROLE_CHAOTIC,
        MH_VAMPIRE, 
        0, 
        MH_ELF | MH_GNOME | MH_DWARF | MH_ORC,
        /*    Str     Int Wis Dex Con Cha */
        {      4,      0,  0,  4,  3,  4 },
        { STR19(19), 18, 18, 20, 20, 20 },
        /* Init   Lower  Higher */
        {  3, 0,  0, 3,  2, 0 },	/* Hit points */
        {  3, 0,  4, 0,  4, 0 }		/* Energy */
    },
    /* Array terminator */
    { }
};

/* The player's race, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Race urace = {
    "something",
    "undefined",
    "something",
    "Xxx",
    { 0, 0, 0 },
    NON_PM,
    NON_PM,
    NON_PM,
    NON_PM,
    NON_PM,
    0,
    0,
    0,
    0,
    /*    Str     Int Wis Dex Con Cha */
    { 3, 3, 3, 3, 3, 3 },
    { STR18(100), 18, 18, 18, 18, 18 },
    /* Init   Lower  Higher */
    { 2, 0, 0, 2, 1, 0 }, /* Hit points */
    { 1, 0, 2, 0, 2, 0 }  /* Energy */
};

/* Table of all genders */
const struct Gender genders[] = {
    { "male", "he", "him", "his", "Mal", ROLE_MALE },
    { "female", "she", "her", "her", "Fem", ROLE_FEMALE },
    { "nonbinary", "they", "them", "their", "Nbn", ROLE_NEUTER },
};

const struct Orientation orientations[] = {
    { "straight", SEX_STRAIGHT },
    { "gay", SEX_GAY },
    { "asexual", SEX_ACE },
    { "bisexual", SEX_BI },
};

/* Table of all alignments */
const struct Align aligns[] = {
    { "law", "lawful", "Law", ROLE_LAWFUL, A_LAWFUL },
    { "balance", "neutral", "Neu", ROLE_NEUTRAL, A_NEUTRAL },
    { "chaos", "chaotic", "Cha", ROLE_CHAOTIC, A_CHAOTIC },
    { "evil", "unaligned", "Una", 0, A_NONE }
};

/* Filters */
static struct {
    boolean roles[SIZE(roles)];
    boolean subroles[SIZE(subroles)];
    short mask;
} rfilter;

STATIC_DCL int NDECL(randrole_filtered);
STATIC_DCL char *FDECL(promptsep, (char *, int));
STATIC_DCL int FDECL(role_gendercount, (int, int));
STATIC_DCL int FDECL(race_alignmentcount, (int));

int
randsubrole()
{
    return rn2(SIZE(subroles) - 1);
}

/* used by str2XXX() */
static char NEARDATA randomstr[] = "random";

boolean
validrole(rolenum)
int rolenum;
{
    return (boolean) (rolenum >= 0 && rolenum < SIZE(roles) - 1);
}

int
randrole(for_display)
boolean for_display;
{
    int res = SIZE(roles) - 1;

    if (for_display)
        res = rn2_on_display_rng(res);
    else
        res = rn2(res);
    return res;
}

STATIC_OVL int
randrole_filtered()
{
    int i, n = 0, set[SIZE(roles)];

    /* this doesn't rule out impossible combinations but attempts to
       honor all the filter masks */
    for (i = 0; i < SIZE(roles); ++i)
        if (ok_role(ROLE_NONE, i, ROLE_NONE, ROLE_NONE, ROLE_NONE)
            && ok_subrole(ROLE_RANDOM, i, ROLE_NONE, ROLE_NONE, ROLE_NONE)
            && ok_race(ROLE_NONE, i, ROLE_RANDOM, ROLE_NONE, ROLE_NONE)
            && ok_gend(ROLE_NONE, i, ROLE_NONE, ROLE_RANDOM, ROLE_NONE)
            && ok_align(ROLE_NONE, i, ROLE_NONE, ROLE_NONE, ROLE_RANDOM))
            set[n++] = i;
    return n ? set[rn2(n)] : randrole(FALSE);
}

int
str2subrole(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return ROLE_NONE;

    /* Match as much of str as is provided */
    len = strlen(str);
    for (i = 0; subroles[i].name.m; i++) {
        /* Does it match the male name? */
        if (!strncmpi(str, subroles[i].name.m, len))
            return i;
        /* Or the female name? */
        if (roles[i].name.f && !strncmpi(str, subroles[i].name.f, len))
            return i;
    }

    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return ROLE_RANDOM;

    /* Couldn't find anything appropriate */
    return ROLE_NONE;
}

int
str2role(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return ROLE_NONE;

    /* Match as much of str as is provided */
    len = strlen(str);
    for (i = 0; roles[i].name.m; i++) {
        /* Does it match the male name? */
        if (!strncmpi(str, roles[i].name.m, len))
            return i;
        /* Or the female name? */
        if (roles[i].name.f && !strncmpi(str, roles[i].name.f, len))
            return i;
        /* Or the nonbinary name? */
        if (roles[i].name.n && !strncmpi(str, roles[i].name.n, len))
            return i;
        /* Or the filecode? */
        if (!strcmpi(str, roles[i].filecode))
            return i;
    }

    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return ROLE_RANDOM;

    /* Couldn't find anything appropriate */
    return ROLE_NONE;
}

/* The combined allow mask of role and subrole.
 * subrole replaces role if the SR_ALLOW flag is set.
 **/
int allow_role(subrolenum, rolenum)
int subrolenum, rolenum;
{
    if (subrolenum >= 0) {
        if (subroles[subrolenum].flags & SR_ALLOW) {
            return subroles[subrolenum].r.allow;
        }
    }
    return roles[rolenum].allow;
}

boolean
validrace(subrolenum, rolenum, racenum)
int subrolenum, rolenum, racenum;
{
    /* Assumes validrole */
    return (boolean) (racenum >= 0 && racenum < SIZE(races) - 1
                      && (allow_role(subrolenum, rolenum) & races[racenum].allow
                          & ROLE_RACEMASK));
}

int
randrace(subrolenum, rolenum)
int subrolenum, rolenum;
{
    int i, n = 0;

    /* Count the number of valid races */
    for (i = 0; races[i].noun; i++)
        if (allow_role(subrolenum, rolenum) & races[i].allow & ROLE_RACEMASK)
            n++;

    /* Pick a random race */
    /* Use a factor of 100 in case of bad random number generators */
    if (n)
        n = rn2(n * 100) / 100;
    for (i = 0; races[i].noun; i++)
        if (allow_role(subrolenum, rolenum) & races[i].allow & ROLE_RACEMASK) {
            if (n)
                n--;
            else
                return i;
        }

    /* This role has no permitted races? */
    return rn2(SIZE(races) - 1);
}

int
str2race(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return ROLE_NONE;

    /* Match as much of str as is provided */
    len = strlen(str);
    for (i = 0; races[i].noun; i++) {
        /* Does it match the noun? */
        if (!strncmpi(str, races[i].noun, len))
            return i;
        /* Or the filecode? */
        if (!strcmpi(str, races[i].filecode))
            return i;
    }

    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return ROLE_RANDOM;

    /* Couldn't find anything appropriate */
    return ROLE_NONE;
}

boolean
validgend(subrolenum, rolenum, racenum, gendnum)
int subrolenum, rolenum, racenum, gendnum;
{
    /* Assumes validrole and validrace */
    return (boolean) (gendnum >= 0 && gendnum < ROLE_GENDERS
                      && (allow_role(subrolenum, rolenum) & races[racenum].allow
                          & genders[gendnum].allow & ROLE_GENDMASK));
}

int
randgend(subrolenum, rolenum, racenum)
int subrolenum, rolenum, racenum;
{
    int i, n = 0;

    /* Count the number of valid genders */
    for (i = 0; i < ROLE_GENDERS; i++)
        if (allow_role(subrolenum, rolenum) & races[racenum].allow & genders[i].allow
            & ROLE_GENDMASK)
            n++;

    /* Pick a random gender */
    if (n)
        n = rn2(n);
    for (i = 0; i < ROLE_GENDERS; i++)
        if (allow_role(subrolenum, rolenum) & races[racenum].allow & genders[i].allow
            & ROLE_GENDMASK) {
            if (n)
                n--;
            else
                return i;
        }

    /* This role/race has no permitted genders? */
    return rn2(ROLE_GENDERS);
}

int
str2gend(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return ROLE_NONE;

    /* Match as much of str as is provided */
    len = strlen(str);
    for (i = 0; i < ROLE_GENDERS; i++) {
        /* Does it match the adjective? */
        if (!strncmpi(str, genders[i].adj, len))
            return i;
        /* Or the filecode? */
        if (!strcmpi(str, genders[i].filecode))
            return i;
    }
    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return ROLE_RANDOM;

    /* Couldn't find anything appropriate */
    return ROLE_NONE;
}

int
str2orientation(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return SEX_STRAIGHT;

    /* Match as much of str as is provided */
    len = strlen(str);
    for (i = 0; i < ROLE_ORIENTATIONS; i++) {
        /* Does it match the adjective? */
        if (!strncmpi(str, orientations[i].adj, len))
            return i;
    }
    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return rn2(ROLE_ORIENTATIONS);

    /* Couldn't find anything appropriate */
    return SEX_STRAIGHT;
}

boolean
validalign(subrolenum, rolenum, racenum, alignnum)
int subrolenum, rolenum, racenum, alignnum;
{
    /* Assumes validrole and validrace */
    return (boolean) (alignnum >= 0 && alignnum < ROLE_ALIGNS
                      && (allow_role(subrolenum, rolenum) & races[racenum].allow
                          & aligns[alignnum].allow & ROLE_ALIGNMASK));
}

int
randalign(subrolenum, rolenum, racenum)
int subrolenum, rolenum, racenum;
{
    int i, n = 0;

    /* Count the number of valid alignments */
    for (i = 0; i < ROLE_ALIGNS; i++)
        if (allow_role(subrolenum, rolenum) & races[racenum].allow & aligns[i].allow
            & ROLE_ALIGNMASK)
            n++;

    /* Pick a random alignment */
    if (n)
        n = rn2(n);
    for (i = 0; i < ROLE_ALIGNS; i++)
        if (allow_role(subrolenum, rolenum) & races[racenum].allow & aligns[i].allow
            & ROLE_ALIGNMASK) {
            if (n)
                n--;
            else
                return i;
        }

    /* This role/race has no permitted alignments? */
    return rn2(ROLE_ALIGNS);
}

int
str2align(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return ROLE_NONE;

    /* Match as much of str as is provided */
    len = strlen(str);
    for (i = 0; i < ROLE_ALIGNS; i++) {
        /* Does it match the adjective? */
        if (!strncmpi(str, aligns[i].adj, len))
            return i;
        /* Or the filecode? */
        if (!strcmpi(str, aligns[i].filecode))
            return i;
    }
    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return ROLE_RANDOM;

    /* Couldn't find anything appropriate */
    return ROLE_NONE;
}

/* is rolenum compatible with any subrolenum/racenum/gendnum/alignnum constraints? */
boolean
ok_role(subrolenum, rolenum, racenum, gendnum, alignnum)
int subrolenum, rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (rolenum >= 0 && rolenum < SIZE(roles) - 1) {
        if (rfilter.roles[rolenum])
            return FALSE;
        allow = allow_role(subrolenum, rolenum);
        if (racenum >= 0 && racenum < SIZE(races) - 1
            && !(roles[rolenum].mhrace & races[racenum].selfmask))
            return FALSE;
        if (gendnum >= 0 && gendnum < ROLE_GENDERS
            && !(allow & genders[gendnum].allow & ROLE_GENDMASK))
            return FALSE;
        if (alignnum >= 0 && alignnum < ROLE_ALIGNS
            && !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
            return FALSE;
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < SIZE(roles) - 1; i++) {
            if (rfilter.roles[i])
                continue;
            allow = allow_role(subrolenum, rolenum);
            if (racenum >= 0 && racenum < SIZE(races) - 1
                && !(roles[i].mhrace & races[racenum].selfmask))
                continue;
            if (gendnum >= 0 && gendnum < ROLE_GENDERS
                && !(allow & genders[gendnum].allow & ROLE_GENDMASK))
                continue;
            if (alignnum >= 0 && alignnum < ROLE_ALIGNS
                && !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
                continue;
            return TRUE;
        }
        return FALSE;
    }
}

/* pick a random role subject to any subrolenum/racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_subrole(rolenum, racenum, gendnum, alignnum, pickhow)
int rolenum, racenum, gendnum, alignnum, pickhow;
{
    int i;
    int subroles_ok = 0, set[SIZE(subroles)];

    for (i = 0; i < SIZE(subroles) - 1; i++) {
        if (ok_subrole(i, rolenum, racenum, gendnum, alignnum)
            && ok_race(i, rolenum, (racenum >= 0) ? racenum : ROLE_RANDOM,
                       gendnum, alignnum)
            && ok_gend(i, rolenum, racenum,
                       (gendnum >= 0) ? gendnum : ROLE_RANDOM, alignnum)
            && ok_race(i, rolenum, racenum,
                       gendnum, (alignnum >= 0) ? alignnum : ROLE_RANDOM))
            set[subroles_ok++] = i;
    }
    if (subroles_ok == 0 || (subroles_ok > 1 && pickhow == PICK_RIGID))
        return ROLE_NONE;
    return set[rn2(subroles_ok)];
}

/* pick a random role subject to any subrolenum/racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_role(subrolenum, racenum, gendnum, alignnum, pickhow)
int subrolenum, racenum, gendnum, alignnum, pickhow;
{
    int i;
    int roles_ok = 0, set[SIZE(roles)];

    for (i = 0; i < SIZE(roles) - 1; i++) {
        if (ok_role(subrolenum, i, racenum, gendnum, alignnum)
            && ok_race(subrolenum, i, (racenum >= 0) ? racenum : ROLE_RANDOM,
                       gendnum, alignnum)
            && ok_gend(subrolenum, i, racenum,
                       (gendnum >= 0) ? gendnum : ROLE_RANDOM, alignnum)
            && ok_race(subrolenum, i, racenum,
                       gendnum, (alignnum >= 0) ? alignnum : ROLE_RANDOM))
            set[roles_ok++] = i;
    }
    if (roles_ok == 0 || (roles_ok > 1 && pickhow == PICK_RIGID))
        return ROLE_NONE;
    return set[rn2(roles_ok)];
}

/* is subrolenum compatible with any subrolenum/rolenum/gendnum/alignnum constraints? */
boolean
ok_subrole(subrolenum, rolenum, racenum, gendnum, alignnum)
int subrolenum, rolenum, racenum, gendnum, alignnum;
{
    nhUse(racenum);
    nhUse(gendnum);
    nhUse(alignnum);
    if ((rolenum >= 0) && (!strcmp(roles[rolenum].name.m, subroles[subrolenum].role)))
        return TRUE;
    return FALSE;
}

/* does a role have any subroles? */
boolean
has_subroles(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int subrolenum = 0;
    do {
        if (ok_subrole(subrolenum, rolenum, racenum, gendnum, alignnum)) return TRUE;
        subrolenum++;
    } while (subroles[subrolenum].name.m);
    return FALSE;
}

/* is racenum compatible with any subrolenum/rolenum/gendnum/alignnum constraints? */
boolean
ok_race(subrolenum, rolenum, racenum, gendnum, alignnum)
int subrolenum, rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (racenum >= 0 && racenum < SIZE(races) - 1) {
        if (rfilter.mask & races[racenum].selfmask)
            return FALSE;
        allow = races[racenum].allow;
        if (rolenum >= 0 && rolenum < SIZE(roles) - 1
            && !(allow & allow_role(subrolenum, rolenum) & ROLE_RACEMASK))
            return FALSE;
        if (gendnum >= 0 && gendnum < ROLE_GENDERS
            && !(allow & genders[gendnum].allow & ROLE_GENDMASK))
            return FALSE;
        if (alignnum >= 0 && alignnum < ROLE_ALIGNS
            && !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
            return FALSE;
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < SIZE(races) - 1; i++) {
            if (rfilter.mask & races[i].selfmask)
                continue;
            allow = races[i].allow;
            if (rolenum >= 0 && rolenum < SIZE(roles) - 1
                && !(allow & allow_role(subrolenum, rolenum) & ROLE_RACEMASK))
                continue;
            if (gendnum >= 0 && gendnum < ROLE_GENDERS
                && !(allow & genders[gendnum].allow & ROLE_GENDMASK))
                continue;
            if (alignnum >= 0 && alignnum < ROLE_ALIGNS
                && !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
                continue;
            return TRUE;
        }
        return FALSE;
    }
}

/* Pick a random race subject to any subrolenum/rolenum/gendnum/alignnum constraints.
   If pickhow == PICK_RIGID a race is returned only if there is
   a single possibility. */
int
pick_race(subrolenum, rolenum, gendnum, alignnum, pickhow)
int subrolenum, rolenum, gendnum, alignnum, pickhow;
{
    int i;
    int races_ok = 0;

    for (i = 0; i < SIZE(races) - 1; i++) {
        if (ok_race(subrolenum, rolenum, i, gendnum, alignnum))
            races_ok++;
    }
    if (races_ok == 0 || (races_ok > 1 && pickhow == PICK_RIGID))
        return ROLE_NONE;
    races_ok = rn2(races_ok);
    for (i = 0; i < SIZE(races) - 1; i++) {
        if (ok_race(subrolenum, rolenum, i, gendnum, alignnum)) {
            if (races_ok == 0)
                return i;
            else
                races_ok--;
        }
    }
    return ROLE_NONE;
}

/* is gendnum compatible with any subrolenum/rolenum/racenum/alignnum constraints? */
/* gender and alignment are not comparable (and also not constrainable) */
boolean
ok_gend(subrolenum, rolenum, racenum, gendnum, alignnum)
int subrolenum, rolenum, racenum, gendnum;
int alignnum UNUSED;
{
    int i;
    short allow;

    if (gendnum >= 0 && gendnum < ROLE_GENDERS) {
        if (rfilter.mask & genders[gendnum].allow)
            return FALSE;
        allow = genders[gendnum].allow;
        if (rolenum >= 0 && rolenum < SIZE(roles) - 1
            && !(allow & allow_role(subrolenum, rolenum) & ROLE_GENDMASK))
            return FALSE;
        if (racenum >= 0 && racenum < SIZE(races) - 1
            && !(allow & races[racenum].allow & ROLE_GENDMASK))
            return FALSE;
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < ROLE_GENDERS; i++) {
            if (rfilter.mask & genders[i].allow)
                continue;
            allow = genders[i].allow;
            if (rolenum >= 0 && rolenum < SIZE(roles) - 1
                && !(allow & allow_role(subrolenum, rolenum) & ROLE_GENDMASK))
                continue;
            if (racenum >= 0 && racenum < SIZE(races) - 1
                && !(allow & races[racenum].allow & ROLE_GENDMASK))
                continue;
            return TRUE;
        }
        return FALSE;
    }
}

/* pick a random gender subject to any subrolenum/rolenum/racenum/alignnum constraints */
/* gender and alignment are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID a gender is returned only if there is  */
/* a single possibility */
int
pick_gend(subrolenum, rolenum, racenum, alignnum, pickhow)
int subrolenum, rolenum, racenum, alignnum, pickhow;
{
    int i;
    int gends_ok = 0;

    for (i = 0; i < ROLE_GENDERS; i++) {
        if (ok_gend(subrolenum, rolenum, racenum, i, alignnum))
            gends_ok++;
    }
    if (gends_ok == 0 || (gends_ok > 1 && pickhow == PICK_RIGID))
        return ROLE_NONE;
    gends_ok = rn2(gends_ok);
    for (i = 0; i < ROLE_GENDERS; i++) {
        if (ok_gend(subrolenum, rolenum, racenum, i, alignnum)) {
            if (gends_ok == 0)
                return i;
            else
                gends_ok--;
        }
    }
    return ROLE_NONE;
}

/* is alignnum compatible with any subrolenum/rolenum/racenum/gendnum constraints? */
/* alignment and gender are not comparable (and also not constrainable) */
boolean
ok_align(subrolenum, rolenum, racenum, gendnum, alignnum)
int subrolenum, rolenum, racenum;
int gendnum UNUSED;
int alignnum;
{
    int i;
    short allow;

    if (alignnum >= 0 && alignnum < ROLE_ALIGNS) {
        if (rfilter.mask & aligns[alignnum].allow)
            return FALSE;
        allow = aligns[alignnum].allow;
        if (rolenum >= 0 && rolenum < SIZE(roles) - 1
            && !(allow & allow_role(subrolenum, rolenum) & ROLE_ALIGNMASK))
            return FALSE;
        if (racenum >= 0 && racenum < SIZE(races) - 1
            && !(allow & races[racenum].allow & ROLE_ALIGNMASK))
            return FALSE;
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < ROLE_ALIGNS; i++) {
            if (rfilter.mask & aligns[i].allow)
                return FALSE;
            allow = aligns[i].allow;
            if (rolenum >= 0 && rolenum < SIZE(roles) - 1
                && !(allow & allow_role(subrolenum, rolenum) & ROLE_ALIGNMASK))
                continue;
            if (racenum >= 0 && racenum < SIZE(races) - 1
                && !(allow & races[racenum].allow & ROLE_ALIGNMASK))
                continue;
            return TRUE;
        }
        return FALSE;
    }
}

/* Pick a random alignment subject to any subrolenum/rolenum/racenum/gendnum constraints;
   alignment and gender are not comparable (and also not constrainable).
   If pickhow == PICK_RIGID an alignment is returned only if there is
   a single possibility. */
int
pick_align(subrolenum, rolenum, racenum, gendnum, pickhow)
int subrolenum, rolenum, racenum, gendnum, pickhow;
{
    int i;
    int aligns_ok = 0;

    for (i = 0; i < ROLE_ALIGNS; i++) {
        if (ok_align(subrolenum, rolenum, racenum, gendnum, i))
            aligns_ok++;
    }
    if (aligns_ok == 0 || (aligns_ok > 1 && pickhow == PICK_RIGID))
        return ROLE_NONE;
    aligns_ok = rn2(aligns_ok);
    for (i = 0; i < ROLE_ALIGNS; i++) {
        if (ok_align(subrolenum, rolenum, racenum, gendnum, i)) {
            if (aligns_ok == 0)
                return i;
            else
                aligns_ok--;
        }
    }
    return ROLE_NONE;
}

void
rigid_role_checks()
{
    int tmp;

    /* Some roles are limited to a single race, alignment, or gender and
     * calling this routine prior to XXX_player_selection() will help
     * prevent an extraneous prompt that actually doesn't allow
     * you to choose anything further. Note the use of PICK_RIGID which
     * causes the pick_XX() routine to return a value only if there is one
     * single possible selection, otherwise it returns ROLE_NONE.
     *
     */
    if (flags.initrole == ROLE_RANDOM) {
        /* If the role was explicitly specified as ROLE_RANDOM
         * via -uXXXX-@ or OPTIONS=role:random then choose the role
         * in here to narrow down later choices.
         */
        flags.initrole = pick_role(flags.initsubrole, flags.initrace, flags.initgend,
                                   flags.initalign, PICK_RANDOM);
        if (flags.initrole < 0)
            flags.initrole = randrole_filtered();
    }
    if (flags.initrace == ROLE_RANDOM
        && (tmp = pick_race(flags.initsubrole, flags.initrole, flags.initgend,
                            flags.initalign, PICK_RANDOM)) != ROLE_NONE)
        flags.initrace = tmp;
    if (flags.initalign == ROLE_RANDOM
        && (tmp = pick_align(flags.initsubrole, flags.initrole, flags.initrace,
                             flags.initgend, PICK_RANDOM)) != ROLE_NONE)
        flags.initalign = tmp;
    if (flags.initgend == ROLE_RANDOM
        && (tmp = pick_gend(flags.initsubrole, flags.initrole, flags.initrace,
                            flags.initalign, PICK_RANDOM)) != ROLE_NONE)
        flags.initgend = tmp;

    if (flags.initrole != ROLE_NONE) {
        if (flags.initrace == ROLE_NONE)
            flags.initrace = pick_race(flags.initsubrole, flags.initrole, flags.initgend,
                                       flags.initalign, PICK_RIGID);
        if (flags.initalign == ROLE_NONE)
            flags.initalign = pick_align(flags.initsubrole, flags.initrole, flags.initrace,
                                         flags.initgend, PICK_RIGID);
        if (flags.initgend == ROLE_NONE)
            flags.initgend = pick_gend(flags.initsubrole, flags.initrole, flags.initrace,
                                       flags.initalign, PICK_RIGID);
    }
}

boolean
setsubrolefilter(bufp)
const char *bufp;
{
    int i;
    boolean reslt = TRUE;

    if ((i = str2subrole(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.subroles[i] = TRUE;
    else if ((i = str2role(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
		rfilter.roles[i] = TRUE;
    else if ((i = str2race(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.mask |= races[i].selfmask;
    else if ((i = str2gend(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.mask |= genders[i].allow;
    else if ((i = str2align(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.mask |= aligns[i].allow;
    else
        reslt = FALSE;
    return reslt;
}

boolean
setrolefilter(bufp)
const char *bufp;
{
    int i;
    boolean reslt = TRUE;

    if ((i = str2role(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.roles[i] = TRUE;
    else if ((i = str2race(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.mask |= races[i].selfmask;
    else if ((i = str2gend(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.mask |= genders[i].allow;
    else if ((i = str2align(bufp)) != ROLE_NONE && i != ROLE_RANDOM)
        rfilter.mask |= aligns[i].allow;
    else
        reslt = FALSE;
    return reslt;
}

void
clearsubrolefilter()
{
    int i;

    for (i = 0; i < SIZE(subroles); ++i)
        rfilter.subroles[i] = FALSE;
    rfilter.mask = 0;
}

boolean
gotsubrolefilter()
{
    return FALSE;
}

boolean
gotrolefilter()
{
    int i;

    if (rfilter.mask)
        return TRUE;
    for (i = 0; i < SIZE(roles); ++i)
        if (rfilter.roles[i])
            return TRUE;
    return FALSE;
}

void
clearrolefilter()
{
    int i;

    for (i = 0; i < SIZE(roles); ++i)
        rfilter.roles[i] = FALSE;
    rfilter.mask = 0;
}

#define BP_ALIGN 0
#define BP_GEND 1
#define BP_RACE 2
#define BP_ROLE 3
#define BP_SUBROLE 4
#define NUM_BP 5

STATIC_VAR char pa[NUM_BP], post_attribs;

STATIC_OVL char *
promptsep(buf, num_post_attribs)
char *buf;
int num_post_attribs;
{
    const char *conjuct = "and ";

    if (num_post_attribs > 1 && post_attribs < num_post_attribs
        && post_attribs > 1)
        Strcat(buf, ",");
    Strcat(buf, " ");
    --post_attribs;
    if (!post_attribs && num_post_attribs > 1)
        Strcat(buf, conjuct);
    return buf;
}

STATIC_OVL int
role_gendercount(subrolenum, rolenum)
int subrolenum;
int rolenum;
{
    int gendcount = 0;

    if (validrole(rolenum)) {
        if (allow_role(subrolenum, rolenum) & ROLE_MALE)
            ++gendcount;
        if (allow_role(subrolenum, rolenum) & ROLE_FEMALE)
            ++gendcount;
        if (allow_role(subrolenum, rolenum) & ROLE_NEUTER)
            ++gendcount;
        if (roles[rolenum].allow & ROLE_NEUTER)
            ++gendcount;
    }
    return gendcount;
}

/* Change urole based on your subrole. This is run from role_init on creation
 * and restoring.
 */
STATIC_OVL void
set_subrole(subrole)
int subrole;
{
    if (has_subroles(flags.initrole, flags.initrace, flags.initgend, flags.initalign)) {
        const struct Subrole *sr = &subroles[subrole];
        const struct Role *r = &sr->r;
        unsigned f = sr->flags;
        usubrole = subroles[flags.initsubrole];

        /* Does this subrole change... */

        /* the role's name? */
        if (f & SR_NAME) {
            urole.name = r->name;
        }

        /* names for experience levels? */
        if (f & SR_RANK) {
            memcpy(urole.rank, r->rank, sizeof(urole.rank));
        }

        /* the gods' names? */
        if (f & SR_GODS) {
            urole.lgod = r->lgod;
            urole.ngod = r->ngod;
            urole.cgod = r->cgod;
        }

        /* the quest? */
        if (f & SR_QUEST) {
            urole.homebase = r->homebase;
            urole.intermed = r->intermed;

            urole.ldrnum = r->ldrnum;
            urole.guardnum = r->guardnum;
            urole.enemy1num = r->enemy1num;
            urole.enemy2num = r->enemy2num;
            urole.enemy1num = r->enemy1num;
            urole.enemy2num = r->enemy2num;
            urole.enemy1sym = r->enemy1sym;
            urole.enemy2sym = r->enemy2sym;
            urole.questarti = r->questarti;
        }

        /* the allowed races, genders, or alignment? */
        if (f & SR_ALLOW) {
            urole.allow = r->allow;
        }

        /* your preferred pet? */
        if (f & SR_PET) {
            urole.petnum = r->petnum;
        }

        /* your attributes? */
        if (f & SR_ATTRIB) {
            urole.xlev = r->xlev;
            urole.initrecord = r->initrecord;
            urole.hpadv = r->hpadv;
            urole.enadv = r->enadv;
            memcpy(urole.attrbase, r->attrbase, sizeof(urole.attrbase));
            memcpy(urole.attrdist, r->attrdist, sizeof(urole.attrdist));
        }

        /* your spell statistics? */
        if (f & SR_SPELL) {
            urole.spelbase = r->spelbase;
            urole.spelheal = r->spelheal;
            urole.spelshld = r->spelshld;
            urole.spelarmr = r->spelarmr;
            urole.spelstat = r->spelstat;
            urole.spelspec = r->spelspec;
            urole.spelsbon = r->spelsbon;
        }
    }
}

STATIC_OVL int
race_alignmentcount(racenum)
int racenum;
{
    int aligncount = 0;

    if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
        if (races[racenum].allow & ROLE_CHAOTIC)
            ++aligncount;
        if (races[racenum].allow & ROLE_LAWFUL)
            ++aligncount;
        if (races[racenum].allow & ROLE_NEUTRAL)
            ++aligncount;
    }
    return aligncount;
}

char *
root_plselection_prompt(suppliedbuf, buflen, subrolenum, rolenum, racenum, gendnum,
                        alignnum)
char *suppliedbuf;
int buflen, subrolenum, rolenum, racenum, gendnum, alignnum;
{
    int k, gendercount = 0, aligncount = 0;
    char buf[BUFSZ];
    static char err_ret[] = " character's";
    boolean donefirst = FALSE;

    if (!suppliedbuf || buflen < 1)
        return err_ret;

    /* initialize these static variables each time this is called */
    post_attribs = 0;
    for (k = 0; k < NUM_BP; ++k)
        pa[k] = 0;
    buf[0] = '\0';
    *suppliedbuf = '\0';

    /* How many alignments are allowed for the desired race? */
    if (racenum != ROLE_NONE && racenum != ROLE_RANDOM)
        aligncount = race_alignmentcount(racenum);

    if (alignnum != ROLE_NONE && alignnum != ROLE_RANDOM
        && ok_align(subrolenum, rolenum, racenum, gendnum, alignnum)) {
        /* if race specified, and multiple choice of alignments for it */
        if ((racenum >= 0) && (aligncount > 1)) {
            if (donefirst)
                Strcat(buf, " ");
            Strcat(buf, aligns[alignnum].adj);
            donefirst = TRUE;
        } else {
            if (donefirst)
                Strcat(buf, " ");
            Strcat(buf, aligns[alignnum].adj);
            donefirst = TRUE;
        }
    } else {
        /* in case we got here by failing the ok_align() test */
        if (alignnum != ROLE_RANDOM)
            alignnum = ROLE_NONE;
        /* if alignment not specified, but race is specified
           and only one choice of alignment for that race then
           don't include it in the later list */
        if ((((racenum != ROLE_NONE && racenum != ROLE_RANDOM)
              && ok_race(subrolenum, rolenum, racenum, gendnum, alignnum))
             && (aligncount > 1))
            || (racenum == ROLE_NONE || racenum == ROLE_RANDOM)) {
            pa[BP_ALIGN] = 1;
            post_attribs++;
        }
    }
    /* <your lawful> */

    /* How many genders are allowed for the desired role? */
    if (validrole(rolenum))
        gendercount = role_gendercount(subrolenum, rolenum);

    if (gendnum != ROLE_NONE && gendnum != ROLE_RANDOM) {
        if (validrole(rolenum)) {
            /* if role specified, and multiple choice of genders for it,
               and name of role itself does not distinguish gender */
            if ((rolenum != ROLE_NONE) && (gendercount > 1)
                && !roles[rolenum].name.f) {
                if (donefirst)
                    Strcat(buf, " ");
                Strcat(buf, genders[gendnum].adj);
                donefirst = TRUE;
            }
        } else {
            if (donefirst)
                Strcat(buf, " ");
            Strcat(buf, genders[gendnum].adj);
            donefirst = TRUE;
        }
    } else {
        /* if gender not specified, but role is specified
                and only one choice of gender then
                don't include it in the later list */
        if ((validrole(rolenum) && (gendercount > 1))
            || !validrole(rolenum)) {
            pa[BP_GEND] = 1;
            post_attribs++;
        }
    }
    /* <your lawful female> */

    if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
        if (validrole(rolenum)
            && ok_race(subrolenum, rolenum, racenum, gendnum, alignnum)) {
            if (donefirst)
                Strcat(buf, " ");
            Strcat(buf, (rolenum == ROLE_NONE) ? races[racenum].noun
                                               : races[racenum].adj);
            donefirst = TRUE;
        } else if (!validrole(rolenum)) {
            if (donefirst)
                Strcat(buf, " ");
            Strcat(buf, races[racenum].noun);
            donefirst = TRUE;
        } else {
            pa[BP_RACE] = 1;
            post_attribs++;
        }
    } else {
        pa[BP_RACE] = 1;
        post_attribs++;
    }
    /* <your lawful female gnomish> || <your lawful female gnome> */

    if (validrole(rolenum)) {
        if (donefirst)
            Strcat(buf, " ");
        if (gendnum != ROLE_NONE) {
            if (gendnum == 1 && roles[rolenum].name.f)
                Strcat(buf, roles[rolenum].name.f);
            else if (gendnum == 2 && roles[rolenum].name.n)
                Strcat(buf, roles[rolenum].name.n);
            else
                Strcat(buf, roles[rolenum].name.m);
        } else {
            if (roles[rolenum].name.f) {
                Strcat(buf, roles[rolenum].name.m);
                Strcat(buf, "/");
                Strcat(buf, roles[rolenum].name.f);
            } else
                Strcat(buf, roles[rolenum].name.m);
            if (roles[rolenum].name.n) {
                Strcat(buf, "/");
                Strcat(buf, roles[rolenum].name.n);
            }
        }
        donefirst = TRUE;
    } else if (rolenum == ROLE_NONE) {
        pa[BP_ROLE] = 1;
        post_attribs++;
    }

    if ((racenum == ROLE_NONE || racenum == ROLE_RANDOM)
        && !validrole(rolenum)) {
        if (donefirst)
            Strcat(buf, " ");
        Strcat(buf, "character");
        donefirst = TRUE;
    }
    /* <your lawful female gnomish cavewoman> || <your lawful female gnome>
     *    || <your lawful female character>
     */
    if (buflen > (int) (strlen(buf) + 1)) {
        Strcpy(suppliedbuf, buf);
        return suppliedbuf;
    } else
        return err_ret;
}

char *
build_plselection_prompt(buf, buflen, subrolenum, rolenum, racenum, gendnum, alignnum)
char *buf;
int buflen, subrolenum, rolenum, racenum, gendnum, alignnum;
{
    const char *defprompt = "Shall I pick a character for you? [ynaq] ";
    int num_post_attribs = 0;
    char tmpbuf[BUFSZ], *p;

    if (buflen < QBUFSZ)
        return (char *) defprompt;

    Strcpy(tmpbuf, "Shall I pick ");
    if (racenum != ROLE_NONE || validrole(rolenum))
        Strcat(tmpbuf, "your ");
    else
        Strcat(tmpbuf, "a ");
    /* <your> */

    (void) root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
                                   subrolenum, rolenum, racenum, gendnum, alignnum);
    /* "Shall I pick a character's role, race, gender, and alignment for you?"
       plus " [ynaq] (y)" is a little too long for a conventional 80 columns;
       also, "pick a character's <anything>" sounds a bit stilted */
    strsubst(tmpbuf, "pick a character", "pick character");

    Sprintf(buf, "%s", s_suffix(tmpbuf));
    /* don't bother splitting caveman/cavewoman or priest/priestess
       in order to apply possessive suffix to both halves, but do
       change "priest/priestess'" to "priest/priestess's" */
    if ((p = strstri(buf, "priest/priestess'")) != 0
        && p[sizeof "priest/priestess'" - sizeof ""] == '\0')
        strkitten(buf, 's');

    /* buf should now be:
     *    <your lawful female gnomish cavewoman's>
     * || <your lawful female gnome's>
     * || <your lawful female character's>
     *
     * Now append the post attributes to it
     */
    num_post_attribs = post_attribs;
    if (!num_post_attribs) {
        /* some constraints might have been mutually exclusive, in which case
           some prompting that would have been omitted is needed after all */
        if (flags.initrole == ROLE_NONE && !pa[BP_ROLE])
            pa[BP_ROLE] = ++post_attribs;
        if (flags.initsubrole == ROLE_NONE && !pa[BP_SUBROLE])
            pa[BP_SUBROLE] = ++post_attribs;
        if (flags.initrace == ROLE_NONE && !pa[BP_RACE])
            pa[BP_RACE] = ++post_attribs;
        if (flags.initalign == ROLE_NONE && !pa[BP_ALIGN])
            pa[BP_ALIGN] = ++post_attribs;
        if (flags.initgend == ROLE_NONE && !pa[BP_GEND])
            pa[BP_GEND] = ++post_attribs;
        num_post_attribs = post_attribs;
    }
    if (num_post_attribs) {
        if (pa[BP_RACE]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "race");
        }
        if (pa[BP_ROLE]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "role");
        }
        if (pa[BP_SUBROLE]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "subrole");
        }
        if (pa[BP_GEND]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "gender");
        }
        if (pa[BP_ALIGN]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "alignment");
        }
    }
    Strcat(buf, " for you? [ynaq] ");
    return buf;
}

#undef BP_ALIGN
#undef BP_GEND
#undef BP_RACE
#undef BP_ROLE
#undef BP_SUBROLE
#undef NUM_BP

void
plnamesuffix()
{
    char *sptr, *eptr;
    int i;

    /* some generic user names will be ignored in favor of prompting */
    if (sysopt.genericusers) {
        if (*sysopt.genericusers == '*') {
            *plname = '\0';
        } else {
            i = (int) strlen(plname);
            if ((sptr = strstri(sysopt.genericusers, plname)) != 0
                && (sptr == sysopt.genericusers || sptr[-1] == ' ')
                && (sptr[i] == ' ' || sptr[i] == '\0'))
                *plname = '\0'; /* call askname() */
        }
    }

    do {
        if (!*plname)
            askname(); /* fill plname[] if necessary, or set defer_plname */

        /* Look for tokens delimited by '-' */
        if ((eptr = index(plname, '-')) != (char *) 0)
            *eptr++ = '\0';
        while (eptr) {
            /* Isolate the next token */
            sptr = eptr;
            if ((eptr = index(sptr, '-')) != (char *) 0)
                *eptr++ = '\0';

            /* Try to match it to something */
            if ((i = str2role(sptr)) != ROLE_NONE)
                flags.initrole = i;
            else if ((i = str2race(sptr)) != ROLE_NONE)
                flags.initrace = i;
            else if ((i = str2gend(sptr)) != ROLE_NONE)
                flags.initgend = i;
            else if ((i = str2align(sptr)) != ROLE_NONE)
                flags.initalign = i;
        }
    } while (!*plname && !iflags.defer_plname);

    /* commas in the plname confuse the record file, convert to spaces */
    for (sptr = plname; *sptr; sptr++) {
        if (*sptr == ',')
            *sptr = ' ';
    }
}

/* show current settings for name, role, race, gender, and alignment
   in the specified window */
void
role_selection_prolog(which, where)
int which;
winid where;
{
    static const char NEARDATA choosing[] = " choosing now",
                               not_yet[] = " not yet specified",
                               rand_choice[] = " random";
    char buf[BUFSZ];
    int r, s, c, g, a, allowmask;

    r = flags.initrole;
    s = flags.initsubrole;
    c = flags.initrace;
    g = flags.initgend;
    a = flags.initalign;
    if (r >= 0) {
        allowmask = roles[r].allow;
        if (roles[r].mhrace == MH_HUMAN)
            c = 0; /* races[human] */
        else if (c >= 0 && !(roles[r].mhrace & races[c].allow))
            c = ROLE_RANDOM;
        if ((allowmask & ROLE_GENDMASK) == ROLE_MALE)
            g = 0; /* role forces male (hypothetical) */
        else if ((allowmask & ROLE_GENDMASK) == ROLE_FEMALE)
            g = 1; /* role forces female (valkyrie) */
        if ((allowmask & ROLE_ALIGNMASK) == AM_LAWFUL)
            a = 0; /* aligns[lawful] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_NEUTRAL)
            a = 1; /* aligns[neutral] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_CHAOTIC)
            a = 2; /* alings[chaotic] */
    }
    if (c >= 0) {
        allowmask = races[c].allow;
        if ((allowmask & ROLE_ALIGNMASK) == AM_LAWFUL)
            a = 0; /* aligns[lawful] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_NEUTRAL)
            a = 1; /* aligns[neutral] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_CHAOTIC)
            a = 2; /* alings[chaotic] */
        /* [c never forces gender] */
    }
    /* [g and a don't constrain anything sufficiently
       to narrow something done to a single choice] */

    Sprintf(buf, "%12s ", "name:");
    Strcat(buf, (which == RS_NAME) ? choosing : !*plname ? not_yet : plname);
    putstr(where, 0, buf);
    Sprintf(buf, "%12s ", "role:");
    Strcat(buf, (which == RS_ROLE) ? choosing : (r == ROLE_NONE)
                                                    ? not_yet
                                                    : (r == ROLE_RANDOM)
                                                          ? rand_choice
                                                          : roles[r].name.m);
    if (r >= 0 && (roles[r].name.f || roles[r].name.n)) {
        /* distinct female name [caveman/cavewoman, priest/priestess] */
        if (g == 1)
            /* female specified; replace male role name with female one */
            Sprintf(index(buf, ':'), ": %s", roles[r].name.f);
        else if (g == 2)
            /* nonbinary specified; replace male role name with female one */
            Sprintf(index(buf, ':'), ": %s", roles[r].name.n);
        else if (g < 0)
            /* gender unspecified; append slash and female role name */
            Sprintf(eos(buf), "/%s", roles[r].name.f);
    }
    putstr(where, 0, buf);

    if (s >= 0) {
        Sprintf(buf, "%12s ", "subrole:");
        Strcat(buf, (which == RS_SUBROLE) ? choosing : (s == ROLE_NONE)
                                                        ? not_yet
                                                        : (s == ROLE_RANDOM)
                                                              ? rand_choice
                                                              : subroles[s].name.m);
        if (s >= 0 && subroles[s].name.f) {
            /* distinct female name [caveman/cavewoman, priest/priestess] */
            if (g == 1)
                /* female specified; replace male role name with female one */
                Sprintf(index(buf, ':'), ": %s", subroles[s].name.f);
            else if (g < 0)
                /* gender unspecified; append slash and female role name */
                Sprintf(eos(buf), "/%s", subroles[s].name.f);
        }
        putstr(where, 0, buf);
    }

    Sprintf(buf, "%12s ", "race:");
    Strcat(buf, (which == RS_RACE) ? choosing : (c == ROLE_NONE)
                                                    ? not_yet
                                                    : (c == ROLE_RANDOM)
                                                          ? rand_choice
                                                          : races[c].noun);
    putstr(where, 0, buf);
    Sprintf(buf, "%12s ", "gender:");
    Strcat(buf, (which == RS_GENDER) ? choosing : (g == ROLE_NONE)
                                                      ? not_yet
                                                      : (g == ROLE_RANDOM)
                                                            ? rand_choice
                                                            : genders[g].adj);
    putstr(where, 0, buf);
    Sprintf(buf, "%12s ", "alignment:");
    Strcat(buf, (which == RS_ALGNMNT) ? choosing : (a == ROLE_NONE)
                                                       ? not_yet
                                                       : (a == ROLE_RANDOM)
                                                             ? rand_choice
                                                             : aligns[a].adj);
    putstr(where, 0, buf);
    
    /* Add subrole info */
    if (s >= 0) {
        const int width = 31; // because "alignment: not yet specified"
        int length = strlen(subroles[s].info);
        char *buf = malloc(length+1);
        char *ibuf = buf;
        memcpy(buf, subroles[s].info, length+1);
        putstr(where, 0, "");
        do {
            if (length <= width) {
                putstr(where, 0, buf);
                length = 0;
            } else {
                char *at = buf+width;
                while ((*at != ' ') && (at != buf)) at--;
                if (at == buf) at = buf+width; // skip 1 - but shouldn't happen
                *at = 0;
                putstr(where, 0, buf);
                buf = at+1;
                length = strlen(buf);
            }
        } while (length);
        free(ibuf);
    }
}

/* add a "pick alignment first"-type entry to the specified menu */
void
role_menu_extra(which, where, preselect)
int which;
winid where;
boolean preselect;
{
    static NEARDATA const char RS_menu_let[] = {
        '=',  /* name */
        '?',  /* role */
        '!',  /* subrole */
        '/',  /* race */
        '\"', /* gender */
        '[',  /* alignment */
    };
    anything any;
    char buf[BUFSZ];
    const char *what = 0, *constrainer = 0, *forcedvalue = 0;
    int f = 0, r, c, g, a, i, allowmask;

    r = flags.initrole;
    c = flags.initrace;
    switch (which) {
    case RS_NAME:
        what = "name";
        break;
    case RS_ROLE:
        what = "role";
        f = r;
        for (i = 0; i < SIZE(roles); ++i)
            if (i != f && !rfilter.roles[i])
                break;
        if (i == SIZE(roles)) {
            constrainer = "filter";
            forcedvalue = "role";
        }
        break;
    case RS_SUBROLE:
        what = "subrole";
        f = flags.initsubrole;
        for (i = 0; i < SIZE(subroles); ++i)
            if (i != f && !rfilter.subroles[i])
                break;
        if (i == SIZE(subroles)) {
            constrainer = "filter";
            forcedvalue = "subrole";
        }
        break;
    case RS_RACE:
        what = "race";
        f = flags.initrace;
        c = ROLE_NONE; /* override player's setting */
        if (r >= 0) {
            allowmask = roles[r].mhrace;
            if (allowmask == MH_HUMAN)
                c = 0; /* races[human] */
            if (c >= 0) {
                constrainer = "role";
                forcedvalue = races[c].noun;
            } else if (f >= 0
                       && (allowmask & ~rfilter.mask) == races[f].selfmask) {
                /* if there is only one race choice available due to user
                   options disallowing others, race menu entry is disabled */
                constrainer = "filter";
                forcedvalue = "race";
            }
        }
        break;
    case RS_GENDER:
        what = "gender";
        f = flags.initgend;
        g = ROLE_NONE;
        if (r >= 0) {
            allowmask = roles[r].allow & ROLE_GENDMASK;
            if (allowmask == ROLE_MALE)
                g = 0; /* genders[male] */
            else if (allowmask == ROLE_FEMALE)
                g = 1; /* genders[female] */
            else if (allowmask == ROLE_NEUTER)
                g = 3; /* genders[nonbinary] */
            if (g >= 0) {
                constrainer = "role";
                forcedvalue = genders[g].adj;
            } else if (f >= 0
                       && (allowmask & ~rfilter.mask) == genders[f].allow) {
                /* if there is only one gender choice available due to user
                   options disallowing other, gender menu entry is disabled */
                constrainer = "filter";
                forcedvalue = "gender";
            }
        }
        break;
    case RS_ALGNMNT:
        what = "alignment";
        f = flags.initalign;
        a = ROLE_NONE;
        if (r >= 0) {
            allowmask = roles[r].allow & ROLE_ALIGNMASK;
            if (allowmask == AM_LAWFUL)
                a = 0; /* aligns[lawful] */
            else if (allowmask == AM_NEUTRAL)
                a = 1; /* aligns[neutral] */
            else if (allowmask == AM_CHAOTIC)
                a = 2; /* aligns[chaotic] */
            if (a >= 0)
                constrainer = "role";
        }
        if (c >= 0 && !constrainer) {
            allowmask = races[c].allow & ROLE_ALIGNMASK;
            if (allowmask == AM_LAWFUL)
                a = 0; /* aligns[lawful] */
            else if (allowmask == AM_NEUTRAL)
                a = 1; /* aligns[neutral] */
            else if (allowmask == AM_CHAOTIC)
                a = 2; /* aligns[chaotic] */
            if (a >= 0)
                constrainer = "race";
        }
        if (f >= 0 && !constrainer
            && (ROLE_ALIGNMASK & ~rfilter.mask) == aligns[f].allow) {
            /* if there is only one alignment choice available due to user
               options disallowing others, algn menu entry is disabled */
            constrainer = "filter";
            forcedvalue = "alignment";
        }
        if (a >= 0)
            forcedvalue = aligns[a].adj;
        break;
    }

    any = zeroany; /* zero out all bits */
    if (constrainer) {
        any.a_int = 0;
        /* use four spaces of padding to fake a grayed out menu choice */
        Sprintf(buf, "%4s%s forces %s", "", constrainer, forcedvalue);
        add_menu(where, NO_GLYPH, &any, 0, 0, ATR_NONE, buf,
                 MENU_UNSELECTED);
    } else if (what) {
        any.a_int = RS_menu_arg(which);
        Sprintf(buf, "Pick%s %s first", (f >= 0) ? " another" : "", what);
        add_menu(where, NO_GLYPH, &any, RS_menu_let[which], 0, ATR_NONE, buf,
                 MENU_UNSELECTED);
    } else if (which == RS_filter) {
        any.a_int = RS_menu_arg(RS_filter);
        add_menu(where, NO_GLYPH, &any, '~', 0, ATR_NONE,
                 "Reset role/race/&c filtering", MENU_UNSELECTED);
    } else if (which == ROLE_RANDOM) {
        any.a_int = ROLE_RANDOM;
        add_menu(where, NO_GLYPH, &any, '*', 0, ATR_NONE, "Random",
                 preselect ? MENU_SELECTED : MENU_UNSELECTED);
    } else if (which == ROLE_NONE) {
        any.a_int = ROLE_NONE;
        add_menu(where, NO_GLYPH, &any, 'q', 0, ATR_NONE, "Quit",
                 preselect ? MENU_SELECTED : MENU_UNSELECTED);
    } else {
        impossible("role_menu_extra: bad arg (%d)", which);
    }
}

/*
 *      Special setup modifications here:
 *
 *      Unfortunately, this is going to have to be done
 *      on each newgame or restore, because you lose the permonst mods
 *      across a save/restore.  :-)
 *
 *      1 - The Rogue Leader is the Tourist Nemesis.
 *      2 - Priests start with a random alignment - convert the leader and
 *          guardians here.
 *      3 - Priests also get their of deities from a randomly chosen role.
 *      4 - [obsolete] Elves can have one of two different leaders,
 *          but can't work it out here because it requires hacking the
 *          level file data (see sp_lev.c).
 *
 * This code also replaces quest_init().
 */
void
role_init()
{
    int alignmnt;
    struct permonst *pm;

    /* Strip the role letter out of the player name.
     * This is included for backwards compatibility.
     */
    plnamesuffix();

    /* Check for a valid role.  Try flags.initrole first. */
    if (!validrole(flags.initrole)) {
        /* Try the player letter second */
        if ((flags.initrole = str2role(pl_character)) < 0)
            /* None specified; pick a random role */
            flags.initrole = randrole_filtered();
    }

    /* We now have a valid role index.  Copy the role name back. */
    /* This should become OBSOLETE */
    Strcpy(pl_character, roles[flags.initrole].name.m);
    pl_character[PL_CSIZ - 1] = '\0';

    /* Check for a valid race */
    if (!validrace(flags.initsubrole, flags.initrole, flags.initrace))
        flags.initrace = randrace(flags.initsubrole, flags.initrole);

    /* Check for a valid gender.  If new game, check both initgend
     * and female.  On restore, assume flags.gender is correct. */
    flags.gender = flags.initgend;

    if (flags.pantheon == -1) { /* new game */
        if (!validgend(flags.initsubrole, flags.initrole, flags.initrace, flags.gender))
            flags.gender = !flags.gender;
    }
    if (!validgend(flags.initsubrole, flags.initrole, flags.initrace, flags.initgend))
        /* Note that there is no way to check for an unspecified gender. */
        flags.initgend = flags.gender;

    /* Check for a valid alignment */
    if (!validalign(flags.initsubrole, flags.initrole, flags.initrace, flags.initalign))
        /* Pick a random alignment */
        flags.initalign = randalign(flags.initsubrole, flags.initrole, flags.initrace);
    alignmnt = aligns[flags.initalign].value;

    /* Initialize urole and urace */
    urole = roles[flags.initrole];
    set_subrole(flags.initsubrole);
    urace = races[flags.initrace];

    /* Fix up the quest leader */
    if (urole.ldrnum != NON_PM) {
        pm = &mons[urole.ldrnum];
        pm->msound = MS_LEADER;
        pm->mflags2 |= (M2_PEACEFUL);
        pm->mflags3 |= M3_CLOSE;
        pm->maligntyp = alignmnt * 3;
        /* if gender is random, we choose it now instead of waiting
           until the leader monster is created */
        quest_status.ldrgend =
            is_neuter(pm) ? 2 : is_female(pm) ? 1 : is_male(pm)
                                                        ? 0
                                                        : (rn2(100) < 50);
    }

    /* Fix up the quest guardians */
    if (urole.guardnum != NON_PM) {
        pm = &mons[urole.guardnum];
        pm->mflags2 |= (M2_PEACEFUL);
        pm->maligntyp = alignmnt * 3;
    }

    /* Fix up the quest nemesis */
    if (urole.neminum != NON_PM) {
        pm = &mons[urole.neminum];
        pm->msound = MS_NEMESIS;
        pm->mflags2 &= ~(M2_PEACEFUL);
        pm->mflags2 |= (M2_NASTY | M2_STALK | M2_HOSTILE);
        pm->mflags3 &= ~(M3_CLOSE);
        pm->mflags3 |= M3_WANTSARTI | M3_WAITFORU;
        /* if gender is random, we choose it now instead of waiting
           until the nemesis monster is created */
        quest_status.nemgend = is_neuter(pm) ? 2 : is_female(pm) ? 1
                                   : is_male(pm) ? 0 : (rn2(100) < 50);
    }

    /* Fix up the god names */
    if (flags.pantheon == -1) {             /* new game */
        flags.pantheon = flags.initrole;    /* use own gods */
        while (!roles[flags.pantheon].lgod) /* unless they're missing */
            flags.pantheon = randrole(FALSE);
    }
    if (!urole.lgod) {
        urole.lgod = roles[flags.pantheon].lgod;
        urole.ngod = roles[flags.pantheon].ngod;
        urole.cgod = roles[flags.pantheon].cgod;
    }
    /* 0 or 1; no gods are neuter, nor is gender randomized */
    quest_status.godgend = !strcmpi(align_gtitle(alignmnt), "goddess");

#if 0
/*
 * Disable this fixup so that mons[] can be const.  The only
 * place where it actually matters for the hero is in set_uasmon()
 * and that can use mons[race] rather than mons[role] for this
 * particular property.  Despite the comment, it is checked--where
 * needed--via instrinsic 'Infravision' which set_uasmon() manages.
 */
    /* Fix up infravision */
    if (mons[urace.malenum].mflags3 & M3_INFRAVISION) {
        /* although an infravision intrinsic is possible, infravision
         * is purely a property of the physical race.  This means that we
         * must put the infravision flag in the player's current race
         * (either that or have separate permonst entries for
         * elven/non-elven members of each class).  The side effect is that
         * all NPCs of that class will have (probably bogus) infravision,
         * but since infravision has no effect for NPCs anyway we can
         * ignore this.
         */
        mons[urole.malenum].mflags3 |= M3_INFRAVISION;
        if (urole.femalenum != NON_PM)
            mons[urole.femalenum].mflags3 |= M3_INFRAVISION;
        if (urole.nbnum != NON_PM)
            mons[urole.nbnum].mflags3 |= M3_INFRAVISION;
    }
#endif /*0*/

    /* Artifacts are fixed in hack_artifacts() */

    /* Success! */
    return;
}

const char *
Hello(mtmp)
struct monst *mtmp;
{
    switch (Role_switch) {
    case PM_KNIGHT:
        return "Salutations"; /* Olde English */
    case PM_SAMURAI:
        return (mtmp && is_shopkeeper(mtmp->data))
                    ? "Irasshaimase"
                    : "Konnichi wa"; /* Japanese */
    case PM_PIRATE:
        return "Ahoy"; /* Pirate */
    case PM_TOURIST:
        return "Aloha"; /* Hawaiian */
    case PM_VALKYRIE:
        return
#ifdef MAIL
               (mtmp && mtmp->data == &mons[PM_MAIL_DAEMON]) ? "Hallo" :
#endif
               "Velkommen"; /* Norse */
    default:
        return "Hello";
    }
}

const char *
Goodbye()
{
    switch (Role_switch) {
    case PM_KNIGHT:
        return "Fare thee well"; /* Olde English */
    case PM_SAMURAI:
        return "Sayonara"; /* Japanese */
    case PM_TOURIST:
        return "Aloha"; /* Hawaiian */
    case PM_VALKYRIE:
        return "Farvel"; /* Norse */
    default:
        return "Goodbye";
    }
}

const char *
sibling_gender()
{
    const char* ret;
    switch(flags.gender) {
    case 0:
        ret = "brother";
        break;
    case 1:
        ret = "sister";
        break;
    default:
        ret = "sibling";
        break;
    }
    return ret;
}

const char *
child_gender()
{
    const char* ret;
    switch(flags.gender) {
    case 0:
        ret = "son";
        break;
    case 1:
        ret = "daughter";
        break;
    default:
        ret = "child";
        break;
    }
    return ret;
}

int
monnum_gender(flag, race)
int flag;
boolean race;
{
    int mnum;
    if (race) {
        mnum = (flag == GEND_F && urace.femalenum != NON_PM)
            ? urace.femalenum
            : (flag == GEND_N && urace.nbnum != NON_PM)
                ? urace.nbnum
                : urace.malenum;
    } else {
        mnum = (flag == GEND_F && urole.femalenum != NON_PM)
                ? urole.femalenum
                : (flag == GEND_N && urole.nbnum != NON_PM)
                    ? urole.nbnum
                    : urole.malenum;
    }
    return mnum;
}

const char*
rolename_gender(flag)
int flag;
{
    return (flag == GEND_F && urole.name.f)
                ? urole.name.f
                : (flag == GEND_N && urole.name.n)
                ? urole.name.n
                : urole.name.m;
}

/* role.c */

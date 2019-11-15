/* NetHack 3.6	mkroom.h	$NHDT-Date: 1560851014 2019/06/18 09:43:34 $  $NHDT-Branch: NetHack-3.6 $:$NHDT-Revision: 1.16 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Pasi Kallinen, 2016. */
/* NetHack may be freely redistributed.  See license for details. */
/* File changed 3/7/18 by NullCGT */

#ifndef MKROOM_H
#define MKROOM_H

/* mkroom.h - types and structures for room and shop initialization */

struct mkroom {
    schar lx, hx, ly, hy; /* usually xchar, but hx may be -1 */
    schar rtype;          /* type of room (zoo, throne, etc...) */
    schar orig_rtype;     /* same as rtype, but not zeroed later */
    schar rlit;           /* is the room lit ? */
    schar needfill;       /* sp_lev: does the room need filling? */
    schar needjoining;    /* sp_lev */
    schar doorct;         /* door count */
    schar fdoor;          /* index for the first door of the room */
    schar nsubrooms;      /* number of subrooms */
    boolean irregular;    /* true if room is non-rectangular */
    struct mkroom *sbrooms[MAX_SUBROOMS]; /* Subrooms pointers */
    struct monst *resident; /* priest/shopkeeper/guard for this room */
};

struct shclass {
    const char *name; /* name of the shop type */
    char symb;        /* this identifies the shop type */
    int prob;         /* the shop type probability in % */
    schar shdist;     /* object placement type */
#define D_SCATTER 0   /* normal placement */
#define D_SHOP 1      /* shop-like placement */
#define D_TEMPLE 2    /* temple-like placement */
    struct itp {
        int iprob; /* probability of an item type */
        int itype; /* item type: if >=0 a class, if < 0 a specific item */
    } iprobs[8];
    const char *const *shknms; /* list of shopkeeper names for this type */
};

extern NEARDATA struct mkroom rooms[(MAXNROFROOMS + 1) * 2];
extern NEARDATA struct mkroom *subrooms;
/* the normal rooms on the current level are described in rooms[0..n] for
 * some n<MAXNROFROOMS
 * the vault, if any, is described by rooms[n+1]
 * the next rooms entry has hx -1 as a flag
 * there is at most one non-vault special room on a level
 */

extern struct mkroom *dnstairs_room, *upstairs_room, *sstairs_room;

extern NEARDATA coord doors[DOORMAX];

/* values for rtype in the room definition structure */
enum roomtype_types {
    OROOM      =  0, /* ordinary room */
    COURT      =  2, /* contains a throne */
    SWAMP      =  3, /* contains pools */
    VAULT      =  4, /* detached room usually reached via teleport trap */
    BEEHIVE    =  5, /* contains killer bees and royal jelly */
    LAB        =  6, /* contains amalgamations and bad clones, along with a sink */
    DEN        =  7, /* contains wild animals */
    MORGUE     =  8, /* contains corpses, undead and graves */
    BARRACKS   =  9, /* contains soldiers and their gear */
    ZOO        = 10, /* floor covered with treasure and monsters */
    DELPHI     = 11, /* contains Oracle and peripherals */
    TEMPLE     = 12, /* contains a shrine (altar attended by priest[ess]) */
    LEPREHALL  = 13, /* leprechaun hall (Tom Proudfoot) */
    COCKNEST   = 14, /* cockatrice nest (Tom Proudfoot) */
    ARMORY     = 15, /* armory (L's dilapidated armory patch) */
    ANTHOLE    = 16, /* ants (Tom Proudfoot) */
    LEMUREPIT  = 17,  /* contains lemures and horned devils */
    BLACKFOYER = 18, /* black market */
    ARTROOM    = 19,    /* interesting walls */
    SHOPBASE   = 20, /* everything above this is a shop */
    ARMORSHOP  = 21, /* specific shop defines for level compiler */
    SCROLLSHOP = 22,
    POTIONSHOP = 23,
    WEAPONSHOP = 24,
    FOODSHOP   = 25,
    RINGSHOP   = 26,
    WANDSHOP   = 27,
    TOOLSHOP   = 28,
    BOOKSHOP   = 29,
    ARCHERYSHOP = 30,
    MASKSHOP   = 31,
    JUNKSHOP   = 32,
    FODDERSHOP = 33, /* health food store */
    CANDLESHOP = 34,
    BLACKSHOP  = 35
};

#define MAXRTYPE (BLACKSHOP) /* maximum valid room type */
#define UNIQUESHOP (BLACKSHOP) /* shops here & above not randomly gen'd. */

/* Special type for search_special() */
#define ANY_TYPE (-1)
#define ANY_SHOP (-2)

#define NO_ROOM     0 /* indicates lack of room-occupancy */
#define SHARED      1 /* indicates normal shared boundary */
#define SHARED_PLUS 2 /* indicates shared boundary - extra adjacent-square
                       * searching required */
#define ROOMOFFSET  3 /* (levl[x][y].roomno - ROOMOFFSET) gives rooms[] index,
                       * for inside-squares and non-shared boundaries */

#define IS_ROOM_PTR(x)      ((x) >= rooms && (x) < rooms + MAXNROFROOMS)
#define IS_ROOM_INDEX(x)    ((x) >= 0 && (x) < MAXNROFROOMS)
#define IS_SUBROOM_PTR(x)   ((x) >= subrooms && (x) < subrooms + MAXNROFROOMS)
#define IS_SUBROOM_INDEX(x) ((x) > MAXNROFROOMS && (x) < (MAXNROFROOMS * 2))
#define ROOM_INDEX(x)       ((x) -rooms)
#define SUBROOM_INDEX(x)    ((x) -subrooms)
#define IS_LAST_ROOM_PTR(x) (ROOM_INDEX(x) == nroom)
#define IS_LAST_SUBROOM_PTR(x) (!nsubroom || SUBROOM_INDEX(x) == nsubroom)

#endif /* MKROOM_H */

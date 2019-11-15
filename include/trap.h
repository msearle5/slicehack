/* NetHack 3.6	trap.h	$NHDT-Date: 1547255912 2019/01/12 01:18:32 $  $NHDT-Branch: NetHack-3.6.2-beta01 $:$NHDT-Revision: 1.17 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Pasi Kallinen, 2016. */
/* NetHack may be freely redistributed.  See license for details. */

/* note for 3.1.0 and later: no longer manipulated by 'makedefs' */

/* Edited on 4/13/18 by NullCGT */

#ifndef TRAP_H
#define TRAP_H

union vlaunchinfo {
    short v_launch_otyp; /* type of object to be triggered */
    coord v_launch2;     /* secondary launch point (for boulders) */
    uchar v_conjoined;   /* conjoined pit locations */
    short v_tnote;       /* boards: 12 notes        */
};

struct trap {
    struct trap *ntrap;
    xchar tx, ty;
    d_level dst; /* destination for portals */
    coord launch;
    Bitfield(ttyp, 5);
    Bitfield(tseen, 1);
    Bitfield(once, 1);
    Bitfield(madeby_u, 1); /* So monsters may take offence when you trap
                              them.  Recognizing who made the trap isn't
                              completely unreasonable, everybody has
                              their own style.  This flag is also needed
                              when you untrap a monster.  It would be too
                              easy to make a monster peaceful if you could
                              set a trap for it and then untrap it. */
    union vlaunchinfo vl;
#define launch_otyp vl.v_launch_otyp
#define launch2 vl.v_launch2
#define conjoined vl.v_conjoined
#define tnote vl.v_tnote
};

extern struct trap *ftrap;
#define newtrap() (struct trap *) alloc(sizeof(struct trap))
#define dealloc_trap(trap) free((genericptr_t)(trap))

/* reasons for statue animation */
#define ANIMATE_NORMAL 0
#define ANIMATE_SHATTER 1
#define ANIMATE_SPELL 2

/* reasons for animate_statue's failure */
#define AS_OK 0            /* didn't fail */
#define AS_NO_MON 1        /* makemon failed */
#define AS_MON_IS_UNIQUE 2 /* statue monster is unique */

/* Note: if adding/removing a trap, adjust trap_engravings[] in mklev.c */

/* unconditional traps */
enum trap_types {
    NO_TRAP      =  0,
    ARROW_TRAP   =  1,
    DART_TRAP    =  2,
    ROCKTRAP     =  3,
    SQKY_BOARD   =  4,
    BEAR_TRAP    =  5,
    LANDMINE     =  6,
    ROLLING_BOULDER_TRAP = 7,
    SLP_GAS_TRAP =  8,
    RUST_TRAP    =  9,
    FIRE_TRAP    = 10,
    BUZZSAW_TRAP = 11,
    WHIRLWIND_TRAP = 12,
    GLYPH_OF_NEUTRALITY = 13,
    GLYPH_OF_LAW = 14,
    GLYPH_OF_CHAOS = 15,
    PIT          = 16,
    SPIKED_PIT   = 17,
    HOLE         = 18,
    TRAPDOOR     = 19,
    TELEP_TRAP   = 20,
    LEVEL_TELEP  = 21,
    MAGIC_PORTAL = 22,
    WEB          = 23,
    STATUE_TRAP  = 24,
    MAGIC_TRAP   = 25,
    ANTI_MAGIC   = 26,
    POLY_TRAP    = 27,
    VIBRATING_SQUARE = 28,

    TRAPNUM      = 29
};

#define is_pit(ttyp) ((ttyp) == PIT || (ttyp) == SPIKED_PIT)
#define is_hole(ttyp)  ((ttyp) == HOLE || (ttyp) == TRAPDOOR)

#endif /* TRAP_H */

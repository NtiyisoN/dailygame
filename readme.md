
Compile and run:

	make

Needs `g++` to compile.



Goals for version "0.2":
1. ✓You can upgrade character stats by spending gold.
2. ✓You can fight(rudimentary combat system).
3. ✓You can restore HP.
4.  You can level up (at this point, the leveling won't really add anything, it will only mean the player has to fight harder foes to gain higher levels. Woohoo number go up!).
5.  You can (and have to) fight fight progressively harder foes with better rewards.


Required for version "1.0":

1.  Robust, flexible savefile: more database-like. maybe tab-separated key-value pairs, so that stuff can move around, be added etc.
2.  Separation between "log"(mostly for debug) and "interface"(what the player should see)
3.  Robust argument parsing: use POSIX `getopt(3POSIX)`, or GNU `getopt(3)` or `argp.h` from GNU lib C.

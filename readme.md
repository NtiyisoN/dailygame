
Compile and run:

	make

Needs `g++` to compile.



The state upon which I will deem this to be version "0.1":
1. ✓You can create new game.
2. ✓You can save the game to file.
3. ✓You can load gamestate from file.
4. ✓The game can calculate how much time elapsed since last save.
5. ✓The game gives you income for elapsed time.


Version "0.2":
1. ✓You can upgrade character stats by spending gold.
2. ✓You can fight(rudimentary combat system).
3. ✓You can restore HP.
4.  You can level up leveling(at this point, the leveling won't really add anything, it will only mean the player has to fight harder foes to gain higher levels).
5.  You can (and have to) fight fight progressively harder foes with better rewards.


for version "1.0":

1.  Robust, flexible savefile: more database-like. maybe tab-separated key-value pairs, so that stuff can move around, be added etc.
2.  Separation between "log"(mostly for debug) and "interface"(what the player should see)

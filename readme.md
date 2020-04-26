
Compile and run:

	make

Needs `g++` to compile.



The state upon which I will deem this to be version "0.1":
✓ 1. You can create new game.
✓ 2. You can save the game to file.
✓ 3. You can load gamestate from file.
✓ 4. The game can calculate how much time elapsed since last save.
✓ 5. The game gives you income for elapsed time.


Version "0.2":
✓ 1. upgrading character stats by spending gold
✓ 2. rudimentary combat system
✓ 3. healing wounds with money
  4. leveling(the level doesn't need to really add anything, it will be enough if it means the player has to fight harder foes)
  5. a way to fight progressively harder foes with better rewards, instead of only gathering gold that does nothing


for version "1.0":
  1. robust, flexible savefile: more database-like. maybe tab-separated key-value pairs, so that stuff can move around, be added etc.
  2. separation between "log"(mostly for debug) and "interface"(what the player should see)

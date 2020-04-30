
Goals for version "0.4":
1. ✓(mmorpg-parody) Arbitrarily restrict the player's capacity to carry money, so that they have to upgrade their vault. This will be a yet another resource sink!
1.1.  Helper function: display how long it will take to fill your vault.
2.  Player must pay money to level up

Goals for version "0.5": (update-name "RealTime")
1.  (technical) cleaner system for handling elapsed time between sessions.
2.  (mmorpg-parody) Arbitrarily restrict the player from doing actions, so they can only do a few things per day. (Stamina?)
3.  (mmorpg-parody) Arbitrarily demand to wait some time when upgrading anything. (maybe this should be optional? or some funny mechanic, in which you can spend money to hasten the process)

Required for version "1.0":

1.  Robust, flexible savefile: more database-like. maybe tab-separated key-value pairs, so that stuff can move around, be added etc.
2.  Separation between "log"(mostly for debug) and "interface"(what the player should see)
3.  Robust argument parsing: use POSIX `getopt(3POSIX)`, or GNU `getopt(3)` or `argp.h` from GNU lib C.
4.  Better abstraction for upgrade system. Copying code makes for a big mess.
5. ✓ Move player's `CombatEntity` inside `player_data`
5.1. write `struct Progress`(and make it part of player_data) for storing progres data

When version "1.0" is functionally complete:

1.  Drastically lower the income, since currently the player gets big money fast. In release, the player will get very little money, and slowly.


Unimportant gimmicks, do it eventually(maybe even never):
1. "Inventory" - You can store items(whatever that will be). (it's more important to do the generic progression, and only eventually work on non-core stuff that adds complexity without doin the core)
2. "Combat" - add complexity to it
3. "RPG" - more player character stats
4. "name" - add ability to name your character
5. penalty for dying
6. interface: ncurses
7. interface: stdin


Rough ideas:
1. Add progres to leveling up all stats. This way, player could spent part of money on one stat? Or maybe not - this could potentially devalue the "vault" mechanic.

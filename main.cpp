#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <time.h>


/*
The state upon which I will deem this to be version "0.1":
✓ 1. You can create new game.
✓ 2. You can save the game to file.
✓ 3. You can load gamestate from file.
✓ 4. The game can calculate how much time elapsed since last save.
✓ 5. The game gives you income for elapsed time.
*/


const char * STRING_FILENAME_SAVE = "save_dailygame";

const int seconds_per_tick = 1;
const int income_per_tick  = 2;

const int seconds_max_per_seesion = (32 * 60 * 60);


struct gamestate {
	time_t time_game_created;
	time_t time_last_saved;
	struct {
		int money;
		int level;
		int health;
		int bonus_attack;
		int bonus_defense;
	} player;
};

void print_gamestate( const struct gamestate gs)
{
	printf(" gs.time_game_created   =%lu\n" , gs.time_game_created    );
	printf(" gs.time_last_saved     =%lu\n" , gs.time_last_saved      );
	printf(" gs.player.money        =%d\n"  , gs.player.money         );
	printf(" gs.player.level        =%d\n"  , gs.player.level         );
	printf(" gs.player.health       =%d\n"  , gs.player.health        );
	printf(" gs.player.bonus_attack =%d\n"  , gs.player.bonus_attack  );
	printf(" gs.player.bonus_defense=%d\n"  , gs.player.bonus_defense );
}


int calculate_elapsed_ticks(time_t const time_before , time_t const time_now)
{
	return (time_now - time_before) / seconds_per_tick;;
}




void
write_savefile(
	const char * const filename
	, const struct gamestate * gs )
{
	FILE * savefile = fopen(filename , "w");
	printf( "opened '%s'" , filename  );
	assert(savefile);

	fprintf( savefile ,  "%lu\n" , gs->time_game_created );
	fprintf( savefile ,  "%lu\n" , gs->time_last_saved );
	fprintf( savefile ,  "%d\n"  , gs->player.money);
	fprintf( savefile ,  "%d\n"  , gs->player.level);
	fprintf( savefile ,  "%d\n"  , gs->player.health);
	fprintf( savefile ,  "%d\n"  , gs->player.bonus_attack);
	fprintf( savefile ,  "%d\n"  , gs->player.bonus_defense);

	fclose(savefile);
}

void
save_game(
	const char * const filename
	, struct gamestate * gs
	, time_t time_saved
	) {
	printf("(time:%lu) saving to file '%s'\n" , time_saved , filename);
	gs->time_last_saved = time_saved;
	write_savefile(filename , gs);
}



struct gamestate read_savefile(const char * const filename )
{
	struct gamestate gs;
	FILE * savefile = fopen(filename , "r");
	assert(savefile);

	fscanf( savefile ,  "%lu\n" , &(gs.time_game_created ));
	fscanf( savefile ,  "%lu\n" , &(gs.time_last_saved ));
	fscanf( savefile ,  "%d\n"  , &(gs.player.money));
	fscanf( savefile ,  "%d\n"  , &(gs.player.level));
	fscanf( savefile ,  "%d\n"  , &(gs.player.health));
	fscanf( savefile ,  "%d\n"  , &(gs.player.bonus_attack));
	fscanf( savefile ,  "%d\n"  , &(gs.player.bonus_defense));

	fclose(savefile);
	return gs;
}




struct gamestate create_new_game(time_t const time_game_created) {
	struct gamestate gs = {
		time_game_created ,
		time_game_created ,
		{
			0  ,
			0  ,
			16 ,
			1  ,
			1  ,
		}
	}; 

	return gs;
}


void create_new_savefile(const char * const filename , time_t const time_game_created ) {
	struct gamestate gs = create_new_game(time_game_created);
	write_savefile(filename , &gs);
}


//enum savefile_state {
//	savefile_state_
//	savefile_state_
//	savefile_state_
//};

bool does_savefile_exist(const char * const filename) {
	if( access( filename , F_OK ) != -1 ) {
	    return true; // file exists
	} else {
	    return false; // file doesn't exist
	}
}






int main()
{
	time_t gametime = time(NULL);
	printf("time: %lu\n" , gametime);

	struct gamestate state;

	if( does_savefile_exist(STRING_FILENAME_SAVE) ) {
		state = read_savefile(STRING_FILENAME_SAVE);
		printf( "Loaded game from file '%s'\n" , STRING_FILENAME_SAVE );
	} else {
		printf( "Savefile doesn't exist, creating '%s' with new game data.\n" , STRING_FILENAME_SAVE );
		state = create_new_game(gametime);
		write_savefile(STRING_FILENAME_SAVE , &state);
	}


	// This is where the "game" starts
	int elapsed = calculate_elapsed_ticks(state.time_last_saved , gametime);
	printf("elapsed:%d\n" , elapsed );

	int income = elapsed * income_per_tick;
	printf("income:%d\n" , income );

	state.player.money += income;

	print_gamestate(state);


	save_game(STRING_FILENAME_SAVE , &state , gametime);
}

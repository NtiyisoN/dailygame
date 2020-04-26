#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <string.h>


const char * STRING_FILENAME_SAVE = "0.save";

const int SECONDS_PER_TICK = 1;
const int INCOME_TICKS_PER_TIMEUNIT = 0x10;
const int INCOME_PER_TICK  = 1;


// const int seconds_max_per_seesion = (32 * 60 * 60);

int const HEALING_COST_PER_HP = 0x4;


struct CombatEntity {
	int hp_current;
	int hp_max;
	int bonus_attack;
	int bonus_defense;
	int level;

	CombatEntity();
	CombatEntity(int const);
};


CombatEntity::CombatEntity(int const _level) {
	level = _level;
	int const hp = _level * 4;
	hp_current = hp;
	hp_max = hp;
	bonus_attack =  _level;
	bonus_defense = _level;
}

CombatEntity::CombatEntity() {
	hp_current = 0x10;
	hp_max     = 0x10;
	bonus_attack  = 0;
	bonus_defense = 0;
}



struct PlayerData {
	int money;
	int progres_income;
	int progres_level;

	PlayerData();
};


PlayerData::PlayerData() {
	money = 0;
	progres_income = 0;
	progres_level = 0;
}


struct gamestate {
	time_t time_game_created;
	time_t time_last_saved;
	CombatEntity player;
	PlayerData player_data;
	gamestate();
	gamestate(time_t const _time_game_created);
};


gamestate::gamestate() { }


gamestate::gamestate(
		time_t const _time_game_created)
{
	time_game_created = _time_game_created;
	time_last_saved   = _time_game_created;
	player = CombatEntity();
	player.hp_max = 8 * player.level;
}


void print_gamestate( const struct gamestate gs)
{
	// printf("gs.time_game_created	%lu\n" , gs.time_game_created    );
	// printf("gs.time_last_saved	%lu\n" , gs.time_last_saved      );
	printf("Money 	 %d\n" , gs.player_data.money         );
	//printf("gs.player_data.progres_income	%d\n" , gs.player_data.progres_income  );
	printf("Level:%d (progress:%d)\n"
			, gs.player.level
			, gs.player_data.progres_level       );
	printf("HP	%d (max:%d)\n"  , gs.player.hp_current , gs.player.hp_max );
	printf("ATK	%d\n"  , gs.player.bonus_attack  );
	printf("DEF	%d\n"  , gs.player.bonus_defense );
}


int calculate_elapsed_ticks(time_t const time_before , time_t const time_now)
{
	return (time_now - time_before) / SECONDS_PER_TICK;;
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
	fprintf( savefile ,  "%d\n"  , gs->player_data.money);
	fprintf( savefile ,  "%d\n"  , gs->player_data.progres_income);
	fprintf( savefile ,  "%d\n"  , gs->player_data.progres_level);
	fprintf( savefile ,  "%d\n"  , gs->player.level);
	fprintf( savefile ,  "%d\n"  , gs->player.hp_current);
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
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.money));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.progres_income));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.progres_level));
	fscanf( savefile ,  "%d\n"  , &(gs.player.level));
	fscanf( savefile ,  "%d\n"  , &(gs.player.hp_current));
	fscanf( savefile ,  "%d\n"  , &(gs.player.bonus_attack));
	fscanf( savefile ,  "%d\n"  , &(gs.player.bonus_defense));

	fclose(savefile);
	return gs;
}






void create_new_savefile(const char * const filename , time_t const time_game_created ) {
	gamestate gs = gamestate(time_game_created);
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




/* GAME */
int
get_upgrade_cost(
		int const desired_level
) {
	return ( 4 << (desired_level));
}


void
player_upgrade_attack(
		struct gamestate * gs
) {
	int const upgrade_cost = get_upgrade_cost(1 + (gs->player.bonus_attack));
	if( gs->player_data.money > upgrade_cost ) {
		gs->player_data.money -= upgrade_cost;
		(++(gs->player.bonus_attack));
		printf("Upgraded attack to level:%d. Cost:%d\n" , gs->player.bonus_attack , upgrade_cost);
	} else {
		printf("insufficient funds to upgrade attack to level %d; need:%d , have:%d\n" , gs->player.bonus_attack , upgrade_cost , gs->player_data.money );
	}
}


void
player_upgrade_defense(
		struct gamestate * gs
) {
	int const upgrade_cost = get_upgrade_cost(1 + (gs->player.bonus_defense));
	if( gs->player_data.money > upgrade_cost ) {
		gs->player_data.money -= upgrade_cost;
		(++(gs->player.bonus_defense));
		printf("Upgraded defense to level:%d. Cost:%d\n" , gs->player.bonus_defense , upgrade_cost);
	} else {
		printf("insufficient funds to upgrade defense to level %d; need:%d , have:%d\n" , gs->player.bonus_defense , upgrade_cost , gs->player_data.money );
	}
}




int
roll_d6() {
	return rand() % 6;
}


enum result_attack {
	result_attack_alive ,
	result_attack_dead  ,
};


enum result_attack
combat_perform_attack(
		 CombatEntity const  * attacker
		,CombatEntity * target
		) {
	int dice_roll = roll_d6();
	int damage_roll = dice_roll + attacker->bonus_attack + (-target->bonus_defense) ;
	printf( "damage_roll:%d(diceroll:%d, atk:%d , def:%d)\n" , damage_roll , dice_roll , attacker->bonus_attack , target->bonus_defense);
	if( damage_roll < 0 ) {
		damage_roll = 0;
	}
	target->hp_current -= damage_roll;
	if(target->hp_current < 0 ) {
		return result_attack_dead;
	}
	return result_attack_alive;
}


enum result_combat {
	result_combat_draw ,
	result_combat_winner_0 ,
	result_combat_winner_1 ,
	result_combat_error ,
};


enum result_combat
combat_perform_combat(
		 CombatEntity * ent_0
		,CombatEntity * ent_1
		,int rounds
		) {

	assert(rounds > 0);
	assert(ent_0);
	assert(ent_1);

	int r = 0;
	enum result_attack result;
	bool died_0 = false;
	bool died_1 = false;
	printf( "  Combat started, max rounds: %d\n" , rounds);
	for(r = 0; r < rounds; ++r) {
		printf( "ent_0  " );
		result = combat_perform_attack( ent_0 , ent_1 );
		if(result == result_attack_dead ) {
			died_0 = true;
			goto jump_end_combat;
		}
		printf( "ent_1  " );
		result = combat_perform_attack( ent_1 , ent_0 );
		if(result == result_attack_dead ) {
			died_1 = true;
			goto jump_end_combat;
		}
	}

jump_end_combat:
	printf( "  Combat ended after %d rounds.(death? 0:%d,1:%d)" , r , died_0 , died_1 );
	if(  ( (!died_0) && (!died_1) )
		 ||
	     (died_0 && died_1) ) {
		return result_combat_draw;
	} else if( died_0 && (!died_1) ) {
		return result_combat_winner_0;
	} else if( died_1 && (!died_0) ) {
		return result_combat_winner_1;
	}

	return result_combat_error;
}


void
print_combat_result(
		 enum result_combat result
		,const char * const str_ent_0
		,const char * const str_ent_1 )
{
	printf("  Result of battle between \"%s\" and \"%s\":\n" , str_ent_0 , str_ent_1 );
	switch(result) {
		case result_combat_draw:
			printf( "Draw\n" );
			break;
		case result_combat_winner_0:
			printf( "Winner:\"%s\"\n" , str_ent_0 );
			break;
		case result_combat_winner_1:
			printf( "Winner:\"%s\"\n" , str_ent_1 );
			break;
		default:
			printf("An error occured, unrecognized combat_result=%d\n" , result);
	}
}




int get_exp_reward(
		 int const level_player
		,int const level_foe )
{
	int const reward = ( level_foe - level_player );
	if( reward > 0 ) {
		return reward;
	}
	return 0;
}




void
gamestate_heal(gamestate * gs) {
	printf( "Healing...  " );
	if( gs->player.hp_current >= gs->player.hp_max ) {
		printf("didn't heal. hp not lower than max.\n");
		return;
	}
	int const hp_difference = ( gs->player.hp_max  + (- gs->player.hp_current)  );
	int const cost = hp_difference * HEALING_COST_PER_HP;
	gs->player_data.money -= cost;
	gs->player.hp_current = gs->player.hp_max;
	printf("Healed hp:%d, spent money:%d, current money:%d\n" , hp_difference , cost , gs->player_data.money);
}




/* UI */
void
handle_player_death(gamestate * state)
{
	printf( "You died, because your life fell below 0. Don't worry though, this isn't a permadeath game, so you only lost some money for getting 'resurrected'.\nYou will have to pay money to be healed before your next fight.\nTo restore health, run the game with argument '-r'.\n" );
	state->player_data.money += (state.player.hp_current * HEALING_COST_PER_HP); /* penalty for death. Yes, player can go into negative money! */
	state->player.hp_current = 0;
}


/* USER_INTERFACE USER INTERFACE UI CLI */

void print_cli_help() {
	printf("-h	print help\n");
	printf("-a	upgrade attack\n");
	printf("-d	upgrade defense\n");
	printf("-f	fight\n");
	printf("-r	restore hp, you will go into zero-interest debt(negative money)\n");
	// printf("-H	heal one hp\n");
}




int main(int argc , char * argv[])
{
	bool flag_heal = false;
	bool flag_upgrade_attack = false;
	bool flag_upgrade_defense = false;
	bool flag_action_combat = false;

	for(int i = 1; i < argc; ++i) {
		// TODO
		if( 0 == strcmp(argv[i] , "-h") ) {
			print_cli_help();
		} else if( 0 == strcmp(argv[i] , "-a")  ) {
			flag_upgrade_attack = true;
		} else if( 0 == strcmp(argv[i] , "-d") ) {
			flag_upgrade_defense = true;
		} else if( 0 == strcmp(argv[i] , "-f") ) {
			flag_action_combat = true;
		} else if( 0 == strcmp(argv[i] , "-r") ) {
			flag_heal = true;
		} else {
			printf("unrecognized option '%s'" , argv[i]);
		}
	}

	srand(time(NULL));
	time_t gametime = time(NULL);
	printf("time: %lu\n" , gametime);

	struct gamestate state = gamestate(gametime);

	if( does_savefile_exist(STRING_FILENAME_SAVE) ) {
		state = read_savefile(STRING_FILENAME_SAVE);
		printf( "Loaded game from file '%s'\n" , STRING_FILENAME_SAVE );
	} else {
		printf( "Savefile doesn't exist, creating '%s' with new game data.\n" , STRING_FILENAME_SAVE );
		write_savefile(STRING_FILENAME_SAVE , &state);
	}


	// This is where the "game" starts
	int elapsed = calculate_elapsed_ticks(state.time_last_saved , gametime);
	printf("elapsed	%d\n" , elapsed );

	int const income_progres_old = state.player_data.progres_income + elapsed;
	int const income = (income_progres_old / INCOME_TICKS_PER_TIMEUNIT) * INCOME_PER_TICK;
	int const income_progres = income_progres_old % INCOME_TICKS_PER_TIMEUNIT;
	printf("income	%d(progres:%d)\n" , income, income_progres );

	state.player_data.money += income;
	state.player_data.progres_income = income_progres;



	if(flag_upgrade_attack) {
		printf("upgrading attack\n");
		player_upgrade_attack(&state);
	}

	if(flag_upgrade_defense) {
		printf("upgrading defense\n");
		player_upgrade_defense(&state);
	}

	if(flag_heal) {
		gamestate_heal(&state);
	}

	if(flag_action_combat) {
		// TODO
		if( state.player.hp_current < 1 ) {
			printf("you need at least 1 hp to fight(you have %d hp)\n" , state.player.hp_current );
		} else {
			CombatEntity foe = CombatEntity(1);
			printf("starting combat with level 1 foe. (4 rounds).\n");
			enum result_combat result = combat_perform_combat( &(state.player) , &foe , 4 );
			print_combat_result( result , "you" , "foe" );
			if( state.player.hp_current < 0 ) { // player died
				handle_player_death(&state);
			} else {
				printf( "You survived, with %d hp left.\n" , state.player.hp_current);
			}
			if( result == result_combat_winner_0 ) {
				int const reward = get_exp_reward(state.player.level , 1);
				printf( "For your victory, you gain reward of %d\n" , reward );
				state.player_data.progres_level += reward;
			}
		}
	}



	print_gamestate(state);


	save_game(STRING_FILENAME_SAVE , &state , gametime);


	return 0;
}

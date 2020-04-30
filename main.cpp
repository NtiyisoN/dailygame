#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#include "config.h"


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
	int const hp = MONSTER_HP_BASE + (_level * MONSTER_HP_PER_LEVEL);
	hp_current = hp;
	hp_max = hp;
	bonus_attack =  _level;
	bonus_defense = _level;
}

CombatEntity::CombatEntity() {
	level = 1;
	hp_current = DEFAULT_MAX_HEALTH;
	hp_max     = DEFAULT_MAX_HEALTH;
	bonus_attack  = 0;
	bonus_defense = 0;
}


CombatEntity
create_enemy( int const difficulty_level ) {
	CombatEntity enemy = CombatEntity( difficulty_level );
	enemy.bonus_attack  += MONSTER_ATK_BASE;
	enemy.bonus_defense += MONSTER_DEF_BASE;
	return enemy;
}


int get_vault_capacity(int const vault_level);
int get_number_of_income_ticks_until_vault_full( 
		 int const vault_level
		,int const money
		,int const income );

struct PlayerData {
	int money;
	int progres_income;
	int progres_level;
	int income;
	int vault;

	int vault_capacity() const {
		return get_vault_capacity(vault);
	};

	int ticks_until_vault_full() const {
		return
			get_number_of_income_ticks_until_vault_full(
					 vault
					,money
					,income );
	};
	CombatEntity entity;

	PlayerData();
};


PlayerData::PlayerData() {
	money = 0;
	income = DEFAULT_INCOME;
	progres_income = 0;
	progres_level = 0;
	vault = 0;
}

int
get_required_progres_for_next_level(int const current_level) {
	return
		(PROGRESS_FOR_NEXT_LEVEL_BASE
		 <<
		 (current_level
		  / PROGRESS_FOR_NEXT_LEVEL_DIVISOR));
}


int
get_vault_capacity(int const vault_level) {
	return (VAULT_CAPACITY_BASE << vault_level) ;
}


int
get_number_of_income_ticks_until_vault_full(
		 int const vault_level
		,int const money
		,int const income ) {
	return (get_vault_capacity(vault_level) - money) / income;
};




struct gamestate {
	time_t time_game_created;
	time_t time_last_saved;
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
	player_data.entity.hp_max = 8 * player_data.entity.level;
}


void print_gamestate( const struct gamestate gs)
{
	// printf("gs.time_game_created	%lu\n" , gs.time_game_created    );
	// printf("gs.time_last_saved	%lu\n" , gs.time_last_saved      );
	printf("Money/Maxmoney(income|vault_level) 	 %d/%d(%d|%d)(ticks_to_fill:%d)\n"
			, gs.player_data.money
			, get_vault_capacity(gs.player_data.vault)
			, gs.player_data.income
			, gs.player_data.vault
			, gs.player_data.ticks_until_vault_full()
		  );
	//printf("gs.player_data.progres_income	%d\n" , gs.player_data.progres_income  );
	printf("Level	%d (progress:%d/%d)\n"
			, gs.player_data.entity.level
			, gs.player_data.progres_level
			, get_required_progres_for_next_level(gs.player_data.entity.level) );
	printf("HP	%d (max:%d)\n"  , gs.player_data.entity.hp_current , gs.player_data.entity.hp_max );
	printf("ATK	%d\n"  , gs.player_data.entity.bonus_attack  );
	printf("DEF	%d\n"  , gs.player_data.entity.bonus_defense );
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
	assert(savefile);

	fprintf( savefile ,  "%lu\n" , gs->time_game_created );
	fprintf( savefile ,  "%lu\n" , gs->time_last_saved );
	fprintf( savefile ,  "%d\n"  , gs->player_data.money);
	fprintf( savefile ,  "%d\n"  , gs->player_data.income);
	fprintf( savefile ,  "%d\n"  , gs->player_data.vault);
	fprintf( savefile ,  "%d\n"  , gs->player_data.progres_income);
	fprintf( savefile ,  "%d\n"  , gs->player_data.progres_level);
	fprintf( savefile ,  "%d\n"  , gs->player_data.entity.level);
	fprintf( savefile ,  "%d\n"  , gs->player_data.entity.hp_current);
	fprintf( savefile ,  "%d\n"  , gs->player_data.entity.hp_max);
	fprintf( savefile ,  "%d\n"  , gs->player_data.entity.bonus_attack);
	fprintf( savefile ,  "%d\n"  , gs->player_data.entity.bonus_defense);

	fclose(savefile);
	printf( "saved to '%s'\n" , filename  );
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
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.income));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.vault));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.progres_income));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.progres_level));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.entity.level));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.entity.hp_current));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.entity.hp_max));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.entity.bonus_attack));
	fscanf( savefile ,  "%d\n"  , &(gs.player_data.entity.bonus_defense));

	fclose(savefile);
	return gs;
}






void create_new_savefile(const char * const filename , time_t const time_game_created ) {
	gamestate gs = gamestate(time_game_created);
	write_savefile(filename , &gs);
}



bool does_savefile_exist(const char * const filename) {
	if( access( filename , F_OK ) != -1 ) {
	    return true; // file exists
	} else {
	    return false; // file doesn't exist
	}
}




/* GAME */

enum upgrade_type {
	upgrade_type_attack ,
	upgrade_type_defense ,
	upgrade_type_hpmax ,
	upgrade_type_income ,
	upgrade_type_vault ,
};

int
get_upgrade_cost_generic(
		 enum upgrade_type type
		,int const stat_level
) {
	int upgrade_cost = 0;
	switch(type) {
		case upgrade_type_attack:
		case upgrade_type_defense:
			upgrade_cost
				= ( 1 << (stat_level));
			break;
		case upgrade_type_hpmax:
			upgrade_cost
				= stat_level <= DEFAULT_MAX_HEALTH
				? 1
				: 1 << ((stat_level - DEFAULT_MAX_HEALTH) / 4) ;
			break;
		case upgrade_type_income:
			upgrade_cost
				= ( UPGRADE_INCOME_COST_BASE << stat_level );
			break;
		case upgrade_type_vault:
			upgrade_cost
				= ( UPGRADE_VAULT_COST_BASE << stat_level );
			break;
		default:
			printf("Unrecognized upgrade_type %d" , type);
	}
	return upgrade_cost; // TODO: better error handling
}


int
get_upgrade_maxlevel_generic(
		 enum upgrade_type type
		,int const player_level
) {
	switch(type) {
		case upgrade_type_attack:
		case upgrade_type_defense:
		case upgrade_type_vault: /* "flat" upgrade levels: directly taken from player's level */
			return  player_level;
		case upgrade_type_hpmax:
			return
				DEFAULT_MAX_HEALTH
				+ ( player_level * MAX_HEALTH_UPGRADE_PER_LEVEL ) ;
		case upgrade_type_income:
			return
				DEFAULT_INCOME
				+ ( player_level / UPGRADE_INCOME_LEVEL_DISTANCE ) ;
		default:
			printf("Unrecognized upgrade_type %d" , type);
	}
	return 0; // TODO: better error handling
}

int
get_upgrade_cost(
		int const current_level
) {
	return ( 1 << (current_level));
}

int
get_upgrade_cost_health(
		int desired_health
) {
	if( desired_health <= DEFAULT_MAX_HEALTH ) {
		return 1;
	} else {
		return ( 1 << ((desired_health - DEFAULT_MAX_HEALTH) / 2));
	}
}



int
get_max_upgrade_health(
		int const level
) {
	return DEFAULT_MAX_HEALTH
		+ ( level * MAX_HEALTH_UPGRADE_PER_LEVEL ) ;
}


int
get_upgrade_cost_income(
		int const current_income
) {
	return ( 0x10 << current_income );
	
}


int
get_max_upgrade_income(
		int const level
) {
	return get_upgrade_maxlevel_generic( upgrade_type_income , level );
//	return DEFAULT_INCOME
//		+ ( level / UPGRADE_INCOME_LEVEL_DISTANCE ) ;
}


void
player_upgrade_income(
		struct gamestate * gs
) {

	int const max_income = get_max_upgrade_income(gs->player_data.entity.level);
	if( gs->player_data.income >= max_income ) {
		printf( "You cannot upgrade income anymore.\n" );
		return;
	}

	int const upgrade_cost = get_upgrade_cost_income(gs->player_data.income);
	if( gs->player_data.money > upgrade_cost ) {
		gs->player_data.money -= upgrade_cost;
		(++(gs->player_data.income));
		printf("Upgraded income to level:%d. Cost:%d\n"
				, gs->player_data.income
				, upgrade_cost);
	} else {
		printf("insufficient funds to upgrade income to level %d; need:%d , have:%d\n"
				, gs->player_data.income
				, upgrade_cost
				, gs->player_data.money );
	}
}


void
player_upgrade_vault(
		struct gamestate * gs
) {

	int const max_vault
		= get_upgrade_maxlevel_generic(
				 upgrade_type_vault
				,gs->player_data.entity.level);
	if( gs->player_data.vault >= max_vault ) {
		printf( "You cannot upgrade vault anymore.\n" );
		return;
	}

	int const upgrade_cost = get_upgrade_cost_generic(upgrade_type_vault , gs->player_data.vault);
	if( gs->player_data.money > upgrade_cost ) {
		gs->player_data.money -= upgrade_cost;
		(++(gs->player_data.vault));
		printf("Upgraded vault to level:%d. Cost:%d\n"
				, gs->player_data.vault
				, upgrade_cost);
	} else {
		printf("insufficient funds to upgrade vault to level %d; need:%d , have:%d\n"
				, gs->player_data.vault
				, upgrade_cost
				, gs->player_data.money );
	}
}


void
player_upgrade_attack(
		struct gamestate * gs
) {
	int const upgrade_cost
		= get_upgrade_cost_generic(
				 upgrade_type_attack
				,gs->player_data.entity.bonus_attack);
	printf( "Upgrading attack will cost %d... " , upgrade_cost );
	if( gs->player_data.entity.bonus_attack >= gs->player_data.entity.level ) {
		printf( "You cannot upgrade attack anymore. Fight something to gain level.\n" );
		return;
	}
	if( gs->player_data.money > upgrade_cost ) {
		gs->player_data.money -= upgrade_cost;
		(++(gs->player_data.entity.bonus_attack));
		printf("Upgraded attack to level:%d. Cost:%d\n" , gs->player_data.entity.bonus_attack , upgrade_cost);
	} else {
		printf("insufficient funds to upgrade attack to level %d; need:%d , have:%d\n" , gs->player_data.entity.bonus_attack , upgrade_cost , gs->player_data.money );
	}
}


void
player_upgrade_defense(
		struct gamestate * gs
) {
	int const upgrade_cost
		= get_upgrade_cost_generic(
				 upgrade_type_defense
				,gs->player_data.entity.bonus_defense);
	printf( "Upgrading defense will cost %d... " , upgrade_cost );
	if( gs->player_data.entity.bonus_defense >= gs->player_data.entity.level  ) {
		printf( "You cannot upgrade defense anymore. Fight something to gain level.\n" );
		return;
	}
	if( gs->player_data.money > upgrade_cost ) {
		gs->player_data.money -= upgrade_cost;
		(++(gs->player_data.entity.bonus_defense));
		printf("Upgraded defense to level:%d. Cost:%d\n" , gs->player_data.entity.bonus_defense , upgrade_cost);
	} else {
		printf("insufficient funds to upgrade defense to level %d; need:%d , have:%d\n" , gs->player_data.entity.bonus_defense , upgrade_cost , gs->player_data.money );
	}
}


void
player_upgrade_health(
		struct gamestate * gs
) {
	int const upgrade_cost
		= get_upgrade_cost_generic(
				 upgrade_type_hpmax
				,gs->player_data.entity.hp_max);
	printf( "Upgrading hpmax will cost %d... " , upgrade_cost );
	if( gs->player_data.entity.hp_max >= (get_max_upgrade_health(gs->player_data.entity.level))  ) {
		printf( "You cannot upgrade health anymore, you need to level up.\n" );
		return;
	}
	if( gs->player_data.money > upgrade_cost ) {
		gs->player_data.money -= upgrade_cost;
		(++(gs->player_data.entity.hp_max));
		printf("Upgraded hp_max to:%d. Cost:%d\n" , gs->player_data.entity.hp_max , upgrade_cost);
	} else {
		printf("insufficient funds to upgrade health; need:%d , have:%d\n"
				, upgrade_cost
				, gs->player_data.money );
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
	printf( "damage_roll:%d(roll:%d,a:%d,d:%d," , damage_roll , dice_roll , attacker->bonus_attack , target->bonus_defense);
	if( damage_roll < 0 ) {
		damage_roll = 0;
	}
	target->hp_current -= damage_roll;
	printf( "h:%d)\n" , target->hp_current );
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
	 printf( "  Combat started, (a:%d,d:%d,h:%d) vs (a:%d,d:%d,h:%d), max rounds: %d\n"
			,ent_0->bonus_attack
			,ent_0->bonus_defense
			,ent_0->hp_current
			,ent_1->bonus_attack
			,ent_1->bonus_defense
			,ent_1->hp_current
			,rounds);
	for(r = 0; r < rounds; ++r) {
		printf( "Round%d, (a:%d,d:%d,h:%d) vs (a:%d,d:%d,h:%d)\n"
				,r 
				,ent_0->bonus_attack
				,ent_0->bonus_defense
				,ent_0->hp_current
				,ent_1->bonus_attack
				,ent_1->bonus_defense
				,ent_1->hp_current );
		printf( "e0 " );
		result = combat_perform_attack( ent_0 , ent_1 );
		if(result == result_attack_dead ) {
			died_0 = true;
			goto jump_end_combat;
		}
		printf( "e1 " );
		result = combat_perform_attack( ent_1 , ent_0 );
		if(result == result_attack_dead ) {
			died_1 = true;
			goto jump_end_combat;
		}
		printf("\n");
	}

jump_end_combat:
	printf( "  Combat ended after %d rounds.(hp left: %d , %d )(death? 0:%d,1:%d)\n"
			,r
			,ent_0->hp_current
			,ent_1->hp_current
			,died_0 , died_1 );
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
	printf("  Result of battle between \"%s\" and \"%s\":\n"
			, str_ent_0
			, str_ent_1 );
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
	int const reward = (level_foe - level_player );
	if( reward > 0 ) {
		return reward;
	}
	return 0;
}


void
add_exp(
		 gamestate * gs
		,int const exp )
{
	assert( exp >= 0 );
	if(exp == 0) {
		return;
	}

	int *player_exp = &(gs->player_data.progres_level);
	(*player_exp) += exp;
	int const next_level_exp = get_required_progres_for_next_level( gs->player_data.entity.level );
	if( (*player_exp) >= next_level_exp ) {
		(*player_exp) = next_level_exp;
		printf( "You have enough exp to level up! Note, that you won't receive any rewards for winning fights until you level up.\n");
	}
}


int
get_cost_for_advancing_character_level( int const char_level )
{
	int const cost = (COST_LEVEL_UP_BASE << char_level);
	return
		(cost > 1)
		? cost
		: 1;
}


void
advance_level( gamestate * gs )
{
	int * char_level = &(gs->player_data.entity.level);
	int * char_progres_level = &(gs->player_data.progres_level);
	if( (*char_progres_level)
	     < get_required_progres_for_next_level( (*char_level) ) ) {
		printf("You need more exp before you can level up.\n");
		return;
	}
	int * money = &(gs->player_data.money);
	int const cost = get_cost_for_advancing_character_level( (*char_level) );
	if( cost > (*money) ) {
		printf( "To level up, you need more money. need:%d, have:%d\n"
				,cost
				,(*money) );
		return;
	}
	(*money) -= cost;
	++(*char_level);
	(*char_progres_level) = 0;
	printf("Leveled up to %d. money spent:%d,left:%d\n"
			,*char_level
			,cost
			,*money );
}




int
get_healing_cost(int const hp)
{
	int const cost
		= (HEALING_COST_PER_HP)
		* (1 + (hp / HEALING_COST_LEVELS_PER_INCREASE));
	if(cost < HEALING_COST_PER_HP) {
		return HEALING_COST_PER_HP;
	} else {
		return cost;
	}
}


void
gamestate_heal(gamestate * gs) {
	printf( "Healing...  " );

	int * player_money = &(gs->player_data.money);
	int * player_hp_current = &(gs->player_data.entity.hp_current);
	int player_hp_missing = gs->player_data.entity.hp_max - gs->player_data.entity.hp_current;
	fprintf( stderr , "STDERR $%d hp%d/%d\n" , *player_money , *player_hp_current , player_hp_missing );

	int heal_total = 0;
	int cost_total = 0;
	int cost_per_hp = get_healing_cost( (*player_hp_current) );

	if( (*player_hp_current) < 0 ) {
		printf( "BUG! hp is lower than 0. didn't heal.\n" );
		return;
	}
	if( !(player_hp_missing > 0) ) {
		printf("didn't heal. 0 missing hp.\n");
		return;
	}
	if( (*player_money) < cost_per_hp ) {
		printf( "didn't heal. You don't have enough money to heal even one hp!\n" );
		return;
	}

	while(
			(cost_total < (*player_money) )
			&&
			(heal_total < player_hp_missing)
		 )
	{
		cost_total += cost_per_hp;
		(++heal_total);
		cost_per_hp = get_healing_cost( (*player_hp_current) + heal_total );
	}

	(*player_hp_current) += heal_total;
	(*player_money) -= cost_total;

	printf("Healed hp:%d, spent money:%d, current money:%d\n"
			, heal_total
			, cost_total
			, (*player_money));
}


/* UI */
void
handle_player_death(gamestate * state)
{
	printf( "You died, because your life fell below 0. Don't worry though, this isn't a permadeath game, so you only lost some money for getting 'resurrected'.\nYou will have to pay money to be healed before your next fight.\nTo restore health, run the game with argument '-r'.\n" );
	int healing_cost = (state->player_data.entity.hp_current * HEALING_COST_PER_HP);
	int const max_healing_cost = state->player_data.entity.hp_max * HEALING_COST_PER_HP;
	if( healing_cost > max_healing_cost ) {
		healing_cost = max_healing_cost; // cap healing cost
	}
	state->player_data.money += healing_cost; /* penalty for death. Yes, player can go into negative money! */
	state->player_data.entity.hp_current = 0;
}


/* USER_INTERFACE USER INTERFACE UI CLI */

void print_cli_help() {
	printf("-h	print help\n");
	printf("-a	upgrade attack\n");
	printf("-d	upgrade defense\n");
	printf("-l	upgrade health\n");
	printf("-i	upgrade income\n");
	printf("-f	fight\n");
	printf("-r	restore hp, you will go into zero-interest debt(negative money)\n");
}


void
special_debug(void)
{
	int const cost_atk
		= get_upgrade_cost_generic(
				upgrade_type_attack
				, 5);
	int const cost_def
		= get_upgrade_cost_generic(
				upgrade_type_defense
				, 5);
	int const cost_hpmax
		= get_upgrade_cost_generic(upgrade_type_hpmax , 37)
		+ get_upgrade_cost_generic(upgrade_type_hpmax , 38)
		+ get_upgrade_cost_generic(upgrade_type_hpmax , 39)
		+ get_upgrade_cost_generic(upgrade_type_hpmax , 40) ;
	int const cost_income
		= get_upgrade_cost_generic(upgrade_type_income , 2) ;

	printf("total_upgrade_cost: %d(atk:%d,def:%d,hp:%d,income:%d)"
			, cost_atk + cost_def + cost_hpmax + cost_income
			, cost_atk
			, cost_def
			, cost_hpmax
			, cost_income );

	exit(EXIT_SUCCESS);
}




int main(int argc , char * argv[])
{
	bool flag_heal = false;
	bool flag_upgrade_attack  = false;
	bool flag_upgrade_defense = false;
	bool flag_upgrade_health  = false;
	bool flag_upgrade_income  = false;
	bool flag_upgrade_vault  = false;
	bool flag_upgrade_level  = false;
	bool flag_action_combat = false;
	bool flag_player_chose_enemy_level = false;
	int chosen_enemy_level = 0;

	for(int i = 1; i < argc; ++i) {
		// TODO
		if( 0 == strcmp(argv[i] , "-h") ) {
			print_cli_help();
		} else if( 0 == strcmp(argv[i] , "-D")  ) {
			special_debug();
		} else if( 0 == strcmp(argv[i] , "-c")  ) {
			flag_upgrade_level = true;
		} else if( 0 == strcmp(argv[i] , "-a")  ) {
			flag_upgrade_attack = true;
		} else if( 0 == strcmp(argv[i] , "-d") ) {
			flag_upgrade_defense = true;
		} else if( 0 == strcmp(argv[i] , "-l") ) {
			flag_upgrade_health = true;
		} else if( 0 == strcmp(argv[i] , "-i") ) {
			flag_upgrade_income = true;
		} else if( 0 == strcmp(argv[i] , "-v") ) {
			flag_upgrade_vault = true;
		} else if( 0 == strncmp(argv[i] , "-f" , 2) ) {
			flag_action_combat = true;
			if( *(argv[i] + 2) != '\0' ) {
				int const scan_val = sscanf( (argv[i] + 2)
						, "%d"
						, &chosen_enemy_level);
				if( scan_val == 1 ) {
					flag_player_chose_enemy_level = true;
					printf("player chose enemy level %d\n" , chosen_enemy_level);
				} else {
					printf("error on trying to reading chosen enemy level from argument %s\n" , argv[i]);
				}
			}
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
	int const income_ticks = (income_progres_old / INCOME_TICKS_PER_TIMEUNIT) * INCOME_PER_TICK;
	int const income_progres = income_progres_old % INCOME_TICKS_PER_TIMEUNIT;
	int const income = (income_ticks * state.player_data.income);

	
	int const new_player_money = state.player_data.money + income;
	int const vault_capacity = get_vault_capacity(state.player_data.vault);
	if(new_player_money >= vault_capacity ) {
		printf( "You vault is full! You won't get new money until you spend some\n" );
		state.player_data.money = vault_capacity;
	} else {
		state.player_data.money = new_player_money;
	}
	state.player_data.progres_income = income_progres;
	printf("income	%d(ticks:%d,progres:%d)\n" , income , income_ticks, income_progres );


	if(flag_upgrade_attack) {
		printf("upgrading attack\n");
		player_upgrade_attack(&state);
	}

	if(flag_upgrade_defense) {
		printf("upgrading defense\n");
		player_upgrade_defense(&state);
	}
	if(flag_upgrade_health) {
		printf("upgrading health\n");
		player_upgrade_health(&state);
	}
	if(flag_upgrade_income) {
		printf("upgrading income\n");
		player_upgrade_income(&state);
	}
	if(flag_upgrade_vault) {
		printf("upgrading vault\n");
		player_upgrade_vault(&state);
	}
	if(flag_upgrade_level) {
		printf("upgrading level\n");
		advance_level(&state);
	}

	if(flag_heal) {
		gamestate_heal(&state);
	}

	if(flag_action_combat) {
		// TODO
		if( state.player_data.entity.hp_current < 1 ) {
			printf("you need at least 1 hp to fight(you have %d hp)\n" , state.player_data.entity.hp_current );
		} else {
			const int enemy_level
				= flag_player_chose_enemy_level
				? chosen_enemy_level
				: (1 + state.player_data.entity.level) ;
			const int number_of_rounds = 3 + state.player_data.entity.level;
			CombatEntity foe = create_enemy(enemy_level); /* generate worthy opponent, that is opponent with level equal to player */
			printf("starting combat with level %d foe. (%d rounds).\n"
					, enemy_level
					, number_of_rounds);
			enum result_combat result
				= combat_perform_combat(
					 &(state.player_data.entity)
					,&foe
					,number_of_rounds );
			print_combat_result( result , "you" , "foe" );
			if( state.player_data.entity.hp_current < 0 ) { // player died
				handle_player_death(&state);
			} else {
				printf( "You survived, with %d hp left.\n"
						, state.player_data.entity.hp_current );
			}
			if( result == result_combat_winner_0 ) {
				int const reward = get_exp_reward(
						state.player_data.entity.level
						, enemy_level);
				printf( "For your victory, you gain reward of %d\n"
						, reward );
				add_exp( &state , reward );
			}
		}
	}



	print_gamestate(state);


	save_game(STRING_FILENAME_SAVE , &state , gametime);



	return 0;
}

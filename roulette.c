#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define LOW_THIRD_LOW 	1
#define LOW_THIRD_HIGH  12

#define MID_THIRD_LOW 	13
#define MID_THIRD_HIGH  24

#define HIGH_THIRD_LOW  25
#define HIGH_THIRD_HIGH 36

typedef struct {

	uint32_t low;
	uint32_t high;

} bet_t;

void set_low_third(bet_t * bet){
	bet->low   = LOW_THIRD_LOW;
	bet->high  = LOW_THIRD_HIGH;
}

void set_mid_third(bet_t * bet){
	bet->low   = MID_THIRD_LOW;
	bet->high  = MID_THIRD_HIGH;
}

void set_high_third(bet_t * bet){
	bet->low   = HIGH_THIRD_LOW;
	bet->high  = HIGH_THIRD_HIGH;
}

bool outcome_low(uint32_t outcome){
	return (LOW_THIRD_LOW <= outcome && outcome <= LOW_THIRD_HIGH);
}

bool outcome_mid(uint32_t outcome){
	return (MID_THIRD_LOW <= outcome && outcome <= MID_THIRD_HIGH);
}

bool outcome_high(uint32_t outcome){
	return (HIGH_THIRD_LOW <= outcome && outcome <= HIGH_THIRD_HIGH);
}

bool outcome_win(bet_t * bet_a, bet_t * bet_b, uint32_t outcome){

	bool win = (
		  (bet_a->low <= outcome && bet_a->high >= outcome)
		||(bet_b->low <= outcome && bet_b->high >= outcome)
	);

	return win; 
}

void move_bets_on_win(bet_t * bet_a, bet_t * bet_b, uint32_t outcome){

	if(outcome_low(outcome)){

		set_mid_third(bet_a);
		set_high_third(bet_b);

	} else if (outcome_mid(outcome)){

		set_low_third(bet_a);
		set_high_third(bet_b);

	} else if (outcome_high(outcome)){

		set_low_third(bet_a);
		set_mid_third(bet_b);
	}
}

int32_t main(void){

	srand(time(NULL));

	uint32_t bet_table_size = 10;
	uint32_t num_trials 	= 100;
	uint32_t num_slots 		= 37;
	uint32_t reset_spin 	= 6;

	uint32_t spins_required[bet_table_size];
	double   spin_percent[bet_table_size];
	double   spin_prc_tot[bet_table_size];

	memset(spins_required, 0, bet_table_size * sizeof(uint32_t));
	memset(spin_prc_tot,   0, bet_table_size * sizeof(double));

	double   bet_table[bet_table_size];

	bet_table[0]  = 0.20;
	bet_table[1]  = 0.60;
	bet_table[2]  = 1.80;
	bet_table[3]  = 5.40;
	bet_table[4]  = 16.20;
	bet_table[5]  = 48.60;
	bet_table[6]  = 145.80;
	bet_table[7]  = 437.40;
	bet_table[8]  = 1312.20;
	bet_table[9]  = 3936.60;

	double cash_start 	= 100.00;
	double profit_loss	= 0.0;
	double cash 		= cash_start;

	uint32_t num_busts   = 0;
	uint32_t spins_total = 0;
	uint32_t trial, spin;

	bet_t bet_a_struct, bet_b_struct;
	bet_t * bet_a = &bet_a_struct;
	bet_t * bet_b = &bet_b_struct;

	printf("starting\n");

	// set initial positions
	set_low_third(bet_a);
	set_mid_third(bet_b);

	for(trial = 0; trial < num_trials; ++trial){

		for(spin = 0; spin < bet_table_size; ++spin, ++spins_total){

			if(spin+1 == reset_spin)
				break;

			double bet_total = bet_table[spin];
			double bet_each  = bet_total / 2;

			// printf("trial %d spin %d\n", trial, spin+1);
			// printf("cash %f bet each req'd %f\n", cash, bet_each);
			// printf("bet_a low %d bet_a high %d bet_b low %d bet_b high %d\n",
			// 	bet_a->low, bet_a->high, bet_b->low, bet_b->high);

			if(cash < bet_total){
				++num_busts;
				cash = cash_start;
				//printf("-- bust --\n\n");
				break;
			}

			cash 		-= bet_total;
			profit_loss -= bet_total;

			uint32_t outcome = (uint32_t) (rand() % num_slots);

			//printf("ball lands on %d", outcome);

			if(outcome < 10) printf("\t");

			if(outcome_win(bet_a, bet_b, outcome)){

				//printf(" - win\n\n");

				cash 		+= 3 * bet_each;
				profit_loss += 3 * bet_each;

				++spins_required[spin];

				move_bets_on_win(bet_a, bet_b, outcome);

				break;

			} else {
				//printf(" - loss\n\n");
			}
		}
	}

	for(spin = 0; spin < bet_table_size; ++spin)
		spin_percent[spin] = (double) (spins_required[spin]) / (double) num_trials;

	uint32_t s;

	for(spin = 0; spin < bet_table_size; ++spin)
		for(s = 0; s <= spin; ++s)
			spin_prc_tot[spin] += spin_percent[s];
		
	printf("\n\n--- results ---\n\n");

	printf("spins to win distribution: \n");

	for(spin = 0; spin < bet_table_size; ++spin)
		printf("[%d]:\t %d \t %f \t %f \n", spin+1, spins_required[spin], spin_percent[spin], spin_prc_tot[spin]);
	

	printf("total spins: %d num busts: %d, ending cash: %f P&L: %f\n", spins_total, num_busts, cash, profit_loss);

	return 0;
}
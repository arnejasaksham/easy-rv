#include "F_gait.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define n 10

void print_data(uint64_t count, monitorvars_gait_t mon, io_gait_t io) {
    printf("Tick: %4ld;\tPHASE: %2d;\tSTATE: %2d;\tSTATUS(Match): ", count, io.np, mon._policy_Matching_state);
    switch(gait_check_rv_status_Matching(&mon)) {
        case 0: printf("TRUE"); break;
        case 1: printf("CURRENTLY TRUE"); break;
        case 2: printf("CURRENTLY FALSE"); break;
        default: printf("FALSE"); break;
    }
    printf("\r\n");
}

int main(int argc, char *argv[])
{
	//const int n = atoi(argv[1]);
	
	printf("1- Initial Contact\n2- LoadingResponse\n3- Mid Stance\n4- Terminal Stance\n5- Pre Swing\n6-Initial Swing\n7-Mid Swing\n8-Terminal Swing\n");
	
	monitorvars_gait_t mon[n];
	io_gait_t io[n];
	uint64_t ds1[n] = {3, 3, 4, 4, 3, 4, 3, 3, 4, 3},
		 dts[n] = {5, 3, 3, 4, 2, 3, 3, 4, 3, 3},
		 ss[n]  = {5, 4, 5, 4, 5, 4, 4, 4, 4, 4},
		 std[n] = {4, 3, 2, 5, 4, 4, 2, 3, 5, 3},
		 ds2[n] = {2, 3, 4, 4, 3, 5, 4, 4, 3, 4},
		 itm[n] = {3, 2, 2, 4, 2, 2, 2, 3, 3, 3},
		 mtt[n] = {6, 5, 6, 5, 6, 5, 5, 5, 5, 4},
		 tti[n] = {3, 4, 3, 4, 3, 3, 3, 3, 4, 3};

	for(int i=0;i<n;i++)
	{
		gait_init_all_vars(&(mon[i]), &(io[i]), ds1[i], dts[i], ss[i], std[i], ds2[i], itm[i], mtt[i], tti[i]);
		io[i].np = 4;
		}

	uint64_t count = 0;
	uint8_t np=0;
	
	while(count < 50)
	{
		while(np<1 || np>8)
		{
			printf("Enter next phase: ");
			scanf("%hhu", &np);
			if(np<1 || np>8)
				printf("Valid phases are 1 to 8.\n1- Initial Contact\n2- LoadingResponse\n3- Mid Stance\n4- Terminal Stance\n5- Pre Swing\n6-Initial Swing\n7-Mid Swing\n8-Terminal Swing\n");
		}
		for(int i=0;i<n;i++)
		{
			io[i].np = np;
			
			gait_run_via_monitor(&(mon[i]), &(io[i]));
			printf("Monitor %2d--> ", i);
			print_data(count, mon[i], io[i]);
		}
		printf("\n");
		np=0;
		count++;
	}
}

void gait_run(io_gait_t *io) {
    //nothing to do, the gait has no operations
}


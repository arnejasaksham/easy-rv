#include "F_gait.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void print_data(uint32_t count, monitorvars_gait_t mon, io_gait_t io) {
    printf("Tick: %4d;\tPHASE: %2d;\tSTATE: %2d;\tSTATUS(Match): ", count, io.np, mon._policy_Matching_state);
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
	const int n = atoi(argv[1]);
	monitorvars_gait_t mon[n];
	io_gait_t io[n];
	uint64_t ds1[2]={1,2}, dts[2]={2,4}, ss[2]={3,6}, std[2]={4,7}, ds2[2]={5,5}, itm[2]={6,2}, mtt[2]={7,3}, tti[2]={8,8};

	for(int i=0;i<n;i++)
	{
		gait_init_all_vars(&(mon[i]), &(io[i]), ds1[i], dts[i], ss[i], std[i], ds2[i], itm[i], mtt[i], tti[i]);
		io[i].np = 4;
		}

	uint32_t count = 0;
	
	while(count++ < 50)
	{
		for(int i=0;i<n;i++)
		{
			if(count == 8) io[i].np = 5;
			if(count == 15) io[i].np = 6;
			if(count == 17) io[i].np = 7;
			if(count == 19) io[i].np = 8;
			if(count == 25) io[i].np = 1;
			if(count == 30) io[i].np = 2;
			if(count == 35) io[i].np = 3;

			//wait here until gait expires
			
			gait_run_via_monitor(&(mon[i]), &(io[i]));
			printf("Monitor %2d--> ", i);
			print_data(count, mon[i], io[i]);
		}
		printf("\n");
	}
}

void gait_run(io_gait_t *io) {
    //nothing to do, the gait has no operations
}


#include "F_gait.h"
#include <stdio.h>
#include <stdint.h>

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

int main() {
    monitorvars_gait_t mon;
    io_gait_t io;
    
    gait_init_all_vars(&mon, &io);

    uint32_t count = 0;
    io.np = 4;
    while(count++ < 100) {
        if(count == 8) io.np = 5;
        if(count == 15) io.np = 6;
        if(count == 17) io.np = 7;
        if(count == 19) io.np = 8;
        if(count == 25) io.np = 1;
        if(count == 30) io.np = 2;
        if(count == 35) io.np = 3;

        //wait here until gait expires

        gait_run_via_monitor(&mon, &io);

        print_data(count, mon, io);
    }
}

void gait_run(io_gait_t *io) {
    //nothing to do, the gait has no operations
}


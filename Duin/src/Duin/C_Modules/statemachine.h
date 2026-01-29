#ifndef C_STATEMACHINE_H
#define C_STATEMACHINE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SM_NAME_LENGTH 50
#define SM_MAX_STATES 256
#define SM_STACK_SIZE 8

#define SM_ERR int
#define SM_ERR_OK 0
#define SM_ERR_NAME_NOT_FOUND 1
#define SM_ERR_NO_CALLBACK 2

typedef struct
{
    char state_name[SM_NAME_LENGTH];
    void (*state_enter)(void);
    void (*state_update)(double);
    void (*state_physics_update)(double);
    void (*state_draw)(double);
    void (*state_handle_ui)(double);
    void (*state_exit)(void);
} SM_State;

typedef struct
{
    char machine_name[SM_NAME_LENGTH];
    SM_State *_sm_registered_states[SM_MAX_STATES];
    int _sm_registered_states_count;
    SM_State *_sm_current_state;
    SM_State *_sm_previous_state;
} SM_Machine;

void sm_create_state_machine(SM_Machine *fsm, const char *machine_name);
void sm_register_state(SM_Machine *fsm, SM_State *state, const char *state_name);
SM_ERR sm_switch_state(SM_Machine *fsm, const char *state_name);
void sm_switch_previous_state(SM_Machine *fsm);
void sm_switch_state_pointer(SM_Machine *fsm, SM_State *new_state);
SM_ERR sm_execute_state_update(SM_Machine *fsm, double delta);
SM_ERR sm_execute_state_physics_update(SM_Machine *fsm, double delta);
SM_ERR sm_execute_state_draw(SM_Machine *fsm, double delta);
SM_ERR sm_execute_state_handle_ui(SM_Machine *fsm, double delta);

#endif // C_STATEMACHINE_H

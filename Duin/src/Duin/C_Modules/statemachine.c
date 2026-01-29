#include "statemachine.h"

void sm_register_state(SM_Machine *fsm, SM_State *state, const char *state_name)
{
    if (fsm->_sm_registered_states_count + 1 >= SM_MAX_STATES)
    {
        printf("Error: Max number of states reached in machine '%s'! \n", fsm->machine_name);
        return;
    }

    for (int i = 0; i < fsm->_sm_registered_states_count; i++)
    {
        if (strcmp(fsm->_sm_registered_states[i]->state_name, state_name) == 0)
        {
            printf("Error: State with name '%s' already registered in machine '%s'.\n", state_name, fsm->machine_name);
            return;
        }
    }

    strncpy(state->state_name, state_name, sizeof(state->state_name) - 1);
    state->state_name[sizeof(state->state_name) - 1] = '\0';

    fsm->_sm_registered_states[fsm->_sm_registered_states_count++] = state;
}

void sm_create_state_machine(SM_Machine *fsm, const char *machine_name)
{
    fsm->_sm_current_state = NULL;
    fsm->_sm_previous_state = NULL;
    fsm->_sm_registered_states_count = 0;
    strncpy(fsm->machine_name, machine_name, sizeof(fsm->machine_name) - 1);
    fsm->machine_name[sizeof(fsm->machine_name) - 1] = '\0';
}

SM_ERR sm_switch_state(SM_Machine *fsm, const char *state_name)
{
    for (int i = 0; i < fsm->_sm_registered_states_count; i++)
    {
        if (strcmp(fsm->_sm_registered_states[i]->state_name, state_name) == 0)
        {
            sm_switch_state_pointer(fsm, fsm->_sm_registered_states[i]);
            return SM_ERR_OK;
        }
    }

    printf("Error: State with name '%s' not found in machine '%s'.\n", state_name, fsm->machine_name);
    return SM_ERR_NAME_NOT_FOUND;
}

void sm_switch_previous_state(SM_Machine *fsm)
{
    sm_switch_state_pointer(fsm, fsm->_sm_previous_state);
}

void sm_switch_state_pointer(SM_Machine *fsm, SM_State *new_state)
{
    if (fsm->_sm_current_state != NULL && fsm->_sm_current_state->state_exit != NULL)
    {
        fsm->_sm_current_state->state_exit();
    }

    fsm->_sm_previous_state = fsm->_sm_current_state;
    fsm->_sm_current_state = new_state;

    if (fsm->_sm_current_state != NULL && fsm->_sm_current_state->state_enter != NULL)
    {
        fsm->_sm_current_state->state_enter();
    }
}

SM_ERR sm_execute_state_update(SM_Machine *fsm, double delta)
{
    if (fsm->_sm_current_state != NULL && fsm->_sm_current_state->state_update != NULL)
    {
        fsm->_sm_current_state->state_update(delta);
        return SM_ERR_OK;
    }

    return SM_ERR_NO_CALLBACK;
}

SM_ERR sm_execute_state_physics_update(SM_Machine *fsm, double delta)
{
    if (fsm->_sm_current_state != NULL && fsm->_sm_current_state->state_physics_update != NULL)
    {
        fsm->_sm_current_state->state_physics_update(delta);
        return SM_ERR_OK;
    }

    return SM_ERR_NO_CALLBACK;
}

SM_ERR sm_execute_state_draw(SM_Machine *fsm, double delta)
{
    if (fsm->_sm_current_state != NULL && fsm->_sm_current_state->state_draw != NULL)
    {
        fsm->_sm_current_state->state_draw(delta);
        return SM_ERR_OK;
    }

    return SM_ERR_NO_CALLBACK;
}

SM_ERR sm_execute_state_handle_ui(SM_Machine *fsm, double delta)
{
    if (fsm->_sm_current_state != NULL && fsm->_sm_current_state->state_handle_ui != NULL)
    {
        fsm->_sm_current_state->state_handle_ui(delta);
        return SM_ERR_OK;
    }

    return SM_ERR_NO_CALLBACK;
}

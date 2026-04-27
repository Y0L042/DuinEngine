options gen2
options rtti

require ../../../Headers public

require dn_log

require ../../../state_machine.das public

class Player_OnGround_Idle : State {
    def override _enter() : void {
        dn_info("Entering State: [Player | OnGround | Idle]")
    }
}
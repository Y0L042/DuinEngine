options gen2
options rtti

require ../../Headers.das

require dn_log
require dn_gameobject
require dn_ecs
require dn_gameworld
require dn_renderer
require dn_camera
require dn_input
require dn_physics

require ../../state_machine.das
require ./StateMachine/player_on_ground_idle.das
require ../../ecs_components.das
require ../../ecs_systems.das

class Player : DnGameObject {
    sm : StateMachine?

    MAX_PITCH = 87.0f * (PI / 180.0f)

    inverse : int = 1
    cnt : int
    speed : float = 6.0
    v_offset : float3 = float3(0.0, 2.0, 0.0)
    position : float3 = float3(5.0, 2.0, 5.0)


    game_world : DnGameWorld?
    player_entity : DnEntity = DnEntity()
    characterbody : DnCharacterBody?
    camera : DnCamera?
    observer : ecs_observer_t

    def Player(var game_world_ : DnGameWorld?) : void {
        game_world = game_world_

        sm = new StateMachine()
        sm.change_state(new Player_OnGround_Idle())


        assume world = game_world.ecs_world()

        player_entity = game_world.create_entity("Player")

        camera = dn_camera_init(new DnCamera())
        camera.set_impl(
            position + v_offset,
            float3(0.0, 0.0, 0.0),
            float3(0.0, 1.0, 0.0),
            80.0
        )
        dn_set_active_camera(camera)
        if (dn_get_active_camera() == null) {
            dn_fatal("Failed to set player camera as active camera!")
        }
    }

    def set_camera_active() : void {
        dn_set_active_camera(camera)
    }

    def override _init() : void {
        dn_info("Player init!")

        // ecs_add_hook(type<CharacterBodyDesc>, EcsHookEvent.ONSET, @ capture(copy(characterbody), copy(position)) (world : ecs_world_t?, e : ecs_entity_t, cb_cmp : CharacterBodyDesc) : void {
        //     dn_info("Creating CharacterBody {cb_cmp}")
        //     characterbody = new DnCharacterBody()
        //     characterbody.create_capsule(
        //         70.0,
        //         1.0,
        //         60.0,
        //         true,
        //         true,
        //         true,
        //         true,
        //         1.8,
        //         0.25
        //     )
        //     characterbody.initialize(position)
        // })

        player_entity.add(IsPlayer())
        player_entity.set(PlayerMoveInput(dir = float3(0.0f, 0.0f, 0.0f)))
        player_entity.set(MoveSpeed(walk = 7.0f, sprint = 12.0f))
        player_entity.set(CameraYaw(radians = 0.0f))
        player_entity.set(CameraPitch(radians = 0.0f, min_rad = -MAX_PITCH, max_rad = MAX_PITCH))
        player_entity.set(CharacterBodyDesc())
    }

    def override _ready() : void {
        dn_info("Player ready!")
    } 

    def override _update(delta : double) : void {}

    def override _physics_update(delta : double) : void {
        let ref = ref_time_ticks()
        if (game_world == null) {
            dn_warn("player_entity \{{player_entity}\} | game_world \{{game_world}\}")
            return
        }
        system_player_input(game_world.ecs_world(), camera)
 
        var input : PlayerMoveInput? = player_entity.get_mut(type<PlayerMoveInput>)
        if (input != null && characterbody != null) {
            characterbody.move(input.dir, delta)
        }

        position = characterbody != null ? characterbody.get_position() : position
        // position = characterbody.get_position()
        camera.move_to(position + v_offset)
        // dn_info("PLAYER _physics_update {double(get_time_usec(ref)) / 1000d}")
    }

    def override _draw() : void {}

    def override _draw_ui() : void {}

    def override _debug() : void {}
}

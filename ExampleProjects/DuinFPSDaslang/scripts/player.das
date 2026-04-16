options gen2
options rtti

require daslib/rtti

require dn_log
require dn_gameobject
require dn_ecs
require dn_gameworld

class Player : dn_GameObject {
    inverse : int = 1
    cnt : int
    speed : float = 6.0
    game_world : dn_GameWorld?
    player_entity : dn_Entity?



    def Player(var game_world_ : dn_GameWorld?) : void {
        game_world = game_world_
        player_entity = game_world.create_entity("Player")
    }

    def override _init() : void {
    }

    def override _ready() : void {
        dn_info("Player ready!")
    }

    def override _update(delta : double) : void {}

    def override _physics_update(delta : double) : void {     
        if ((player_entity == null) || (game_world == null)) {
            dn_warn("player_entity \{{player_entity}\} | game_world \{{game_world}\}")
            return
        }

        let pos = game_world.get_global_position(player_entity)
        let dist : float = 10.0
        if (pos.x > dist) {
            inverse = -1
            dn_info("Player CNT: {cnt++}")
        } elif (pos.x < -dist) {
            inverse = 1
        }

        let world = game_world.ecs_world()
        dn_entity_set_transform3d(world, player_entity.entity,
                                    dn_make_transform3d(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0))
        dn_entity_set_velocity3d(world, player_entity.entity, dn_make_velocity3d(speed * float(inverse), 0.0, 0.0))
    }

    def override _draw() : void {}

    def override _draw_ui() : void {}

    def override _debug() : void {}
}

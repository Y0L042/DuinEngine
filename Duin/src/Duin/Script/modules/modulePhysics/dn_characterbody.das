options gen2

module dn_characterbody shared public

require dn_characterbody_core public

class dn_CharacterBody {
    _handle : void?

    def create_capsule(
        mass : float,
        max_strength : float,
        max_slope_angle : float,
        enable_stair_stepping : bool,
        enable_stick_to_floor : bool,
        player_can_push_others : bool,
        others_can_push_player : bool,
        height : float,
        radius : float) : void
    {
        _handle = dn_character_body_capsule_create_impl(
            mass, max_strength, max_slope_angle,
            enable_stair_stepping, enable_stick_to_floor,
            player_can_push_others, others_can_push_player,
            height, radius)
    }

    def create_sphere(
        mass : float,
        max_strength : float,
        max_slope_angle : float,
        enable_stair_stepping : bool,
        enable_stick_to_floor : bool,
        player_can_push_others : bool,
        others_can_push_player : bool,
        radius : float) : void
    {
        _handle = dn_character_body_sphere_create_impl(
            mass, max_strength, max_slope_angle,
            enable_stair_stepping, enable_stick_to_floor,
            player_can_push_others, others_can_push_player,
            radius)
    }

    def create_box(
        mass : float,
        max_strength : float,
        max_slope_angle : float,
        enable_stair_stepping : bool,
        enable_stick_to_floor : bool,
        player_can_push_others : bool,
        others_can_push_player : bool,
        sides : float3) : void
    {
        _handle = dn_character_body_box_create_impl(
            mass, max_strength, max_slope_angle,
            enable_stair_stepping, enable_stick_to_floor,
            player_can_push_others, others_can_push_player,
            sides)
    }

    def destroy() : void {
        if (_handle != null) {
            dn_character_body_destroy_impl(_handle)
            _handle = null
        }
    }

    def initialize(position : float3) : void {
        dn_character_body_initialize_impl(_handle, position)
    }

    def set_position(position : float3) : void {
        dn_character_body_set_position_impl(_handle, position)
    }

    def get_position() : float3 {
        return dn_character_body_get_position_impl(_handle)
    }

    def get_center_of_mass() : float3 {
        return dn_character_body_get_center_of_mass_impl(_handle)
    }

    def set_foot_position(position : float3) : void {
        dn_character_body_set_foot_position_impl(_handle, position)
    }

    def get_foot_position() : float3 {
        return dn_character_body_get_foot_position_impl(_handle)
    }

    def get_velocity() : float3 {
        return dn_character_body_get_velocity_impl(_handle)
    }

    def is_on_floor() : bool {
        return dn_character_body_is_on_floor_impl(_handle) != 0
    }

    def is_on_floor_only() : bool {
        return dn_character_body_is_on_floor_only_impl(_handle) != 0
    }

    def move(displacement : float3, delta : double) : void {
        dn_character_body_move_impl(_handle, displacement, delta)
    }
}

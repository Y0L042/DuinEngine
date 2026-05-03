#pragma once
#include <daScript/simulate/simulate.h>

void *dn_create_gameobject_impl(void *classPtr, const das::StructInfo *info, das::Context *context);
void  dn_add_child_object_impl(void *selfHandle, void *childHandle, das::Context *context);
void  dn_remove_child_object_impl(void *selfHandle, void *childHandle, das::Context *context);
int   dn_get_children_count_impl(void *handle);

void  dn_enable_impl(void *handle, bool enable);
void  dn_enable_on_event_impl(void *handle, bool enable);
void  dn_enable_update_impl(void *handle, bool enable);
void  dn_enable_physics_update_impl(void *handle, bool enable);
void  dn_enable_draw_impl(void *handle, bool enable);
void  dn_enable_draw_ui_impl(void *handle, bool enable);
void  dn_enable_debug_impl(void *handle, bool enable);
void  dn_enable_children_impl(void *handle, bool enable);

bool  dn_is_on_event_enabled_impl(void *handle);
bool  dn_is_update_enabled_impl(void *handle);
bool  dn_is_physics_update_enabled_impl(void *handle);
bool  dn_is_draw_enabled_impl(void *handle);
bool  dn_is_draw_ui_enabled_impl(void *handle);
bool  dn_is_debug_enabled_impl(void *handle);
bool  dn_is_children_enabled_impl(void *handle);
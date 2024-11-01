#ifndef SERIALISATION_MANAGER_H
#define SERIALISATION_MANAGER_H

#include <rapidjson/document.h>
#include <flecs.h>

int ReadJSONFile(const char *path, rapidjson::Document *outDoc);
int WriteJSONFile(const char *path, rapidjson::Document *inDoc);

int LoadEntities(const char *path, ecs_world_t *ecs);
int SaveEntities(const char *path, ecs_world_t *ecs);

#endif /* SERIALISATION_MANAGER_H */

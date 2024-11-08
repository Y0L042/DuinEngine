#ifndef SERIALISATION_MANAGER_H
#define SERIALISATION_MANAGER_H

#include <rapidjson/document.h>
#include <flecs.h>

int ReadJSONFile(const char *path, rapidjson::Document *outDoc);
int WriteJSONFile(const char *path, rapidjson::Document *inDoc);

#endif /* SERIALISATION_MANAGER_H */

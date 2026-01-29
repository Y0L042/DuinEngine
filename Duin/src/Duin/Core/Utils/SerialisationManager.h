#ifndef SERIALISATION_MANAGER_H
#define SERIALISATION_MANAGER_H

#include <rapidjson/document.h>
#include <flecs.h>
#include <string>

namespace duin
{

class JSONDocument;
class JSONMember
{
  public:
    JSONMember(JSONDocument &document);
    JSONMember(JSONDocument &document, rapidjson::Value *value);

    int IsValid();
    int HasMember(const char *member);
    JSONMember GetMember(const char *member);
    std::string GetMemberDataAsString();

    rapidjson::Value *GetValue();

  private:
    JSONDocument &document;
    rapidjson::Value *value;
    int isValid;
};

class JSONDocument
{
  public:
    int HasMember(const char *member);
    JSONMember GetMember(const char *member);

    rapidjson::Document &GetDocument();

  private:
    rapidjson::Document document;
};

int ReadJSONFile(const char *path, JSONDocument &outDoc);
int WriteJSONFile(const char *path, JSONDocument &inDoc);

} // namespace duin

#endif /* SERIALISATION_MANAGER_H */

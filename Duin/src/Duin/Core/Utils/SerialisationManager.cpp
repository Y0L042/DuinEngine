#include "dnpch.h"
#include "SerialisationManager.h"

#include <cstdio>
#include <fstream>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

#define MAX_JSON_FILE_CHARS 65536

namespace duin
{

int ReadJSONFile(const char *path, JSONDocument &outDoc)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        DN_CORE_WARN("Failed to open file for reading: {}", path);
        return 1;
    }

    char *buffer = (char *)malloc(sizeof(char) * MAX_JSON_FILE_CHARS);
    rapidjson::FileReadStream is(fp, buffer, MAX_JSON_FILE_CHARS);
    outDoc.GetDocument().ParseStream(is);

    free(buffer);
    fclose(fp);

    if (outDoc.GetDocument().HasParseError())
    {
        DN_CORE_WARN("JSON parse error in file: {}", path);
        return 1;
    }
    DN_CORE_INFO("JSON file read successfully: {}", path);

    return 0;
}

int WriteJSONFile(const char *path, JSONDocument &inDoc)
{
    FILE *fp = fopen(path, "wb");
    if (!fp)
    {
        std::cerr << "Failed to open file for writing: " << path << "\n";
        return 1;
    }

    char *buffer = (char *)malloc(sizeof(char) * MAX_JSON_FILE_CHARS);
    rapidjson::FileWriteStream os(fp, buffer, MAX_JSON_FILE_CHARS);
    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    inDoc.GetDocument().Accept(writer);

    free(buffer);
    fclose(fp);

    return 0;
}

JSONMember::JSONMember(JSONDocument &document) : document(document), value(nullptr), isValid(0)
{
}

JSONMember::JSONMember(JSONDocument &document, rapidjson::Value *value)
    : document(document), value(value), isValid(value != nullptr ? 1 : 0)
{
}

int JSONMember::IsValid()
{
    return isValid;
}

int JSONMember::HasMember(const char *member)
{
    if (value && value->IsObject() && value->HasMember(member))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    return 0;
}

JSONMember JSONMember::GetMember(const char *member)
{
    if (value && value->IsObject() && value->HasMember(member))
    {
        return JSONMember(document, &(*value)[member]);
    }
    else
    {
        DN_CORE_WARN("{} is not a member!", member);
        return JSONMember(document);
    }
    return JSONMember(document);
}

std::string JSONMember::GetMemberDataAsString()
{
    if (!isValid)
    {
        DN_CORE_WARN("JSONMember not valid!");
        return std::string();
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    value->Accept(writer);
    std::string jsonString = buffer.GetString();

    DN_CORE_INFO("{}", jsonString);

    return jsonString;
}

rapidjson::Value *JSONMember::GetValue()
{
    return value;
}

int JSONDocument::HasMember(const char *member)
{
    if (document.HasMember(member))
    {
        return 1;
    }
    return 0;
}

JSONMember JSONDocument::GetMember(const char *member)
{
    if (document.HasMember(member))
    {
        return JSONMember(*this, &document[member]);
    }
    DN_CORE_WARN("{} is not a member!", member);
    return JSONMember(*this);
}

rapidjson::Document &JSONDocument::GetDocument()
{
    return document;
}

} // namespace duin

#include "dnpch.h"
#include "serialisation_manager.h"

#include <cstdio>
#include <fstream>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

#define MAX_JSON_FILE_CHARS 65536

int ReadJSONFile(const char *path, rapidjson::Document *outDoc)
{
    FILE* fp = fopen(path, "rb");
    if (!fp) {
        std::cerr << "Failed to open file for reading: " << path << "\n";
        return 1;
    }

    char buffer[MAX_JSON_FILE_CHARS];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    outDoc->ParseStream(is);

    fclose(fp);

    if (outDoc->HasParseError()) {
        std::cerr << "JSON parse error in file: " << path << "\n";
        return 1;
    }

    return 0;
}

int WriteJSONFile(const char *path, rapidjson::Document *inDoc)
{
    FILE* fp = fopen(path, "wb");
    if (!fp) {
        std::cerr << "Failed to open file for writing: " << path << "\n";
        return 1;
    }

    char buffer[MAX_JSON_FILE_CHARS];
    rapidjson::FileWriteStream os(fp, buffer, sizeof(buffer));
    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    inDoc->Accept(writer);

    fclose(fp);
    return 0;
}

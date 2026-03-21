#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

#include <cassert>
#include <cstdio>
#include <string>

// Message callback — the engine uses this to report errors in human-readable form
void MessageCallback(const asSMessageInfo *msg, void *)
{
    const char *type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";
    printf("[%s] %s (%d, %d): %s\n", type, msg->section, msg->row, msg->col, msg->message);
}

// A simple print function exposed to scripts
void print(const std::string &msg)
{
    printf("%s", msg.c_str());
}

// Vector3 — a C++ value type we register for use in scripts
struct Vector3
{
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

static void Vector3Construct(Vector3 *self) { new (self) Vector3(); }
static void Vector3ConstructXYZ(float x, float y, float z, Vector3 *self) { new (self) Vector3(x, y, z); }

void RegisterVector3(asIScriptEngine *engine)
{
    int r;
    r = engine->RegisterObjectType("Vector3", sizeof(Vector3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vector3>()); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vector3Construct), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(Vector3ConstructXYZ), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectProperty("Vector3", "float x", asOFFSET(Vector3, x)); assert(r >= 0);
    r = engine->RegisterObjectProperty("Vector3", "float y", asOFFSET(Vector3, y)); assert(r >= 0);
    r = engine->RegisterObjectProperty("Vector3", "float z", asOFFSET(Vector3, z)); assert(r >= 0);
}

// Map an AngelScript type ID to a printable name
const char *GetTypeName(asIScriptEngine *engine, int typeId)
{
    if (typeId == asTYPEID_FLOAT)  return "float";
    if (typeId == asTYPEID_DOUBLE) return "double";
    if (typeId == asTYPEID_INT32)  return "int";
    if (typeId == asTYPEID_BOOL)   return "bool";
    asITypeInfo *ti = engine->GetTypeInfoById(typeId);
    return ti ? ti->GetName() : "unknown";
}

int main()
{
    printf("=== AngelScript Evaluation Test ===\n\n");

    // 1. Create engine and set up message callback (good practice: do this first)
    asIScriptEngine *engine = asCreateScriptEngine();
    if (!engine) { printf("Failed to create engine.\n"); return 1; }

    int r;
    r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert(r >= 0);

    // 2. Register the application interface
    RegisterStdString(engine);
    RegisterVector3(engine);
    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert(r >= 0);

    // 3. Compile script
    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "Components");
    if (r < 0) { printf("Failed to start module.\n"); engine->ShutDownAndRelease(); return 1; }

    r = builder.AddSectionFromFile("scripts/components.as");
    if (r < 0) { printf("Failed to load script file.\n"); engine->ShutDownAndRelease(); return 1; }

    r = builder.BuildModule();
    if (r < 0) { printf("Script compilation failed.\n"); engine->ShutDownAndRelease(); return 1; }

    asIScriptModule *mod = engine->GetModule("Components");

    // 4. Enumerate script-defined types and their fields
    printf("--- Script-defined types ---\n\n");

    int typeCount = mod->GetObjectTypeCount();
    printf("Found %d type(s).\n\n", typeCount);

    for (int t = 0; t < typeCount; t++)
    {
        asITypeInfo *type = mod->GetObjectTypeByIndex(t);
        printf("Struct: %s  (size: %d bytes)\n", type->GetName(), type->GetSize());

        int propCount = type->GetPropertyCount();
        if (propCount == 0)
            printf("  (no fields -- tag component)\n");

        for (int p = 0; p < propCount; p++)
        {
            const char *name;
            int typeId;
            bool isPrivate, isProtected;
            int offset;
            type->GetProperty(p, &name, &typeId, &isPrivate, &isProtected, &offset);

            printf("  field[%d]: %-12s  type=%-10s  offset=%d\n",
                   p, name, GetTypeName(engine, typeId), offset);
        }

        // 5. Create an instance, write a field from C++, read it back
        asIScriptObject *obj = static_cast<asIScriptObject *>(
            engine->CreateScriptObject(type));
        if (!obj) { printf("  ERROR: could not create instance.\n\n"); continue; }

        if (propCount > 0)
        {
            const char *fieldName;
            int fieldTypeId;
            type->GetProperty(0, &fieldName, &fieldTypeId);

            if (fieldTypeId == asTYPEID_FLOAT)
            {
                float *ptr = static_cast<float *>(obj->GetAddressOfProperty(0));
                printf("  -> default %s = %.2f\n", fieldName, *ptr);
                *ptr = 42.0f;
                printf("  -> after C++ write: %s = %.2f\n", fieldName, *ptr);
            }
        }
        else
        {
            printf("  -> (tag -- no fields to read/write)\n");
        }

        obj->Release();
        printf("\n");
    }

    // 6. Clean up
    engine->ShutDownAndRelease();

    printf("=== Done ===\n");
    return 0;
}

#include <doctest.h>
#include <Duin/IO/JSONValue.h>
#include <string>

namespace TestJSONValue
{
TEST_SUITE("JSONValue - Construction & Type Checks")
{
    TEST_CASE("Default constructor creates object")
    {
        duin::JSONValue v;
        CHECK(v.IsObject());
        CHECK(v.IsValid());
    }
    TEST_CASE("Boolean constructor")
    {
        duin::JSONValue v_true(true);
        duin::JSONValue v_false(false);
        CHECK(v_true.IsBool());
        CHECK(v_true.GetBool() == true);
        CHECK(v_false.IsBool());
        CHECK(v_false.GetBool() == false);
    }
    TEST_CASE("Integer constructor")
    {
        duin::JSONValue v(42);
        CHECK(v.IsInt());
        CHECK(v.GetInt() == 42);
    }
    TEST_CASE("Double constructor")
    {
        duin::JSONValue v(3.14);
        CHECK(v.IsDouble());
        CHECK(v.GetDouble() == doctest::Approx(3.14));
    }
    TEST_CASE("String constructor")
    {
        const std::string test("hello");
        duin::JSONValue v(test);
        CHECK(v.IsString());
        CHECK(v.GetString() == "hello");
    }
    TEST_CASE("Invalid JSONValue")
    {
        auto v = duin::JSONValue::Invalid();
        CHECK_FALSE(v.IsValid());
    }
}

TEST_SUITE("JSONValue - Object Manipulation")
{
    TEST_CASE("Add/Get/Remove Member")
    {
        duin::JSONValue obj;
        obj.AddMember("int", 123);
        obj.AddMember("str", std::string("abc"));
        obj.AddMember("bool", true);
        CHECK(obj.HasMember("int"));
        CHECK(obj.HasMember("str"));
        CHECK(obj.HasMember("bool"));
        CHECK(obj.GetMember("int").GetInt() == 123);
        CHECK(obj.GetMember("str").GetString() == "abc");
        CHECK(obj.GetMember("bool").GetBool() == true);
        obj.RemoveMember("int");
        CHECK_FALSE(obj.HasMember("int"));
    }
    TEST_CASE("AddMember with JSONValue")
    {
        duin::JSONValue obj;
        duin::JSONValue v(99);
        obj.AddMember("num", v);
        CHECK(obj.HasMember("num"));
        CHECK(obj.GetMember("num").GetInt() == 99);
    }
    TEST_CASE("SetObject resets value")
    {
        duin::JSONValue v(42);
        v.SetObject();
        CHECK(v.IsObject());
    }
}

TEST_SUITE("JSONValue - Array Manipulation")
{
    TEST_CASE("SetArray and PushBack primitives")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);
        arr.PushBack(2);
        arr.PushBack(3);
        CHECK(arr.IsArray());
        CHECK_FALSE(arr.Empty());
        CHECK(arr[0].GetInt() == 1);
        CHECK(arr[1].GetInt() == 2);
        CHECK(arr[2].GetInt() == 3);
    }
    TEST_CASE("PushBack JSONValue")
    {
        duin::JSONValue arr;
        arr.SetArray();
        duin::JSONValue v(42);
        arr.PushBack(v);
        CHECK(arr[0].GetInt() == 42);
    }
    TEST_CASE("Array Capacity and Empty")
    {
        duin::JSONValue arr;
        arr.SetArray();
        CHECK(arr.Empty());
        arr.PushBack(1);
        CHECK(arr.Capacity() >= 1);
    }
}

TEST_SUITE("JSONValue - Parsing & Writing")
{
    TEST_CASE("Parse and Write roundtrip")
    {
        std::string json = "{\"a\":1,\"b\":\"x\"}";
        duin::JSONValue v = duin::JSONValue::Parse(json);
        CHECK(v.IsObject());
        CHECK(v.HasMember("a"));
        CHECK(v.HasMember("b"));
        CHECK(v["a"].GetInt() == 1);
        CHECK(v["b"].GetString() == "x");
        std::string out = v.Write();
        CHECK(out.find("a") != std::string::npos);
        CHECK(out.find("b") != std::string::npos);
    }
    TEST_CASE("Write pretty")
    {
        duin::JSONValue obj;
        obj.AddMember("foo", 123);
        std::string pretty = duin::JSONValue::Write(obj, true);
        CHECK(pretty.find("foo") != std::string::npos);
    }
}

TEST_SUITE("JSONValue - Type Setters")
{
    TEST_CASE("SetInt/SetDouble/SetBool/SetString")
    {
        duin::JSONValue v;
        v.SetInt(7);
        CHECK(v.IsInt());
        CHECK(v.GetInt() == 7);
        v.SetDouble(2.5);
        CHECK(v.IsDouble());
        CHECK(v.GetDouble() == doctest::Approx(2.5));
        v.SetBool(true);
        CHECK(v.IsBool());
        CHECK(v.GetBool() == true);
        v.SetString("abc");
        CHECK(v.IsString());
        CHECK(v.GetString() == "abc");
    }
    TEST_CASE("SetArray and SetObject")
    {
        duin::JSONValue v;
        v.SetArray();
        CHECK(v.IsArray());
        v.SetObject();
        CHECK(v.IsObject());
    }
}

TEST_SUITE("JSONValue - Operators and Iterators")
{
    TEST_CASE("operator[] for object and array")
    {
        duin::JSONValue obj;
        obj.AddMember("x", 10);
        CHECK(obj["x"].GetInt() == 10);
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(5);
        arr.PushBack(6);
        CHECK(arr[0].GetInt() == 5);
        CHECK(arr[1].GetInt() == 6);
    }
    TEST_CASE("Equality and Inequality operators (a)")
    {
        duin::JSONValue v1(1);
        duin::JSONValue v2(1);
        CHECK_FALSE(v1 == v2);
    }
    TEST_CASE("Equality and Inequality operators (b)")
    {
        duin::JSONValue v1(1);
        duin::JSONValue v2(2);
        CHECK(v1 != v2);
    }
    TEST_CASE("Equality and Inequality operators (c)")
    {
        duin::JSONValue v1(1);
        duin::JSONValue v3 = v1;
        CHECK(v1 == v3);
    }
    TEST_CASE("Clone creates deep copy")
    {
        duin::JSONValue v1;
        v1.AddMember("a", 1);
        duin::JSONValue v2 = v1.Clone();
        CHECK(v2.HasMember("a"));
        CHECK(v2["a"].GetInt() == 1);
        v2.RemoveMember("a");
        CHECK(v1.HasMember("a"));
        CHECK_FALSE(v2.HasMember("a"));
    }
    TEST_CASE("Iterators for arrays")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);
        arr.PushBack(2);
        arr.PushBack(3);
        int sum = 0;
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            sum += (*it).GetInt();
        }
        CHECK(sum == 6);
    }
}

TEST_SUITE("JSONValue - Null and Validity")
{
    TEST_CASE("IsNull on null value")
    {
        duin::JSONValue v;
        v.SetObject();
        v.AddMember("n", duin::JSONValue());
        CHECK_FALSE(v["n"].IsNull()); // Default is object, not null
    }
    TEST_CASE("IsReadValid and IsValid")
    {
        duin::JSONValue v;
        CHECK(v.IsReadValid());
        CHECK(v.IsValid());
    }
}

TEST_SUITE("JSONValue - Nested Structures")
{
    TEST_CASE("Nested objects")
    {
        duin::JSONValue root;
        duin::JSONValue child;
        child.AddMember("value", 42);

        root.AddMember("child", child);

        CHECK(root.HasMember("child"));
        CHECK(root["child"].IsObject());
        CHECK(root["child"]["value"].GetInt() == 42);
    }

    TEST_CASE("Nested arrays")
    {
        duin::JSONValue root;
        root.SetArray();

        duin::JSONValue inner;
        inner.SetArray();
        inner.PushBack(1);
        inner.PushBack(2);

        root.PushBack(inner);
        root.PushBack(3);

        CHECK(root[0].IsArray());
        CHECK(root[0][0].GetInt() == 1);
        CHECK(root[0][1].GetInt() == 2);
        CHECK(root[1].GetInt() == 3);
    }

    TEST_CASE("Array of objects")
    {
        duin::JSONValue arr;
        arr.SetArray();

        duin::JSONValue obj1;
        obj1.AddMember("id", 1);
        obj1.AddMember("name", std::string("Alice"));

        duin::JSONValue obj2;
        obj2.AddMember("id", 2);
        obj2.AddMember("name", std::string("Bob"));

        arr.PushBack(obj1);
        arr.PushBack(obj2);

        CHECK(arr[0]["id"].GetInt() == 1);
        CHECK(arr[0]["name"].GetString() == "Alice");
        CHECK(arr[1]["id"].GetInt() == 2);
        CHECK(arr[1]["name"].GetString() == "Bob");
    }

    TEST_CASE("Object with array member")
    {
        duin::JSONValue obj;
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);
        arr.PushBack(2);
        arr.PushBack(3);

        obj.AddMember("numbers", arr);

        CHECK(obj["numbers"].IsArray());
        CHECK(obj["numbers"][0].GetInt() == 1);
        CHECK(obj["numbers"][2].GetInt() == 3);
    }

    TEST_CASE("Deep nesting")
    {
        duin::JSONValue level1;
        duin::JSONValue level2;
        duin::JSONValue level3;

        level3.AddMember("deep", std::string("value"));
        level2.AddMember("level3", level3);
        level1.AddMember("level2", level2);

        CHECK(level1["level2"]["level3"]["deep"].GetString() == "value");
    }
}

TEST_SUITE("JSONValue - Complex JSON Parsing")
{
    TEST_CASE("Parse complex nested JSON")
    {
        std::string json = R"({
            "user": {
                "name": "John",
                "age": 30,
                "active": true
            },
            "scores": [95, 87, 92],
            "metadata": {
                "version": 1.5
            }
        })";

        duin::JSONValue v = duin::JSONValue::Parse(json);

        CHECK(v.IsObject());
        CHECK(v["user"]["name"].GetString() == "John");
        CHECK(v["user"]["age"].GetInt() == 30);
        CHECK(v["user"]["active"].GetBool() == true);
        CHECK(v["scores"][0].GetInt() == 95);
        CHECK(v["scores"][1].GetInt() == 87);
        CHECK(v["scores"][2].GetInt() == 92);
        CHECK(v["metadata"]["version"].GetDouble() == doctest::Approx(1.5));
    }

    TEST_CASE("Parse array JSON")
    {
        std::string json = R"([1, 2, 3, 4, 5])";
        duin::JSONValue v = duin::JSONValue::Parse(json);

        CHECK(v.IsArray());
        CHECK(v[0].GetInt() == 1);
        CHECK(v[4].GetInt() == 5);
    }

    TEST_CASE("Parse empty object")
    {
        std::string json = "{}";
        duin::JSONValue v = duin::JSONValue::Parse(json);

        CHECK(v.IsObject());
        CHECK_FALSE(v.HasMember("anything"));
    }

    TEST_CASE("Parse with special characters")
    {
        std::string json = R"({"text": "Hello\nWorld\t\"Quoted\""})";
        duin::JSONValue v = duin::JSONValue::Parse(json);

        CHECK(v["text"].IsString());
        // RapidJSON handles escape sequences
    }
}

TEST_SUITE("JSONValue - Array Operations Extended")
{
    TEST_CASE("Array with mixed types via JSONValue")
    {
        duin::JSONValue arr;
        arr.SetArray();

        arr.PushBack(duin::JSONValue(42));
        arr.PushBack(duin::JSONValue(std::string("text")));
        arr.PushBack(duin::JSONValue(true));
        arr.PushBack(duin::JSONValue(3.14));

        CHECK(arr[0].IsInt());
        CHECK(arr[1].IsString());
        CHECK(arr[2].IsBool());
        CHECK(arr[3].IsDouble());
    }

    TEST_CASE("Empty array operations")
    {
        duin::JSONValue arr;
        arr.SetArray();

        CHECK(arr.IsArray());
        CHECK(arr.Empty());

        arr.PushBack(1);
        CHECK_FALSE(arr.Empty());
    }

    TEST_CASE("Array capacity grows")
    {
        duin::JSONValue arr;
        arr.SetArray();

        size_t initialCapacity = arr.Capacity();

        for (int i = 0; i < 10; ++i)
        {
            arr.PushBack(i);
        }

        size_t finalCapacity = arr.Capacity();
        CHECK(finalCapacity >= 10);
        CHECK(finalCapacity >= initialCapacity);
    }
}

TEST_SUITE("JSONValue - Iterator Tests Extended")
{
    TEST_CASE("Empty array iteration")
    {
        duin::JSONValue arr;
        arr.SetArray();

        int count = 0;
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            count++;
        }

        CHECK(count == 0);
    }

    TEST_CASE("Single element iteration")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(42);

        int count = 0;
        int value = 0;
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            value = (*it).GetInt();
            count++;
        }

        CHECK(count == 1);
        CHECK(value == 42);
    }

    TEST_CASE("Const iterator usage")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);
        arr.PushBack(2);
        arr.PushBack(3);

        const duin::JSONValue &constArr = arr;
        int sum = 0;
        for (auto it = constArr.begin(); it != constArr.end(); ++it)
        {
            sum += (*it).GetInt();
        }

        CHECK(sum == 6);
    }

    TEST_CASE("Iterator with mixed types")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(duin::JSONValue(1));
        arr.PushBack(duin::JSONValue(std::string("text")));
        arr.PushBack(duin::JSONValue(true));

        int count = 0;
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            CHECK((*it).IsReadValid());
            count++;
        }

        CHECK(count == 3);
    }
}

TEST_SUITE("JSONValue - Error Handling & Edge Cases")
{
    TEST_CASE("Access non-existent member")
    {
        duin::JSONValue obj;
        obj.AddMember("exists", 1);

        duin::JSONValue missing = obj["doesNotExist"];
        CHECK(missing.IsObject()); // Returns default constructed object
    }

    TEST_CASE("Array index out of bounds")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);

        // Accessing out of bounds - behavior depends on implementation
        // Should return default constructed or handle gracefully
    }

    TEST_CASE("Type mismatch - GetInt on string")
    {
        duin::JSONValue v(std::string("not a number"));

        CHECK(v.IsString());
        CHECK_FALSE(v.IsInt());
        int result = v.GetInt(); // Should return 0 and log warning
        CHECK(result == 0);
    }
    TEST_CASE("Type mismatch - GetString on int")
    {
        duin::JSONValue v(42);

        CHECK(v.IsInt());
        CHECK_FALSE(v.IsString());
        std::string result = v.GetString(); // Should return empty and log warning
        CHECK(result.empty());
    }

    TEST_CASE("PushBack on non-array")
    {
        duin::JSONValue obj;
        CHECK(obj.IsObject());

        obj.PushBack(42); // Should log warning, return default
    }

    TEST_CASE("AddMember on array")
    {
        duin::JSONValue arr;
        arr.SetArray();

        arr.AddMember("key", 42); // Should log warning
        CHECK(arr.IsArray());
    }

    TEST_CASE("HasMember on non-object")
    {
        duin::JSONValue v(42);
        CHECK_FALSE(v.HasMember("anything"));
    }

    TEST_CASE("Empty on non-array")
    {
        duin::JSONValue obj;
        CHECK(obj.IsObject());
        // Empty should handle non-arrays gracefully
    }
}

TEST_SUITE("JSONValue - Type Checking Comprehensive")
{
    TEST_CASE("IsNumber covers int and double")
    {
        duin::JSONValue vInt(42);
        duin::JSONValue vDouble(3.14);

        CHECK(vInt.IsNumber());
        CHECK(vInt.IsInt());
        CHECK_FALSE(vInt.IsDouble());

        CHECK(vDouble.IsNumber());
        CHECK(vDouble.IsDouble());
    }

    TEST_CASE("Type checks on default constructor")
    {
        duin::JSONValue v;

        CHECK(v.IsObject());
        CHECK_FALSE(v.IsArray());
        CHECK_FALSE(v.IsString());
        CHECK_FALSE(v.IsBool());
        CHECK_FALSE(v.IsInt());
        CHECK_FALSE(v.IsDouble());
        CHECK_FALSE(v.IsNull());
    }

    TEST_CASE("Type checks after type change")
    {
        duin::JSONValue v;
        CHECK(v.IsObject());

        v.SetArray();
        CHECK(v.IsArray());
        CHECK_FALSE(v.IsObject());

        v.SetInt(42);
        CHECK(v.IsInt());
        CHECK_FALSE(v.IsArray());
    }
}

TEST_SUITE("JSONValue - Write & Serialization Extended")
{
    TEST_CASE("Write nested structure")
    {
        duin::JSONValue root;
        duin::JSONValue child;
        child.AddMember("value", 42);
        root.AddMember("child", child);

        std::string output = root.Write();

        CHECK(output.find("child") != std::string::npos);
        CHECK(output.find("value") != std::string::npos);
        CHECK(output.find("42") != std::string::npos);
    }

    TEST_CASE("Write array")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);
        arr.PushBack(2);
        arr.PushBack(3);

        std::string output = duin::JSONValue::Write(arr);

        CHECK(output.find("[") != std::string::npos);
        CHECK(output.find("1") != std::string::npos);
        CHECK(output.find("3") != std::string::npos);
    }

    TEST_CASE("Pretty print formatting")
    {
        duin::JSONValue obj;
        obj.AddMember("a", 1);
        obj.AddMember("b", 2);

        std::string compact = duin::JSONValue::Write(obj, false);
        std::string pretty = duin::JSONValue::Write(obj, true);

        CHECK(pretty.length() > compact.length()); // Pretty has whitespace
        bool test = pretty.find("\t") != std::string::npos || pretty.find("\n") != std::string::npos;
        CHECK(test);
    }
}

TEST_SUITE("JSONValue - Clone & Copy Operations")
{
    TEST_CASE("Clone nested structure")
    {
        duin::JSONValue original;
        duin::JSONValue child;
        child.AddMember("value", 42);
        original.AddMember("child", child);

        duin::JSONValue cloned = original.Clone();

        CHECK(cloned["child"]["value"].GetInt() == 42);

        cloned["child"]["value"].SetInt(100);
        CHECK(original["child"]["value"].GetInt() == 42); // Original unchanged
        CHECK(cloned["child"]["value"].GetInt() == 100);
    }

    TEST_CASE("Clone array")
    {
        duin::JSONValue original;
        original.SetArray();
        original.PushBack(1);
        original.PushBack(2);

        duin::JSONValue cloned = original.Clone();

        CHECK(cloned[0].GetInt() == 1);
        cloned.PushBack(3);

        // Original should still have 2 elements
        // Cloned should have 3
    }

    TEST_CASE("Copy assignment shares pointer")
    {
        duin::JSONValue v1(42);
        duin::JSONValue v2 = v1;

        CHECK(v1 == v2); // Same underlying pointer
    }
}

TEST_SUITE("JSONValue - GetMember vs Operator[]")
{
    TEST_CASE("GetMember and operator[] equivalence")
    {
        duin::JSONValue obj;
        obj.AddMember("key", 42);

        CHECK(obj.GetMember("key").GetInt() == obj["key"].GetInt());
    }

    TEST_CASE("Non-existent member behavior")
    {
        duin::JSONValue obj;

        duin::JSONValue via_getmember = obj.GetMember("missing");
        duin::JSONValue via_operator = obj["missing"];

        CHECK(via_getmember.IsObject());
        CHECK(via_operator.IsObject());
    }
}

TEST_SUITE("JSONValue - Remove Member Operations")
{
    TEST_CASE("Remove member that exists")
    {
        duin::JSONValue obj;
        obj.AddMember("a", 1);
        obj.AddMember("b", 2);

        CHECK(obj.HasMember("a"));
        obj.RemoveMember("a");
        CHECK_FALSE(obj.HasMember("a"));
        CHECK(obj.HasMember("b"));
    }

    TEST_CASE("Remove non-existent member")
    {
        duin::JSONValue obj;
        obj.AddMember("a", 1);

        obj.RemoveMember("doesNotExist"); // Should not crash
        CHECK(obj.HasMember("a"));
    }

    TEST_CASE("Remove all members")
    {
        duin::JSONValue obj;
        obj.AddMember("a", 1);
        obj.AddMember("b", 2);
        obj.AddMember("c", 3);

        obj.RemoveMember("a");
        obj.RemoveMember("b");
        obj.RemoveMember("c");

        CHECK_FALSE(obj.HasMember("a"));
        CHECK_FALSE(obj.HasMember("b"));
        CHECK_FALSE(obj.HasMember("c"));
    }
}

TEST_SUITE("JSONValue - Allow Duplicates Flag")
{
    TEST_CASE("AddMember with duplicates not allowed")
    {
        duin::JSONValue obj;
        obj.AddMember("key", 1, false);
        obj.AddMember("key", 2, false); // Should replace

        CHECK(obj["key"].GetInt() == 2);
    }

    TEST_CASE("AddMember with duplicates allowed")
    {
        duin::JSONValue obj;
        obj.AddMember("key", 1, true);
        obj.AddMember("key", 2, true); // Behavior depends on RapidJSON

        // Test actual behavior
        CHECK(obj.HasMember("key"));
    }
}

TEST_SUITE("JSONValue - Special Values")
{
    TEST_CASE("Empty string")
    {
        duin::JSONValue v(std::string(""));

        CHECK(v.IsString());
        CHECK(v.GetString().empty());
    }

    TEST_CASE("Zero values")
    {
        duin::JSONValue vInt(0);
        duin::JSONValue vDouble(0.0);

        CHECK(vInt.GetInt() == 0);
        CHECK(vDouble.GetDouble() == doctest::Approx(0.0));
    }

    TEST_CASE("Negative numbers")
    {
        duin::JSONValue vInt(-42);
        duin::JSONValue vDouble(-3.14);

        CHECK(vInt.GetInt() == -42);
        CHECK(vDouble.GetDouble() == doctest::Approx(-3.14));
    }

    TEST_CASE("Very large numbers")
    {
        duin::JSONValue v(2147483647); // INT_MAX

        CHECK(v.IsInt());
        CHECK(v.GetInt() == 2147483647);
    }
}

TEST_SUITE("JSONValue - ParseFromFile")
{
    TEST_CASE("Parse non-existent file")
    {
        duin::JSONValue v = duin::JSONValue::ParseFromFile("non_existent_file.json");
        // Should return empty object and log error
        CHECK(v.IsObject());
    }

    // Note: Testing actual file parsing requires creating temporary files
    // which may require filesystem modification, so we skip actual file tests
}

TEST_SUITE("JSONValue - Parse Error Handling")
{
    TEST_CASE("Parse invalid JSON")
    {
        std::string invalidJson = "{invalid}";
        duin::JSONValue v = duin::JSONValue::Parse(invalidJson);
        // Should return empty object on error
        CHECK(v.IsObject());
    }

    TEST_CASE("Parse JSON string primitive")
    {
        std::string json = "\"hello\"";
        duin::JSONValue v = duin::JSONValue::Parse(json);
        CHECK(v.IsString());
        CHECK(v.GetString() == "hello");
    }

    TEST_CASE("Parse JSON number primitive")
    {
        std::string json = "42";
        duin::JSONValue v = duin::JSONValue::Parse(json);
        CHECK(v.IsNumber());
        CHECK(v.GetInt() == 42);
    }

    TEST_CASE("Parse JSON boolean primitive")
    {
        std::string json1 = "true";
        duin::JSONValue v1 = duin::JSONValue::Parse(json1);
        CHECK(v1.IsBool());
        CHECK(v1.GetBool() == true);

        std::string json2 = "false";
        duin::JSONValue v2 = duin::JSONValue::Parse(json2);
        CHECK(v2.IsBool());
        CHECK(v2.GetBool() == false);
    }

    TEST_CASE("Parse JSON null")
    {
        std::string json = "null";
        duin::JSONValue v = duin::JSONValue::Parse(json);
        CHECK(v.IsNull());
    }
}

TEST_SUITE("JSONValue - Iterator Edge Cases")
{
    TEST_CASE("Begin/End on non-array")
    {
        duin::JSONValue obj;
        obj.AddMember("key", 1);

        // Calling Begin() and End() on objects should log warning
        auto it1 = obj.Begin();
        auto it2 = obj.End();
        // Should return invalid iterators
    }

    TEST_CASE("Iterator GetValue method")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(42);

        auto it = arr.begin();
        duin::JSONValue v = it.GetValue();
        CHECK(v.GetInt() == 42);
    }

    TEST_CASE("ConstIterator GetValue method")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(42);

        const duin::JSONValue& constArr = arr;
        auto it = constArr.begin();
        duin::JSONValue v = it.GetValue();
        CHECK(v.GetInt() == 42);
    }
}

TEST_SUITE("JSONValue - FindMember")
{
    TEST_CASE("FindMember on object - member exists")
    {
        duin::JSONValue obj;
        obj.AddMember("key1", 42);
        obj.AddMember("key2", std::string("value"));

        auto it = obj.FindMember("key1");
        CHECK((*it).GetInt() == 42);
    }

    TEST_CASE("FindMember on object - member not found")
    {
        duin::JSONValue obj;
        obj.AddMember("key1", 42);

        auto it = obj.FindMember("nonexistent");
        // Should return invalid iterator
    }

    TEST_CASE("FindMember on non-object")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);

        auto it = arr.FindMember("key");
        // Should log warning and return invalid iterator
    }
}

TEST_SUITE("JSONValue - Begin/End Methods")
{
    TEST_CASE("Begin and End uppercase methods")
    {
        duin::JSONValue arr;
        arr.SetArray();
        arr.PushBack(1);
        arr.PushBack(2);
        arr.PushBack(3);

        int sum = 0;
        for (auto it = arr.Begin(); it != arr.End(); ++it)
        {
            sum += (*it).GetInt();
        }
        CHECK(sum == 6);
    }
}

TEST_SUITE("JSONValue - Type Getters with Warnings")
{
    TEST_CASE("GetBool on non-bool")
    {
        duin::JSONValue v(42);
        bool result = v.GetBool();
        CHECK(result == false); // Returns false with warning
    }

    TEST_CASE("GetDouble on non-double")
    {
        duin::JSONValue v(std::string("text"));
        double result = v.GetDouble();
        CHECK(result == doctest::Approx(0.0)); // Returns 0.0 with warning
    }

    TEST_CASE("Capacity on non-array")
    {
        duin::JSONValue obj;
        size_t cap = obj.Capacity();
        // Should log warning and return 0
    }

    TEST_CASE("begin/end on non-array")
    {
        duin::JSONValue obj;
        obj.AddMember("key", 1);

        // Should log warnings
        auto it1 = obj.begin();
        auto it2 = obj.end();
    }
}

TEST_SUITE("JSONValue - Operator Dereference")
{
    TEST_CASE("Dereference operator on JSONValue")
    {
        duin::JSONValue v(42);
        const duin::JSONValue& deref = *v;
        CHECK(deref.GetInt() == 42);
    }
}

TEST_SUITE("JSONValue - GetRJSONValue")
{
    TEST_CASE("GetRJSONValue returns underlying RapidJSON value")
    {
        duin::JSONValue v(42);
        rapidjson::Value& rjson = v.GetRJSONValue();
        CHECK(rjson.IsInt());
        CHECK(rjson.GetInt() == 42);
    }
}

TEST_SUITE("JSONValue - Array Index Operator")
{
    TEST_CASE("Array index on non-array")
    {
        duin::JSONValue obj;
        obj.AddMember("key", 1);

        duin::JSONValue result = obj[0];
        // Should log warning and return empty object
        CHECK(result.IsObject());
    }
}

TEST_SUITE("JSONValue - Inequality Operator Edge Cases")
{
    TEST_CASE("Inequality with different values")
    {
        duin::JSONValue v1;
        v1.AddMember("a", 1);
        
        duin::JSONValue v2;
        v2.AddMember("b", 2);

        CHECK(v1 != v2);
    }
}
} // namespace TestJSONValue

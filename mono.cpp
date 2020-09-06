#include <iostream>
#include <vector>
#include <unordered_map>
#include <variant>
using namespace std;

class Identifier
{
private:

    string name;
    int id;
    int level;

    static int last_id;
    static unordered_map<string, int> id_map;
    static id_for_name(const string& name)
    {
        auto res = id_map.insert({name, last_id});
        if (res.second) ++last_id;
        return res.first->second;
    }

public:

    Identifier (const string& name):
        name(name),
        id(id_for_name(name)),
        level(0) {}

    bool operator==(const Identifier& other)
    {
        return id == other.id && level == other.level;
    }
};

class Value
{
private:

    class Constructor
    {
        Identifier id;
        vector<Value*> args;
    };

    class Function
    {
        struct Case
        {
            Value* templ;
            Value* body;
        };

        vector<Case> cases;
    };

    class Variable
    {
        Identifier id;
    };

    class Application
    {
        Value* fun;
        Value* arg;
    };

    // consider lazy substitution variant type

    variant<Constructor, Function, Variable, Application> data;

public:

    bool match(Value& other)
    {
        return false;
    }

};

int main()
{
    return 0;
}

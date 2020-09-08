#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <variant>
#include <assert.h>
using namespace std;

class Identifier
{
    int id;

    static int last_id;
    static unordered_map<string, int> id_map;
    static unordered_map<string, int> name_map;
    static id_for_name(const string& name)
    {
        auto res = id_map.insert({name, last_id});
        if (res.second)
        {
            name_map.insert({res})
            ++last_id;
        }
        return res.first->second;
    }

public:

    Identifier (const string& name):
        id(id_for_name(name)) {}

    Identifier (Identifier id):
        id(id.id) {}

    string get_name()
    {
        return name_map[id];
    }

    bool operator==(Identifier other)
    {
        return id == other.id;
    }

    bool operator<(Identifier other)
    {
        return id < other.id;
    }
};

class Value
{
    typedef shared_ptr<Value> ValuePtr;
    typedef set<Identifier> IdentifierSet;
    typedef map<Identifier, ValuePtr> IdentifierMap;

    class Variable
    {
        Identifier id;

    public:

        Variable(Identifier id):
            id(id) {}

        ValuePtr substitute(IdentifierMap& idMap)
        {
            auto it = idMap.find(id);
            assert(it != idMap.end());
            return it->second;
        }
    };

    class Constructor
    {
        Identifier id;
        vector<ValuePtr> args;

    public:

        /// TO-DO: move constructors
        Constructor(Identifier id, const vector<ValuePtr>& args):
            id(id),
            args(args) {}

        Constructor substitute(IdentifierMap& idMap)
        {
            vector<ValuePtr> newArgs;
            for (auto arg : args)
            {
                newArgs.push_back(arg->substitute(idMap));
            }
            Constructor(id, newArgs);
        }
    };

    class Function
    {
        class Case
        {
            ValuePtr templ;
            ValuePtr body;

        public:

            Case(ValuePtr teml, ValuePtr body):
                templ(templ)
                body(body) {}

            Case substitute(IdentifierMap& idMap)
            {
                /// TO-DO: implement
            }
        };

        vector<Case> cases;
    };

    class Application
    {
        ValuePtr fun;
        ValuePtr arg;

    public:

        Application(ValuePtr fun, ValuePtr arg):
            fun(fun),
            arg(arg) {}

        Application substitute(IdentifierMap& idMap)
        {
            Application(fun->substitute, arg->substitute);
        }
    };

    // consider lazy substitution variant type

    IdentifierSet free;

    variant<Variable, Constructor, Function, Application> result;

    ValuePtr substitute(IdentifierMap& idMap)
    {
        ///TO-DO: This can be done linearly

        //remove irrelevant mappings
        IdentifierSet newFree = free;
        std::vector<std::pair<Identifier, ValuePtr>> removed;
        for (auto it = idMap.begin(); id != idMap.end(); ++id)
        {
            auto it2 = newFree.find(it->first);
            if (it2 == newFree.end())
            {
                removed.push_back(*it);
                it = idMap.erase(it);
            }
            else
            {
                newFree.erase(it2);
            }
        }

        if (!idMap.empty())
        {
            switch (result.index())
            {
            case 0:

            case 1:

            case 2:

            case 3:

            default:
                assert(false);
            }
        }

        // restore the removed mappings
        for (const auto& idValue : removed)
        {
            idMap.insert(idValue);
        }
    }

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

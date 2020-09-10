#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <variant>
#include <memory>
#include <assert.h>
using namespace std;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

class Identifier
{
    int id;

    static int last_id;
    static unordered_map<string, int> id_map;
    static unordered_map<int, string> name_map;
    static int id_for_name(const string& name)
    {
        auto res = id_map.insert({name, last_id});
        if (res.second)
        {
            name_map.insert({last_id, name});
            ++last_id;
        }
        return res.first->second;
    }

public:

    Identifier (const string& name):
        id(id_for_name(name)) {}

    Identifier (const Identifier& id):
        id(id.id) {}

    string get_name() const
    {
        return name_map[id];
    }

    bool operator==(Identifier other) const
    {
        return id == other.id;
    }

    bool operator<(Identifier other) const
    {
        return id < other.id;
    }
};

class Value : public std::enable_shared_from_this<Value>
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

        ValuePtr substitute(IdentifierMap& idMap) const
        {
            return idMap[id];
        }
    };

    class Constructor
    {
        Identifier id;
        vector<ValuePtr> args;

    public:

        // TO-DO: move constructors
        Constructor(Identifier id, vector<ValuePtr>&& args):
            id(id),
            args(forward<vector<ValuePtr>>(args)) {}

        Constructor substitute(IdentifierMap& idMap) const
        {
            vector<ValuePtr> newArgs;
            for (auto arg : args)
            {
                newArgs.push_back(arg->substitute(idMap));
            }
            return Constructor(id, move(newArgs));
        }
    };

    class Function
    {
        class Case
        {
            ValuePtr templ;
            ValuePtr body;

            Case() {}
            
        public:

            Case(ValuePtr teml, ValuePtr body):
                templ(templ),
                body(body) {}

            Case substitute(IdentifierMap& idMap) const
            {
                // TO-DO: This can be done linearly, also merged with one in main substitute

                // remove bound mappings
                std::vector<std::pair<Identifier, ValuePtr>> removed;
                for (auto it = idMap.begin(); it != idMap.end(); ++it)
                {
                    auto it2 = templ->free.find(it->first);
                    if (it2 != templ->free.end())
                    {
                        removed.push_back(*it);
                        it = idMap.erase(it);
                    }
                }

                Case newCase;
                if (!idMap.empty())
                {
                    newCase = Case(templ, body->substitute(idMap));
                }
                else
                {
                    newCase = *this;
                }

                // restore the removed mappings
                for (const auto& idValue : removed)
                {
                    idMap.insert(idValue);
                }

                return newCase;
            }
        };

        vector<Case> cases;

    public:

        Function(vector<Case>&& cases):
            cases(forward<vector<Case>>(cases)) {}

        Function substitute(IdentifierMap& idMap) const
        {
            vector<Case> newCases;
            for (const auto& currCase : cases)
            {
                newCases.push_back(currCase.substitute(idMap));
            }
            return Function(move(newCases));
        }
    };

    class Application
    {
        ValuePtr fun;
        ValuePtr arg;

    public:

        Application(ValuePtr fun, ValuePtr arg):
            fun(fun),
            arg(arg) {}

        Application substitute(IdentifierMap& idMap) const
        {
            return Application(fun->substitute(idMap), arg->substitute(idMap));
        }
    };

    typedef variant<Variable, Constructor, Function, Application> NodeType;

    NodeType result;
    IdentifierSet free;

    Value(NodeType&& result, IdentifierSet&& free):
        result(forward<NodeType>(result)),
        free(forward<IdentifierSet>(free)) {}

    ValuePtr substitute(IdentifierMap& idMap)
    {
        // TO-DO: This can be done linearly

        // remove irrelevant mappings
        IdentifierSet newFree = free;
        std::vector<std::pair<Identifier, ValuePtr>> removed;
        for (auto it = idMap.begin(); it != idMap.end(); ++it)
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

        ValuePtr newValue;
        if (!idMap.empty())
        {
            visit(overload
            {
                [&](Variable& var)
                {
                    assert(newFree.empty());
                    newValue = var.substitute(idMap);
                },
                [&](const auto& res)
                {
                    newValue = ValuePtr(new Value(res.substitute(idMap), move(newFree)));
                }
            }, result);
        }
        else
        {
            newValue = shared_from_this();
        }
        

        // restore the removed mappings
        for (const auto& idValue : removed)
        {
            idMap.insert(idValue);
        }

        return newValue;
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

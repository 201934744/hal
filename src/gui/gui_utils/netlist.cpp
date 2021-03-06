#include "gui_utils/netlist.h"
#include "gui/gui_globals.h"

#include "netlist/gate.h"
#include "netlist/module.h"

// TODO Consider these for moving into the core if they are useful

#include <unordered_set>

namespace hal
{
    namespace gui_utility
    {
        std::shared_ptr<Module> first_common_ancestor(std::shared_ptr<Module> m1, std::shared_ptr<Module> m2)
        {
            std::unordered_set<u32> parents_m1;
            while (m1 != nullptr)
            {
                parents_m1.insert(m1->get_id());
                m1 = m1->get_parent_module();
            }
            while (m2 != nullptr)
            {
                if (parents_m1.find(m2->get_id()) != parents_m1.end())
                {
                    return m2;
                }
                m2 = m2->get_parent_module();
            }
            return nullptr;
        }

        std::shared_ptr<Module> first_common_ancestor(std::unordered_set<std::shared_ptr<Module>> modules, const std::unordered_set<std::shared_ptr<Gate>>& gates)
        {
            if (modules.empty() && gates.empty())
            {
                return nullptr;
            }
            std::unordered_set<std::shared_ptr<Module>> modules_resolved;
            // resolve all modules to their parent modules (otherwise a module could be returned as
            // its own parent)
            for (const auto& m : modules)
            {
                auto p = m->get_parent_module();
                // this happens m is the top module (which has no parents, thus we can't find
                // any common ancestors)
                if (p == nullptr)
                    return nullptr;
                modules_resolved.insert(p);
            }
            // resolve all gates to their parent modules, since we don't want to work with gates at all
            for (const auto& g : gates)
            {
                modules_resolved.insert(g->get_module());
            }
            // pick two modules and resolve them to their first common ancestor,
            auto module_list = std::vector<std::shared_ptr<Module>>(modules_resolved.begin(), modules_resolved.end());
            auto result      = module_list[0];
            for (u32 i = 1; i < module_list.size(); ++i)
            {
                // if the top module is the first common ancestor at any time, we can stop searching (early exit)
                if (result->get_parent_module() == nullptr)
                {
                    break;
                }
                result = first_common_ancestor(result, module_list[i]);
            }
            // the final module is the first common ancestor of all elements
            return result;
        }

        QSet<u32> parent_modules(std::shared_ptr<Module> m)
        {
            assert(m);
            QSet<u32> parents;
            while (m = m->get_parent_module())
            {
                parents.insert(m->get_id());
            }
            return parents;
        }

        QSet<u32> parent_modules(std::shared_ptr<Gate> g)
        {
            assert(g);
            std::shared_ptr<Module> m = g->get_module();
            QSet<u32> parents = parent_modules(m);
            parents.insert(m->get_id());
            return parents;
        }
    }    // namespace gui_utility
}

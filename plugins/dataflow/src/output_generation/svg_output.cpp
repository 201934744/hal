#include "output_generation/svg_output.h"

#include "common/grouping.h"
#include "common/netlist_abstraction.h"
#include "core/log.h"
#include "netlist/gate.h"
#include "netlist/net.h"
#include "netlist/netlist.h"

#include <fstream>
#include <iomanip>

namespace hal
{
    namespace svg_output
    {
        void save_state_to_svg(const std::shared_ptr<Grouping>& state, const std::string m_path, const std::string file_name)
        {
            nlohmann::json output_json;

            const auto& netlist_name = state->netlist_abstr.nl->get_design_name();

            output_json["creator"]                                    = "HAL";
            output_json["modules"][netlist_name]["attributes"]["top"] = 1;

            for (const auto& [group_id, group_gates] : state->gates_of_group)
            {
                const auto& group_name = std::to_string(group_id);

                output_json["modules"][netlist_name]["cells"][group_name]["type"]                        = std::to_string(group_gates.size());
                output_json["modules"][netlist_name]["cells"][group_name]["port_directions"][group_name] = "output";
                output_json["modules"][netlist_name]["cells"][group_name]["connections"][group_name] += group_id;

                for (const auto& predecessor_id : state->get_predecessor_groups_of_group(group_id))
                {
                    const auto& predecessor_group_name = std::to_string(predecessor_id);

                    output_json["modules"][netlist_name]["cells"][group_name]["port_directions"][""] = "input";
                    output_json["modules"][netlist_name]["cells"][group_name]["connections"][""] += predecessor_id;
                }
            }

            std::ofstream(m_path + file_name, std::ios_base::app) << std::setw(4) << output_json << std::endl;
        }
    }    // namespace svg_output
}    // namespace hal
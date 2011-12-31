#include <dfhack/Core.h>
#include <dfhack/Console.h>
#include <dfhack/Export.h>
#include <dfhack/PluginManager.h>

#include <dfhack/DataDefs.h>
#include <dfhack/df/world.h>
#include <dfhack/df/ui.h>
#include <dfhack/df/building_stockpilest.h>
#include <dfhack/df/selection_rect.h>
#include <dfhack/df/viewscreen_dwarfmodest.h>

using std::vector;
using std::string;
using std::endl;
using namespace DFHack;
using namespace df::enums;

using df::global::world;
using df::global::ui;
using df::global::selection_rect;

using df::building_stockpilest;

static command_result copystock(Core *c, vector <string> & parameters);
static bool copystock_guard(Core *c, df::viewscreen *top);

DFhackCExport const char * plugin_name ( void )
{
    return "stockpiles";
}

DFhackCExport command_result plugin_init (Core *c, std::vector <PluginCommand> &commands)
{
    commands.clear();
    if (world && ui) {
        commands.push_back(
            PluginCommand(
                "copystock", "Copy stockpile under cursor.", copystock,
                copystock_guard, &df::viewscreen_dwarfmodest::_identity
            )
        );
    }
    std::cerr << "world: " << sizeof(df::world) << " ui: " << sizeof(df::ui)
              << " b_stock: " << sizeof(building_stockpilest) << endl;
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown ( Core * c )
{
    return CR_OK;
}

static bool copystock_guard(Core *c, df::viewscreen *)
{
    using namespace ui_sidebar_mode;

    switch (ui->main.mode) {
    case Stockpiles:
        return true;
    case BuildingItems:
    case QueryBuilding:
        return !!virtual_cast<building_stockpilest>(world->selected_building);
    default:
        return false;
    }    
}

static command_result copystock(Core * c, vector <string> & parameters)
{
    /* HOTKEY COMMAND: CORE ALREADY SUSPENDED */

    // For convenience: when used in the stockpiles mode, switch to 'q'
    if (ui->main.mode == ui_sidebar_mode::Stockpiles) {
        world->selected_building = NULL; // just in case it contains some kind of garbage
        ui->main.mode = ui_sidebar_mode::QueryBuilding;
        selection_rect->start_x = -30000;

        c->con << "Switched back to query building." << endl;
        return CR_OK;
    }

    building_stockpilest *sp = virtual_cast<building_stockpilest>(world->selected_building);
    if (!sp) {
        c->con.printerr("Selected building isn't a stockpile.\n");
        return CR_FAILURE;
    }

    ui->stockpile.custom_settings = sp->settings;
    ui->main.mode = ui_sidebar_mode::Stockpiles;
    world->selected_stockpile_type = stockpile_category::Custom;

    c->con << "Stockpile options copied." << endl;
    return CR_OK;
}
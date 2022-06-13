#ifndef ECS_H
#define ECS_H

//--------------------------------------------------------------
// HEADER FILES
//--------------------------------------------------------------

//
// SYSTEM
//
#define NOMINMAX
#include "Windows.h"

#include <iostream>
#include <vector>
#include <array>
#include <functional>

//
// EXTERNAL DEPENDENCIES
//
#include "../dependencies/xcore/src/xcore.h"

//--------------------------------------------------------------
// PREDEFINITIONS
//--------------------------------------------------------------
namespace ecs::game_mgr
{
    struct instance;
}

//--------------------------------------------------------------
// FILES
//--------------------------------------------------------------
#include "settings.h"
#include "event.h"
#include "component.h"
#include "entity.h"
#include "tools.h"
#include "pool.h"
#include "archetype.h"
#include "query.h"
#include "system.h"
#include "Game_mgr.h"

//--------------------------------------------------------------
// INLINE FILES
//--------------------------------------------------------------
#include "details/component_inline.h"
#include "details/system_inline.h"
#include "details/pool_inline.h"
#include "details/archetype_inline.h"
#include "details/Game_mgr_inline.h"
#include "details/query_inline.h"
#include "details/tools_inline.h"
#include "details/event_inline.h"

#endif
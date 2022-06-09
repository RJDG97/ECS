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
//#include "xecs_event.h"
#include "component.h"
#include "entity.h"
#include "tools.h"
#include "pool.h"
#include "archetype.h"
//#include "xecs_query.h"
#include "system.h"
//#include "xecs_Game_mgr.h"

//--------------------------------------------------------------
// INLINE FILES
//--------------------------------------------------------------
#include "details/component_inline.h"
#include "details/system_inline.h"
#include "details/pool_inline.h"
#include "details/archetype_inline.h"
//#include "details/xecs_Game_mgr_inline.h"
//#include "details/xecs_query_inline.h"
#include "details/tools_inline.h"
//#include "details/xecs_event_inline.h"

#endif
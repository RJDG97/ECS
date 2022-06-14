/******************************************************************************
filename: settings.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains settings for the ecs system
******************************************************************************/

namespace ecs::settings
{
    constexpr auto max_component_types_v            = 128;
    constexpr auto max_components_per_entity_v      = 32;
    constexpr auto max_entity_count_per_pool_v      = 2000;
    constexpr auto max_entities_v                   = 100000;
    constexpr auto virtual_page_size_v              = 4096;
}

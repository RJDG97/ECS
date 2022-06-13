#pragma once
//---------------------------------------------------------------------------------------
// COMPONENTS
//---------------------------------------------------------------------------------------

struct position
{
    xcore::vector2 m_Value;
};

struct velocity
{
    xcore::vector2 m_Value;
};

struct timer
{
    float m_Value{};
};

struct bullet
{
    ecs::entity::instance m_ShipOwner;
};
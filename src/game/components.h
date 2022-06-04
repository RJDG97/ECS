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
    xecs::component::entity m_ShipOwner;
};
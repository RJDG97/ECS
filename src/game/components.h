/******************************************************************************
filename: components.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains game component definitions
******************************************************************************/

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
#pragma once

#define GLUT_STATIC_LIB
#include "components.h"
#include "systems.h"
#include "GL/glut.h"
#include <random>

//---------------------------------------------------------------------------------------
void InitializeGame(void) noexcept
{
    // Register Components
    s_AsteroidGame.m_GameMgr->RegisterComponents
        < 
          position
        , velocity
        , timer
        , bullet
        >();
    // Register Systems
    s_AsteroidGame.m_GameMgr->RegisterSystems
        <   
          update_movement         // Structural: No
        , space_ship_logic        // Structural: Can Create Bullets
        , bullet_logic            // Structural: Can Destroy Bullets and Ships
        , render_ships            // Structural: No
        , render_bullets          // Structural: No
        , page_flip               // Structural: No
         >();

    // Game Initialization

    // Generate a few random ships
    auto& SpaceShipArchetype = s_AsteroidGame.m_GameMgr->getOrCreateArchetype< position, velocity, timer >();
    for (int i = 0; i < 1000; i++)
    {
        SpaceShipArchetype.CreateEntity([&](position& Position, velocity& Velocity, timer& Timer)
            {
                Position.m_Value.m_X = std::rand() % s_AsteroidGame.m_Width;
                Position.m_Value.m_Y = std::rand() % s_AsteroidGame.m_Height;

                Velocity.m_Value.m_X = (std::rand() / (float)RAND_MAX) - 0.5f;
                Velocity.m_Value.m_Y = (std::rand() / (float)RAND_MAX) - 0.5f;
                Velocity.m_Value.Normalize();

                Timer.m_Value = (std::rand() / (float)RAND_MAX) * 8;
            });
    }
}
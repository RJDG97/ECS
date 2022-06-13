#pragma once

#include "GL/glut.h"

//--------------------------------------------------------------
// PREDEFINITIONS
//--------------------------------------------------------------
namespace ecs::game_mgr
{
    struct instance;
}

//---------------------------------------------------------------------------------------
// GAME
//---------------------------------------------------------------------------------------
static struct game
{
    std::unique_ptr<ecs::game_mgr::instance> m_GameMgr = std::make_unique<ecs::game_mgr::instance>();
    int m_Width = 1024;
    int m_Height = 800;

} s_AsteroidGame;

//---------------------------------------------------------------------------------------
// SYSTEM
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

struct update_movement : ecs::system::instance
{
    constexpr static auto   name_v = "update_movement";

    void operator()(position& Position, velocity& Velocity) const noexcept
    {
        Position.m_Value += Velocity.m_Value;

        // Bounce on edges
        if (Position.m_Value.m_X < 0)
        {
            Position.m_Value.m_X = 0;
            Velocity.m_Value.m_X = -Velocity.m_Value.m_X;
        } 
        else if (Position.m_Value.m_X >= s_AsteroidGame.m_Width)
        {
            Position.m_Value.m_X = s_AsteroidGame.m_Width - 1;
            Velocity.m_Value.m_X = -Velocity.m_Value.m_X;
        }

        if (Position.m_Value.m_Y < 0)
        {
            Position.m_Value.m_Y = 0;
            Velocity.m_Value.m_Y = -Velocity.m_Value.m_Y;
        }
        else if (Position.m_Value.m_Y >= s_AsteroidGame.m_Height)
        {
            Position.m_Value.m_Y = s_AsteroidGame.m_Height - 1;
            Velocity.m_Value.m_Y = -Velocity.m_Value.m_Y;
        }
    }
};

//---------------------------------------------------------------------------------------

struct bullet_logic : ecs::system::instance
{
    constexpr static auto   name_v = "bullet_logic";

    void operator()(entity& Entity, position& Position, timer& Timer, bullet& Bullet) const noexcept
    {
        // If I am dead because some other bullet killed me then there is nothing for me to do...
        if (Entity.isZombie()) return;

        // Update my timer
        Timer.m_Value -= 0.01f;
        if (Timer.m_Value <= 0)
        {
            m_GameMgr.DeleteEntity(Entity);
            return;
        }

        // Check for collisions
        ecs::query::instance Query;
        Query.m_Must.AddFromComponents<position>();

        m_GameMgr.Foreach(m_GameMgr.Search(Query), [&](entity& E, position& Pos) noexcept -> bool
        {
            assert(Entity.isZombie() == false);

            // Our we checking against my self?
            if (&Entity == &E) return false;

            // Is this bullet or ship already dead?
            if (E.isZombie()) return false;

            // Are we colliding with our own ship?
            // If so lets just continue
            if (Bullet.m_ShipOwner.m_Value == E.m_Value) return false;

            constexpr auto distance_v = 3;
            if ((Pos.m_Value - Position.m_Value).getLengthSquared() < distance_v * distance_v)
            {
                m_GameMgr.DeleteEntity(Entity);
                m_GameMgr.DeleteEntity(E);
                return true;
            }

            return false;
        });
    }
};

//---------------------------------------------------------------------------------------

struct space_ship_logic : ecs::system::instance
{
    constexpr static auto   name_v = "space_ship_logic";

    using query = std::tuple
        <
            ecs::query::none_of<bullet>
        >;

    void operator()(entity& Entity, position& Position, timer& Time) const noexcept
    {
        if (Time.m_Value > 0)
        {
            Time.m_Value -= 0.01f;
            return;
        }

        ecs::query::instance    Query;
        Query.m_NoneOf.AddFromComponents<bullet>();
        m_GameMgr.Foreach
        (m_GameMgr.Search(Query)
            , [&](position& Pos) noexcept -> bool
            {
                // Don't shoot myself
                if (&Pos == &Position) return false;

                auto        Direction = Pos.m_Value - Position.m_Value;
                const auto  DistanceSquare = Direction.getLengthSquared();

                // Shoot a bullet if close enough
                constexpr auto min_distance_v = 30;
                if (DistanceSquare < min_distance_v * min_distance_v)
                {
                    Time.m_Value = 8;
                    auto& Archetype = m_GameMgr.getOrCreateArchetype<position, velocity, timer, bullet>();
                    Archetype.CreateEntity([&](position& Pos, velocity& Vel, bullet& Bullet, timer& Timer)
                        {
                            Direction /= std::sqrt(DistanceSquare);
                            Vel.m_Value = Direction * 2.0f;
                            Pos.m_Value = Position.m_Value + Vel.m_Value;

                            Bullet.m_ShipOwner = Entity;
                            Timer.m_Value = 10;
                        });
                    return true;
                }
                return false;
            });
    }
};

//---------------------------------------------------------------------------------------

struct render_bullets : ecs::system::instance
{
    constexpr static auto   name_v = "render_bullets";

    using query = std::tuple
        <
            ecs::query::must<bullet>
        >;

    void operator()(position& Position, velocity& Velocity) const noexcept
    {
        constexpr auto SizeX = 1;
        constexpr auto SizeY = SizeX * 3;
        glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.5, 0.0);
        glVertex2i(Position.m_Value.m_X + Velocity.m_Value.m_X * SizeY, Position.m_Value.m_Y + Velocity.m_Value.m_Y * SizeY);
        glVertex2i(Position.m_Value.m_X + Velocity.m_Value.m_Y * SizeX, Position.m_Value.m_Y - Velocity.m_Value.m_X * SizeX);
        glVertex2i(Position.m_Value.m_X - Velocity.m_Value.m_Y * SizeX, Position.m_Value.m_Y + Velocity.m_Value.m_X * SizeX);
        glEnd();
    }
};

//---------------------------------------------------------------------------------------

struct render_ships : ecs::system::instance
{
    constexpr static auto   name_v = "render_ships";

    using query = std::tuple
        <
            ecs::query::none_of<bullet>
        >;

    void operator()(position& Position, timer& Timer) const noexcept
    {
        constexpr auto Size = 3;
        glBegin(GL_QUADS);
        if (Timer.m_Value > 0) glColor3f(1.0, 1.0, 1.0);
        else                   glColor3f(0.5, 1.0, 0.5);
        glVertex2i(Position.m_Value.m_X - Size, Position.m_Value.m_Y - Size);
        glVertex2i(Position.m_Value.m_X - Size, Position.m_Value.m_Y + Size);
        glVertex2i(Position.m_Value.m_X + Size, Position.m_Value.m_Y + Size);
        glVertex2i(Position.m_Value.m_X + Size, Position.m_Value.m_Y - Size);
        glEnd();
    }
};

//---------------------------------------------------------------------------------------

struct page_flip : ecs::system::instance
{
    constexpr static auto   name_v = "page_flip";

    __inline
        void OnUpdate(void) noexcept
    {
        glutSwapBuffers();
        glClear(GL_COLOR_BUFFER_BIT);
    }
};
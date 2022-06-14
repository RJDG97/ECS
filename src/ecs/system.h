/******************************************************************************
filename: system.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains declerations for systems
******************************************************************************/

namespace ecs::system
{
    //-----------------------------------------------------------------
    // ECS USER DEFINITIONS
    //-----------------------------------------------------------------
    struct overrides
    {
        using query = std::tuple<>;
        constexpr static auto name_v = "unnamed system";

        void OnUpdate( void ) noexcept {}
    };

    struct instance : overrides
    {
        using entity = ecs::entity::instance;

        instance( const instance& ) noexcept = delete;

        inline      
        instance(ecs::game_mgr::instance& GameMgr) noexcept
            : m_GameMgr(GameMgr) {}

        ecs::game_mgr::instance& m_GameMgr;
    };

    template<typename T>
    concept T_SYSTEMS = requires(T)
    { (std::derived_from< T, ecs::system::instance>); };

    //-----------------------------------------------------------------
    // ECS SYSTEM DEFINITIONS
    //-----------------------------------------------------------------
    class mgr final
    {
    public:
        mgr( const mgr& ) noexcept = delete;

        mgr( void ) noexcept = default;

        template < typename T_SYSTEMS >
        void RegisterSystem( ecs::game_mgr::instance& GameMgr ) noexcept;

        inline 
        void Run( void) noexcept;

    private:

        struct info
        {
            using call_run = void(ecs::system::instance&);

            std::unique_ptr<ecs::system::instance> m_System;
            call_run* m_CallRun;
            const char* m_pName;
        };

        std::vector< info > m_Systems;
    };
}
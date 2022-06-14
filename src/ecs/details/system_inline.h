/******************************************************************************
filename: system_inline.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains definitions for systems
******************************************************************************/

namespace ecs::system
{
    namespace details 
    {
        template< typename T_SYSTEMS >
        struct completed final : T_SYSTEMS
        {
            __inline
            completed(ecs::game_mgr::instance& GameMgr) noexcept
            : T_SYSTEMS{ GameMgr }
            {}

            completed( void ) noexcept = delete;

            __inline
            void Run( void ) noexcept
            {
                if constexpr (&T_SYSTEMS::OnUpdate != &instance::OnUpdate)
                    T_SYSTEMS::OnUpdate();
                else
                {
                    ecs::query::instance Query;
                    Query.AddQueryFromTuple(xcore::types::null_tuple_v< T_SYSTEMS::query >);
                    Query.AddQueryFromFunction(*this);
                    T_SYSTEMS::m_GameMgr.Foreach(T_SYSTEMS::m_GameMgr.Search(Query), *this);
                }
            }
        };
    }

    //-------------------------------------------------------------------------------------------
    template < typename T_SYSTEMS >
    void mgr::RegisterSystem( ecs::game_mgr::instance& GameMgr ) noexcept
    { 
        // Add System onto Systems list
        m_Systems.push_back
        (   // Initialize System
            info
            {
                .m_System = std::make_unique< details::completed<T_SYSTEMS> >(GameMgr),

                .m_CallRun = []( ecs::system::instance& System ) noexcept
                { static_cast<details::completed<T_SYSTEMS>&>(System).Run(); }

            ,   .m_pName = T_SYSTEMS::name_v
            }
        );
    }

    //---------------------------------------------------------------------------

    void mgr::Run( void ) noexcept
    {
        for( const auto& S : m_Systems )
            S.m_CallRun( *S.m_System.get() );
    }
}
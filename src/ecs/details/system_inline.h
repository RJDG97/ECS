namespace ecs::system
{
    namespace details 
    {
        template< typename T_USER_SYSTEM > 
        // Template needs to be a system instance
            requires( std::derived_from< T_USER_SYSTEM, ecs::system::instance > )
        struct completed final : T_USER_SYSTEM
        {
            __inline
            completed(ecs::game_mgr::instance& GameMgr) noexcept
            : T_USER_SYSTEM{ GameMgr }
            {}

            completed( void ) noexcept = delete;

            __inline
            void Run( void ) noexcept
            {
                // XCORE_PERF_ZONE_SCOPED_N(T_USER_SYSTEM::name_v)
                if constexpr (&T_USER_SYSTEM::OnUpdate != &instance::OnUpdate)
                {
                    T_USER_SYSTEM::OnUpdate();
                }
                //else
                //{
                //    xecs::query::instance Query;
                //    Query.AddQueryFromTuple(xcore::types::null_tuple_v< T_USER_SYSTEM::query >);
                //    Query.AddQueryFromFunction(*this);
                //    T_USER_SYSTEM::m_GameMgr.Foreach(T_USER_SYSTEM::m_GameMgr.Search(Query), *this);
                //}
            }
        };
    }

    //-------------------------------------------------------------------------------------------
    template < typename T_SYSTEM >
        requires( std::derived_from< T_SYSTEM, ecs::system::instance> )
    void mgr::RegisterSystem( ecs::game_mgr::instance& GameMgr ) noexcept
    { 
        // Add System onto Systems list
        m_Systems.push_back
        (   // Initialize System
            info
            {
                .m_System   = std::make_unique< details::completed<T_SYSTEM> >(GameMgr),

                .m_CallRun  = []( ecs::system::instance& System ) noexcept
                { static_cast<details::completed<T_SYSTEM>&>(System).Run(); }

            ,   .m_pName    = T_SYSTEM::name_v
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
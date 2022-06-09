namespace ecs::archetype
{
    struct instance final
    {
        instance(const instance&) = delete;

        //---------------------------------------------------------------------------

        inline 
        instance(ecs::game_mgr::instance& GameMgr) noexcept
            : m_GameMgr{ GameMgr } {}

        //---------------------------------------------------------------------------
        
        inline 
        void Initialize              
        (
            std::span<const ecs::component::info* const> Infos, 
            const tools::bits&                           Bits 
        ) noexcept;

        //---------------------------------------------------------------------------
        
        template < typename T_CALLBACK = ecs::tools::empty_lambda > 
            requires( 
                        xcore::function::is_callable_v<T_CALLBACK> && 
                        std::is_same_v<typename xcore::function::traits<T_CALLBACK>::return_type, void>
                    ) __inline
        ecs::entity::instance CreateEntity (T_CALLBACK&& Function = ecs::tools::empty_lambda{}) noexcept;

        //---------------------------------------------------------------------------
        
        inline 
        void DestroyEntity(ecs::entity::instance& Entity) noexcept;

        //---------------------------------------------------------------------------

        template < typename T_FUNCTION >
            requires (
                     xcore::function::is_callable_v<T_FUNCTION>
                     && std::is_same_v<typename xcore::function::traits<T_FUNCTION>::return_type, void>
                     && xcore::function::traits<T_FUNCTION>::arg_count_v == 0
                     ) __inline
        void AccessGuard(T_FUNCTION&& Function) noexcept;

        ////---------------------------------------------------------------------------
        //
        //inline 
        //void UpdateStructuralChanges (void) noexcept;

        //---------------------------------------------------------------------------

        using info_array = std::array<const ecs::component::info*, ecs::settings::max_components_per_entity_v >;

        ecs::game_mgr::instance&            m_GameMgr;
        ecs::tools::bits                    m_ComponentBits     {};
        ecs::component::pool::instance      m_Pool              {};
        std::vector<ecs::entity::instance>  m_ToDelete          {};
        int                                 m_ProcessReference  { 0 };
        info_array                          m_InfoData          {};
    };
}

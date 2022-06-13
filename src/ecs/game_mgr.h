namespace ecs::game_mgr
{
    //---------------------------------------------------------------------------

    struct entity_info final
    {
        archetype::instance*            m_pArchetype    {};
        int                             m_PoolIndex     {-1};
        entity::instance::validation   m_Validation    {};
    };

    //---------------------------------------------------------------------------

    struct instance final
    {
        instance( const instance& ) = delete;

        inline 
        instance( void ) noexcept;


        //-----------------------------------
        //  Register Systems and Components
        //-----------------------------------
        template < typename...T_SYSTEMS> 
            requires ( std::derived_from< T_SYSTEMS, ecs::system::instance> && ...)
        void RegisterSystems(void) noexcept;

        //---------------------------------------------------------------------------

        template < typename...T_COMPONENTS> 
            requires ( ((sizeof(T_COMPONENTS) <= ecs::settings::virtual_page_size_v)&& ... ))
        void RegisterComponents(void) noexcept;

        //-----------------------------------
        //              Entity
        //-----------------------------------

        inline 
        ecs::entity::instance AllocNewEntity
        ( 
            int PoolIndex, 
            ecs::archetype::instance& Archetype 
        ) noexcept;

        //---------------------------------------------------------------------------

        inline 
        const entity_info& getEntityDetails
        ( ecs::entity::instance Entity ) const noexcept;

        //---------------------------------------------------------------------------

        inline 
        void DeleteEntity
        ( ecs::entity::instance& Entity ) noexcept;

        //---------------------------------------------------------------------------

        inline void SystemDeleteEntity
        (   
            ecs::entity::instance  DeletedEntity, 
            ecs::entity::instance& SwappedEntity
        ) noexcept;

        //---------------------------------------------------------------------------

        inline void SystemDeleteEntity
        ( ecs::entity::instance DeletedEntity ) noexcept;

        //---------------------------------------------------------------------------

        template < typename T_FUNCTION = ecs::tools::empty_lambda> 
            requires ( xcore::function::is_callable_v<T_FUNCTION> ) __inline
        bool findEntity
        (   
            ecs::entity::instance Entity,
            T_FUNCTION&& Function = ecs::tools::empty_lambda{}
        ) const noexcept;

        //---------------------------------------------------------------------------

        template < typename... T_COMPONENTS >
        std::vector<archetype::instance*> Search
        (const ecs::query::instance& Query) const noexcept;

        //---------------------------------------------------------------------------

        template< typename... T_COMPONENTS >
        std::vector<archetype::instance*> Search
        (std::span<const component::info* const> Types) const noexcept;

        //---------------------------------------------------------------------------

        template < typename... T_COMPONENTS >
        std::vector<archetype::instance*> Search(void) const noexcept;

        //---------------------------------------------------------------------------

        inline
        archetype::instance& getOrCreateArchetype
        ( std::span<const component::info* const> Types ) noexcept;

        //---------------------------------------------------------------------------

        template < typename... T_COMPONENTS >
        archetype::instance& getOrCreateArchetype( void ) noexcept;

        //---------------------------------------------------------------------------

        template< typename T_FUNCTION > requires
        (   
            xcore::function::is_callable_v<T_FUNCTION> && std::is_same_v< bool, 
            typename xcore::function::traits<T_FUNCTION>::return_type >
        ) __inline
        void Foreach 
        ( 
            const std::vector<ecs::archetype::instance*>& List, 
            T_FUNCTION&& Function 
        ) const noexcept;

        //---------------------------------------------------------------------------

        template< typename T_FUNCTION> requires
        ( 
            xcore::function::is_callable_v<T_FUNCTION> && std::is_same_v< void, 
            typename xcore::function::traits<T_FUNCTION>::return_type >
        ) __inline
        void Foreach                 
        ( 
            const std::vector<ecs::archetype::instance*>& List, 
            T_FUNCTION&& Function 
        ) const noexcept;

        //---------------------------------------------------------------------------

        inline
        void Run( void ) noexcept;

        //---------------------------------------------------------------------------

        ecs::system::mgr                                    m_SystemMgr         {};
        ecs::component::mgr                                 m_ComponentMgr      {};
        std::vector<std::unique_ptr<archetype::instance>>   m_lArchetype        {};
        std::vector<tools::bits>                            m_lArchetypeBits    {};
        std::unique_ptr<entity_info[]>                      m_lEntities         = std::make_unique<entity_info[]>(ecs::settings::max_entities_v);
        int                                                 m_EmptyHead         = 0;
    };
}
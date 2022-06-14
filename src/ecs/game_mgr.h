/******************************************************************************
filename: game_mgr.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
  This file contains declerations for the ecs game manager 
 including registry of components and systems and entity management
******************************************************************************/

namespace ecs::game_mgr
{
    template<typename T>
    concept T_FUNCTIONS = requires(T)
    { xcore::function::is_callable_v<T>; };

    template<typename... T>
    concept T_COMPONENTS = requires(T)
    { ((sizeof(T) <= ecs::settings::virtual_page_size_v) && ...); };

    template<typename... T>
    concept T_SYSTEMS = requires(T)
    { (std::derived_from< T, ecs::system::instance> && ...); };

    //---------------------------------------------------------------------------
    // INFO
    //---------------------------------------------------------------------------

    struct entity_info final
    {
        archetype::instance* m_pArchetype;
        int m_PoolIndex {-1};
        entity::instance::validation m_Validation;
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
        void RegisterSystems(void) noexcept;

        //---------------------------------------------------------------------------

        template < typename...T_COMPONENTS> 
        void RegisterComponents(void) noexcept;

        //-----------------------------------
        //              Entity
        //-----------------------------------

        inline 
        ecs::entity::instance AllocNewEntity
        ( int PoolIndex,  ecs::archetype::instance& Archetype) noexcept;

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
        ( ecs::entity::instance  DeletedEntity,ecs::entity::instance& SwappedEntity) noexcept;

        //---------------------------------------------------------------------------

        inline void SystemDeleteEntity
        ( ecs::entity::instance DeletedEntity ) noexcept;

        //---------------------------------------------------------------------------

        template < typename T_FUNCTIONS = ecs::tools::empty_lambda>
        bool findEntity
        ( ecs::entity::instance Entity, T_FUNCTIONS&& Function = ecs::tools::empty_lambda{}) const noexcept;

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

        template< typename T_FUNCTIONS >
            requires ( std::is_same_v< bool, typename xcore::function::traits<T_FUNCTIONS>::return_type > ) __inline
        void Foreach 
        (const std::vector<ecs::archetype::instance*>& List, T_FUNCTIONS&& Function) const noexcept;

        //---------------------------------------------------------------------------

        template< typename T_FUNCTIONS>
            requires ( std::is_same_v< void, typename xcore::function::traits<T_FUNCTIONS>::return_type > ) __inline
        void Foreach 
        (const std::vector<ecs::archetype::instance*>& List, T_FUNCTIONS&& Function) const noexcept;

        //---------------------------------------------------------------------------

        inline
        void Run( void ) noexcept;

        //---------------------------------------------------------------------------

        ecs::system::mgr m_SystemMgr{};
        ecs::component::mgr m_ComponentMgr{};
        std::vector<std::unique_ptr<archetype::instance>> m_lArchetype{};
        std::vector<tools::bits> m_lArchetypeBits{};
        std::unique_ptr<entity_info[]> m_lEntities = std::make_unique<entity_info[]>(ecs::settings::max_entities_v);
        int m_EmptyHead = 0;
    };
}
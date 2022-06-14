/******************************************************************************
filename: game_mgr_inline.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains definitions for the ecs game manager 
 including registry of components and systems and entity management
******************************************************************************/

namespace ecs::game_mgr
{
    //---------------------------------------------------------------------------

    instance::instance( void ) noexcept
    {
        // Register the Entity
        m_ComponentMgr.RegisterComponent<ecs::entity::instance>();

        // Create a link list of empty entries
        for( int i=0; i<(ecs::settings::max_entities_v - 2); i++ )
            m_lEntities[i].m_PoolIndex = i+1;
    }

    //---------------------------------------------------------------------------

    template<typename...T_SYSTEMS> // Ensure args are systems
    void instance::RegisterSystems() noexcept
    {
        (m_SystemMgr.RegisterSystem<T_SYSTEMS>(*this), ... );
    }

    //---------------------------------------------------------------------------

    template< typename...T_COMPONENTS >// Ensure args are components
    void instance::RegisterComponents(void) noexcept
    {
        ((m_ComponentMgr.RegisterComponent<T_COMPONENTS>()), ...);
    }

    //---------------------------------------------------------------------------

    ecs::entity::instance instance::AllocNewEntity( int PoolIndex, ecs::archetype::instance& Archetype ) noexcept
    {
        assert(m_EmptyHead>=0);

        auto  iEntityIndex = m_EmptyHead;
        auto& Entry = m_lEntities[iEntityIndex];
        m_EmptyHead = Entry.m_PoolIndex;

        Entry.m_PoolIndex = PoolIndex;
        Entry.m_pArchetype = &Archetype;
        return
        {
            .m_GlobalIndex = static_cast<std::uint32_t>(iEntityIndex)
        ,   .m_Validation = Entry.m_Validation
        };
    }

    //---------------------------------------------------------------------------

    const entity_info& instance::getEntityDetails(ecs::entity::instance Entity ) const noexcept
    {
        // Return entity details
        auto& Entry = m_lEntities[Entity.m_GlobalIndex];
        assert( Entry.m_Validation == Entity.m_Validation );
        return Entry;
    }

    //---------------------------------------------------------------------------

    void instance::DeleteEntity(ecs::entity::instance& Entity ) noexcept
    {
        assert(Entity.isZombie() == false);
        // Delete entity
        getEntityDetails(Entity).m_pArchetype->DestroyEntity(Entity);
    }

    //---------------------------------------------------------------------------

    void instance::SystemDeleteEntity(ecs::entity::instance DeletedEntity, ecs::entity::instance& SwappedEntity ) noexcept
    { 
        // Swap entities
        m_lEntities[SwappedEntity.m_GlobalIndex].m_PoolIndex = m_lEntities[DeletedEntity.m_GlobalIndex].m_PoolIndex;
        // Delete entity
        SystemDeleteEntity(DeletedEntity);
    }

    //---------------------------------------------------------------------------

    void instance::SystemDeleteEntity(ecs::entity::instance DeletedEntity ) noexcept
    {
        auto& Entry = m_lEntities[DeletedEntity.m_GlobalIndex];
        Entry.m_Validation.m_Generation++;
        Entry.m_Validation.m_bZombie = false;
        Entry.m_PoolIndex            = m_EmptyHead;
        m_EmptyHead = static_cast<int>(DeletedEntity.m_GlobalIndex);
    }

    //---------------------------------------------------------------------------

    template< typename T_FUNCTIONS>
    bool instance::findEntity(ecs::entity::instance Entity, T_FUNCTIONS&& Function) const noexcept
    {
        if (Entity.isZombie())
            return false;

        auto& Entry = m_lEntities[Entity.m_GlobalIndex];
        if (Entry.m_Validation == Entity.m_Validation)
        {
            if constexpr (!std::is_same_v< T_FUNCTIONS, ecs::tools::empty_lambda>)
            {
                Entry.m_pArchetype->AccessGuard([&]
                    {
                        [&] <typename... T_COMPONENTS>(std::tuple<T_COMPONENTS...>*) constexpr noexcept
                        {
                            Function(Entry.m_pArchetype->m_Pool.getComponent<std::remove_reference_t<T_COMPONENTS>>(Entry.m_PoolIndex) ...);
                        }(xcore::types::null_tuple_v<xcore::function::traits<T_FUNCTIONS>::args_tuple>);
                    });
            }
            return true;
        }
        return false;
    }

    //---------------------------------------------------------------------------

    template< typename... T_COMPONENTS >
    std::vector<archetype::instance*> instance::Search(const ecs::query::instance& Query) const noexcept
    {
        std::vector<archetype::instance*> ArchetypesFound;
        for (auto& E : m_lArchetypeBits)
        {   // Find archetypes that meet requirements
            if (Query.Compare(E))
            {
                const auto Index = static_cast<std::size_t>(&E - &m_lArchetypeBits[0]);
                ArchetypesFound.push_back(m_lArchetype[Index].get());
            }
        }

        return std::move(ArchetypesFound);
    }

    //---------------------------------------------------------------------------

    template< typename... T_COMPONENTS >
    std::vector<archetype::instance*> instance::Search(std::span<const component::info* const> Types) const noexcept
    {
        tools::bits Query;
        // Set query bits according to types
        for (const auto& pE : Types)
            Query.setBit(pE->m_UID);

        return Search(Query);
    }

    //---------------------------------------------------------------------------

    template< typename... T_COMPONENTS >
    std::vector<archetype::instance*> instance::Search(void) const noexcept
    {
        return Search(std::array{ &component::info_v<T_COMPONENTS>... });
    }

    //---------------------------------------------------------------------------

    archetype::instance& instance::getOrCreateArchetype( std::span<const component::info* const> Types ) noexcept
    {
        tools::bits Query;
        for (const auto& pE : Types)
        {
            assert(pE->m_UID != ecs::component::info::invalid_id_v );
            Query.setBit(pE->m_UID);
        }
            
        // Make sure the entity is part of the list at this point
        assert( Query.getBit(ecs::component::info_v<ecs::entity::instance>.m_UID) );
        
        for (auto& E : m_lArchetypeBits)
        {
            if ( E.Compare(Query) )
                return *m_lArchetype[static_cast<std::size_t>(&E - m_lArchetypeBits.data())];
        }

        // Create Archetype
        m_lArchetype.push_back( std::make_unique<archetype::instance>(*this) );
        m_lArchetypeBits.push_back( Query );

        auto& Archetype = *m_lArchetype.back();
        Archetype.Initialize(Types, Query);

        return Archetype;
    }

    //---------------------------------------------------------------------------

    template< typename... T_COMPONENTS >
    archetype::instance& instance::getOrCreateArchetype( void ) noexcept
    {
        static_assert( ((std::is_same_v<T_COMPONENTS, ecs::entity::instance> == false ) && ...) );
        static constexpr auto ComponentList = std::array{ &component::info_v<ecs::entity::instance>, &component::info_v<T_COMPONENTS>... };
        return getOrCreateArchetype( ComponentList );
    }

    //---------------------------------------------------------------------------

    template< typename T_FUNCTIONS > requires
    ( std::is_same_v< bool, typename xcore::function::traits<T_FUNCTIONS>::return_type > )
    void instance::Foreach( const std::vector<ecs::archetype::instance*>& List, T_FUNCTIONS&& Function ) const noexcept
    {
        using func_traits = xcore::function::traits<T_FUNCTIONS>;
        // For each entity
        for( const auto& pE : List )
        {
            const auto& Pool = pE->m_Pool;
            auto CachePointers = [&]<typename...T_COMPONENTS>(std::tuple<T_COMPONENTS...>*) constexpr noexcept
            {
                return std::array
                {
                    [&]<typename T_C>(std::tuple<T_C>*) constexpr noexcept
                    {
                        const auto I = Pool.findIndexComponentFromUIDComponent(ecs::component::info_v<T_C>.m_UID);

                        if constexpr (std::is_pointer_v<T_C>) 
                            return (I < 0) ? nullptr : Pool.m_pComponent[I];
                        else                                  
                            return Pool.m_pComponent[I];
                    }( xcore::types::make_null_tuple_v<T_COMPONENTS> )
                    ...
                };
            }( xcore::types::null_tuple_v<func_traits::args_tuple> );

            bool bBreak = false;
            pE->AccessGuard([&]
            {
                for( int i=Pool.Size(); i; i-- )
                {
                    if( [&]<typename... T_COMPONENTS>(std::tuple<T_COMPONENTS...>*) constexpr noexcept
                    {
                        return Function([&]<typename T>(std::tuple<T>*) constexpr noexcept -> T
                        {
                            auto& MyP = CachePointers[xcore::types::tuple_t2i_v<T, typename func_traits::args_tuple>];

                            if constexpr (std::is_pointer_v<T>)
                                if (MyP == nullptr)
                                    return reinterpret_cast<T>(nullptr);

                            auto p = MyP;                     // Back up the pointer
                            MyP += sizeof(std::decay_t<T>); // Get ready for the next entity

                            if constexpr (std::is_pointer_v<T>) 
                                return reinterpret_cast<T>(p);
                            else                                
                                return reinterpret_cast<T>(*p);
                        }( xcore::types::make_null_tuple_v<T_COMPONENTS> ) 
                        ...);
                    }( xcore::types::null_tuple_v<func_traits::args_tuple> ) ) 
                    {
                        bBreak = true;
                        break;
                    }
                }
            });
            if(bBreak) break;
        }
    }

    //---------------------------------------------------------------------------
    template< typename T_FUNCTIONS > requires
    ( std::is_same_v< void, typename xcore::function::traits<T_FUNCTIONS>::return_type > )
    void instance::Foreach( const std::vector<ecs::archetype::instance*>& List, T_FUNCTIONS&& Function ) const noexcept
    {
        using func_traits = xcore::function::traits<T_FUNCTIONS>;
        
        for( const auto& pE : List )
        {
            const auto& Pool = pE->m_Pool;
            auto CachePointers = [&]<typename...T_COMPONENTS>(std::tuple<T_COMPONENTS...>*) constexpr noexcept
            {
                return std::array
                {
                    [&]<typename T_C>(std::tuple<T_C>*) constexpr noexcept
                    {
                        const auto I = Pool.findIndexComponentFromUIDComponent(ecs::component::info_v<T_C>.m_UID);

                        if constexpr (std::is_pointer_v<T_C>) 
                            return (I < 0) ? nullptr : Pool.m_pComponent[I];
                        else                                  
                            return Pool.m_pComponent[I];
                    }( xcore::types::make_null_tuple_v<T_COMPONENTS> )
                    ...
                };
            }( xcore::types::null_tuple_v<func_traits::args_tuple> );

            pE->AccessGuard([&]
            {
                for( int i=Pool.Size(); i; i-- )
                {
                    [&]<typename... T_COMPONENTS>(std::tuple<T_COMPONENTS...>*) constexpr noexcept
                    {
                        Function([&]<typename T>(std::tuple<T>*) constexpr noexcept -> T
                        {
                            auto& MyP = CachePointers[xcore::types::tuple_t2i_v<T, typename func_traits::args_tuple>];

                            if constexpr (std::is_pointer_v<T>) 
                                if (MyP == nullptr) 
                                    return reinterpret_cast<T>(nullptr);

                            auto p = MyP;                   // Back up the pointer
                            MyP += sizeof(std::decay_t<T>); // Get ready for the next entity

                            if constexpr (std::is_pointer_v<T>) 
                                return reinterpret_cast<T>(p);
                            else                                
                                return reinterpret_cast<T>(*p);
                        }( xcore::types::make_null_tuple_v<T_COMPONENTS> ) 
                        ...);
                    }( xcore::types::null_tuple_v<func_traits::args_tuple> );
                }
            });
        }
    }

    //---------------------------------------------------------------------------

    void instance::Run( void ) noexcept
    {
        XCORE_PERF_FRAME_MARK()
        XCORE_PERF_FRAME_MARK_START("ecs::Frame")

        // Run systems
        m_SystemMgr.Run();

        XCORE_PERF_FRAME_MARK_END("ecs::Frame")
    }
}
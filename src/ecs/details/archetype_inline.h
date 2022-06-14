/******************************************************************************
filename: archetype_inline.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains Archetype-Entity manager definitions
******************************************************************************/

namespace ecs::archetype
{
    //--------------------------------------------------------------------------------------------

    void instance::Initialize
    ( std::span<const ecs::component::info* const> Infos, const tools::bits& Bits ) noexcept
    {
        // Deep copy the infos just in case the user gave us data driven infos
        for( int i=0; i<Infos.size(); i++ )
            m_InfoData[i] = Infos[i];
    
        m_Pool.Initialize( { m_InfoData.data(), Infos.size() } );
        m_ComponentBits = Bits;
    }

    //--------------------------------------------------------------------------------------------

    template < typename T_FUNCTIONS >
    ecs::entity::instance instance::CreateEntity(T_FUNCTIONS&& Function ) noexcept
    {
        using func_traits = xcore::function::traits<T_FUNCTIONS>;

        return[&]< typename... T_COMPONENTS >(std::tuple<T_COMPONENTS...>*) constexpr noexcept
        {
            assert(m_ComponentBits.getBit(component::info_v<T_COMPONENTS>.m_UID) && ...);

            // Allocate the entity component
            const int   EntityIndexInPool = m_Pool.Append();
            const auto  Entity = m_GameMgr.AllocNewEntity(EntityIndexInPool, *this);
            m_Pool.getComponent<ecs::entity::instance>(EntityIndexInPool) = Entity;

            // Call the user initializer if any
            if constexpr (false == std::is_same_v<ecs::tools::empty_lambda, T_FUNCTIONS >)
                Function(m_Pool.getComponent<std::remove_reference_t<T_COMPONENTS>>(EntityIndexInPool) ...);

            return Entity;
        }( xcore::types::null_tuple_v<func_traits::args_tuple> );
    }

    //--------------------------------------------------------------------------------------------

    void instance::DestroyEntity(ecs::entity::instance& Entity) noexcept
    {
        // Check if Entity was already destroyed
        assert( Entity.isZombie() == false );
        // Mark for destruction
        Entity.m_Validation.m_bZombie = true;
        m_GameMgr.m_lEntities[Entity.m_GlobalIndex].m_Validation.m_bZombie = true;
            
        // Add to delete list
        m_lToDelete.push_back(Entity);
        // Delete items on list if no processes are running
        if( 0 == m_ProcessReference ) UpdateStructuralChanges();
    }

    //--------------------------------------------------------------------------------------------
     
    template< typename T_FUNCTIONS >
    requires ( xcore::function::traits<T_FUNCTIONS>::arg_count_v == 0 )
    void instance::AccessGuard(T_FUNCTIONS&& Function ) noexcept
    {
        m_ProcessReference++;
        Function();
        if(--m_ProcessReference == 0 ) UpdateStructuralChanges();
    }

    //--------------------------------------------------------------------------------------------

    void instance::UpdateStructuralChanges( void ) noexcept
    { 
        // Check if items are to be deleted
        if(m_lToDelete.size() )
        {   // Delete each entity in the list
            for( auto& Entity : m_lToDelete)
            {
                auto& EntityDetails = m_GameMgr.getEntityDetails(Entity);
                // Check if deleting an archetype
                assert(EntityDetails.m_pArchetype == this);

                // Free Entity
                m_Pool.Delete(EntityDetails.m_PoolIndex);
                // Check if last entity
                if(EntityDetails.m_PoolIndex != m_Pool.Size())
                    m_GameMgr.SystemDeleteEntity(Entity, m_Pool.getComponent<ecs::entity::instance>(EntityDetails.m_PoolIndex));

                else// Delete last entity
                    m_GameMgr.SystemDeleteEntity(Entity);

            }
            m_lToDelete.clear();
        }
    }
}

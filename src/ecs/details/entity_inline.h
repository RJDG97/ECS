namespace ecs::entity
{
    //--------------------------------------------------------------------------------------------

    template < typename T_CALLBACK >
        requires(
            xcore::function::is_callable_v<T_CALLBACK>&&
            std::is_same_v<typename xcore::function::traits<T_CALLBACK>::return_type, void>
        )
        instance mgr::CreateEntity(T_CALLBACK&& Function) noexcept
    {
        using func_traits = xcore::function::traits<T_CALLBACK>;

        return Entity;
    }

    //--------------------------------------------------------------------------------------------

    void mgr::DestroyEntity(ecs::entity::instance& Entity) noexcept
    {
        // Check if Entity was already destroyed
        assert(Entity.isZombie() == false);
        // Mark for destruction
        Entity.m_Validation.m_bZombie
            = m_GameMgr.m_lEntities[Entity.m_GlobalIndex].m_Validation.m_bZombie
            = true;
        // Add to delete list
        m_ToDelete.push_back(Entity);
        // Delete items on list if no processes are running
        if (0 == m_ProcessReference) UpdateStructuralChanges();
    }
}
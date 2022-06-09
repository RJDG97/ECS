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

        return[&]< typename... T_COMPONENTS >(std::tuple<T_COMPONENTS...>*) constexpr noexcept
        {
            assert(m_ComponentBits.getBit(component::info_v<T_COMPONENTS>.m_UID) && ...);

            // Allocate the entity component
            const int   EntityIndexInPool = m_Pool.Append();
            const auto  Entity = m_GameMgr.AllocNewEntity(EntityIndexInPool, *this);
            m_Pool.getComponent<ecs::entity::instance>(EntityIndexInPool) = Entity;

            // Call the user initializer if any
            if constexpr (false == std::is_same_v<ecs::tools::empty_lambda, T_CALLBACK >)
                Function(m_Pool.getComponent<std::remove_reference_t<T_COMPONENTS>>(EntityIndexInPool) ...);

            return Entity;
        }(xcore::types::null_tuple_v<func_traits::args_tuple>);
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
/******************************************************************************
filename: archetype.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains Archetype-Entity manager declerations
******************************************************************************/

namespace ecs::archetype
{
    template<typename T>
    concept T_FUNCTIONS = requires(T)
    {
        xcore::function::is_callable_v<T> && std::is_same_v<typename xcore::function::traits<T>::return_type, void>;
    };

    //-------------------------------------------------------------------------------

    struct instance final
    {
        instance(const instance&) = delete;

        //---------------------------------------------------------------------------

        inline 
        instance(ecs::game_mgr::instance& GameMgr) noexcept : m_GameMgr{GameMgr} {}

        //---------------------------------------------------------------------------
        
        inline 
        void Initialize              
        (std::span<const ecs::component::info* const> Infos, const tools::bits& Bits) noexcept;

        //---------------------------------------------------------------------------
        
        template < typename T_FUNCTIONS = ecs::tools::empty_lambda >
        ecs::entity::instance CreateEntity (T_FUNCTIONS&& Function = ecs::tools::empty_lambda{}) noexcept;

        //---------------------------------------------------------------------------
        
        inline 
        void DestroyEntity (ecs::entity::instance& Entity) noexcept;

        //---------------------------------------------------------------------------

        template < typename T_FUNCTIONS >
            requires (xcore::function::traits<T_FUNCTIONS>::arg_count_v == 0) __inline
        void AccessGuard (T_FUNCTIONS&& Function) noexcept;

        //---------------------------------------------------------------------------
        
        inline 
        void UpdateStructuralChanges (void) noexcept;

        //---------------------------------------------------------------------------

        using info_array = std::array<const ecs::component::info*, ecs::settings::max_components_per_entity_v >;

        ecs::game_mgr::instance& m_GameMgr;
        ecs::tools::bits m_ComponentBits;
        ecs::component::pool::instance m_Pool;
        std::vector<ecs::entity::instance> m_lToDelete;
        int m_ProcessReference{0};
        info_array m_InfoData;
    };
}

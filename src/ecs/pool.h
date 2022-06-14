/******************************************************************************
filename: pool.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains component memory pool definitions for entities
******************************************************************************/

namespace ecs::component::pool
{
    struct instance final
    {
    public:

        instance(void) noexcept = default;

        instance(const instance&) noexcept = delete;

        inline
        ~instance(void) noexcept;

        inline
        void Initialize(std::span<const component::info* const > Span) noexcept;

        inline
        void Clear(void) noexcept;

        inline
        int Append(void) noexcept;

        inline
        void Delete(int Index) noexcept;

        constexpr
        int Size(void) const noexcept;

        template< typename T_COMPONENT>
        T_COMPONENT& getComponent(std::uint32_t EntityIndex) const noexcept;

        inline
        void FreePage(const component::info & Info, std::byte * pData) noexcept;

        constexpr
        int findIndexComponentFromUIDComponent(std::uint16_t UIDComponent) const noexcept;

        std::span<const component::info* const >                            m_Infos         {};
        int                                                                 m_Size          {};
        std::array<std::byte*, ecs::settings::max_components_per_entity_v>  m_pComponent    {};
    };
}

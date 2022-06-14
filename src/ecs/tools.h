/******************************************************************************
filename: tools.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains declerations for tools
******************************************************************************/

namespace ecs::tools
{
    using empty_lambda = decltype([]() {});

    struct bits final
    {
        __inline 
        void setBit( int Bit ) noexcept;

        constexpr __inline
        bool getBit( int Bit ) const noexcept;

        constexpr
        bool Compare( const bits& B ) const noexcept;

        template< typename... T_COMPONENTS > __inline
        void AddFromComponents( void ) noexcept;

        std::array<std::uint64_t, ((ecs::settings::max_component_types_v-1)/64)+1> m_Bits{};
    };
}

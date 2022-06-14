/******************************************************************************
filename: component_inline.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains Component and its manager declerations
******************************************************************************/

namespace ecs::component
{
    namespace details
    {
        template< typename T >
        consteval info CreateInfo(void) noexcept
        {
            return info
            {
                .m_UID = info::invalid_id_v
            ,   .m_Size = static_cast<std::uint32_t>(sizeof(T))
                // Constructor pointer
            ,   .m_pConstructFn = std::is_trivially_constructible_v<T>      
                    ? nullptr : []( std::byte* p ) noexcept 
                        { new(p) T; }
                // Destructor pointer
            ,   .m_pDestructFn  = std::is_trivially_destructible_v<T>       
                    ? nullptr : []( std::byte* p ) noexcept 
                        { std::destroy_at(reinterpret_cast<T*>(p)); }
                // Move pointer
            ,   .m_pMoveFn      = std::is_trivially_move_assignable_v<T>    
                    ? nullptr : []( std::byte* p1, std::byte* p2 ) noexcept
                        { *reinterpret_cast<T*>(p1) = std::move(*reinterpret_cast<T*>(p2)); }
            };
        }
    }

    //------------------------------------------------------------------------------

    template< typename T_COMPONENT >
    void mgr::RegisterComponent(void) noexcept
    {
        if (component::info_v<T_COMPONENT>.m_UID == info::invalid_id_v)
            component::info_v<T_COMPONENT>.m_UID = m_UniqueID++;
    }
}

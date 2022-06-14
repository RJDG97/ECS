/******************************************************************************
filename: tools_inline.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains definitions for tools
******************************************************************************/

namespace ecs::tools
{
    //------------------------------------------------------------------------------------
    void bits::setBit(int Bit) noexcept
    {
        int x = Bit / 64;
        int y = Bit % 64;
        m_Bits[x] |= (1ull << y);
    }

    //------------------------------------------------------------------------------------
    constexpr
    bool bits::getBit(int Bit) const noexcept
    {
        int x = Bit / 64;
        int y = Bit % 64;
        return m_Bits[x] & (1ull << y);
    }

    //------------------------------------------------------------------------------------
    constexpr
    bool bits::Compare(const bits& B) const noexcept
    {
        for (int i = 0, size = static_cast<int>(m_Bits.size()); i < size; ++i)
            if (m_Bits[i] == B.m_Bits[i])
                return false;

        return true;
    }

    //------------------------------------------------------------------------------------
    template< typename... T_COMPONENTS >
    void bits::AddFromComponents( void ) noexcept
    {
        ((setBit(ecs::component::info_v<T_COMPONENTS>.m_UID)), ...);
    }
}

/******************************************************************************
filename: pool_inline.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains component memory pool declerations for entities
******************************************************************************/

namespace ecs::component::pool
{
    //-------------------------------------------------------------------------------------

    instance::~instance(void) noexcept
    {
        if (m_pComponent[0])
        {
            Clear();
            for (int i = 0; i < m_Infos.size(); i++)
                VirtualFree(m_pComponent[i], 0, MEM_RELEASE);
        }
    }

    //-------------------------------------------------------------------------------------
    // This function return which page is the last byte of a given entry
    //-------------------------------------------------------------------------------------
    constexpr inline 
    int getPageFromIndex( const component::info& Info, int iEntity ) noexcept
    {  
        return ((iEntity * Info.m_Size)-1) / ecs::settings::virtual_page_size_v;
    }

    //-------------------------------------------------------------------------------------

    void instance::Initialize( std::span<const component::info* const > Span ) noexcept
    {
        m_Infos = Span;
        for( int i=0; i< m_Infos.size(); i++ )
        {
            assert(m_Infos[i]->m_Size <= ecs::settings::virtual_page_size_v);
            auto nPages = getPageFromIndex( *m_Infos[i], ecs::settings::max_entity_count_per_pool_v ) + 1;
            m_pComponent[i] = reinterpret_cast<std::byte*>(VirtualAlloc(nullptr, nPages * ecs::settings::virtual_page_size_v, MEM_RESERVE, PAGE_NOACCESS));
            assert(m_pComponent[i]);
        }
    }

    //-------------------------------------------------------------------------------------

    void instance::Clear( void ) noexcept
    {
        while(m_Size)
            Delete(m_Size-1);
    }

    //-------------------------------------------------------------------------------------

    int instance::Append( void ) noexcept
    {
        assert( m_Size < (ecs::settings::max_entities_v-1) );

        for( int i = 0; i < m_Infos.size(); i++ )
        {
            const auto& MyInfo  = *m_Infos[i];
            const auto NexPage = getPageFromIndex(MyInfo, m_Size+1);

            // Create pages when needed 
            if( getPageFromIndex(MyInfo, m_Size) != NexPage)
            {
                // Add new page for entity within components entity pool
                auto pNewPagePtr = m_pComponent[i] + ecs::settings::virtual_page_size_v * NexPage;
                // Allocate memory
                auto p = reinterpret_cast<std::byte*>(VirtualAlloc(pNewPagePtr, ecs::settings::virtual_page_size_v, MEM_COMMIT, PAGE_READWRITE));
                assert(p == pNewPagePtr);
            }

            // Construct if required
            if( MyInfo.m_pConstructFn ) MyInfo.m_pConstructFn( &m_pComponent[i][m_Size * MyInfo.m_Size] );
        }

        return m_Size++;
    }

    //-------------------------------------------------------------------------------------

    void instance::Delete( int Index ) noexcept
    { 
        // Check if index is valid
        assert(Index < m_Size ); 
        assert(Index>=0);
        
        m_Size--;

        for (int i = 0; i < m_Infos.size(); i++)
        {
            const auto& MyInfo = *m_Infos[i];
            auto pData = m_pComponent[i];

            if (Index == m_Size) // Index is last page
            {
                // Call component destructor
                if (MyInfo.m_pDestructFn)
                    MyInfo.m_pDestructFn(&pData[m_Size * MyInfo.m_Size]);
            }
            else if (MyInfo.m_pMoveFn) // If move function is available
            {   // Move Component from last page into "deleted" page
                MyInfo.m_pMoveFn(&pData[Index * MyInfo.m_Size], &pData[m_Size * MyInfo.m_Size]);
            }
            else
            {   // Call component destructor
                if (MyInfo.m_pDestructFn)
                    MyInfo.m_pDestructFn(&pData[Index * MyInfo.m_Size]);
                // Copy over Last page into current page
                memcpy(&pData[Index * MyInfo.m_Size], &pData[m_Size * MyInfo.m_Size], MyInfo.m_Size);
            }
            // De Allocate page
            FreePage(MyInfo, pData);
        }
    }

    inline
    void instance::FreePage(const component::info& Info, std::byte* pData) noexcept
    {
        // Free page if we cross over
        const auto    LastEntryPage = getPageFromIndex(Info, m_Size + 1);
        if (getPageFromIndex(Info, m_Size) != LastEntryPage)
        {
            auto pRaw = &pData[ecs::settings::virtual_page_size_v * LastEntryPage];
            auto b = VirtualFree(pRaw, ecs::settings::virtual_page_size_v, MEM_DECOMMIT);
            assert(b);
        }
    }

    //-------------------------------------------------------------------------------------
    constexpr
    int instance::Size( void ) const noexcept
    {
        return m_Size;
    }

    //-------------------------------------------------------------------------------------
    constexpr
    int instance::findIndexComponentFromUIDComponent( const std::uint16_t UIDComponent ) const noexcept
    {
        // Search through Component IDs
        for (size_t i = 0; i < m_Infos.size(); i++)
        {   // If component ID is the same as the given UID, return index
            if (m_Infos[i]->m_UID == UIDComponent) return static_cast<int>(i);
        }
        return -1; 
    }

    //-------------------------------------------------------------------------------------

    template< typename T_COMPONENT >
    T_COMPONENT& instance::getComponent( const std::uint32_t EntityIndex ) const noexcept
    {
        static_assert( std::is_same_v<T_COMPONENT, std::decay_t<T_COMPONENT>> );
        // Get index of Component
        const auto iComponent = findIndexComponentFromUIDComponent( ecs::component::info_v<T_COMPONENT>.m_UID );
        // Return entity from component pool
        return *reinterpret_cast<T_COMPONENT*>( &m_pComponent[iComponent][ EntityIndex * sizeof(T_COMPONENT) ] );
    }
}

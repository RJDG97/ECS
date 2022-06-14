/******************************************************************************
filename: event.h
author: Renzo Joseph D. Garcia renzo.garcia@digipen.edu
Project: Midterm Project
Description:
 This file contains event definitions
******************************************************************************/

namespace ecs::event
{
    template< typename...T_ARGS >
    struct instance final
    {
        //--------------------------------------------------------------
        // INFO
        //--------------------------------------------------------------
        struct info
        {
            using callback = void(void* pPtr, T_ARGS...);
            callback* m_pCallback;
            void* m_pClass;
        };

        instance(const instance&) = delete;
        instance(void) noexcept = default;

        template<auto T_FUNCTION_PTR_V, typename  T> __inline
        void Register(T& Class) noexcept;

        constexpr __inline
        void NotifyAll(T_ARGS... Args) const noexcept;

        std::vector<info>   m_Delegates{};
    };
}

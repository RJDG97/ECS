namespace ecs::component
{
    // INFO
    struct info final
    {
        constexpr static auto invalid_id_v = 0xffff;

        using construct_fn  = void(std::byte*) noexcept;
        using destruct_fn   = void(std::byte*) noexcept;
        using move_fn       = void(std::byte* Dst, std::byte* Src ) noexcept;

        mutable std::uint16_t   m_UID;
        std::uint32_t           m_Size;
        // Function pointers
        construct_fn*           m_pConstructFn;
        destruct_fn*            m_pDestructFn;
        move_fn*                m_pMoveFn;
    };

    namespace details
    {
        template< typename T >
        consteval info CreateInfo(void) noexcept;

        template< typename T >
        static constexpr auto info_v = CreateInfo<T>();
    }

    template< typename T >
    constexpr auto& info_v = details::info_v<std::decay_t<T>>;

    // MGR
    struct mgr final
    {
        template < typename T_COMPONENT >
        void RegisterComponent (void) noexcept;

        inline 
        static int m_UniqueID = 0;
    };
}

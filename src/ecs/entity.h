namespace ecs::entity
{
    // ENTITY
    union instance final
    {
        union validation final
        {
            std::uint32_t       m_Value;
            struct
            {
                std::uint32_t   m_Generation : 31,    // Which generation to avoid collisions
                    m_bZombie : 1;        // If the entity is mark as a zombie
            };

            constexpr bool operator == (const validation& V) const noexcept { return m_Value == V.m_Value; }
        };
        static_assert(sizeof(validation) == sizeof(std::uint32_t));

        std::uint64_t       m_Value;
        struct
        {
            std::uint32_t   m_GlobalIndex;      // Index of the entity in the global pool in the game_mgr
            validation      m_Validation;       // Determine the state and give an additional unique element
        };

        constexpr bool isZombie(void) const noexcept
        {
            return m_Validation.m_bZombie;
        }

        constexpr bool operator == (const instance& V) const noexcept
        {
            return m_Value == V.m_Value;
        }
    };
    static_assert(sizeof(instance) == sizeof(std::uint64_t));
}

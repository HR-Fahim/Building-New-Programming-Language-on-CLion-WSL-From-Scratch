//
// Created by Asus on 10/2/2023.
//

//#ifndef HYDROGEN_ARENA_HPP
//#define HYDROGEN_ARENA_HPP
//
//#endif //HYDROGEN_ARENA_HPP

#pragma once

class ArenaAllocator {
    public:
    inline explicit ArenaAllocator(size_t bytes): m_size(bytes) {
        m_buffer = static_cast<std::byte*>(malloc(bytes));
        m_offset = m_buffer;
    }

    template<typename T>
    inline T* alloc() {
        void* offset = m_offset;
        m_offset += sizeof(T);
        // return offset;
        return static_cast<T*>(offset);
    }

    inline ArenaAllocator(const ArenaAllocator& other) = delete;
    inline ArenaAllocator operator=(const ArenaAllocator& other) = delete;

    inline ~ArenaAllocator() {
        free(m_buffer);
    }

    private:
    size_t m_size;
    std::byte* m_buffer;
    std::byte* m_offset;
};


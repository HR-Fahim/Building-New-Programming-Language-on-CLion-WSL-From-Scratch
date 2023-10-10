//
// Created by Asus on 10/2/2023.
//

#pragma once

class ArenaAllocator
{
public:
    // Constructor
    // Initializes the ArenaAllocator with a given size in bytes
    inline explicit ArenaAllocator(size_t bytes) : m_size(bytes)
    {
        m_buffer = static_cast<std::byte *>(malloc(bytes));
        m_offset = m_buffer;
    }

    // Allocates memory for an object of type T
    // Returns a pointer to the allocated memory
    template <typename T>
    inline T *alloc()
    {
        void *offset = m_offset;
        m_offset += sizeof(T);
        // return offset;
        return static_cast<T *>(offset);
    }

    // Copy constructor (deleted)
    inline ArenaAllocator(const ArenaAllocator &other) = delete;

    // Assignment operator (deleted)
    inline ArenaAllocator operator=(const ArenaAllocator &other) = delete;

    // Destructor
    // Frees the memory allocated by the ArenaAllocator
    inline ~ArenaAllocator()
    {
        free(m_buffer);
    }

private:
    size_t m_size;       // The size of the allocated memory in bytes
    std::byte *m_buffer; // Pointer to the allocated memory
    std::byte *m_offset; // Current offset within the allocated memory
};
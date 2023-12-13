/*! @file pool_alloc.h
 *  @author Alexandru Ilies and Jorn ten Kate
 *  @date 10.12.2023
 *  @version 1.0.0
 *  @brief Template Class for a Custom Pool Allocator
 *  @copyright GNU Public License.
 */

#ifndef ALLOCATORS_POOL_ALLOC_H
#define ALLOCATORS_POOL_ALLOC_H
#include <iostream>
#include <memory>
#include <sys/mman.h>
#include <cmath>
#include <cstddef>

#define DEFAULT_LENGTH 4096     //4096 chunks possible in a memory pool
/*! @class pool_alloc Template Class
 *  @brief Manages memory for STL datastructures interpreted as memory pool allocator
 *
 *  @details This class defines a custom memory pool allocator for different STL datastructures
 */

template <typename T>
class pool_alloc : public std::allocator<T>
{
    using type_value = T;
    using pointer = T*;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    /*!
     *  Define a ChunkHead for easier pointer arithmetics
     *
     *  @details Keeps track of the chunks availability and how many bytes are stored in the chunk
     */
    struct ChunkHead
    {
        bool allocated;
        size_t bytes_allocated;
    };

    /*!
     *  Define a Chunk for easier pointer arithmetics
     *
     *  @details Keep the chunk information and the pointer to the available memory slots
     */
    struct Chunk
    {
        ChunkHead header;
        T data[1];
    };

    void* poolBegin_, *poolEnd_;
    static constexpr size_t pool_ByteSize = DEFAULT_LENGTH * sizeof(Chunk);
    static constexpr size_t DEFAULT_DIV = sizeof(Chunk);      // Alignment is based on the size of the `Chunk` structure

    /*!
     *  @details Function that allocates a memory pool using the mmap() function to request memory from kernel
     */
    void allocate_pool() {
        poolBegin_ = mmap(nullptr,
                          pool_ByteSize,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS,
                          -1, 0);
        if (poolBegin_ == MAP_FAILED) throw std::bad_alloc();

        reinterpret_cast<ChunkHead*>(poolBegin_)->bytes_allocated = 0;
        reinterpret_cast<ChunkHead*>(poolBegin_)->allocated = false;
        poolEnd_ = reinterpret_cast<std::byte*>(poolBegin_) + pool_ByteSize;
        std::cout << "Memory Pool allocated at: " << poolBegin_ << "\n";
    }

public:
    /*!
   * @details Hard-Copy Constructor of the allocator - assigns the poolBegin_ as null
   */
    template <class U> explicit pool_alloc(const pool_alloc<U>&) noexcept : poolBegin_{nullptr} {};

    /*!
   * @details Constructor of the allocator - assigns the poolBegin_ as null
   */
    explicit pool_alloc() noexcept : poolBegin_{nullptr}  {};

    /*!
     * @details Destructor of the allocator - deallocates the entire memory pool
     */
    ~pool_alloc() noexcept
    {
        if(poolBegin_) {
            munmap(poolBegin_, pool_ByteSize);
            std::cout << "Memory Pool deallocated at: " << poolBegin_ << "\n";
        }
    }

    /*!
     * @details Function that gives to the user n bytes from the memory pool
     * @n - Number of bytes to allocates
     * @return pointer to the available memory chunk
     */
    pointer allocate(size_type n)
    {
        if(n > pool_ByteSize) throw std::logic_error("Cannot allocate this amount of bytes");

        if(!poolBegin_) allocate_pool();

        auto searchPtr = reinterpret_cast<ChunkHead*>(poolBegin_);
        while(searchPtr->allocated){
            static size_t padding = (DEFAULT_DIV * std::ceil((float)((float)(searchPtr->bytes_allocated + sizeof(ChunkHead))/ DEFAULT_DIV))) - searchPtr->bytes_allocated - sizeof(ChunkHead);
            static size_t skip = sizeof(ChunkHead) + searchPtr->bytes_allocated + padding;
            searchPtr = reinterpret_cast<ChunkHead*>(reinterpret_cast<std::byte*>(searchPtr) + skip);

            if(std::distance(reinterpret_cast<std::byte*>(searchPtr), reinterpret_cast<std::byte*>(poolEnd_)) <= 0)
                throw std::runtime_error("Trying to allocate outside of the Memory Pool");
        }


        searchPtr->allocated = true;
        searchPtr->bytes_allocated += n;

        std::cout << "Memory given from: " << reinterpret_cast<Chunk*>(searchPtr)->data << "\n";
        return reinterpret_cast<Chunk*>(searchPtr)->data;
    }

    /*!
     * @details Function that takes n bytes from the user
     * @n - Number of bytes to deallocates
     * @param ptr - pointer to the position to deallocate
     * @param n - size of the bytes to deallocate from that position
     */
    void deallocate (pointer ptr, size_type n)
    {
        auto chunkBegin = reinterpret_cast<Chunk*>(reinterpret_cast<std::byte*>(ptr) - sizeof(ChunkHead));
        chunkBegin->header.bytes_allocated -= n;
        if(!chunkBegin->header.bytes_allocated) chunkBegin->header.allocated = false;
        std::cout << "Memory taken from: " << ptr << "\n";
    }

    /*!
     * @details Structure that is useful for rebinding different sub-structures allocators
     */
    template <typename U>
    struct rebind {
        using other = pool_alloc<U>;
    };

};

#endif //ALLOCATORS_POOL_ALLOC_H

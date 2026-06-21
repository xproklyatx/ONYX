#pragma once
#include <cstdint>   
#include <cstring>
#include <array>
#define KB(x) (x * 1024)
#define MB(x) (x * 1024 * 1024)
#define GB(x) (x * 1024 * 1024 * 1024)
#define PAGE_SIZE 4096
#define ARENA_DEFAULT_RESERVE_SIZE GB(1)
#define VERTEX_ARENA 0
#define SHADER_ARENA 1
#define MATERIAL_ARENA 2
#define TEXTURE_ARENA 3
#define CBUFFER_ARENA 4
#define FRAME_ARENA 5
struct Arena
{
    void* base;
    uint8_t* buffer;
    size_t reserved;
    size_t committed;
    size_t offset;
    size_t page_size;
};

class ArenaManager
{
  public:
    static void Initialize();
    template <typename T> 
    static T* Allocate(size_t index)
    {
        size_t alignment = alignof(T);
        size_t size = sizeof(T);
        if (!arenas[index] || size == 0 || !IsPowerOfTwo(alignment))
            return nullptr;
        uintptr_t current = reinterpret_cast<uintptr_t>(arenas[index]->buffer) + arenas[index]->offset;
        uintptr_t aligned = AlignUp(current, alignment);
        size_t padding = static_cast<size_t>(aligned - current);
        size_t new_offset = arenas[index]->offset + padding + size;
        if (new_offset > (arenas[index]->reserved - (reinterpret_cast<uint8_t*>(arenas[index]->buffer) - reinterpret_cast<uint8_t*>(arenas[index]->base))))
            return nullptr;
        if (!ArenaEnsureCapacity(index, new_offset))
            return nullptr;
        void* ptr = arenas[index]->buffer + arenas[index]->offset + padding;
        arenas[index]->offset = new_offset;
        memset(ptr, 0, size);
        return static_cast<T*>(ptr);
    }
    static void Commit(size_t index, size_t size);
    static void ResetArena(size_t index);
    static void FreeArena(size_t index);
    static size_t ArenaGetRemaining(size_t index);
  private:
    static void* OsAlloc(void* address, size_t size);
    static void* OsReserve(size_t size);
    static void OsDecommit(void* address, size_t size);
    static void OsRelease(void* address);
    static int IsPowerOfTwo(size_t x);
    static size_t AlignUp(size_t value, size_t alignment);
    static size_t AlignDown(size_t value, size_t alignment);
    static int ArenaEnsureCapacity(size_t index, size_t required_offset);
    static std::array<Arena*, 6> arenas;
};
#ifdef ARENA_IMPLEMENTATION
#include <windows.h>
void* ArenaManager::OsAlloc(void* address, size_t size)
{
    return VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
}

void* ArenaManager::OsReserve(size_t size)
{
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
}

void ArenaManager::OsDecommit(void* address, size_t size)
{
    VirtualFree(address, size, MEM_DECOMMIT);
}

void ArenaManager::OsRelease(void* address)
{
    VirtualFree(address, 0, MEM_RELEASE);
}
int ArenaManager::IsPowerOfTwo(size_t x)
{
    return x != 0 && (x & (x - 1)) == 0;
}
size_t ArenaManager::AlignUp(size_t value, size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}
size_t ArenaManager::AlignDown(size_t value, size_t alignment)
{
    return value & ~(alignment - 1);
}
std::array<Arena*, 6> ArenaManager::arenas{};
void ArenaManager::Initialize()
{
    for (auto& arena : arenas)
    {
        size_t reserve_size = ARENA_DEFAULT_RESERVE_SIZE;
        reserve_size = AlignUp(reserve_size, PAGE_SIZE);
        void* base = OsReserve(reserve_size);
        if (!base)
            return;
        size_t arena_struct_size = AlignUp(sizeof(Arena), PAGE_SIZE);
        size_t initial_commit = arena_struct_size + PAGE_SIZE;
        if (initial_commit > reserve_size)
            initial_commit = reserve_size;
        arena = reinterpret_cast<Arena*>(OsAlloc(base, initial_commit));
        if (!arena)
        {
            OsRelease(base);
            return;
        }
        arena->base = base;
        arena->buffer = reinterpret_cast<uint8_t*>(base) + arena_struct_size;
        arena->reserved = reserve_size;
        arena->committed = initial_commit;
        arena->offset = 0;
        arena->page_size = PAGE_SIZE;
    }
}
void ArenaManager::Commit(size_t index, size_t size)
{
    if (!arenas[index])
        return;

    size_t struct_offset = static_cast<size_t>(reinterpret_cast<uint8_t*>(arenas[index]->buffer) -
                                               reinterpret_cast<uint8_t*>(arenas[index]->base));

    size_t required = struct_offset + size;
    required = AlignUp(required, arenas[index]->page_size);

    if (required <= arenas[index]->committed)
        return;

    if (required > arenas[index]->reserved)
        return;

    size_t commit_size = required - arenas[index]->committed;
    void* commit_addr = reinterpret_cast<uint8_t*>(arenas[index]->base) + arenas[index]->committed;

    void* result = OsAlloc(commit_addr, commit_size);
    if (result)
        arenas[index]->committed = required;
}
void ArenaManager::ResetArena(size_t index)
{
    if (!arenas[index])
        return;
    size_t struct_end = static_cast<size_t>(reinterpret_cast<uint8_t*>(arenas[index]->buffer) -
                                            reinterpret_cast<uint8_t*>(arenas[index]->base));
    size_t keep_committed = AlignUp(struct_end, arenas[index]->page_size);
    if (arenas[index]->committed > keep_committed)
    {
        void* decommit_addr = reinterpret_cast<uint8_t*>(arenas[index]->base) + keep_committed;
        size_t decommit_size = arenas[index]->committed - keep_committed;
        OsDecommit(decommit_addr, decommit_size);
        arenas[index]->committed = keep_committed;
    }
    arenas[index]->offset = 0;
}
void ArenaManager::FreeArena(size_t index)
{
    if (!arenas[index])
        return;
    OsRelease(arenas[index]->base);
}
size_t ArenaManager::ArenaGetRemaining(size_t index)
{
    if (!arenas[index])
        return 0;
    size_t buffer_capacity = arenas[index]->reserved - (reinterpret_cast<uint8_t*>(arenas[index]->buffer) -
                                                        reinterpret_cast<uint8_t*>(arenas[index]->base));
    return buffer_capacity - arenas[index]->offset;
}
int ArenaManager::ArenaEnsureCapacity(size_t index, size_t required_offset)
{
    if (!arenas[index])
        return 0;
    size_t struct_offset = static_cast<size_t>(reinterpret_cast<uint8_t*>(arenas[index]->buffer) -
                                               reinterpret_cast<uint8_t*>(arenas[index]->base));
    size_t total_required = struct_offset + required_offset;
    total_required = AlignUp(total_required, arenas[index]->page_size);
    if (total_required <= arenas[index]->committed)
        return 1;
    if (total_required > arenas[index]->reserved)
        return 0;
    size_t commit_size = total_required - arenas[index]->committed;
    void* commit_addr = reinterpret_cast<uint8_t*>(arenas[index]->base) + arenas[index]->committed;
    void* result = OsAlloc(commit_addr, commit_size);
    if (!result)
        return 0;
    arenas[index]->committed = total_required;
    return 1;
}
#endif 
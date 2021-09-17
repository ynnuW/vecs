#ifndef VECS_HPP
#define VECS_HPP

#include <cstddef>
#include <type_traits>
#include <tuple>
#include <boost/pfr.hpp>

// TODO: use Pack struct to pack parameter pack
// TODO: add allocater for chunks
// TODO: add allocator for chunks array // this is reall small but whatever, not fan of std::vector
// TODO: add something so we can have array of 1 bits (or 2, 3, 4, etc bits) something like a bitset system without wasting space. probably want to add a extra type using thingy instead of only using LANE_WIDTH+ and LANE_WIDTH==0

namespace vecs {

// NOTE: AOS if LANE_WIDTH_ == 0
template <typename Type_, size_t LANE_WIDTH_ = 0>
struct Component {
    using Type = Type_;
    static constexpr size_t LANE_WIDTH = LANE_WIDTH_;
};

template <typename... Components_>
struct ArcheType {
    using Components = std::tuple<Components_...>;
    using Types = std::tuple<typename Components_::Type...>;
    using View = std::tuple<std::conditional_t<static_cast<bool>(Components_::LANE_WIDTH), std::add_pointer_t<typename Components_::Type::template SOA<Components_::LANE_WIDTH>>, std::add_pointer_t<typename Components_::Type>>...>;
};

template<typename ArcheType_>
struct Chunk {
    using ArcheType = ArcheType_;
    using View = typename ArcheType::View;

    View view; // TODO: this is not needed, but void* seems like UB atm :/
    void* data; // cast back to View when accessing, its probably UB tho :/
    size_t n;
};

template <typename ArcheType_>
struct Chunks {
    using ArcheType = ArcheType_;

    
    
    Chunk<ArcheType>* array;
    size_t n;
};

template <typename... ArcheTypes>
struct World {
    std::tuple<Chunks<ArcheTypes>...> chunks;
};

template <class T, class... U>
struct contains : std::disjunction<std::is_same<T, U>...>{};
template <typename...>
struct is_subset_of : std::false_type{};
template <typename... Types1, typename ... Types2>
struct is_subset_of<std::tuple<Types1...>, std::tuple<Types2...>> : std::conjunction<contains<Types1, Types2...>...>{};

template <size_t index, typename... RequiredComponents, typename... ArcheTypes>
constexpr auto get_chunks_containing_helper(World<ArcheTypes...>* world)
{
    if constexpr (index == sizeof...(ArcheTypes)) {
        return std::tuple<>{};
    } else {
        using IndexedArcheTypeChunk = typename std::tuple_element_t<index, decltype(world->chunks)>;
        if constexpr (is_subset_of<std::tuple<RequiredComponents...>, typename IndexedArcheTypeChunk::ArcheType::Types>::value) {
            return std::tuple_cat(std::tuple<IndexedArcheTypeChunk*>{&std::get<index>(world->chunks)}, get_chunks_containing_helper<index + 1, RequiredComponents...>(world));
        } else {
            return get_chunks_containing_helper<index + 1, RequiredComponents...>(world);
        }
    }
}

template <typename... RequiredComponents, typename... ArcheTypes, size_t index = 0>
constexpr auto get_chunks_containing(World<ArcheTypes...>* world)
{
    return get_chunks_containing_helper<0, RequiredComponents...>(world);
}

template<typename... RequiredComponents, typename... Components>
constexpr auto unpack_chunk(Chunk<ArcheType<Components...>>* chunk)
{
    // return std::tuple<RequiredComponents...>{std::get<RequiredComponents>(chunk->view)...}; // TODO: forward?
    // static_assert(false);


    // TODO: fix this with void*
    // TODO: filter based on RequiredComponents
    return chunk->view;
    // return reinterpret_cast<typename Chunk<ArcheType<Components...>>::View>(chunk->data);
}

// TODO: find something to fix when allocation spreads multipe chunks. 
    // return structure with Chunk* with starting index (max index is chunk->n)
// TODO: use the buffer to prevent memory allocation, use alloca and calculate the amount of {Chunk*,size_t} in the worst case (current chunks all full)
// auto allocate_entities(void* buffer)
// {
//     struct Result {

//     };
// }

template <typename... Components, typename... ArcheTypes>
constexpr auto allocate_entity(World<ArcheTypes...>* world)
{
    struct Result {
        
        size_t index;
    };

    

    return Result{};
}

// TODO:
void free_entities()
{

}

}

#endif /* VECS_HPP */

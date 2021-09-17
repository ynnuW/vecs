#include "../vecs/vecs.hpp"

#include <utility>
#include  <span>

typedef float f32;

struct Position {
    template <size_t N>
    struct SOA {
        f32 x[N];
        f32 y[N];
        f32 z[N];
    };
    
    f32 x;
    f32 y;
    f32 z;
};

struct Orientation {
    template <size_t N>
    struct SOA {
        f32 x[N];
        f32 y[N];
        f32 z[N];
        f32 w[N];
    };
    
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

struct LinearMomentum {
    template <size_t N>
    struct SOA {
        f32 x[N];
        f32 y[N];
        f32 z[N];
    };
    
    f32 x;
    f32 y;
    f32 z;
};

struct AngularMomentum {
    template <size_t N>
    struct SOA {
        f32 x[N];
        f32 y[N];
        f32 z[N];
    };
    
    f32 x;
    f32 y;
    f32 z;
};

struct Mass {
    template <size_t N>
    struct SOA {
        f32 one_over_value[N];
    };

    f32 one_over_value;
};

using World = vecs::World<vecs::ArcheType<vecs::Component<Position, 8>,
                                          vecs::Component<Orientation, 8>,
                                          vecs::Component<LinearMomentum, 8>,
                                          vecs::Component<AngularMomentum, 8>,
                                          vecs::Component<Mass>>,
                          vecs::ArcheType<vecs::Component<Position, 8>>>;

int main()
{
    World world{};

    // NOTE: no allocate support yet :/

    const f32 gravity_z{9.81f};
    const f32 dt{1.f / 128.f};

    // NOTE: waiting on for... in c++23 :/
    std::apply([&](auto&&... args){ (([&]() {
        for (auto& chunk : std::span(args->array, args->n)) {
            // NOTE: auto [positions]{vecs::unpack<Position>unpack(&chunk)} doesn't work yet
            auto [positions, orientations, linear_momenta, angular_momenta, masses]{vecs::unpack_chunk<Position, Orientation, LinearMomentum, AngularMomentum, Mass>(&chunk)};

            const size_t LANE_WIDTH{8}; // TODO: replace with actual check
            
            for (size_t i = 0; i < chunk.n; i += LANE_WIDTH) { for (size_t j = 0; j < LANE_WIDTH; ++j) {
                linear_momenta[i].z[j] += gravity_z / masses[i + j].one_over_value * dt;

                positions[i].x[j] += linear_momenta[i].x[j] * masses[i + j].one_over_value * dt;
                positions[i].y[j] += linear_momenta[i].y[j] * masses[i + j].one_over_value * dt;
                positions[i].z[j] += linear_momenta[i].z[j] * masses[i + j].one_over_value * dt;
            } }
        }
    }()),  ...); }, vecs::get_chunks_containing<Position, Orientation, LinearMomentum, AngularMomentum, Mass>(&world));

    return 0;
}

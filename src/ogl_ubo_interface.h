
#pragma pack (push, 1)
    
namespace std140 {
    struct float_ {
        static_assert(sizeof(f32) == 4, "");
        static constexpr uptr   align () { return 4; }
        f32                     raw;
        
        void set (f32 f) {
            raw = f;
        }
        f32 get () const {
            return raw;
        }
    };
    struct bool_ {
        static constexpr uptr   align () { return 4; };
        u32                     raw;
        
        void set (bool b) {
            static_assert(  static_cast<u32>(false) == 0 &&
                            static_cast<u32>(true) == 1, "");
            raw = static_cast<u32>(b);
        }
    };
    struct sint_ {
        static_assert(sizeof(GLint) == 4, "");
        static constexpr uptr   align () { return 4; };
        GLint                   raw;
        
        void set (GLint u) {
            raw = u;
        }
    };
    struct uint_ {
        static_assert(sizeof(GLuint) == 4, "");
        static constexpr uptr   align () { return 4; };
        GLuint                  raw;
        
        void set (GLuint u) {
            raw = u;
        }
    };
    
    struct ivec2 {
        static constexpr uptr   align () { return 2 * sizeof(sint_); };
        GLint                   raw[2];
        
        void set (tv2<GLint> v) {
            raw[0] = v.x;
            raw[1] = v.y;
        }
    };
    struct uvec2 {
        static constexpr uptr   align () { return 2 * sizeof(uint_); };
        GLuint                  raw[2];
        
        void set (tv2<GLuint> v) {
            raw[0] = v.x;
            raw[1] = v.y;
        }
    };
    struct uvec4 {
        static constexpr uptr   align () { return 4 * sizeof(uint_); };
        GLuint                  raw[4];
        
        void set (tv4<GLuint> v) {
            raw[0] = v.x;
            raw[1] = v.y;
            raw[2] = v.z;
            raw[3] = v.w;
        }
    };
    
    struct vec2 {
        static constexpr uptr   align () { return 2 * sizeof(float_); };
        f32                     raw[2];
        
        void set (v2 v) {
            raw[0] = v.x;
            raw[1] = v.y;
        }
    };
    struct vec3 {
        static constexpr uptr   align () { return 4 * sizeof(float_); };
        f32                     raw[3];
        
        void set (v3 v) {
            raw[0] = v.x;
            raw[1] = v.y;
            raw[2] = v.z;
        }
    };
    struct vec4 {
        static constexpr uptr   align () { return 4 * sizeof(float_); };
        static_assert((4 * sizeof(float_)) >= 16, "");
        f32                     raw[4];
        
        void set (v4 v) {
            raw[0] = v.x;
            raw[1] = v.y;
            raw[2] = v.z;
            raw[3] = v.w;
        }
    };
    
    template <typename T, uptr LEN_>
    struct array {
        static constexpr uptr   LEN = LEN_;
        static constexpr uptr   align () { return alignUp(T::align(), vec4::align()); };
        static constexpr uptr   stride () { return alignUp(sizeof(T), vec4::align()); };
        byte                    raw[stride() * LEN];
        
        T& operator[] (uptr indx) {
            assert(indx < LEN);
            return reinterpret_cast<T&>(*reinterpret_cast<T*>(&raw[0] +(indx * stride())));
        }
        template <typename INDEXABLE>
        void setAll (INDEXABLE val) {
            for (uptr i=0; i<LEN; ++i)
                this->operator[](i).set(val[i]);
        }
    };
    
    #if 0
    struct mat3 {
        static constexpr uptr   align () { return array<vec3, 3>::align(); };
        static_assert(array<vec3, 3>::align() >= 16, "");
        vec4                    arr[3];
        
        void set (c_tm3<f32> m) {
            arr[0].set(v4(m[0], 0.0f));
            arr[1].set(v4(m[1], 0.0f));
            arr[2].set(v4(m[2], 0.0f));
        }
    };
    struct mat4 {
        static constexpr uptr   align () { return array<vec4, 4>::align(); };
        static_assert(array<vec4, 4>::align() >= 16, "");
        vec4                    arr[4];
        
        void set (m4 m) {
            arr[0].set(m[0]);
            arr[1].set(m[1]);
            arr[2].set(m[2]);
            arr[3].set(m[3]);
        }
    };
    #endif
    
    template <uptr L, uptr R> // MSVC somehow can compile this, but not ASSERT_STD140_ALIGNMENT with a resular %
    struct _Modulus {
        static constexpr uptr VAL = L % R;
    };
}
    
    #define ASSERT_STD140_STRUCT_SIZE(structure) \
        static_assert(std140::_Modulus<sizeof(structure), structure::align()>::VAL == 0, "std140 alignment failed, add padding.")
    #define ASSERT_STD140_ALIGNMENT(structure, member) \
        static_assert(std140::_Modulus<offsetof(structure, member), decltype(structure::member)::align()>::VAL == 0, "std140 alignment failed, add padding.")
    #define ASSERT_STD140_ALIGNMENT_IDENTICAL(std140_struct, structure, member) \
        static_assert(offsetof(std140_struct, member) == offsetof(structure, member), "std140 alignment failed, add padding.")
    
#pragma pack (pop)

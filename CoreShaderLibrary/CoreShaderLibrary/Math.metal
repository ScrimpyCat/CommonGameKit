/*
 *  Copyright (c) 2020, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef math_metal
#define math_metal

#ifdef __METAL_VERSION__
#include <metal_stdlib>

namespace std = metal;
using namespace metal;

#warning Converting double to float
#define double float

#if __METAL_VERSION__ <= 220
#warning Converting 64-bit integers to 32-bit
#define int64_t int32_t
#define uint64_t uint32_t
#endif

template<typename> struct is_void { static constant bool value = false; };
template<> struct is_void<void> { static constant bool value = true; };

#define enable_if_void(T) enable_if(is_void<T>::value)
#else
#include <iostream>
#include <cmath>
#include <typeinfo>
#include <cxxabi.h>

#define constant const
#define thread

#define enable_if_void(T) enable_if(std::is_void<T>::value)
#endif

#define enable_if(B) typename std::enable_if<B, int>::type = 0
#define enable_if_integral(T) enable_if(std::is_integral<T>::value)
#define enable_if_floating_point(T) enable_if(std::is_floating_point<T>::value)
#define enable_if_arithmetic(T) enable_if(std::is_arithmetic<T>::value)
#define enable_if_scalar(T) enable_if(is_scalar<T>::value)
#define enable_if_vector(T) enable_if(is_vector<T>::value || std::is_vector<T>::value)
#define enable_if_not_vector(T) enable_if(!is_vector<T>::value && !std::is_vector<T>::value)
#define enable_if_vector_size(T, N) enable_if(vector_size<T>::value == N)
#define enable_if_even(N) enable_if(N % 2 == 0)
#define enable_if_odd(N) enable_if(N % 2 != 0)
#define arg_end typename VA_END_MARKER = void, enable_if_void(VA_END_MARKER)

namespace math
{
    namespace fixed
    {
        enum sign
        {
            pos,
            neg
        };
        
#ifndef __METAL_VERSION__
        std::ostream &operator<<(std::ostream &os, const enum sign s)
        {
            switch (s)
            {
                case sign::pos:
                    return os << "pos";
                    
                case sign::neg:
                    return os << "neg";
            }
            
            return os;
        }
#endif
        
        template<int M, int F> class scalar;
        template<typename> struct is_scalar { static constant bool value = false; };
        template<int M, int F> struct is_scalar<scalar<M, F>> { static constant bool value = true; };
        
        template<int M, int F> class scalar
        {
            union {
                struct {
                    unsigned int fractional : F;
                    int magnitude : M;
                };
                struct {
                    int : (M - 1) + F;
                    enum sign sign : 1;
                };
                int value : M + F;
            };
            
        public:
            inline scalar<M, F>()
            {
                magnitude = 0;
                fractional = 0;
            }
            
            inline scalar<M, F>(enum sign s, int mag, int frac)
            {
                magnitude = mag;
                fractional = frac;
                sign = s;
            }
            
            template<typename T, enable_if_scalar(T)>
            inline scalar<M, F>(T v)
            {
                *this = static_cast<double>(v);
            }
            
            template<typename T, enable_if_arithmetic(T)>
            inline scalar<M, F>(T v)
            {
                *this = v;
            }
            
            template<typename T, enable_if_integral(T)>
            inline explicit operator T() const
            {
                return magnitude + sign;
            }
            
            template<typename T, enable_if_floating_point(T)>
            inline explicit operator T() const
            {
                return (double)value / (1 << F);
            }
            
            template<typename T, enable_if_integral(T)>
            inline scalar<M, F> operator=(T rhs)
            {
                magnitude = rhs;
                fractional = 0;
                
                return *this;
            }
            
            template<typename T, enable_if_floating_point(T)>
            inline scalar<M, F> operator=(T rhs)
            {
                value = (double)rhs * (1 << F);
                
                return *this;
            }
            
            template<typename T>
            inline scalar<M, F> operator+(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                a.value += b.value;
                
                return a;
            }
            
            template<typename T>
            inline scalar<M, F> operator+=(T rhs)
            {
                return (*this = *this + rhs);
            }
            
            template<typename T>
            inline scalar<M, F> operator-(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                a.value -= b.value;
                
                return a;
            }
            
            template<typename T>
            inline scalar<M, F> operator-=(T rhs)
            {
                return (*this = *this - rhs);
            }
            
            template<typename T>
            inline scalar<M, F> operator*(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                int64_t r = a.value;
                r *= b.value;
                a.value = r >> F;
                
                return a;
            }
            
            template<typename T>
            inline scalar<M, F> operator*=(T rhs)
            {
                return (*this = *this * rhs);
            }
            
            template<typename T>
            inline scalar<M, F> operator/(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                int64_t r = a.value << F;
                r /= b.value;
                a.value = r;
                
                return a;
            }
            
            template<typename T>
            inline scalar<M, F> operator/=(T rhs)
            {
                return (*this = *this / rhs);
            }
            
            inline scalar<M, F> abs() const
            {
                scalar<M, F> r(*this);
                
                if (sign == sign::neg) r.value = -r.value;
                
                return r;
            }
            
            inline scalar<M, F> sqrt() const { return sqrt_impl<F>(); }
            
            template<typename T>
            inline bool operator==(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                return a.value == b.value;
            }
            
            template<typename T>
            inline bool operator!=(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                return a.value == b.value;
            }
            
            template<typename T>
            inline bool operator<(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                return a.value < b.value;
            }
            
            template<typename T>
            inline bool operator<=(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                return a.value <= b.value;
            }
            
            template<typename T>
            inline bool operator>(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                return a.value > b.value;
            }
            
            template<typename T>
            inline bool operator>=(T rhs) const
            {
                scalar<M, F> a(*this), b(rhs);
                return a.value >= b.value;
            }
            
            friend scalar<M, F> sqrt(scalar<M, F> v)
            {
                return v.sqrt();
            }
            
            friend scalar<M, F> abs(scalar<M, F> v)
            {
                return v.abs();
            }
            
#ifndef __METAL_VERSION__
            friend std::ostream &operator<<(std::ostream &os, const scalar<M, F> &v)
            {
                os << static_cast<const double&>(v);
                
                return os << " : scalar<" << M << ", " << F << ">(" << static_cast<enum sign>(v.sign) << ", " << v.magnitude << ", " << v.fractional << ")";
            }
#endif
            
        private:
            template<int N, enable_if_odd(N), int MF = M + F, enable_if(MF == 32 && M != 1)> inline scalar<M, F> sqrt_impl() const
            {
                scalar<M - 1, F + 1> t = *this;
                scalar<M, F> r = t.sqrt();
                
                return r;
            }
            
            template<int N, enable_if_odd(N), int MF = M + F, enable_if(MF == 32 && M == 1)> inline scalar<M, F> sqrt_impl() const
            {
                scalar<M, F - 1> t = *this;
                scalar<M, F> r = t.sqrt();
                
                return r;
            }
            
            template<int N, enable_if_odd(N), int MF = M + F, enable_if(MF < 32)> inline scalar<M, F> sqrt_impl() const
            {
                scalar<M, F + 1> t = *this;
                scalar<M, F> r = t.sqrt();
                
                return r;
            }
            
            template<int N, enable_if_even(N)> inline scalar<M, F> sqrt_impl() const
            {
                uint32_t x = value;
                
                uint32_t root = 0;
                uint32_t remHi = 0;
                uint32_t remLo = x;
                uint32_t count = 15 + (F >> 1);
                
                do {
                    remHi = (remHi << 2) | (remLo >> 30);
                    remLo <<= 2;
                    root <<= 1;
                    uint32_t testDiv = (root << 1) + 1;
                    if (remHi >= testDiv)
                    {
                        remHi -= testDiv;
                        root += 1;
                    }
                } while (count-- != 0);
                
                scalar<M, F> r;
                r.value = root;
                
                return r;
            }
        };
    }
    
    template<int C>
    struct axes
    {
        enum
        {
            x,
            y,
            z,
            w,
            s = x,
            t = y,
            u = z,
            v = w,
            r = x,
            g = y,
            b = z,
            a = w
        };
    };
    
    template<> struct axes<0> {};
    
    template<>
    struct axes<1>
    {
        enum
        {
            x,
            s = x,
            r = x
        };
    };
        
    template<>
    struct axes<2>
    {
        enum
        {
            x,
            y,
            s = x,
            t = y,
            r = x,
            g = y
        };
    };
    
    template<>
    struct axes<3>
    {
        enum
        {
            x,
            y,
            z,
            s = x,
            t = y,
            u = z,
            r = x,
            g = y,
            b = z
        };
    };
    
    template<typename E, int N> class vector;
    template<typename> struct is_vector { static constant bool value = false; };
    template<typename E, int N> struct is_vector<vector<E, N>> { static constant bool value = true; };
    
    template<typename> struct vector_size { static constant int value = 0; };
    template<typename E, int N> struct vector_size<vector<E, N>> { static constant int value = N; };
    template<typename T, int N> struct vector_size<T __attribute__((ext_vector_type(N)))> { static constant int value = N; };
        
    template<typename E, int N> class vector
    {
        E p[N];
        
    public:
        
        typedef math::axes<N> axis;
        
        inline vector<E, N>()
        {
            *this = 0;
        }
        
        template<typename T>
        inline vector<E, N>(T v)
        {
            *this = v;
        }
        
        template<typename T>
        inline vector<E, N>(T x, T y)
        {
            p[0] = x;
            p[1] = y;
        }
        
        template<typename T>
        inline vector<E, N>(T x, T y, T z)
        {
            p[0] = x;
            p[1] = y;
            p[2] = z;
        }
        
        template<typename T>
        inline vector<E, N>(T x, T y, T z, T w)
        {
            p[0] = x;
            p[1] = y;
            p[2] = z;
            p[3] = w;
        }
        
        template<typename T, enable_if_vector(T)>
        inline vector<E, N> operator=(T rhs)
        {
            for (int i = 0; i < N; i++) p[i] = rhs[i];
            
            return *this;
        }
        
        template<typename T, enable_if_not_vector(T)>
        inline vector<E, N> operator=(T rhs)
        {
            for (int i = 0; i < N; i++) p[i] = rhs;
            
            return *this;
        }
        
        thread E& operator[](size_t i)
        {
            return p[i];
        }
        
        const thread E& operator[](size_t i) const
        {
            return p[i];
        }
        
#define v1(v, x) v.get<decltype(v)::axis::x>()
#define v2(v, x, y) v.get<decltype(v)::axis::x, decltype(v)::axis::y>()
#define v3(v, x, y, z) v.get<decltype(v)::axis::x, decltype(v)::axis::y, decltype(v)::axis::z>()
#define v4(v, x, y, z, w) v.get<decltype(v)::axis::x, decltype(v)::axis::y, decltype(v)::axis::z, decltype(v)::axis::w>()
        
#define x(v) v1(v, x)
#define y(v) v1(v, y)
#define z(v) v1(v, z)
#define w(v) v1(v, w)
        
#define xx(v) v2(v, x, x)
#define xy(v) v2(v, x, y)
#define xz(v) v2(v, x, z)
#define xw(v) v2(v, x, w)
#define yx(v) v2(v, y, x)
#define yy(v) v2(v, y, y)
#define yz(v) v2(v, y, z)
#define yw(v) v2(v, y, w)
#define zx(v) v2(v, z, x)
#define zy(v) v2(v, z, y)
#define zz(v) v2(v, z, z)
#define zw(v) v2(v, z, w)
#define wx(v) v2(v, w, x)
#define wy(v) v2(v, w, y)
#define wz(v) v2(v, w, z)
#define ww(v) v2(v, w, w)
        
#define xxx(v) v3(v, x, x, x)
#define xxy(v) v3(v, x, x, y)
#define xxz(v) v3(v, x, x, z)
#define xxw(v) v3(v, x, x, w)
#define xyx(v) v3(v, x, y, x)
#define xyy(v) v3(v, x, y, y)
#define xyz(v) v3(v, x, y, z)
#define xyw(v) v3(v, x, y, w)
#define xzx(v) v3(v, x, z, x)
#define xzy(v) v3(v, x, z, y)
#define xzz(v) v3(v, x, z, z)
#define xzw(v) v3(v, x, z, w)
#define xwx(v) v3(v, x, w, x)
#define xwy(v) v3(v, x, w, y)
#define xwz(v) v3(v, x, w, z)
#define xww(v) v3(v, x, w, w)
#define yxx(v) v3(v, y, x, x)
#define yxy(v) v3(v, y, x, y)
#define yxz(v) v3(v, y, x, z)
#define yxw(v) v3(v, y, x, w)
#define yyx(v) v3(v, y, y, x)
#define yyy(v) v3(v, y, y, y)
#define yyz(v) v3(v, y, y, z)
#define yyw(v) v3(v, y, y, w)
#define yzx(v) v3(v, y, z, x)
#define yzy(v) v3(v, y, z, y)
#define yzz(v) v3(v, y, z, z)
#define yzw(v) v3(v, y, z, w)
#define ywx(v) v3(v, y, w, x)
#define ywy(v) v3(v, y, w, y)
#define ywz(v) v3(v, y, w, z)
#define yww(v) v3(v, y, w, w)
#define zxx(v) v3(v, z, x, x)
#define zxy(v) v3(v, z, x, y)
#define zxz(v) v3(v, z, x, z)
#define zxw(v) v3(v, z, x, w)
#define zyx(v) v3(v, z, y, x)
#define zyy(v) v3(v, z, y, y)
#define zyz(v) v3(v, z, y, z)
#define zyw(v) v3(v, z, y, w)
#define zzx(v) v3(v, z, z, x)
#define zzy(v) v3(v, z, z, y)
#define zzz(v) v3(v, z, z, z)
#define zzw(v) v3(v, z, z, w)
#define zwx(v) v3(v, z, w, x)
#define zwy(v) v3(v, z, w, y)
#define zwz(v) v3(v, z, w, z)
#define zww(v) v3(v, z, w, w)
#define wxx(v) v3(v, w, x, x)
#define wxy(v) v3(v, w, x, y)
#define wxz(v) v3(v, w, x, z)
#define wxw(v) v3(v, w, x, w)
#define wyx(v) v3(v, w, y, x)
#define wyy(v) v3(v, w, y, y)
#define wyz(v) v3(v, w, y, z)
#define wyw(v) v3(v, w, y, w)
#define wzx(v) v3(v, w, z, x)
#define wzy(v) v3(v, w, z, y)
#define wzz(v) v3(v, w, z, z)
#define wzw(v) v3(v, w, z, w)
#define wwx(v) v3(v, w, w, x)
#define wwy(v) v3(v, w, w, y)
#define wwz(v) v3(v, w, w, z)
#define www(v) v3(v, w, w, w)
        
#define xxxx(v) v4(v, x, x, x, x)
#define xxxy(v) v4(v, x, x, x, y)
#define xxxz(v) v4(v, x, x, x, z)
#define xxxw(v) v4(v, x, x, x, w)
#define xxyx(v) v4(v, x, x, y, x)
#define xxyy(v) v4(v, x, x, y, y)
#define xxyz(v) v4(v, x, x, y, z)
#define xxyw(v) v4(v, x, x, y, w)
#define xxzx(v) v4(v, x, x, z, x)
#define xxzy(v) v4(v, x, x, z, y)
#define xxzz(v) v4(v, x, x, z, z)
#define xxzw(v) v4(v, x, x, z, w)
#define xxwx(v) v4(v, x, x, w, x)
#define xxwy(v) v4(v, x, x, w, y)
#define xxwz(v) v4(v, x, x, w, z)
#define xxww(v) v4(v, x, x, w, w)
#define xyxx(v) v4(v, x, y, x, x)
#define xyxy(v) v4(v, x, y, x, y)
#define xyxz(v) v4(v, x, y, x, z)
#define xyxw(v) v4(v, x, y, x, w)
#define xyyx(v) v4(v, x, y, y, x)
#define xyyy(v) v4(v, x, y, y, y)
#define xyyz(v) v4(v, x, y, y, z)
#define xyyw(v) v4(v, x, y, y, w)
#define xyzx(v) v4(v, x, y, z, x)
#define xyzy(v) v4(v, x, y, z, y)
#define xyzz(v) v4(v, x, y, z, z)
#define xyzw(v) v4(v, x, y, z, w)
#define xywx(v) v4(v, x, y, w, x)
#define xywy(v) v4(v, x, y, w, y)
#define xywz(v) v4(v, x, y, w, z)
#define xyww(v) v4(v, x, y, w, w)
#define xzxx(v) v4(v, x, z, x, x)
#define xzxy(v) v4(v, x, z, x, y)
#define xzxz(v) v4(v, x, z, x, z)
#define xzxw(v) v4(v, x, z, x, w)
#define xzyx(v) v4(v, x, z, y, x)
#define xzyy(v) v4(v, x, z, y, y)
#define xzyz(v) v4(v, x, z, y, z)
#define xzyw(v) v4(v, x, z, y, w)
#define xzzx(v) v4(v, x, z, z, x)
#define xzzy(v) v4(v, x, z, z, y)
#define xzzz(v) v4(v, x, z, z, z)
#define xzzw(v) v4(v, x, z, z, w)
#define xzwx(v) v4(v, x, z, w, x)
#define xzwy(v) v4(v, x, z, w, y)
#define xzwz(v) v4(v, x, z, w, z)
#define xzww(v) v4(v, x, z, w, w)
#define xwxx(v) v4(v, x, w, x, x)
#define xwxy(v) v4(v, x, w, x, y)
#define xwxz(v) v4(v, x, w, x, z)
#define xwxw(v) v4(v, x, w, x, w)
#define xwyx(v) v4(v, x, w, y, x)
#define xwyy(v) v4(v, x, w, y, y)
#define xwyz(v) v4(v, x, w, y, z)
#define xwyw(v) v4(v, x, w, y, w)
#define xwzx(v) v4(v, x, w, z, x)
#define xwzy(v) v4(v, x, w, z, y)
#define xwzz(v) v4(v, x, w, z, z)
#define xwzw(v) v4(v, x, w, z, w)
#define xwwx(v) v4(v, x, w, w, x)
#define xwwy(v) v4(v, x, w, w, y)
#define xwwz(v) v4(v, x, w, w, z)
#define xwww(v) v4(v, x, w, w, w)
#define yxxx(v) v4(v, y, x, x, x)
#define yxxy(v) v4(v, y, x, x, y)
#define yxxz(v) v4(v, y, x, x, z)
#define yxxw(v) v4(v, y, x, x, w)
#define yxyx(v) v4(v, y, x, y, x)
#define yxyy(v) v4(v, y, x, y, y)
#define yxyz(v) v4(v, y, x, y, z)
#define yxyw(v) v4(v, y, x, y, w)
#define yxzx(v) v4(v, y, x, z, x)
#define yxzy(v) v4(v, y, x, z, y)
#define yxzz(v) v4(v, y, x, z, z)
#define yxzw(v) v4(v, y, x, z, w)
#define yxwx(v) v4(v, y, x, w, x)
#define yxwy(v) v4(v, y, x, w, y)
#define yxwz(v) v4(v, y, x, w, z)
#define yxww(v) v4(v, y, x, w, w)
#define yyxx(v) v4(v, y, y, x, x)
#define yyxy(v) v4(v, y, y, x, y)
#define yyxz(v) v4(v, y, y, x, z)
#define yyxw(v) v4(v, y, y, x, w)
#define yyyx(v) v4(v, y, y, y, x)
#define yyyy(v) v4(v, y, y, y, y)
#define yyyz(v) v4(v, y, y, y, z)
#define yyyw(v) v4(v, y, y, y, w)
#define yyzx(v) v4(v, y, y, z, x)
#define yyzy(v) v4(v, y, y, z, y)
#define yyzz(v) v4(v, y, y, z, z)
#define yyzw(v) v4(v, y, y, z, w)
#define yywx(v) v4(v, y, y, w, x)
#define yywy(v) v4(v, y, y, w, y)
#define yywz(v) v4(v, y, y, w, z)
#define yyww(v) v4(v, y, y, w, w)
#define yzxx(v) v4(v, y, z, x, x)
#define yzxy(v) v4(v, y, z, x, y)
#define yzxz(v) v4(v, y, z, x, z)
#define yzxw(v) v4(v, y, z, x, w)
#define yzyx(v) v4(v, y, z, y, x)
#define yzyy(v) v4(v, y, z, y, y)
#define yzyz(v) v4(v, y, z, y, z)
#define yzyw(v) v4(v, y, z, y, w)
#define yzzx(v) v4(v, y, z, z, x)
#define yzzy(v) v4(v, y, z, z, y)
#define yzzz(v) v4(v, y, z, z, z)
#define yzzw(v) v4(v, y, z, z, w)
#define yzwx(v) v4(v, y, z, w, x)
#define yzwy(v) v4(v, y, z, w, y)
#define yzwz(v) v4(v, y, z, w, z)
#define yzww(v) v4(v, y, z, w, w)
#define ywxx(v) v4(v, y, w, x, x)
#define ywxy(v) v4(v, y, w, x, y)
#define ywxz(v) v4(v, y, w, x, z)
#define ywxw(v) v4(v, y, w, x, w)
#define ywyx(v) v4(v, y, w, y, x)
#define ywyy(v) v4(v, y, w, y, y)
#define ywyz(v) v4(v, y, w, y, z)
#define ywyw(v) v4(v, y, w, y, w)
#define ywzx(v) v4(v, y, w, z, x)
#define ywzy(v) v4(v, y, w, z, y)
#define ywzz(v) v4(v, y, w, z, z)
#define ywzw(v) v4(v, y, w, z, w)
#define ywwx(v) v4(v, y, w, w, x)
#define ywwy(v) v4(v, y, w, w, y)
#define ywwz(v) v4(v, y, w, w, z)
#define ywww(v) v4(v, y, w, w, w)
#define zxxx(v) v4(v, z, x, x, x)
#define zxxy(v) v4(v, z, x, x, y)
#define zxxz(v) v4(v, z, x, x, z)
#define zxxw(v) v4(v, z, x, x, w)
#define zxyx(v) v4(v, z, x, y, x)
#define zxyy(v) v4(v, z, x, y, y)
#define zxyz(v) v4(v, z, x, y, z)
#define zxyw(v) v4(v, z, x, y, w)
#define zxzx(v) v4(v, z, x, z, x)
#define zxzy(v) v4(v, z, x, z, y)
#define zxzz(v) v4(v, z, x, z, z)
#define zxzw(v) v4(v, z, x, z, w)
#define zxwx(v) v4(v, z, x, w, x)
#define zxwy(v) v4(v, z, x, w, y)
#define zxwz(v) v4(v, z, x, w, z)
#define zxww(v) v4(v, z, x, w, w)
#define zyxx(v) v4(v, z, y, x, x)
#define zyxy(v) v4(v, z, y, x, y)
#define zyxz(v) v4(v, z, y, x, z)
#define zyxw(v) v4(v, z, y, x, w)
#define zyyx(v) v4(v, z, y, y, x)
#define zyyy(v) v4(v, z, y, y, y)
#define zyyz(v) v4(v, z, y, y, z)
#define zyyw(v) v4(v, z, y, y, w)
#define zyzx(v) v4(v, z, y, z, x)
#define zyzy(v) v4(v, z, y, z, y)
#define zyzz(v) v4(v, z, y, z, z)
#define zyzw(v) v4(v, z, y, z, w)
#define zywx(v) v4(v, z, y, w, x)
#define zywy(v) v4(v, z, y, w, y)
#define zywz(v) v4(v, z, y, w, z)
#define zyww(v) v4(v, z, y, w, w)
#define zzxx(v) v4(v, z, z, x, x)
#define zzxy(v) v4(v, z, z, x, y)
#define zzxz(v) v4(v, z, z, x, z)
#define zzxw(v) v4(v, z, z, x, w)
#define zzyx(v) v4(v, z, z, y, x)
#define zzyy(v) v4(v, z, z, y, y)
#define zzyz(v) v4(v, z, z, y, z)
#define zzyw(v) v4(v, z, z, y, w)
#define zzzx(v) v4(v, z, z, z, x)
#define zzzy(v) v4(v, z, z, z, y)
#define zzzz(v) v4(v, z, z, z, z)
#define zzzw(v) v4(v, z, z, z, w)
#define zzwx(v) v4(v, z, z, w, x)
#define zzwy(v) v4(v, z, z, w, y)
#define zzwz(v) v4(v, z, z, w, z)
#define zzww(v) v4(v, z, z, w, w)
#define zwxx(v) v4(v, z, w, x, x)
#define zwxy(v) v4(v, z, w, x, y)
#define zwxz(v) v4(v, z, w, x, z)
#define zwxw(v) v4(v, z, w, x, w)
#define zwyx(v) v4(v, z, w, y, x)
#define zwyy(v) v4(v, z, w, y, y)
#define zwyz(v) v4(v, z, w, y, z)
#define zwyw(v) v4(v, z, w, y, w)
#define zwzx(v) v4(v, z, w, z, x)
#define zwzy(v) v4(v, z, w, z, y)
#define zwzz(v) v4(v, z, w, z, z)
#define zwzw(v) v4(v, z, w, z, w)
#define zwwx(v) v4(v, z, w, w, x)
#define zwwy(v) v4(v, z, w, w, y)
#define zwwz(v) v4(v, z, w, w, z)
#define zwww(v) v4(v, z, w, w, w)
#define wxxx(v) v4(v, w, x, x, x)
#define wxxy(v) v4(v, w, x, x, y)
#define wxxz(v) v4(v, w, x, x, z)
#define wxxw(v) v4(v, w, x, x, w)
#define wxyx(v) v4(v, w, x, y, x)
#define wxyy(v) v4(v, w, x, y, y)
#define wxyz(v) v4(v, w, x, y, z)
#define wxyw(v) v4(v, w, x, y, w)
#define wxzx(v) v4(v, w, x, z, x)
#define wxzy(v) v4(v, w, x, z, y)
#define wxzz(v) v4(v, w, x, z, z)
#define wxzw(v) v4(v, w, x, z, w)
#define wxwx(v) v4(v, w, x, w, x)
#define wxwy(v) v4(v, w, x, w, y)
#define wxwz(v) v4(v, w, x, w, z)
#define wxww(v) v4(v, w, x, w, w)
#define wyxx(v) v4(v, w, y, x, x)
#define wyxy(v) v4(v, w, y, x, y)
#define wyxz(v) v4(v, w, y, x, z)
#define wyxw(v) v4(v, w, y, x, w)
#define wyyx(v) v4(v, w, y, y, x)
#define wyyy(v) v4(v, w, y, y, y)
#define wyyz(v) v4(v, w, y, y, z)
#define wyyw(v) v4(v, w, y, y, w)
#define wyzx(v) v4(v, w, y, z, x)
#define wyzy(v) v4(v, w, y, z, y)
#define wyzz(v) v4(v, w, y, z, z)
#define wyzw(v) v4(v, w, y, z, w)
#define wywx(v) v4(v, w, y, w, x)
#define wywy(v) v4(v, w, y, w, y)
#define wywz(v) v4(v, w, y, w, z)
#define wyww(v) v4(v, w, y, w, w)
#define wzxx(v) v4(v, w, z, x, x)
#define wzxy(v) v4(v, w, z, x, y)
#define wzxz(v) v4(v, w, z, x, z)
#define wzxw(v) v4(v, w, z, x, w)
#define wzyx(v) v4(v, w, z, y, x)
#define wzyy(v) v4(v, w, z, y, y)
#define wzyz(v) v4(v, w, z, y, z)
#define wzyw(v) v4(v, w, z, y, w)
#define wzzx(v) v4(v, w, z, z, x)
#define wzzy(v) v4(v, w, z, z, y)
#define wzzz(v) v4(v, w, z, z, z)
#define wzzw(v) v4(v, w, z, z, w)
#define wzwx(v) v4(v, w, z, w, x)
#define wzwy(v) v4(v, w, z, w, y)
#define wzwz(v) v4(v, w, z, w, z)
#define wzww(v) v4(v, w, z, w, w)
#define wwxx(v) v4(v, w, w, x, x)
#define wwxy(v) v4(v, w, w, x, y)
#define wwxz(v) v4(v, w, w, x, z)
#define wwxw(v) v4(v, w, w, x, w)
#define wwyx(v) v4(v, w, w, y, x)
#define wwyy(v) v4(v, w, w, y, y)
#define wwyz(v) v4(v, w, w, y, z)
#define wwyw(v) v4(v, w, w, y, w)
#define wwzx(v) v4(v, w, w, z, x)
#define wwzy(v) v4(v, w, w, z, y)
#define wwzz(v) v4(v, w, w, z, z)
#define wwzw(v) v4(v, w, w, z, w)
#define wwwx(v) v4(v, w, w, w, x)
#define wwwy(v) v4(v, w, w, w, y)
#define wwwz(v) v4(v, w, w, w, z)
#define wwww(v) v4(v, w, w, w, w)
        
        
#define enable_if_axis_x(x) enable_if((x) == 0)
#define enable_if_axis_xy(x) enable_if((x) == 0 || (x) == 1)
#define enable_if_axis_xyz(x) enable_if((x) == 0 || (x) == 1 || (x) == 2)
#define enable_if_axis_xyzw(x) enable_if((x) == 0 || (x) == 1 || (x) == 2 || X == 3)
        
        //vec1
        template<int X, arg_end, int C = N, enable_if(C == 1), enable_if_axis_x(X)> inline E get() const
        {
            return p[X];
        }
        
        template<int X, int Y, arg_end, int C = N, enable_if(C == 1), enable_if_axis_x(X), enable_if_axis_x(Y)> inline vector<E, 2> get() const
        {
            return vector<E, 2>(p[X], p[Y]);
        }
        
        template<int X, int Y, int Z, arg_end, int C = N, enable_if(C == 1), enable_if_axis_x(X), enable_if_axis_x(Y), enable_if_axis_x(Z)> inline vector<E, 3> get() const
        {
            return vector<E, 3>(p[X], p[Y], p[Z]);
        }
        
        template<int X, int Y, int Z, int W, arg_end, int C = N, enable_if(C == 1), enable_if_axis_x(X), enable_if_axis_x(Y), enable_if_axis_x(Z), enable_if_axis_x(W)> inline vector<E, 4> get() const
        {
            return vector<E, 4>(p[X], p[Y], p[Z], p[W]);
        }
        
        //vec2
        template<int X, arg_end, int C = N, enable_if(C == 2), enable_if_axis_xy(X)> inline E get() const
        {
            return p[X];
        }
        
        template<int X, int Y, arg_end, int C = N, enable_if(C == 2), enable_if_axis_xy(X), enable_if_axis_xy(Y)> inline vector<E, 2> get() const
        {
            return vector<E, 2>(p[X], p[Y]);
        }
        
        template<int X, int Y, int Z, arg_end, int C = N, enable_if(C == 2), enable_if_axis_xy(X), enable_if_axis_xy(Y), enable_if_axis_x(Z)> inline vector<E, 3> get() const
        {
            return vector<E, 3>(p[X], p[Y], p[Z]);
        }
        
        template<int X, int Y, int Z, int W, arg_end, int C = N, enable_if(C == 2), enable_if_axis_xy(X), enable_if_axis_xy(Y), enable_if_axis_xy(Z), enable_if_axis_xy(W)> inline vector<E, 4> get() const
        {
            return vector<E, 4>(p[X], p[Y], p[Z], p[W]);
        }
        
        //vec3
        template<int X, arg_end, int C = N, enable_if(C == 3), enable_if_axis_xyz(X)> inline E get() const
        {
            return p[X];
        }
        
        template<int X, int Y, arg_end, int C = N, enable_if(C == 3), enable_if_axis_xyz(X), enable_if_axis_xyz(Y)> inline vector<E, 2> get() const
        {
            return vector<E, 2>(p[X], p[Y]);
        }
        
        template<int X, int Y, int Z, arg_end, int C = N, enable_if(C == 3), enable_if_axis_xyz(X), enable_if_axis_xyz(Y), enable_if_axis_xyz(Z)> inline vector<E, 3> get() const
        {
            return vector<E, 3>(p[X], p[Y], p[Z]);
        }
        
        template<int X, int Y, int Z, int W, arg_end, int C = N, enable_if(C == 3), enable_if_axis_xyz(X), enable_if_axis_xyz(Y), enable_if_axis_xyz(Z), enable_if_axis_xyz(W)> inline vector<E, 4> get() const
        {
            return vector<E, 4>(p[X], p[Y], p[Z], p[W]);
        }
        
        //vec4
        template<int X, arg_end, int C = N, enable_if(C == 4), enable_if_axis_xyzw(X)> inline E get() const
        {
            return p[X];
        }
        
        template<int X, int Y, arg_end, int C = N, enable_if(C == 4), enable_if_axis_xyzw(X), enable_if_axis_xyzw(Y)> inline vector<E, 2> get() const
        {
            return vector<E, 2>(p[X], p[Y]);
        }
        
        template<int X, int Y, int Z, arg_end, int C = N, enable_if(C == 4), enable_if_axis_xyzw(X), enable_if_axis_xyzw(Y), enable_if_axis_xyzw(Z)> inline vector<E, 3> get() const
        {
            return vector<E, 3>(p[X], p[Y], p[Z]);
        }
        
        template<int X, int Y, int Z, int W, arg_end, int C = N, enable_if(C == 4), enable_if_axis_xyzw(X), enable_if_axis_xyzw(Y), enable_if_axis_xyzw(Z), enable_if_axis_xyzw(W)> inline vector<E, 4> get() const
        {
            return vector<E, 4>(p[X], p[Y], p[Z], p[W]);
        }
        
        template<typename T, enable_if_vector(T), enable_if_vector_size(T, N)>
        inline vector<E, N> operator+(T rhs) const
        {
            vector<E, N> r = *this;
            
            for (int i = 0; i < N; i++) r[i] += rhs[i];
            
            return r;
        }
        
        template<typename T, enable_if_not_vector(T)>
        inline vector<E, N> operator+(T rhs) const
        {
            return *this + vector<T, N>(rhs);
        }
        
        template<typename T>
        inline vector<E, N> operator+=(T rhs)
        {
            return (*this = *this + rhs);
        }
        
        template<typename T, enable_if_vector(T), enable_if_vector_size(T, N)>
        inline vector<E, N> operator-(T rhs) const
        {
            vector<E, N> r = *this;
            
            for (int i = 0; i < N; i++) r[i] -= rhs[i];
            
            return r;
        }
        
        template<typename T, enable_if_not_vector(T)>
        inline vector<E, N> operator-(T rhs) const
        {
            return *this - vector<T, N>(rhs);
        }
        
        template<typename T>
        inline vector<E, N> operator-=(T rhs)
        {
            return (*this = *this - rhs);
        }
        
        template<typename T, enable_if_vector(T), enable_if_vector_size(T, N)>
        inline vector<E, N> operator*(T rhs) const
        {
            vector<E, N> r = *this;
            
            for (int i = 0; i < N; i++) r[i] *= rhs[i];
            
            return r;
        }
        
        template<typename T, enable_if_not_vector(T)>
        inline vector<E, N> operator*(T rhs) const
        {
            return *this * vector<T, N>(rhs);
        }
        
        template<typename T>
        inline vector<E, N> operator*=(T rhs)
        {
            return (*this = *this * rhs);
        }
        
        template<typename T, enable_if_vector(T), enable_if_vector_size(T, N)>
        inline vector<E, N> operator/(T rhs) const
        {
            vector<E, N> r = *this;
            
            for (int i = 0; i < N; i++) r[i] /= rhs[i];
            
            return r;
        }
        
        template<typename T, enable_if_not_vector(T)>
        inline vector<E, N> operator/(T rhs) const
        {
            return *this / vector<T, N>(rhs);
        }
        
        template<typename T>
        inline vector<E, N> operator/=(T rhs)
        {
            return (*this = *this / rhs);
        }
        
        inline E sum() const
        {
            E s = 0;
            
            for (int i = 0; i < N; i++) s += p[i];
            
            return s;
        }
        
        template<typename T>
        inline E dot(T rhs) const
        {
            vector<E, N> p = *this * rhs;
            
            return p.sum();
        }
        
        inline E length() const
        {
            return sqrt(dot(*this));
        }
        
        inline vector<E, N> normalize() const
        {
            return *this / length();
        }
        
        inline vector<E, N> abs()
        {
            vector<E, N> r = *this;
            
            for (int i = 0; i < N; i++) r[i] = abs(r[i]);
            
            return r;
        }
        
#ifndef __METAL_VERSION__
        friend std::ostream &operator<<(std::ostream &os, const vector<E, N> &v)
        {
            os << "(";
            for (int i = 0; i < N - 1; i++) os << static_cast<double>(v[i]) << ", ";
            for (int i = N - 1; i < N; i++) os << static_cast<double>(v[i]);
            os << ")";
            
            os << " : vec<" << abi::__cxa_demangle(typeid(E).name(), nullptr, nullptr, nullptr) << ", " << N << ">(";
            
            for (int i = 0; i < N - 1; i++) os << v[i] << ", ";
            for (int i = N - 1; i < N; i++) os << v[i];
            
            return os << ")";
        }
#endif
    };
}

#endif

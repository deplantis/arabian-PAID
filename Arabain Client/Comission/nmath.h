#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <numbers>
#include <functional>

namespace nsl {
    template<typename T>
    concept arithmetic_type = requires(T && c) {
        { c + c };
        { c - c };
        { c* c };
        { c / c };
    };
}

namespace nsl {
    template <nsl::arithmetic_type T>
    constexpr T pow(T a, T b) {
        return b == 0 ? 1 : a * pow(a, b - 1);
    }

    template <nsl::arithmetic_type T>
    constexpr T rescale(T v, T min_from, T max_from, T min_to, T max_to) {
        return ((max_to - min_to) * (v - min_from)) / (max_from - min_from) + min_to;
    }

    template <nsl::arithmetic_type T>
    constexpr T pair(T a, T b) {
        T A = a >= 0 ? 2 * a : -2 * a - 1;
        T B = b >= 0 ? 2 * b : -2 * b - 1;
        return (A + B) * (A + B + 1) / 2 + A;
    }

    template <nsl::arithmetic_type T>
    constexpr T deg2rad(T v) {
        return ((T)v * (T)std::numbers::pi) / (T)180;
    }

    template <nsl::arithmetic_type T>
    constexpr T rad2deg(T v) {
        return ((T)v / (T)std::numbers::pi) * (T)180;
    }

    template <nsl::arithmetic_type T, std::size_t N> requires(N > 0)
        struct vec {
        constexpr static auto size = N;

        std::array<T, N> components;

        constexpr vec() : components({}) {}
        constexpr vec(std::array<T, N> c) : components(c) {}

        template <typename ...Tv> requires(sizeof...(Tv) == N)
            constexpr explicit vec(Tv&&... args) : components({ (T)std::forward<Tv>(args)... }) {}

        constexpr inline T getX() const requires(N > 0) { return components[0]; }
        constexpr inline T getY() const requires(N > 1) { return components[1]; }
        constexpr inline T getZ() const requires(N > 2) { return components[2]; }
        constexpr inline T getW() const requires(N > 3) { return components[3]; }

        constexpr inline void setX(T n) requires(N > 0) { components[0] = n; }
        constexpr inline void setY(T n) requires(N > 1) { components[1] = n; }
        constexpr inline void setZ(T n) requires(N > 2) { components[2] = n; }
        constexpr inline void setW(T n) requires(N > 3) { components[3] = n; }

        constexpr inline T operator  [](std::size_t i) const { return components[i]; }
        constexpr inline T& operator [](std::size_t i) { return components[i]; }
        constexpr inline T operator  ()(std::size_t i) const { return components[i]; }
        constexpr inline T& operator ()(std::size_t i) { return components[i]; }

        template <nsl::arithmetic_type Tv>
        constexpr operator vec<Tv, N>() const {
            std::array<Tv, N> vec;
            std::copy(vec.begin(), vec.end(), components);
            return vec;
        }

        /*
         * Expand →a(x, y, z, ...) to →a(x, y, z, w, ...)
         */
        constexpr vec<T, N + 1> expand() const {
            std::array<T, N + 1> out;
            std::copy(components.begin(), components.end(), out.begin());
            return out;
        }

        /*
         * Shrink →a(x, y, z, w, ...) to →a(x, y, z, ...)
         */
        constexpr vec<T, N - 1> shrink() const requires(N > 1) {
            std::array<T, N - 1> out;
            std::copy(components.begin(), components.end() - 1, out.begin());
            return out;
        }

        /*
         * Transform components by func
         */
        template <typename F>
        constexpr vec transform(F&& func) const {
            std::array<T, N> out;
            std::transform(components.begin(), components.end(), out.begin(), func);
            return out;
        }

        /*
         * Compute →a + →b
         */
        [[nodiscard]]
        constexpr vec operator + (const vec& vec) const {
            std::array<T, N> out;
            std::transform(components.begin(), components.end(), vec.components.begin(), out.begin(), std::plus<T>());
            return out;
        }

        /*
         * Compute →a - →b
         */
        [[nodiscard]]
        constexpr vec operator - (const vec& vec) const {
            std::array<T, N> out;
            std::transform(components.begin(), components.end(), vec.components.begin(), out.begin(), std::minus<T>());
            return out;
        }

        /*
         * Compute →a * →b
         */
        [[nodiscard]]
        constexpr vec operator * (const vec& vec) const {
            std::array<T, N> out;
            std::transform(components.begin(), components.end(), vec.components.begin(), out.begin(), std::multiplies<T>());
            return out;
        }

        /*
         * Compute →a / →b
         */
        [[nodiscard]]
        constexpr vec operator / (const vec& vec) const {
            std::array<T, N> out;
            std::transform(components.begin(), components.end(), vec.components.begin(), out.begin(), std::divides<T>());
            return out;
        }

        /*
         * Compute →a * b
         */
        [[nodiscard]]
        constexpr vec operator * (T n) const {
            return transform([&n](T v) { return v * n; });
        }

        /*
         * Compute →a / b
         */
        [[nodiscard]]
        constexpr vec operator / (T n) const {
            return transform([&n](T v) { return v / n; });
        }

        /*
         * Compute →a + →b
         */
        constexpr vec& operator += (const vec& vec) {
            return (*this = *this + vec, *this);
        }

        /*
         * Compute →a - →b
         */
        constexpr vec& operator -= (const vec& vec) {
            return (*this = *this - vec, *this);
        }

        /*
         * Compute →a * →b
         */
        constexpr vec& operator *= (const vec& vec) {
            return (*this = *this * vec, *this);
        }

        /*
         * Compute →a / →b
         */
        constexpr vec& operator /= (const vec& vec) {
            return (*this = *this / vec, *this);
        }

        /*
         * Compute →a * b
         */
        constexpr vec& operator *= (T n) {
            return (*this = *this * n, *this);
        }

        /*
         * Compute →a / b
         */
        constexpr vec& operator /= (T n) {
            return (*this = *this / n, *this);
        }

        /*
         * Transform components by deg2rad(x)
         */
        constexpr vec deg2rad() const {
            return transform([](T v) { return nsl::deg2rad(v); });
        }

        /*
         * Transform components by rad2deg(x)
         */
        constexpr vec rad2deg() const {
            return transform([](T v) { return nsl::rad2deg(v); });
        }

        constexpr bool operator == (const vec& vec) const { return components == vec.components; }
        constexpr bool operator != (const vec& vec) const { return components != vec.components; }

        /*
         * Todo: add comment
         */
        [[nodiscard]]
        constexpr T dist_sqrt(const vec& vec) const {
            T sum = 0;
            for (auto i = 0; i < N; i++) {
                sum += pow((*this)[i] - vec[i], (T)2);
            }
            return sum;
        }

        /*
         * Todo: add comment
         */
        /* constexpr */ T operator <=> (const vec& vec) const { return sqrt(dist_sqrt(vec)); }

        /*
         * Compute →a · →b
         */
        [[nodiscard]]
        constexpr T dot(const vec& vec) const {
            return std::inner_product(components.begin(), components.end(), vec.components.begin(), T());
        }

        /*
         * Compute →a ✕ →b
         * Only exists in 3D
         */
        constexpr vec cross(const vec& vec) const requires(N == 3) {
            return vec(
                (*this)[1] * vec[2] - (*this)[2] * vec[1],
                (*this)[2] * vec[0] - (*this)[0] * vec[2],
                (*this)[0] * vec[1] - (*this)[1] * vec[0]
            );
        }

        /*
         * Todo: add comment
         */
        [[nodiscard]]
        constexpr T length_sqrt() const {
            return std::accumulate(components.begin(), components.end(), T(), [](T a, T b) {
                return pow(b, (T)2);
                });
        }

        /* constexpr */ vec normalize() const {
            T len = length_sqrt();
            if (len > 0) return *this / std::sqrt(len);
            else return vec();
        }

        /*
         * Todo: add comment
         */
        [[nodiscard]]
        /* constexpr */ T length() const {
            return sqrt(length_sqrt());
        }

        vec direction(T n) const requires(N == 2) {
            return vec(std::cos(n), std::sin(n));
        }

        /* constexpr */ std::string to_string() {
            std::string fmt;

            for (std::size_t i = 0; i < components.size(); i++) {
                if (i != 0) fmt.append(", ");
                fmt.append(std::to_string(components[i]));
            }

            std::string out(fmt.size() + 32, '\0');
            out.resize(std::sprintf(&out[0], "Vector%lli(%s)", N, &fmt[0]));

            return out;
        }
    };

    template <std::size_t N>
    using veci = vec<int, N>;
    template <std::size_t N>
    using vecf = vec<float, N>;
    template <std::size_t N>
    using vecd = vec<double, N>;

    using vec2i = veci<2>;
    using vec2f = vecf<2>;
    using vec2d = vecd<2>;
    using vec3i = veci<3>;
    using vec3f = vecf<3>;
    using vec3d = vecd<3>;
    using vec4i = veci<4>;
    using vec4f = vecf<4>;
    using vec4d = vecd<4>;

    template <nsl::arithmetic_type T, std::size_t C, std::size_t R> requires(C > 0 && R > 0)
        struct mat {
        static constexpr auto cols = C;
        static constexpr auto rows = R;

        std::array<T, C* R> components;

        constexpr mat() : components({}) {}
        constexpr mat(std::array<T, C* R> c) : components(c) {}

        template <typename ...Tv> requires(sizeof...(Tv) == C * R)
            constexpr explicit mat(Tv&&... args) : components({ (T)std::forward<Tv>(args)... }) {}

        constexpr inline T operator  [](std::size_t i) const { return components[i]; }
        constexpr inline T& operator [](std::size_t i) { return components[i]; }
        constexpr inline T operator  ()(std::size_t c, std::size_t r) const { return components[c * C + r]; }
        constexpr inline T& operator ()(std::size_t c, std::size_t r) { return components[c * C + r]; }

        constexpr void set(std::array<T, C* R> mat) { components = mat; }

        constexpr void set_row(std::size_t i, vec<T, R> r) {
            for (auto j = 0; j < R; j++) {
                (*this)(i, j) = r[j];
            }
        }

        constexpr bool is_square() const { return C == R; };

        template <nsl::arithmetic_type Tv>
        constexpr operator mat<Tv, C, R>() const {
            std::array<Tv, C* R> mat;
            std::copy(mat.begin(), mat.end(), components);
            return mat;
        }

        template <nsl::arithmetic_type Tv>
        constexpr operator vec<Tv, R>() const requires(C == 1) {
            std::array<Tv, R> vec;
            std::copy(vec.begin(), vec.end(), components);
            return vec;
        }

        /*
         * Transform components by func
         */
        template <typename F>
        constexpr mat transform(F&& func) const {
            std::array<T, C* R> out;
            std::transform(components.begin(), components.end(), out.begin(), func);
            return out;
        }

        /*
         * Compute a(i,j) + b(i,j)
         */
        [[nodiscard]]
        constexpr mat operator + (const mat& mat) const {
            std::array<T, C* R> out;
            std::transform(components.begin(), components.end(), mat.components.begin(), out.begin(), std::plus<T>());
            return out;
        }

        /*
         * Compute a(i,j) - b(i,j)
         */
        [[nodiscard]]
        constexpr mat operator - (const mat& mat) const {
            std::array<T, C* R> out;
            std::transform(components.begin(), components.end(), mat.components.begin(), out.begin(), std::minus<T>());
            return out;
        }

        /*
         * Compute a(i,j) * b(i,j)
         */
        [[nodiscard]]
        constexpr mat operator * (const mat& mat) const {
            std::array<T, C* R> out;
            std::transform(components.begin(), components.end(), mat.components.begin(), out.begin(), std::multiplies<T>());
            return out;
        }

        /*
         * Compute a(i,j) / b(i,j)
         */
        [[nodiscard]]
        constexpr mat operator / (const mat& mat) const {
            std::array<T, C* R> out;
            std::transform(components.begin(), components.end(), mat.components.begin(), out.begin(), std::divides<T>());
            return out;
        }

        /*
         * Compute a(i,j) * →b
         */
        [[nodiscard]]
        constexpr vec<T, R> operator * (const vec<T, R>& v) const requires (C == R) {
            vec<T, R> out;

            for (auto i = 0; i < C; i++) {
                for (auto j = 0; j < R; j++) {
                    out[i] += v[j] * (*this)(i, j);
                }
            }

            return out;
        }

        /*
         * Compute a(i,j) * b
         */
        [[nodiscard]]
        constexpr mat operator * (T n) const {
            return transform([&n](T v) { return v * n; });
        }

        /*
         * Compute a(i,j) / b
         */
        [[nodiscard]]
        constexpr mat operator / (T n) const {
            return transform([&n](T v) { return v / n; });
        }

        /*
         * Compute a(i,j) + b(i,j)
         */
        constexpr mat& operator += (const mat& mat) {
            return (*this = *this + mat, *this);
        }

        /*
         * Compute a(i,j) - b(i,j)
         */
        constexpr mat& operator -= (const mat& mat) {
            return (*this = *this - mat, *this);
        }

        /*
         * Compute a(i,j) * b(i,j)
         */
        constexpr mat& operator *= (const mat& mat) {
            return (*this = *this * mat, *this);
        }

        /*
         * Compute a(i,j) / b(i,j)
         */
        constexpr mat& operator /= (const mat& mat) {
            return (*this = *this / mat, *this);
        }

        /*
         * Compute a(i,j) * b
         */
        constexpr mat& operator *= (T n) {
            return (*this = *this * n, *this);
        }

        /*
         * Compute a(i,j) / b
         */
        constexpr mat& operator /= (T n) {
            return (*this = *this / n, *this);
        }

        /*
         * Transpose a(i,j) -> a(j, i)
         */
        constexpr mat<T, R, C> transpose() {
            mat<T, R, C> out;

            for (auto i = 0; i < C; i++) {
                for (auto j = 0; j < R; j++) {
                    out(j, i) = (*this)(i, j);
                }
            }

            return out;
        }

        static mat identity() requires (C == R) {
            mat out;
            for (auto i = 0; i < C; i++) {
                out(i, i) = 1;
            }
            return out;
        }

        static constexpr mat scale(vec<T, 3> v) requires(C == 4 && R == 4) {
            return scale(v[0], v[1], v[2]);
        }

        static constexpr mat scale(T x, T y, T z) requires(C == 4 && R == 4) {
            mat out = mat::identity();
            out(0, 0) = x;
            out(1, 1) = y;
            out(2, 2) = z;
            return out;
        }

        static constexpr mat rotate(vec<T, 3> v, T angle) requires(C == 4 && R == 4) {
            return rotate(v[0], v[1], v[2], angle);
        }

        static constexpr mat rotate(T x, T y, T z, T angle) requires(C == 4 && R == 4) {
            mat out = mat::identity();
            out(0, 0) = std::cos(-angle) * y * z;
            out(0, 1) = -std::sin(-angle) * z;
            out(0, 2) = std::sin(-angle) * y;
            out(1, 0) = std::sin(-angle) * z;
            out(1, 1) = std::cos(-angle) * x * z;
            out(1, 2) = -std::sin(-angle) * x;
            out(2, 0) = -std::sin(-angle) * y;
            out(2, 1) = std::sin(-angle) * x;
            out(2, 2) = std::cos(-angle) * x * y;

            if (y == 0 and z == 0) out(0, 0) = 1;
            if (x == 0 and z == 0) out(1, 1) = 1;
            if (x == 0 and y == 0) out(2, 2) = 1;

            return out;
        }

        constexpr mat<T, C - 1, R - 1> sub_matrix(std::size_t r, std::size_t c) const {
            mat<T, C - 1, R - 1> out;

            for (auto i = 0, ri = 0; i < C; i++) {
                if (i == r) continue;
                else {
                    for (auto j = 0, rj = 0; j < R; j++) {
                        if (j == c) continue;
                        else {
                            out(ri, rj++) = (*this)(i, j);
                        }
                    }
                    ri++;
                }
            }

            return out;
        }

        constexpr T determinant() const requires(C == R) {
            if constexpr (C == 1) {
                return (*this)[0];
            }
            else if constexpr (C == 2) {
                return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
            }
            else {
                T det = 0;

                for (auto i = 0; i < R; i++) {
                    det += (*this)(0, i) * (i % 2 == 0 ? 1 : -1) * sub_matrix(0, i).determinant();
                }

                return det;
            }
        }

        /*
         * template<typename T, size_t N>
constexpr auto comatrix(const Matrix<T, N> m)
{
    using T_OUT = decltype(m);
    if constexpr (N <= 0)
        return {};
    else if constexpr(N == 1)
        return 1;
    else if constexpr(N == 2)
        return T_OUT(
        {
            {m[1][1], - m[1][0]},
            {- m[0][1],  m[0][0]}
        });
    else
    {
        auto out = T_OUT{};
        for(size_t i = 0 ; i < N ; ++i)
        {
            for(size_t j = 0 ; j < N ; ++j)
            {
                out[i][j] += ((i + j) % 2 == 0 ? 1 : -1)
            * det(create_submatrix(m, i, j));
            }
        }
        return out;
    }
}
         * constexpr auto inverse(const Matrix<T, N> &m)
        {
            //return comatrix(m);
            return (1 / double(det(m)))
                   * Matrix<double, N>(transposed(comatrix(m)));
        }*/

        /* constexpr */ std::string to_string() {
            std::string fmt;

            for (std::size_t i = 0; i < components.size(); i++) {
                if (i != 0) fmt.append(", ");
                fmt.append(std::to_string(components[i]));
            }

            std::string out(fmt.size() + 32, '\0');
            out.resize(std::sprintf(&out[0], "Matrix%llix%lli[%s]", C, R, &fmt[0]));

            return out;
        }
    };

    template <std::size_t C, std::size_t R>
    using mati = mat<int, C, R>;
    template <std::size_t C, std::size_t R>
    using matf = mat<float, C, R>;
    template <std::size_t C, std::size_t R>
    using matd = mat<double, C, R>;

    using mat2i = mati<2, 2>;
    using mat2f = matf<2, 2>;
    using mat2d = matd<2, 2>;
    using mat3i = mati<3, 3>;
    using mat3f = matf<3, 3>;
    using mat3d = matd<3, 3>;
    using mat4i = mati<4, 4>;
    using mat4f = matf<4, 4>;
    using mat4d = matd<4, 4>;
}

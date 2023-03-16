#pragma once

/*
MIT License

Copyright(c) 2020 Jaime Cornejo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstddef>

using h32 = unsigned int;
using h64 = unsigned long long;

constexpr h32 INVALID_HASH_32 = 0u;
constexpr h64 INVALID_HASH_64 = 0ull;

namespace ct_hash_internal
{
    // FNV Hash 32 bit
    constexpr h32 FNV_PRIME_32 = 0x1000193;
    constexpr h32 FNV_OFFSET_BASIS_32 = 0x811C9DC5;

    constexpr h32 fnv1a_32( char const byte, h32 const hash = FNV_OFFSET_BASIS_32 )
    {
        return h64( hash ^ byte ) * FNV_PRIME_32;
    }

    constexpr h32 fnv1a_32( char const* string, h32 const hash = FNV_OFFSET_BASIS_32 )
    {
        return ( string[0] == '\0' ) ? hash : fnv1a_32( string + 1, fnv1a_32( string[0], hash ) );
    }

    h32 fnv1a_32( void const* bytes, std::size_t const num_bytes, h32 hash = FNV_OFFSET_BASIS_32 )
    {
        unsigned char const* bytes8 = reinterpret_cast<unsigned char const*>( bytes );
        for( std::size_t i = 0; i < num_bytes; ++i )
            hash = fnv1a_32( bytes8[i], hash );

        return hash;
    }

    // FNV Hash 64 bit (need to define 'h64 * h64' to avoid C4307 - Integral constant overflow).
    constexpr h64 FNV_PRIME_64 = 0x100000001B3;
    constexpr h64 FNV_OFFSET_BASIS_64 = 0xCBF29CE484222325;

    constexpr h64 lo( h64 const val ) { return val & 0xFFFFFFFF; }
    constexpr h64 hi( h64 const val ) { return val >> 32; }
    constexpr h64 multiply_h64( h64 const lhs, h64 const rhs )
    {
        h64 const lo_mult = ( lo( lhs ) * lo( rhs ) ) & 0xFFFFFFFF;
        h64 const aux_0 = ( hi( lo( lhs ) * lo( rhs ) ) + lo( lhs ) * hi( rhs ) ) & 0xFFFFFFFF;
        h64 const hi_mult = ( aux_0 + hi( lhs ) * lo( rhs ) ) & 0xFFFFFFFF;
        return lo_mult | ( hi_mult << 32 );
    }

    constexpr h64 fnv1a_64( char const byte, h64 const hash = FNV_OFFSET_BASIS_64 )
    {
        return multiply_h64(( hash ^ byte ), FNV_PRIME_64);
    }

    constexpr h64 fnv1a_64( char const* string, h64 const hash = FNV_OFFSET_BASIS_64 )
    {
        return ( string[0] == '\0' ) ? hash : fnv1a_64( string + 1, fnv1a_64( string[0], hash ) );
    }

    h64 fnv1a_64( void const* bytes, std::size_t const num_bytes, h64 hash = FNV_OFFSET_BASIS_64 )
    {
        unsigned char const* bytes8 = reinterpret_cast<unsigned char const*>( bytes );
        for( std::size_t i = 0; i < num_bytes; ++i )
            hash = fnv1a_64( bytes8[i], hash );

        return hash;
    }
}

// Client code interface "create_hash_32" and "create_hash_64"
template<typename T>
constexpr h32 create_hash_32( T* const value );

template<typename T>
constexpr h64 create_hash_64( T* const value );

template<typename T>
h32 create_hash_32( T const value )
{
    return ct_hash_internal::fnv1a_32( &value, sizeof( value ), ct_hash_internal::FNV_OFFSET_BASIS_32 );
}

template<typename T>
h64 create_hash_64( T const value )
{
    return ct_hash_internal::fnv1a_64( &value, sizeof( value ), ct_hash_internal::FNV_OFFSET_BASIS_64 );
}

template<>
constexpr h32 create_hash_32<char const>( char const* string )
{
    h32 ret = ct_hash_internal::FNV_OFFSET_BASIS_32;
    while ( *string != '\0' )
        ret = ct_hash_internal::fnv1a_32( *string++, ret );
    return ret;
}

template<>
constexpr h32 create_hash_32<char>( char* string ) { return create_hash_32<char const>( string ); }

template<>
constexpr h64 create_hash_64<char const>( char const* string )
{
    h64 ret = ct_hash_internal::FNV_OFFSET_BASIS_64;
    while ( *string != '\0' )
        ret = ct_hash_internal::fnv1a_64( *string++, ret );
    return ret;
}

template<>
constexpr h64 create_hash_64<char>( char* string ) { return create_hash_64<char const>( string ); }

// User-defined Literals
h32 operator"" _h32( long double d )                                    { return create_hash_32( d ); }
h32 operator"" _h32( unsigned long long n )                             { return create_hash_32( n ); }
constexpr h32 operator"" _h32( char c )                                 { return ct_hash_internal::fnv1a_32( c ); }
constexpr h32 operator"" _h32( char const* string )                     { return ( string[0] == '\0' ) ? INVALID_HASH_32 : ct_hash_internal::fnv1a_32( string ); }
constexpr h32 operator"" _h32( char const* string, std::size_t size )   { return ( size == 0 ) ? INVALID_HASH_32 : ct_hash_internal::fnv1a_32( string ); }

h64 operator"" _h64( long double d )                                    { return create_hash_64( d ); }
h64 operator"" _h64( unsigned long long n )                             { return create_hash_64( n ); }
constexpr h64 operator"" _h64( char c )                                 { return ct_hash_internal::fnv1a_64( c ); }
constexpr h64 operator"" _h64( char const* string )                     { return ( string[0] == '\0' ) ? INVALID_HASH_64 : ct_hash_internal::fnv1a_64( string ); }
constexpr h64 operator"" _h64( char const* string, std::size_t size )   { return ( size == 0 ) ? INVALID_HASH_64 : ct_hash_internal::fnv1a_64( string ); }


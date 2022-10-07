#pragma once

#include <cstddef>

using h32 = unsigned int;
using h64 = unsigned long long;

constexpr h32 INVALID_HASH_32 = 0u;
constexpr h64 INVALID_HASH_64 = 0ull;

namespace ctHash_Internal
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
        char const* bytes8 = reinterpret_cast<char const*>( bytes );
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
        char const* bytes8 = reinterpret_cast<char const*>( bytes );
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
    return ctHash_Internal::fnv1a_32( &value, sizeof( value ), ctHash_Internal::FNV_OFFSET_BASIS_32 );
}

template<typename T>
h64 create_hash_64( T const value )
{
    return ctHash_Internal::fnv1a_64( &value, sizeof( value ), ctHash_Internal::FNV_OFFSET_BASIS_64 );
}

template<>
constexpr h32 create_hash_32<char const>( char const* string )
{
    h32 ret = ctHash_Internal::FNV_OFFSET_BASIS_32;
    while ( *string != '\0' )
        ret = ctHash_Internal::fnv1a_32( *string++, ret );
    return ret;
}

template<>
constexpr h32 create_hash_32<char>( char* string ) { return create_hash_32<char const>( string ); }

template<>
constexpr h64 create_hash_64<char const>( char const* string )
{
    h64 ret = ctHash_Internal::FNV_OFFSET_BASIS_64;
    while ( *string != '\0' )
        ret = ctHash_Internal::fnv1a_64( *string++, ret );
    return ret;
}

template<>
constexpr h64 create_hash_64<char>( char* string ) { return create_hash_64<char const>( string ); }

// Literals
h32 operator"" _h32( long double d )                                    { return create_hash_32( d ); }
h32 operator"" _h32( unsigned long long n )                             { return create_hash_32( n ); }
constexpr h32 operator"" _h32( char c )                                 { return ctHash_Internal::fnv1a_32( c ); }
constexpr h32 operator"" _h32( char const* string )                     { return ( string[0] == '\0' ) ? INVALID_HASH_32 : ctHash_Internal::fnv1a_32( string ); }
constexpr h32 operator"" _h32( char const* string, std::size_t size )   { return ( size == 0 ) ? INVALID_HASH_32 : ctHash_Internal::fnv1a_32( string ); }

h64 operator"" _h64( long double d )                                    { return create_hash_64( d ); }
h64 operator"" _h64( unsigned long long n )                             { return create_hash_64( n ); }
constexpr h64 operator"" _h64( char c )                                 { return ctHash_Internal::fnv1a_64( c ); }
constexpr h64 operator"" _h64( char const* string )                     { return ( string[0] == '\0' ) ? INVALID_HASH_64 : ctHash_Internal::fnv1a_64( string ); }
constexpr h64 operator"" _h64( char const* string, std::size_t size )   { return ( size == 0 ) ? INVALID_HASH_64 : ctHash_Internal::fnv1a_64( string ); }


#include <iostream>

#include "hash.h"

int main( int argc, char* argv[] )
{
	// Compile Time 32 const string hashing
	char const* string = "ThisIsAString";
	auto const string_hash = create_hash_32( string );
	std::cout << " sh => " << string_hash << std::endl;
	
	h32 arg1 = INVALID_HASH_32;
    if ( argc > 1 )
        arg1 = create_hash_32( argv[1] ); // Runtime 32 hashing

	int ret = -1;
	constexpr h32 skip64Command = "DoNotTest64"_h32; // Compile Time 32 hashing
	std::cout << arg1 << " == " << skip64Command << std::endl;
    
	if (arg1 != skip64Command)
	{
		// Compile Time 64 Hashing
		auto const str = create_hash_64("YourString");

		switch (str)
		{
			case "MyString"_h64: 	ret = 0; break;
			case "YourString"_h64: 	ret = 1; break;
			case "TheirString"_h64: ret = 2; break;
			default:
				// Nothing to do here
				break;
		}

		std::cout << str << " == " << "MyString"_h64 << " ret == " << ret << std::endl;
	}

	// Integral type hashing	
	h32 const d32 = create_hash_32(33453242342ull);
	h32 const d64 = create_hash_64(33453242342ull);
	std::cout << " d32 => " << d32 << " d64 => " << d64 << std::endl;

	// User defined hashing
	struct PODType
	{
		int x, y;
		float f;
		bool b0, b1, b2;
	};

	PODType pod = { 1, 2, 3.4f, true, true, false };
	PODType const pod_2 = { 12, 22, 32.4f, false, false, false };

	auto const pod_hash = create_hash_64( pod );
	auto const pod_hash_2 = create_hash_64( pod_2 );
	std::cout << " pod => " << pod_hash << " pod_2 => " << pod_hash_2 << std::endl;
	
    return ret;
}
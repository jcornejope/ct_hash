# ct_hash

## Single Header, Compile Time Hash library.

The main use of this simple library is to provide a **compile time solution for string hashing**.  
The library supports **32 and 64 bit** hashes. 

~~~
auto const str = create_hash_64( "YourString" );
auto const arg1 = create_hash_32( argv[1] );
~~~

On top of that it allows for compile time and run time integral types or user created types to be hashed.  

~~~
struct PODType{ int x,y; } pod = { 1, 2 };
constexpr h64 pod_hash = create_hash_64( pod );
~~~

It includes handy string literal operator overrides which allows easy to read and maintain code. Supporting advance code blocks with minor overhead language, like switch statements with strings literals.

~~~
switch (str)
{
	case "MyString"_h64: 	ret = 0; break;
	case "YourString"_h64: 	ret = 1; break;
	case "TheirString"_h64: ret = 2; break;
	default:
		// Nothing to do here
		break;
}
~~~

It requires C++14 to allow support for the sting literals feature.

## Integration

Simply copy the **.h** onto your code base. The library defines the types **h32** and **h64** and the string literals **_h32** and **_h64** so if you have somehow those types defined you will need some minor changes.

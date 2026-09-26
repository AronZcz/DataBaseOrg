#include <variant>
#include <string>
#include <vector>
#include <map>


/*
	DESCRIPTION:

	
*/

enum class FLAG : std::uint8_t
{
	PK = 1u << 0,			// 0b00000001
	UNIQUE = 1u << 1,		// 0b00000010
	NOT_NULL = 1u << 2,		// 0b00000100
	AUTO_INC = 1u << 3,		// 0b00001000
	FK = 1u << 4			// 0b00010000
};

class MetadataByte
{
private:
	std::uint8_t metadata { 0b0000000 };
	std::uint8_t d_type{ 0u };
public:
	template<std::size_t N>
	MetadataByte(const FLAG (&flags)[N])
	{
		for (FLAG flag : flags)
		{
			metadata |= static_cast<std::uint8_t>(flag);
		}
	}

	// PROGRAM CAN CHECK WHAT PROPERTIES HAVE COLUMN;
	bool has(FLAG flag) const
	{
		return (metadata & static_cast<std::uint8_t>(flag)) != 0;
	}

	// PROGRAM CAN CHECK TYPE AND DISPLAY CORRECT ONE;
	std::uint8_t Type()
	{
		return (static_cast<std::uint8_t>(d_type));
	}
};

#pragma once
class Column
{
	
};

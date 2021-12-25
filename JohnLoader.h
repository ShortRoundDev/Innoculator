#pragma once

#include "JohnTypes.h"

#include <string>

class JohnLoader
{
public:
	JohnLoader();
	~JohnLoader();

	_Success_(return)
	BOOL loadFile(std::string path, _Out_ LevelToken** token);

private:
	void fixPointers(LevelToken* level);

	template<typename T>
	T* fixPointer(T* in, UINT64 offset);
};


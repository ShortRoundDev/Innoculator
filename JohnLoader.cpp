#include "JohnLoader.h"

JohnLoader::JohnLoader()
{

}

JohnLoader::~JohnLoader()
{

}

_Success_(return)
BOOL JohnLoader::loadFile(std::string path, _Out_ LevelToken** token)
{
	HANDLE file = CreateFileA(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD size = GetFileSize(file, NULL);

	BYTE* buffer = (BYTE*) HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		size
	);
	if (buffer == NULL) {
		CloseHandle(file);
		return false;
	}

	BOOL read = ReadFile(
		file,
		buffer,
		size,
		NULL, NULL
	);
	if (read == FALSE) {
		CloseHandle(file);
		HeapFree(GetProcessHeap(), 0, buffer);
		return false;
	}
	CloseHandle(file);

	*token = (LevelToken*)buffer;

	if (!(
		(*token)->watermark[0] == 'H' &&
		(*token)->watermark[1] == 'A' &&
		(*token)->watermark[2] == 'M'
	)) {
		HeapFree(GetProcessHeap(), 0, buffer);
		*token = NULL;
		return false;
	}

	fixPointers(*token);
	return true;
}


void JohnLoader::fixPointers(LevelToken* level)
{
	UINT64 offset = (UINT64)level; // convert to mem offset;
	level->walls	= fixPointer(level->walls, offset);
	level->entities	= fixPointer(level->entities, offset);

	for (int i = 0; i < level->width * level->height; i++) {
		WallToken* wall = level->walls + i;
		wall->message = fixPointer(wall->message, offset);
	}

	for (UINT64 i = 0; i < level->totalEntities; i++) {
		EntityToken* entity = level->entities + i;
		entity->config = fixPointer(entity->config, offset);
	}
}

template<typename T>
T* JohnLoader::fixPointer(T* in, UINT64 offset)
{
	return (T*)(((BYTE*)in) + offset);
}
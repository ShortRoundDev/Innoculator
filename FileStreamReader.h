#pragma once
#include <algorithm>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <fstream>
#include <iostream>

using namespace Microsoft;

class FileStreamReader : public glTF::IStreamReader
{
public:
	FileStreamReader()
	{

	}

	~FileStreamReader()
	{

	}

	std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override
	{
		std::shared_ptr<std::fstream> file = std::make_shared<std::fstream>(filename, std::ios::in | std::ios::binary);
		if (!file->is_open()) {
			return nullptr;
		}
		return file;
	}

};

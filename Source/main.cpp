#include <filesystem>
#include <fstream>
#include <sstream>

#include "libs/rapidjson/document.h"

#define PATCHED_IDENTIFIER "_patched"

using BYTE = char;

struct Data
{
	Data(const BYTE& offset, std::vector<BYTE>&& value)
		: m_offset(offset)
		, m_value(value) {}
	
	BYTE m_offset;
	std::vector<BYTE> m_value;
};

int main(int argc, char* argv[])
{
	std::vector<Data> data;
	
	// Read config file
	rapidjson::Document json_document;
	const std::filesystem::path config_path("C:\\Sandbox\\FilePatcher\\config.json");
	if(std::filesystem::exists(config_path))
	{
		std::ifstream config_file(config_path);

		std::stringstream buffer;
		buffer << config_file.rdbuf();
		
		json_document.Parse(buffer.str().c_str());

		for (auto& element : json_document["data"].GetArray())
		{
			const BYTE& start_offset = std::stoul(element["start_offset"].GetString(), nullptr, 16);
			std::vector<BYTE>&& byte_values = std::vector<BYTE>();

			const auto& values = element["values"].GetArray();

			for (int i = 0; i < values.Size(); ++i)
				byte_values.push_back(std::stoul(values[i].GetString(), nullptr, 16));
			
			const Data& newData = Data(start_offset, std::move(byte_values));
			data.push_back(newData);
		}
	}
	
	const std::filesystem::path path = std::filesystem::path(argv[argc-1]);
	const std::string& file_name = path.stem().string(); 
	const std::string& extension = path.extension().string();
	
	std::string&& patched_file_name = (file_name + PATCHED_IDENTIFIER + extension).c_str();
	std::ofstream patched_file(patched_file_name.c_str(), std::fstream::binary);

	std::ifstream file_to_patch(path, std::fstream::binary);
	file_to_patch.seekg(0, std::ios::end);
	const size_t bytes_to_read = file_to_patch.tellg();
	file_to_patch.seekg(0, std::ios::beg);

	for (auto current_offset = 0; current_offset < bytes_to_read; ++current_offset)
	{
		BYTE byte = file_to_patch.get();
		patched_file.write(reinterpret_cast<BYTE*>(&byte), sizeof(BYTE));
	}

	return 0;
}

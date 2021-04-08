#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "libs/rapidjson/document.h"

using BYTE = char;

struct Data
{
	Data(const BYTE& offset, std::vector<BYTE>&& value)
		: m_start_offset(offset)
		, m_value(value) {}
	
	BYTE m_start_offset;
	std::vector<BYTE> m_value;
};

int main(int argc, char* argv[])
{
	std::vector<Data> data;

	bool bBackup = true;
	
	// Read config file
	rapidjson::Document json_document;
	const std::filesystem::path config_path("C:\\Sandbox\\FilePatcher\\config.json");
	if(std::filesystem::exists(config_path))
	{
		std::ifstream config_file(config_path);

		std::stringstream buffer;
		buffer << config_file.rdbuf();

		json_document.Parse(buffer.str().c_str());

		bBackup = json_document["backup"].GetBool();
		
		for (auto& element : json_document["data"].GetArray())
		{
			const BYTE& start_offset = std::stoi(element["start_offset"].GetString(), nullptr, 16);
			std::vector<BYTE> byte_values;

			const auto& values = element["values"].GetArray();

			for (int i = 0; i < values.Size(); ++i)
				byte_values.push_back(std::stoi(values[i].GetString(), nullptr, 16));
			
			const Data& newData = Data(start_offset, std::move(byte_values));
			data.push_back(newData);
		}
	}
	
	const std::filesystem::path& path = std::filesystem::path(argv[argc-1]);
	const std::string& file_name = path.stem().string(); 
	const std::string& extension = path.extension().string();

	std::ifstream file_to_patch(path, std::ifstream::binary);
	std::stringstream buffer;
	buffer << file_to_patch.rdbuf();
	
	if(bBackup)
	{
		file_to_patch.seekg(0);
		
		const std::string& backup_file_name = (file_name + extension + ".bak").c_str();
		std::ofstream backup_file(backup_file_name.c_str(), std::ostream::binary);
		backup_file << file_to_patch.rdbuf();
	}

	for (const auto& element : data)
	{
		const BYTE& start_offset = element.m_start_offset;

		buffer.seekp(start_offset);

		for (int i = 0; i < element.m_value.size(); ++i)
			buffer.write(&element.m_value[i], 1);	

		buffer.seekp(0);
	}

	const std::string& patched_file_name = (file_name + extension).c_str();
	std::ofstream patched_file(patched_file_name.c_str(), std::ostream::binary);
	patched_file << buffer.rdbuf();

	return 0;
}

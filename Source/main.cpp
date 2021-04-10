#include <filesystem>
#include <fstream>
#include <sstream>
#include <Windows.h>

#include "ThirdParty/rapidjson/document.h"

struct Data
{
	Data(const BYTE& offset, std::vector<BYTE>&& value)
		: m_start_offset(offset)
		, m_value(value) {}
	
	BYTE m_start_offset;
	std::vector<BYTE> m_value;
};

void ParseConfig(const std::filesystem::path& config_path, std::vector<Data>& data, bool& bBackup)
{
	// Read config file
	rapidjson::Document json_document;
	std::ifstream config_file(config_path);

	std::stringstream json_buffer;
	json_buffer << config_file.rdbuf();

	json_document.Parse(json_buffer.str().c_str());

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

int main(int argc, char* argv[])
{
	// No arg provided, only the default module name
	if(argc == 1)
		return 1;

	// Config lives in the same place as .exe, so we need to get the module path
	TCHAR module_file_name[MAX_PATH]; 
	::GetModuleFileName(nullptr, module_file_name, MAX_PATH);
	const std::filesystem::path module_path(std::string(std::begin(module_file_name), std::end(module_file_name)));
	
	const std::filesystem::path& file_to_patch_path = std::filesystem::path(argv[argc-1]);
	const std::filesystem::path config_path(module_path.parent_path().string() + "\\config.json");

	if(!std::filesystem::exists(file_to_patch_path) || !std::filesystem::exists(config_path))
		return 1;

	bool bBackup = true;
	std::vector<Data> data;
	ParseConfig(config_path, data, bBackup);
	
	const std::string& file_name = file_to_patch_path.stem().string(); 
	const std::string& extension = file_to_patch_path.extension().string();

	std::ifstream file_to_patch(file_to_patch_path, std::ifstream::binary);
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
			buffer.write((char*)&element.m_value[i], 1);	

		buffer.seekp(0);
	}

	const std::string& patched_file_name = (file_name + extension).c_str();
	std::ofstream patched_file(patched_file_name.c_str(), std::ostream::binary);
	patched_file << buffer.rdbuf();

	return 0;
}

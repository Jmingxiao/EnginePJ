#pragma once

namespace Acg
{
	namespace file
	{
		std::string normalise(const std::string& file_name);
		std::string file_stem(const std::string& file_name);
		std::string file_extension(const std::string& file_name);
		std::string change_extension(const std::string& file_name, const std::string& ext);
		std::string parent_path(const std::string& file_name);
	} // namespace file
}
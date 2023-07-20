#include "pch.h"
#include "util.h"

std::string Acg::file::normalise(const std::string& file_name)
{
	std::string nname;
	nname.reserve(file_name.size());
	for (const char c : file_name)
	{
		if (c == '\\')
		{
			if (nname.back() != '/')
			{
				nname += '/';
			}
		}
		else
		{
			nname += c;
		}
	}
	return nname;
}

std::string Acg::file::file_stem(const std::string& file_name)
{
	size_t slash = file_name.find_last_of("\\/");
	size_t dot = file_name.find_last_of(".");
	if (slash != std::string::npos)
	{
		return file_name.substr(slash + 1, dot - slash - 1);
	}
	else
	{
		return file_name.substr(0, dot);
	}
}

std::string Acg::file::file_extension(const std::string& file_name)
{
	size_t separator = file_name.find_last_of(".");
	if (separator == std::string::npos)
	{
		return "";
	}
	else
	{
		return file_name.substr(separator);
	}
}

std::string Acg::file::change_extension(const std::string& file_name, const std::string& ext)
{
	size_t separator = file_name.find_last_of(".");
	if (separator == std::string::npos)
	{
		return file_name + ext;
	}
	else
	{
		return file_name.substr(0, separator) + ext;
	}
}

std::string Acg::file::parent_path(const std::string& file_name)
{
	size_t separator = file_name.find_last_of("\\/");
	if (separator != std::string::npos)
	{
		return file_name.substr(0, separator + 1);
	}
	else
	{
		return "./";
	}
}

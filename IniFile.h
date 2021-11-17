#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <exception>
#include <algorithm>

class IniFile {
	std::map<
		std::string, 
		std::map<std::string, std::string>
	> _data;
	
	std::vector<std::string> _boolValues = { "true", "on", "yes", "y", "1" }; // for realisation function read for bool
public:
	IniFile(std::string path) {
		std::string str;
		std::ifstream file(path);
		std::vector<std::string> strings;
		// открыть файл (path)
		try {
			if (!file.is_open()) {
				throw std::invalid_argument("Fail");
			}
		}
		catch (std::exception e) {
			std::ofstream fout(path);
			fout.close();
			std::ifstream file(path);
		}

			// while(getline(файла))
		while (!file.eof()) {
			if (std::getline(file, str) && !(str.length() == 1 && isspace(str[0]))) {
				strings.push_back(str);
			}
		}

		file.close();
		}

		std::string section;
		std::string key;
		std::string value;

		for (size_t i = 0; i < strings.size(); i++) {
			
			size_t first = 0;
			size_t last = strings[i].size() - 1;
			if (strings[i][0] == '[' && strings[i][last] == ']') {
				section = strings[i].substr(1, last - 1);
				_data.insert(std::pair < std::string, std::map < std::string, std::string>>(section, {}));
			}
			else {
				last = strings[i].find('=');
				key = strings[i].substr(0, last);
				first = last;
				++first;
				last = strings[i].size() - 1;
				value = strings[i].substr(first, last);
				if(!value.empty())
				   _data.at(section).insert(std::pair<std::string, std::string>(key, value));
			}
		}

	
		
		// 		если строка начинается
		//		[ и заканчивается символом ]
		//			это секция
		//		иначе если содержит = 
		//			это ключ-значение
		//			найти в строке =
		//			всё до = - key
		//			после = - value
		//		иначе continue
	}

	void save(std::string path) const;
		
		// for(section : sections)
		// 		<< [ section ]
		//		for(key : section)
		//			<< key = value
	

	/**
	 * Считывает из секции section
	 * значение ключа key
	 * Если не удалось считать - возвращает default value
	 *
	 * Определить для std::string, int, float, bool
	 */
	template<typename T>
	T read(std::string section, std::string key, T defaultValue = T{}) const;


	/**
	 * В ключ key из секции section
	 * записывает значение value
	 *
	 * Определить для std::string, int, float, bool
	 */
	template<typename T>
	void write(std::string section, std::string key, T value);

	/**
	 * Проверяет, существует ли секция section
	 */
	bool sectionExists(std::string section) const;

	/**
	 * Проверяет, существует ли ключ key в секции section
	 */
	bool keyExists(std::string section, std::string key) const;

	/**
	 * Возвращает существующие секции
	 */
	std::vector<std::string> sections() const;

	/**
	 * Возвращает существующие ключи в секции section
	 */
	std::vector<std::string> keys(std::string section) const;
};

// true -  true, True, TRUE, on, On, ON, yes, Yes, YES, y, Y, 1
// false - остальные

template<typename T>
inline T IniFile::read(std::string section, std::string key, T defaultValue) const
{
}




template<>
std::string IniFile::read(std::string section, std::string key, std::string defaultValue) const
{
	if (sectionExists(section) && keyExists(section, key)) {
		return _data.at(section).at(key);
	}
	return defaultValue;
}

template<>
int IniFile::read(std::string section, std::string key, int defaultValue) const {
	try {
		return std::stoi(_data.at(section).at(key));
	}
	catch (std::exception e) {
		return defaultValue;
	}
}

template<>
float IniFile::read(std::string section, std::string key, float defaultValue) const {
	try {
		return std::stof(_data.at(section).at(key));
	}
	catch (std::exception e) {
		return defaultValue;
	}
}

template<>
bool IniFile::read(std::string section, std::string key, bool defaultValue) const {
	if (sectionExists(section) && keyExists(section, key)) {
		std::string keyLC = _data.at(section).at(key);
		std::transform(keyLC.begin(), keyLC.end(), keyLC.begin(), [](unsigned char c) 
			{ return std::tolower(c); });
		if ((keyLC == "true") || (keyLC == "True") || (keyLC == "TRUE") || (keyLC == "on") ||
			(keyLC == "On") || (keyLC == "ON") || (keyLC == "yes") || (keyLC == "Yes") ||
			(keyLC == "YES") || (keyLC == "y") || (keyLC == "Y") && (keyLC == "1")) {
			return true;
		} else {
			return false;
		}
			
	} else {
		return defaultValue;
	}
}




template<typename T>
inline void IniFile::write(std::string section, std::string key, T value)
{
}

template<>
void IniFile::write(std::string section, std::string key, std::string value) {
	if (sectionExists(section)) {
		if (keyExists(section, key)) {
			_data.at(section).at(key) = value;
		} else {
			_data.at(section).insert(std::pair<std::string, std::string>(key, value));
		}
	} else {
		_data.insert(std::pair<std::string, std::map<std::string, std::string>>(section, {}));
		_data.at(section).insert(std::pair<std::string, std::string>(key, value));
	}
}

template<>
void IniFile::write(std::string section, std::string key, int value) {
	if (sectionExists(section)) {
		if (keyExists(section, key)) {
			_data.at(section).at(key) = std::to_string(value);
		} else {
			_data.at(section).insert(std::pair<std::string, std::string>(key, std::to_string(value)));
		}
	} else {
		_data.insert(std::pair<std::string, std::map<std::string, std::string>>(section, {}));
		_data.at(section).insert(std::pair<std::string, std::string>(key, std::to_string(value)));
	}
}

template<>
void IniFile::write(std::string section, std::string key, float value) {
	if (sectionExists(section)) {
		if (keyExists(section, key)) {
			_data.at(section).at(key) = std::to_string(value);
		} else {
			_data.at(section).insert(std::pair<std::string, std::string>(key, std::to_string(value)));
		}
	} else {
		_data.insert(std::pair<std::string, std::map<std::string, std::string>>(section, {}));
		_data.at(section).insert(std::pair<std::string, std::string>(key, std::to_string(value)));
	}
}

template<>
void IniFile::write(std::string section, std::string key, bool value) {
	std::string check;
	if (value == true) {
		check = "true";
	} else {
		check = "false";
	}
	
	if (sectionExists(section)) {
		if (keyExists(section, key)) {
			_data.at(section).at(key) = check;
		} else {
			_data.at(section).insert(std::pair<std::string, std::string>(key, check));
		}
	} else {
		_data.insert(std::pair<std::string, std::map<std::string, std::string>>(section, {}));
		_data.at(section).insert(std::pair<std::string, std::string>(key, check));
	}
}





inline void IniFile::save(std::string path) const
{
	std::ofstream fout;
	fout.open(path);
	if (fout.is_open()) {
		std::vector<std::string> section;
		section = sections();
		for (int i = 0; i < section.size(); i++) {
			fout << "[" << section[i] << "]" << std::endl;
			
			auto key = keys(section[i]);
			for (int j = 0; j < key.size(); j++) {
				auto pair = _data.at(section[i]).find(key[j]);
				fout << pair->first << "=" << pair->second << std::endl;
			}
		}
	} else {
		std::cerr << "Can not open file" << std::endl;
	}
	fout.close();
}


bool IniFile::sectionExists(std::string section) const {
	auto it = _data.find(section);
	return it != _data.end();
}


bool IniFile::keyExists(std::string section, std::string key) const {
	if (!sectionExists(section)) {
		return false;
	}
	auto it = _data.at(section).find(key);
	return it != _data.at(section).end();
}


std::vector<std::string> IniFile::sections() const {
	std::vector<std::string> sections;
	for (auto it = _data.begin(); it != _data.end(); ++it) {
		sections.push_back(it->first);
	}
	return sections;
}


std::vector<std::string> IniFile::keys(std::string section) const {
	std::vector<std::string> keys;
	if (sectionExists(section)) {
		for (auto elem : _data.at(section)) {
			keys.push_back(elem.first);
		}
	}
	return keys;
}


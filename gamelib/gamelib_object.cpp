#include "pch.h"
#include <gamelib_object.hpp>

namespace GameLib {
	Object::Object() {}

	Object::~Object() {}

	void Object::rename(const char* newName) {
		_setName(newName);
	}

	void Object::rename(const std::string& namestr) {
		_setName(namestr.c_str());
	}

	void Object::updatemeta() {
		_setDesc(_updateDesc());
		_setInfo(_updateInfo());
	}

	bool Object::load(const std::string& filename) {
		std::ifstream fin(filename);
		if (!fin)
			return false;
		while (fin) {
			std::string str;
			std::getline(fin, str);
			std::istringstream istr(str);
			readCharStream(istr);
		}
		return true;
	}

	bool Object::write(const std::string& filename) {
		std::ofstream fout(filename);
		if (!fout)
			return false;
		writeCharStream(fout) << std::endl;
		return true;
	}
} // namespace GameLib

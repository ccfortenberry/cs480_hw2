#ifndef GAMELIB_OBJECT_HPP
#define GAMELIB_OBJECT_HPP

#include <gamelib_base.hpp>

namespace GameLib {
	class Object : public std::enable_shared_from_this<Object> {
	public:
		Object();
		virtual ~Object();

		using weak_ptr = std::weak_ptr<Object>;
		using shared_ptr = std::shared_ptr<Object>;
		using const_weak_ptr = const std::weak_ptr<Object>;
		using const_shared_ptr = const std::shared_ptr<Object>;

		shared_ptr getSharedPtr() {
			return shared_from_this();
		}
		weak_ptr getWeakPtr() {
			return shared_from_this();
		}

		// Returns the name of this object
		const std::string& name() const {
			return name_;
		};

		// Returns a short description of this object
		const std::string& desc() const {
			return desc_;
		}

		// Returns detailed information about this object
		const std::string& info() const {
			return info_;
		}

		// Rename the current object
		void rename(const char* namestr);

		// Rename the current object
		void rename(const std::string& namestr);

		// Update the current description and information about the object
		void updatemeta();

		// Writes a serialized text version of this object. This should be one line of information.
		virtual std::ostream& writeCharStream(std::ostream& os) const {
			return os;
		}

		// Reads a serialized text version of this object. This will be one line of information.
		virtual std::istream& readCharStream(std::istream& is) {
			return is;
		}

		// Read line by line the contents of the file, calling readCharStream with an istringstream
		bool load(const std::string& filename);

		// Write line by line the contents of the file, calling writeCharStream with an ofstream
		bool write(const std::string& filename);

	protected:
		// sets the name of the object
		void _setName(std::string&& namestr) {
			name_ = std::move(namestr);
		}
		// sets the description of the object
		void _setDesc(std::string&& descstr) {
			desc_ = std::move(descstr);
		}
		// sets the information about this object
		void _setInfo(std::string&& infostr) {
			info_ = std::move(infostr);
		}

		// return a string to update the name of the object. called at object creation time
		virtual std::string _updateName() {
			return { "noname" };
		}
		// return a string to update the description of the object - called after update()
		virtual std::string _updateDesc() {
			return { "nodesc" };
		}
		// return a string to update the information about the object - called after update()
		virtual std::string _updateInfo() {
			return { "noinfo" };
		}

	private:
		std::string name_;
		std::string desc_;
		std::string info_;
	};

	// calls virtual Object::writeCharStream()
	inline std::ostream& operator<<(std::ostream& s, const Object& o) {
		return o.writeCharStream(s);
	}

	// calls virtual Object::readCharSteam()
	inline std::istream& operator>>(std::istream& s, Object& o) {
		return o.readCharStream(s);
	}
} // namespace GameLib

#endif

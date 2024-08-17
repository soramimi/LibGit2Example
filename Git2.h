#ifndef GIT2_H
#define GIT2_H

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

class Git2 {
	friend class Repository;
public:
	struct TreeItem {
		uint32_t filemode = 0;
		std::string id;
		std::string filename;
		bool isblob() const;
		bool istree() const;
		bool islink() const;
		bool iscommit() const;
	};
public:
	Git2();
	~Git2();

	class Repository {
	private:
		struct Private;
		Private *m;
		Git2 *git2_;
	public:
		Repository(Git2 *git2);
		~Repository();
		bool open(char const *path);
		void close();
		std::optional<std::vector<TreeItem>> ls_tree(std::string const &path);
		std::optional<std::vector<char>> cat_file(std::string const &id);
	};
};

#endif // GIT2_H

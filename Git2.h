#ifndef GIT2_H
#define GIT2_H

#include <stdio.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <optional>
#include <cstdint>

class Git2 {
	friend class Repository;
public:
	struct TreeItem {
		uint32_t filemode = 0;
		std::string id;
//		std::string subdir;
		std::string filename;
		bool isblob() const;
		bool istree() const;
		bool islink() const;
		bool iscommit() const;
//		std::string path() const;
	};
private:
	struct Private;
	Private *m;

	std::optional<std::vector<TreeItem>> ls_tree(std::string const &path);
	std::optional<std::vector<char>> cat_file(std::string const &id);
public:
	Git2();
	~Git2();
	bool open(char const *path);
	void close();

	class Repository {
	private:
		Git2 *git2_;
	public:
		Repository(Git2 *git2, char const *path)
			: git2_(git2)
		{
			git2_->open(path);
		}
		~Repository()
		{
			git2_->close();
		}
		std::optional<std::vector<TreeItem>> ls_tree(std::string const &path)
		{
			return git2_->ls_tree(path);
		}
		std::optional<std::vector<char>> cat_file(std::string const &id)
		{
			return git2_->cat_file(id);
		}
	};
};

#endif // GIT2_H

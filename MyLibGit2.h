#ifndef MYLIBGIT2_H
#define MYLIBGIT2_H

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

class MyLibGit2 {
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
	MyLibGit2();
	~MyLibGit2();

	class Repository {
	private:
		struct Private;
		Private *m;
		MyLibGit2 *git2_;
	public:
		Repository(MyLibGit2 *git2);
		~Repository();
		bool open(char const *path);
		void close();
		std::optional<std::vector<TreeItem>> ls_tree(std::string const &path);
		std::optional<std::vector<char>> cat_file(std::string const &id);
	};
};

#endif // MYLIBGIT2_H

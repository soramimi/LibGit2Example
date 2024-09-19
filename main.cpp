#include "MyLibGit2.h"

#include <stdio.h>



int main(int argc, char **argv)
{
	MyLibGit2 git2;
	MyLibGit2::Repository repo(&git2);
	repo.open("../zlib");
	auto items = repo.ls_tree("doc");
	if (items) {
		for (MyLibGit2::TreeItem const &item : *items) {
			printf("%08x %s %s\n", item.filemode, item.id.c_str(), item.filename.c_str());
		}
	}
//	auto data = repo.cat_file((*items)[1].id);
	repo.close();
}

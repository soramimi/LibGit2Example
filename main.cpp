#include "MyLibGit2.h"

#include <stdio.h>



int main(int argc, char **argv)
{
	MyLibGit2 git2;
	MyLibGit2::Repository repo(&git2);
	repo.open("C:/develop/jstream");
#if 0
	auto items = repo.ls_tree("");
	if (items) {
		for (MyLibGit2::TreeItem const &item : *items) {
			printf("%08x %s %s\n", item.filemode, item.id.c_str(), item.filename.c_str());
		}
	}
#else
	repo.log_all();
#endif
//	auto data = repo.cat_file((*items)[1].id);
	repo.close();
}

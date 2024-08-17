#include "Git2.h"

#include <stdio.h>



int main(int argc, char **argv)
{
	Git2 git2;
	Git2::Repository repo(&git2);
	repo.open("../../Guitar");
	auto items = repo.ls_tree("docs");
	if (items) {
		for (Git2::TreeItem const &item : *items) {
			printf("%08x %s %s\n", item.filemode, item.id.c_str(), item.filename.c_str());
		}
	}
//	auto data = repo.cat_file((*items)[1].id);
	repo.close();
}

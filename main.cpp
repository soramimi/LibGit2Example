#include "Git2.h"

#include <stdio.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <optional>



int main(int argc, char **argv)
{
	Git2 git2;
	Git2::Repository repo(&git2, "/home/soramimi/develop/Guitar");
	git2.open("/home/soramimi/develop/Guitar");
	auto items = repo.ls_tree("docs");
	if (items) {
		for (Git2::TreeItem const &item : *items) {
			printf("%08x %s\n", item.filemode, item.id.c_str());
		}
	}
//	auto data = repo.cat_file((*items)[1].id);
	git2.close();
}

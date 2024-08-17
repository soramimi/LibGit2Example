#include <git2.h>
#include <stdio.h>
#include <string>
#include <stdexcept>

class Git2 {
public:
	class Error : public std::runtime_error {
	private:
		int error;
		int klass;
	public:
		Error(int error, const git_error *e)
			: std::runtime_error(e->message)
			, klass(e->klass)
			, error(error)
		{
		}
	};
public:
	Git2()
	{
		git_libgit2_init();
	}
	~Git2()
	{
		git_libgit2_shutdown();
	}
	std::string oid_string(const git_oid *oid)
	{
		char str[GIT_OID_HEXSZ + 1];
		git_oid_tostr(str, sizeof(str), oid);
		return std::string(str);
	}

	void print_blob_data(const git_blob *blob)
	{
		size_t size = git_blob_rawsize(blob);
		const char *content = (const char *)git_blob_rawcontent(blob);

		printf("Blob size: %zu bytes\n", size);
		printf("Blob content:\n%.*s\n", (int)size, content);
	}

	bool list_tree(git_repository *repo, const git_tree *tree)
	{
		size_t count = git_tree_entrycount(tree);
		for (size_t i = 0; i < count; i++) {
			const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
			const char *filename = git_tree_entry_name(entry);
			git_filemode_t filemode = git_tree_entry_filemode(entry);
			const git_oid *oid = git_tree_entry_id(entry);
			printf("%06o %s %s\n", filemode, filename, oid_string(oid).c_str());
			if (filemode == GIT_FILEMODE_BLOB || filemode == GIT_FILEMODE_BLOB_EXECUTABLE) {
				git_blob *blob = nullptr;
				int error = git_blob_lookup(&blob, repo, oid);
				if (error < 0) throw Error(error, git_error_last());
				print_blob_data(blob);
				git_blob_free(blob);
			} else if (filemode == GIT_FILEMODE_TREE) {
				git_tree *subtree = nullptr;
				int error = git_tree_lookup(&subtree, repo, oid);
				if (error < 0) throw Error(error, git_error_last());
				list_tree(repo, subtree);
				git_tree_free(subtree);
			} else {
				printf("%06o %s\n", filemode, filename);
			}
		}
	}

	int run()
	{
		git_repository *repo = nullptr;
		git_commit *commit = nullptr;
		git_tree *tree = nullptr;
		git_oid oid;

		try {
			// リポジトリを開く
			int error = git_repository_open(&repo, "/home/soramimi/develop/Guitar");
			if (error < 0) throw Error(error, git_error_last());

			// HEADのOIDを取得
			error = git_reference_name_to_id(&oid, repo, "HEAD");
			if (error < 0) throw Error(error, git_error_last());

			// コミットを取得
			error = git_commit_lookup(&commit, repo, &oid);
			if (error < 0) throw Error(error, git_error_last());

			// コミットからツリーを取得
			error = git_commit_tree(&tree, commit);
			if (error < 0) throw Error(error, git_error_last());

			// ツリーの内容をリスト
			error = list_tree(repo, tree);
			if (error < 0) throw Error(error, git_error_last());
		} catch (Error &e) {
			fprintf(stderr, "Git2 Error: %s\n", e.what());
		}

		// リソースの解放
		if (tree) git_tree_free(tree);
		if (commit) git_commit_free(commit);
		if (repo) git_repository_free(repo);

		return 0;
	}
};


int main(int argc, char **argv)
{
	Git2 git2;
	git2.run();
}

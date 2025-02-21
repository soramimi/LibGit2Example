#include "libgit2/include/git2.h"
#include "MyLibGit2.h"
#include <cstring>
#include <stdexcept>
#include <chrono>

class ElapsedTimer {
private:
	std::chrono::system_clock::time_point start_;
public:
	void start()
	{
		start_ = std::chrono::system_clock::now();
	}
	unsigned long elapsed() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_).count();
	}
};

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

bool MyLibGit2::TreeItem::isblob() const
{
	return filemode == GIT_FILEMODE_BLOB || filemode == GIT_FILEMODE_BLOB_EXECUTABLE;
}

bool MyLibGit2::TreeItem::istree() const
{
	return filemode == GIT_FILEMODE_TREE;
}

bool MyLibGit2::TreeItem::islink() const
{
	return filemode == GIT_FILEMODE_LINK;
}

bool MyLibGit2::TreeItem::iscommit() const
{
	return filemode == GIT_FILEMODE_COMMIT;
}

static std::string oid_string(const git_oid *oid)
{
	char str[GIT_OID_HEXSZ + 1];
	git_oid_tostr(str, sizeof(str), oid);
	return std::string(str);
}

static std::vector<char> blob_data(const git_blob *blob)
{
	size_t size = git_blob_rawsize(blob);
	const char *content = (const char *)git_blob_rawcontent(blob);
	return std::vector<char>(content, content + size);
}

static std::optional<std::vector<MyLibGit2::TreeItem>> ls_tree_internal(git_repository *repo, const git_tree *tree, const std::string_view &path)
{
	std::vector<MyLibGit2::TreeItem> items;
	size_t count = git_tree_entrycount(tree);
	for (size_t i = 0; i < count; i++) {
		const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
		const git_oid *oid = git_tree_entry_id(entry);
		MyLibGit2::TreeItem item;
		item.filemode = git_tree_entry_filemode(entry);
		item.id = oid_string(oid);
		item.filename = git_tree_entry_name(entry);
		if (item.filemode == GIT_FILEMODE_TREE) {
			auto Match = [&]() {
				size_t n = item.filename.size();
				if (path.size() < n) return false;
				if (memcmp(path.data(), item.filename.data(), n) != 0) return false;
				if (n < path.size() && path[n] != '/') return false;
				return true;
			};
			if (Match()) {
				size_t j = item.filename.size();
				if (j < path.size() && path[j] == '/') {
					j++;
				}
				git_tree *subtree = nullptr;
				int error = git_tree_lookup(&subtree, repo, oid);
				auto ret = ls_tree_internal(repo, subtree, path.substr(j));
				git_tree_free(subtree);
				if (error < 0) throw Error(error, git_error_last());
				return ret;
			}
		}
		if (path.empty()) {
			items.push_back(item);
		}
	}
	return items;
}

static std::optional<std::vector<MyLibGit2::TreeItem>> ls_tree(git_repository *repo, const std::string &path)
{
	std::optional<std::vector<MyLibGit2::TreeItem>> ret;
	git_commit *commit = nullptr;
	git_tree *tree = nullptr;
	git_oid oid;

	try {
		int error;

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
		ret = ls_tree_internal(repo, tree, path);
	} catch (Error &e) {
		fprintf(stderr, "Git2 Error: %s\n", e.what());
	}

	// リソースの解放
	if (tree) git_tree_free(tree);
	if (commit) git_commit_free(commit);

	return ret;
}

std::optional<std::vector<char> > cat_file(git_repository *repo, const std::string &id)
{
	bool ok = true;
	std::vector<char> data;

	git_oid oid;
	git_blob *blob = nullptr;
	int error;

	try {
		// IDをパース
		error = git_oid_fromstr(&oid, id.c_str());
		if (error < 0) throw Error(error, git_error_last());

		// BLOBを取得
		error = git_blob_lookup(&blob, repo, &oid);
		if (error < 0) throw Error(error, git_error_last());

		// BLOBの内容を表示
		data = blob_data(blob);
	} catch (Error &e) {
		fprintf(stderr, "Git2 Error: %s\n", e.what());
		ok = false;
	}

	// リソースの解放
	if (blob) git_blob_free(blob);

	if (ok) return data;
	return std::nullopt;
}

MyLibGit2::MyLibGit2()
{
	git_libgit2_init();
}

MyLibGit2::~MyLibGit2()
{
	git_libgit2_shutdown();
}

struct MyLibGit2::Repository::Private {
	git_repository *repo = nullptr;
};

MyLibGit2::Repository::Repository(MyLibGit2 *git2)
	: m(new Private)
	, git2_(git2)
{
}

MyLibGit2::Repository::~Repository()
{
	close();
	delete m;
}

bool MyLibGit2::Repository::open(const char *path)
{
	// リポジトリを開く
	int error = git_repository_open(&m->repo, path);
	if (error < 0) {
		fprintf(stderr, "Git2 Error: %s\n", git_error_last()->message);
		return false;
	}
	return true;
}

void MyLibGit2::Repository::close()
{
	if (m->repo) {
		git_repository_free(m->repo);
		m->repo = nullptr;
	}
}

std::optional<std::vector<MyLibGit2::TreeItem> > MyLibGit2::Repository::ls_tree(const std::string &path)
{
	return ::ls_tree(m->repo, path);
}

std::optional<std::vector<char> > MyLibGit2::Repository::cat_file(const std::string &id)
{
	return ::cat_file(m->repo, id);
}


void print_commit_log(git_repository *repo)
{
	git_revwalk *walker = NULL;
	git_oid oid;
	git_commit *commit = NULL;
	const git_signature *author;
	const char *message;

	// リポジトリ内の全てのリファレンスを取得
	git_revwalk_new(&walker, repo);
	git_revwalk_sorting(walker, GIT_SORT_TIME);  // 時系列順にソート
	git_revwalk_push_head(walker);  // HEADからのコミットを辿る

	// 全てのコミットを表示
	while (!git_revwalk_next(&oid, walker)) {
		git_commit_lookup(&commit, repo, &oid);
		author = git_commit_author(commit);
		message = git_commit_message(commit);

		printf("Commit: %s\n", git_oid_tostr_s(&oid));
		printf("Author: %s <%s>\n", author->name, author->email);
		printf("Message: %s\n\n", message);

		git_commit_free(commit);
	}

	git_revwalk_free(walker);
}

void print_commit_parents(git_repository *repo)
{
	git_revwalk *walker = NULL;
	git_oid oid;
	git_commit *commit = NULL;
	git_commit *parent_commit = NULL;
	size_t parent_count;
	size_t i;
	const char *commit_hash;
	const char *parent_hash;

	// 全てのリファレンスを取得
	git_revwalk_new(&walker, repo);
	// git_revwalk_sorting(walker, GIT_SORT_TIME);  // 時系列順にソート
	git_revwalk_push_head(walker);  // HEADからのコミットを辿る

	int count = 0;

	// コミットと親コミットのハッシュを表示
	while (!git_revwalk_next(&oid, walker)) {
		git_commit_lookup(&commit, repo, &oid);
		git_time_t time = git_commit_time(commit);

		// コミットハッシュを取得
		commit_hash = git_oid_tostr_s(&oid);

		// 親コミットの取得
		parent_count = git_commit_parentcount(commit);
		// printf("%lld %s", time, commit_hash);  // コミットハッシュを表示

		for (i = 0; i < parent_count; ++i) {
			git_commit_parent(&parent_commit, commit, i);
			parent_hash = git_oid_tostr_s(git_commit_id(parent_commit));
			// printf(" %s", parent_hash);  // 親コミットハッシュを表示
		}

		// printf("\n");

		git_commit_free(commit);

		count++;
		if (count % 1000 == 0) {
			printf("%d\n", count);
		}
		if (count >= 10000) {
			break;
		}
	}

	git_revwalk_free(walker);
}

void MyLibGit2::Repository::log_all()
{
	ElapsedTimer timer;
	timer.start();
	print_commit_parents(m->repo);
	printf("%lldms\n", timer.elapsed());
}


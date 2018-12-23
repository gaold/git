#ifndef BUILTIN_H
#define BUILTIN_H

#include "git-compat-util.h"
#include "strbuf.h"
#include "cache.h"
#include "commit.h"

/*
 * builtin API - 内置API
 * ===========
 *
 * Adding a new built-in -  增加一个新的内置命令
 * ---------------------
 *
 * There are 4 things to do to add a built-in command implementation to
 * Git:
 * - git 增加一个内置命令需要完成4个步骤
 * . Define the implementation of the built-in command `foo` with
 *   signature:
 * - 定义内置函数FOO的声明，如下
 *	int cmd_foo(int argc, const char **argv, const char *prefix);
 *
 * . Add the external declaration for the function to `builtin.h`.
 * - 在 builtin.h 中，添加扩展声明
 * . Add the command to the `commands[]` table defined in `git.c`.
 *   The entry should look like:
 * - 在git.c 文件中，给结构体中，添加命令结构体，形式如下：
 *	{ "foo", cmd_foo, <options> },
 *
 * where options is the bitwise-or of:
 *- options 选项是按位与获得：
 * `RUN_SETUP`: - 运行时配置信息
 *	If there is not a Git directory to work on, abort.  If there
 *	is a work tree, chdir to the top of it if the command was
 *	invoked in a subdirectory.  If there is no work tree, no
 *	chdir() is done.
 * - 如果没有git 工作路径，不执行；如果有git,自动调整到最外层路径执行。没有工作，不执行 chdir
 * `RUN_SETUP_GENTLY`: - 一般运行时配置
 *	If there is a Git directory, chdir as per RUN_SETUP, otherwise,
 *	don't chdir anywhere.
 *
 * `USE_PAGER`:
 * - 使用页面调度器
 *	If the standard output is connected to a tty, spawn a pager and
 *	feed our output to it.
 * - 如果标准输出链接到TTY，要产生一个页面调度器并且使我们的输出适应tty
 * `NEED_WORK_TREE`:
 * - 需要工作树
 *	Make sure there is a work tree, i.e. the command cannot act
 *	on bare repositories.
 *	This only makes sense when `RUN_SETUP` is also set.
 * - 确认工作树存在，说明命令不能在空仓库时执行, 只有与RUN_SETUP一起配置时才有用。
 * `SUPPORT_SUPER_PREFIX`:
 * - 支持超级前缀
 *	The built-in supports `--super-prefix`.
 *
 * `DELAY_PAGER_CONFIG`:
 * - 延迟页面适配器配置
 *	If RUN_SETUP or RUN_SETUP_GENTLY is set, git.c normally handles
 *	the `pager.<cmd>`-configuration. If this flag is used, git.c
 *	will skip that step, instead allowing the built-in to make a
 *	more informed decision, e.g., by ignoring `pager.<cmd>` for
 *	certain subcommands.
 * - 配置了RUN SETUP 或者 RUN SETUP GENTLY时，git.c 一般会处理 pager.cmd -的配置信息，如果其用这个配置，git.c 会忽略上述动作，允许内置更准确的内置信息。
 * . Add `builtin/foo.o` to `BUILTIN_OBJS` in `Makefile`.
 * - 在 Makefile 中 添加 builtin/foo.h
 * Additionally, if `foo` is a new command, there are 4 more things to do:
 * 另外，foo 是个新命令，有4个事情需要去做。
 * . Add tests to `t/` directory.
 *   增加 测试信息到 t/ 目录中
 * . Write documentation in `Documentation/git-foo.txt`.
 *   写相关介绍信息到 Documengtation/git-foo.txt
 * . Add an entry for `git-foo` to `command-list.txt`.
 *   command-list 添加 git-foo
 * . Add an entry for `/git-foo` to `.gitignore`.
 *   /git-foo 添加到 .gitignore中
 *
 * How a built-in is called
 * ------------------------
 *
 * The implementation `cmd_foo()` takes three parameters, `argc`, `argv,
 * and `prefix`.  The first two are similar to what `main()` of a
 * standalone command would be called with.
 *
 * When `RUN_SETUP` is specified in the `commands[]` table, and when you
 * were started from a subdirectory of the work tree, `cmd_foo()` is called
 * after chdir(2) to the top of the work tree, and `prefix` gets the path
 * to the subdirectory the command started from.  This allows you to
 * convert a user-supplied pathname (typically relative to that directory)
 * to a pathname relative to the top of the work tree.
 *
 * The return value from `cmd_foo()` becomes the exit status of the
 * command.
 */

#define DEFAULT_MERGE_LOG_LEN 20

extern const char git_usage_string[];
extern const char git_more_info_string[];

#define PRUNE_PACKED_DRY_RUN 01
#define PRUNE_PACKED_VERBOSE 02

extern void prune_packed_objects(int);

struct fmt_merge_msg_opts {
	unsigned add_title:1,
		credit_people:1;
	int shortlog_len;
};

extern int fmt_merge_msg(struct strbuf *in, struct strbuf *out,
			 struct fmt_merge_msg_opts *);

/**
 * If a built-in has DELAY_PAGER_CONFIG set, the built-in should call this early
 * when it wishes to respect the `pager.foo`-config. The `cmd` is the name of
 * the built-in, e.g., "foo". If a paging-choice has already been setup, this
 * does nothing. The default in `def` should be 0 for "pager off", 1 for "pager
 * on" or -1 for "punt".
 *
 * You should most likely use a default of 0 or 1. "Punt" (-1) could be useful
 * to be able to fall back to some historical compatibility name.
 */
extern void setup_auto_pager(const char *cmd, int def);

extern int is_builtin(const char *s);

extern int cmd_add(int argc, const char **argv, const char *prefix);
extern int cmd_am(int argc, const char **argv, const char *prefix);
extern int cmd_annotate(int argc, const char **argv, const char *prefix);
extern int cmd_apply(int argc, const char **argv, const char *prefix);
extern int cmd_archive(int argc, const char **argv, const char *prefix);
extern int cmd_bisect__helper(int argc, const char **argv, const char *prefix);
extern int cmd_blame(int argc, const char **argv, const char *prefix);
extern int cmd_branch(int argc, const char **argv, const char *prefix);
extern int cmd_bundle(int argc, const char **argv, const char *prefix);
extern int cmd_cat_file(int argc, const char **argv, const char *prefix);
extern int cmd_checkout(int argc, const char **argv, const char *prefix);
extern int cmd_checkout_index(int argc, const char **argv, const char *prefix);
extern int cmd_check_attr(int argc, const char **argv, const char *prefix);
extern int cmd_check_ignore(int argc, const char **argv, const char *prefix);
extern int cmd_check_mailmap(int argc, const char **argv, const char *prefix);
extern int cmd_check_ref_format(int argc, const char **argv, const char *prefix);
extern int cmd_cherry(int argc, const char **argv, const char *prefix);
extern int cmd_cherry_pick(int argc, const char **argv, const char *prefix);
extern int cmd_clone(int argc, const char **argv, const char *prefix);
extern int cmd_clean(int argc, const char **argv, const char *prefix);
extern int cmd_column(int argc, const char **argv, const char *prefix);
extern int cmd_commit(int argc, const char **argv, const char *prefix);
extern int cmd_commit_graph(int argc, const char **argv, const char *prefix);
extern int cmd_commit_tree(int argc, const char **argv, const char *prefix);
extern int cmd_config(int argc, const char **argv, const char *prefix);
extern int cmd_count_objects(int argc, const char **argv, const char *prefix);
extern int cmd_credential(int argc, const char **argv, const char *prefix);
extern int cmd_describe(int argc, const char **argv, const char *prefix);
extern int cmd_diff_files(int argc, const char **argv, const char *prefix);
extern int cmd_diff_index(int argc, const char **argv, const char *prefix);
extern int cmd_diff(int argc, const char **argv, const char *prefix);
extern int cmd_diff_tree(int argc, const char **argv, const char *prefix);
extern int cmd_difftool(int argc, const char **argv, const char *prefix);
extern int cmd_fast_export(int argc, const char **argv, const char *prefix);
extern int cmd_fetch(int argc, const char **argv, const char *prefix);
extern int cmd_fetch_pack(int argc, const char **argv, const char *prefix);
extern int cmd_fmt_merge_msg(int argc, const char **argv, const char *prefix);
extern int cmd_for_each_ref(int argc, const char **argv, const char *prefix);
extern int cmd_format_patch(int argc, const char **argv, const char *prefix);
extern int cmd_fsck(int argc, const char **argv, const char *prefix);
extern int cmd_gc(int argc, const char **argv, const char *prefix);
extern int cmd_get_tar_commit_id(int argc, const char **argv, const char *prefix);
extern int cmd_grep(int argc, const char **argv, const char *prefix);
extern int cmd_hash_object(int argc, const char **argv, const char *prefix);
extern int cmd_help(int argc, const char **argv, const char *prefix);
extern int cmd_index_pack(int argc, const char **argv, const char *prefix);
extern int cmd_init_db(int argc, const char **argv, const char *prefix);
extern int cmd_interpret_trailers(int argc, const char **argv, const char *prefix);
extern int cmd_log(int argc, const char **argv, const char *prefix);
extern int cmd_log_reflog(int argc, const char **argv, const char *prefix);
extern int cmd_ls_files(int argc, const char **argv, const char *prefix);
extern int cmd_ls_tree(int argc, const char **argv, const char *prefix);
extern int cmd_ls_remote(int argc, const char **argv, const char *prefix);
extern int cmd_mailinfo(int argc, const char **argv, const char *prefix);
extern int cmd_mailsplit(int argc, const char **argv, const char *prefix);
extern int cmd_merge(int argc, const char **argv, const char *prefix);
extern int cmd_merge_base(int argc, const char **argv, const char *prefix);
extern int cmd_merge_index(int argc, const char **argv, const char *prefix);
extern int cmd_merge_ours(int argc, const char **argv, const char *prefix);
extern int cmd_merge_file(int argc, const char **argv, const char *prefix);
extern int cmd_merge_recursive(int argc, const char **argv, const char *prefix);
extern int cmd_merge_tree(int argc, const char **argv, const char *prefix);
extern int cmd_mktag(int argc, const char **argv, const char *prefix);
extern int cmd_mktree(int argc, const char **argv, const char *prefix);
extern int cmd_multi_pack_index(int argc, const char **argv, const char *prefix);
extern int cmd_mv(int argc, const char **argv, const char *prefix);
extern int cmd_name_rev(int argc, const char **argv, const char *prefix);
extern int cmd_notes(int argc, const char **argv, const char *prefix);
extern int cmd_pack_objects(int argc, const char **argv, const char *prefix);
extern int cmd_pack_redundant(int argc, const char **argv, const char *prefix);
extern int cmd_patch_id(int argc, const char **argv, const char *prefix);
extern int cmd_prune(int argc, const char **argv, const char *prefix);
extern int cmd_prune_packed(int argc, const char **argv, const char *prefix);
extern int cmd_pull(int argc, const char **argv, const char *prefix);
extern int cmd_push(int argc, const char **argv, const char *prefix);
extern int cmd_range_diff(int argc, const char **argv, const char *prefix);
extern int cmd_read_tree(int argc, const char **argv, const char *prefix);
extern int cmd_rebase(int argc, const char **argv, const char *prefix);
extern int cmd_rebase__interactive(int argc, const char **argv, const char *prefix);
extern int cmd_receive_pack(int argc, const char **argv, const char *prefix);
extern int cmd_reflog(int argc, const char **argv, const char *prefix);
extern int cmd_remote(int argc, const char **argv, const char *prefix);
extern int cmd_remote_ext(int argc, const char **argv, const char *prefix);
extern int cmd_remote_fd(int argc, const char **argv, const char *prefix);
extern int cmd_repack(int argc, const char **argv, const char *prefix);
extern int cmd_rerere(int argc, const char **argv, const char *prefix);
extern int cmd_reset(int argc, const char **argv, const char *prefix);
extern int cmd_rev_list(int argc, const char **argv, const char *prefix);
extern int cmd_rev_parse(int argc, const char **argv, const char *prefix);
extern int cmd_revert(int argc, const char **argv, const char *prefix);
extern int cmd_rm(int argc, const char **argv, const char *prefix);
extern int cmd_send_pack(int argc, const char **argv, const char *prefix);
extern int cmd_serve(int argc, const char **argv, const char *prefix);
extern int cmd_shortlog(int argc, const char **argv, const char *prefix);
extern int cmd_show(int argc, const char **argv, const char *prefix);
extern int cmd_show_branch(int argc, const char **argv, const char *prefix);
extern int cmd_show_index(int argc, const char **argv, const char *prefix);
extern int cmd_status(int argc, const char **argv, const char *prefix);
extern int cmd_stripspace(int argc, const char **argv, const char *prefix);
extern int cmd_submodule__helper(int argc, const char **argv, const char *prefix);
extern int cmd_symbolic_ref(int argc, const char **argv, const char *prefix);
extern int cmd_tag(int argc, const char **argv, const char *prefix);
extern int cmd_tar_tree(int argc, const char **argv, const char *prefix);
extern int cmd_unpack_file(int argc, const char **argv, const char *prefix);
extern int cmd_unpack_objects(int argc, const char **argv, const char *prefix);
extern int cmd_update_index(int argc, const char **argv, const char *prefix);
extern int cmd_update_ref(int argc, const char **argv, const char *prefix);
extern int cmd_update_server_info(int argc, const char **argv, const char *prefix);
extern int cmd_upload_archive(int argc, const char **argv, const char *prefix);
extern int cmd_upload_archive_writer(int argc, const char **argv, const char *prefix);
extern int cmd_upload_pack(int argc, const char **argv, const char *prefix);
extern int cmd_var(int argc, const char **argv, const char *prefix);
extern int cmd_verify_commit(int argc, const char **argv, const char *prefix);
extern int cmd_verify_tag(int argc, const char **argv, const char *prefix);
extern int cmd_version(int argc, const char **argv, const char *prefix);
extern int cmd_whatchanged(int argc, const char **argv, const char *prefix);
extern int cmd_worktree(int argc, const char **argv, const char *prefix);
extern int cmd_write_tree(int argc, const char **argv, const char *prefix);
extern int cmd_verify_pack(int argc, const char **argv, const char *prefix);
extern int cmd_show_ref(int argc, const char **argv, const char *prefix);
extern int cmd_pack_refs(int argc, const char **argv, const char *prefix);
extern int cmd_replace(int argc, const char **argv, const char *prefix);

#endif

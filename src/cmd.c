// SPDX-License-Identifier: BSD-3-Clause

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"

#define READ		0
#define WRITE		1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
	/* TODO: Execute cd. */
	if(dir != NULL) {
		int result = chdir(get_word(dir));
		return result;
	}

	return 0;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
	/* TODO: Execute exit/quit. */
	// exit(EXIT_SUCCESS);

	return SHELL_EXIT; /* TODO: Replace with actual exit code. */
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father)
{
	/* TODO: Sanity checks. */

	/* TODO: If builtin command, execute the command. */



	char *p = s->verb->string;
	int in = dup(0);
	int out = dup(1);
	int err = dup(2); 
	char *out1, *in1, *err1;
	int ok = 0;

	if(s->err != NULL) {
		int err1fd;
		err1 = s->err->string;

		if(s->io_flags == IO_ERR_APPEND) {
			err1fd = open(err1, O_WRONLY | O_CREAT | O_APPEND, 0644);
			dup2(err1fd, 2);
		}
		else {
			err1fd = open(err1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			dup2(err1fd, 2);
		}
		if(s->out) {
			out1 = s->out->string;
			if(strcmp(err1, out1) == 0) {
				ok = 1;
				dup2(err1fd, 1);
			} else {
				int out1fd;
				if(s->io_flags == IO_REGULAR) {
					out1fd = open(out1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					dup2(out1fd, 1);
				}
				
				if(s->io_flags == IO_OUT_APPEND) {
					out1fd = open(out1, O_WRONLY | O_CREAT | O_APPEND, 0644);
					dup2(out1fd, 1);
				}
				close(out1fd);
			}
		}
		close(err1fd);
	}
	if(s->out != NULL) {
		if(ok == 0) {
			out1 = s->out->string;
			int out1fd;
			if(s->io_flags == IO_REGULAR) {
				out1fd = open(out1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				dup2(out1fd, 1);
			}
				
			if(s->io_flags == IO_OUT_APPEND) {
				out1fd = open(out1, O_WRONLY | O_CREAT | O_APPEND, 0644);
				dup2(out1fd, 1);
			}
			close(out1fd);
		}
	}
	if(s->in != NULL) {
		int in1fd;
		in1 = s->in->string;
		in1fd = open(in1, O_RDONLY);
		dup2(in1fd, 0);
		close(in1fd);
	}
	
	
	if(strcmp(p, "cd") == 0) {
		dup2(in, 0);
		dup2(out, 1);
		dup2(err, 2);
		// close(out);
		// close(in);
		// close(err);
		return shell_cd(s->params);
	}
	if (strcmp(p, "pwd") == 0) {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			dprintf(1, "%s\n", cwd);
			dup2(in, 0);
			dup2(out, 1);
			dup2(err, 2);
			close(out);
			close(in);
			close(err);
			return 0;
		} else {
			return 1;
		}
	} 
	if(strcmp(p, "quit") == 0) {
		return shell_exit();
	}
	if(strcmp(p, "exit") == 0) {
		return shell_exit();
	}
	
	/* TODO: If variable assignment, execute the assignment and return
	 * the exit status.
	 */

	char *verb = get_word(s->verb);

	char *assign = strstr(verb, "=");
	if (assign != NULL) {
		int namelen = assign - verb;
		char *s = malloc(namelen + 1);

		if (s == NULL) {
			perror("malloc");
			return -1;
		}
		strncpy(s, verb, namelen);
		s[namelen] = '\0';
		char *val = assign + 1;
		int result = setenv(s, val, 1);

		free(s);
		return result;
	}
	getenv(verb);

	/* TODO: If external command:
	 *   1. Fork new process
	 *     2c. Perform redirections in child
	 *     3c. Load executable in child
	 *   2. Wait for child
	 *   3. Return exit status
	 */
	

	pid_t p2 = fork();
	if(p2 == 0) {
		int argc = 0;
		char **args = get_argv(s, &argc);
		if (execvp(args[0], args) == 0) {
			exit(-1);
		} else {
			printf("Execution failed for '%s'\n", args[0]);
			exit(-1);
		}
		
	}
	else {
		int status;
		waitpid(p2, &status, 0);
		dup2(in, 0);
		dup2(out, 1);
		dup2(err, 2);
		close(out);
		close(in);
		close(err);
		if (WIFEXITED(status))
			return WEXITSTATUS(status);
		else
			return -1;
	}
	
		
	 /* TODO: Replace with actual exit status. */
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool run_in_parallel(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* TODO: Execute cmd1 and cmd2 simultaneously. */

	return true; /* TODO: Replace with actual exit status. */
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2).
 */
static bool run_on_pipe(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* TODO: Redirect the output of cmd1 to the input of cmd2. */

	return true; /* TODO: Replace with actual exit status. */
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father)
{
	/* TODO: sanity checks */
	if (c->op == OP_NONE) {
		/* TODO: Execute a simple command. */
		int p = parse_simple(c->scmd, level, father);
		return p; /* TODO: Replace with actual exit code of command. */
	}

	switch (c->op) {
	case OP_SEQUENTIAL:
		/* TODO: Execute the commands one after the other. */
		parse_command(c->cmd1, level, father);
		parse_command(c->cmd2, level, father);
		break;

	case OP_PARALLEL:
		/* TODO: Execute the commands simultaneously. */
		return run_in_parallel(c->cmd1, c->cmd2, level, father);
		break;

	case OP_CONDITIONAL_NZERO:
		/* TODO: Execute the second command only if the first one
		 * returns non zero.
		 */
		int cond_nzero;
		cond_nzero = parse_command(c->cmd1, level, father);
		if(cond_nzero != 0) {
			return parse_command(c->cmd2, level, father);
		}
		break;

	case OP_CONDITIONAL_ZERO:
		/* TODO: Execute the second command only if the first one
		 * returns zero.
		 */
		int cond_zero;
		cond_zero = parse_command(c->cmd1, level, father);
		if(cond_zero == 0) {
			return parse_command(c->cmd2, level, father);
		}
		break;

	case OP_PIPE:
		/* TODO: Redirect the output of the first command to the
		 * input of the second.
		 */
		return run_on_pipe(c->cmd1, c->cmd2, level, father);
		break;

	default:
		return SHELL_EXIT;
	}

	return 0; /* TODO: Replace with actual exit code of command. */
}
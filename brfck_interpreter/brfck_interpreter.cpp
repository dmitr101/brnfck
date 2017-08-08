/*
Created by Dima Shynkar.
8/8/2017
some docs:
** env - environment structure
**** arr - array with cells
**** ind - current cell
**** nstck - nesting stack, top contains pointer to deepest current nest
**** nsize - nesting stack size(used to emulate operations)
**** msize - max size e.g. real size
*/
#include "stdafx.h"
#define CELL_COUNT 30000
#define DEFAULT_NESTING 15

struct env
{
	char*	arr;
	size_t	ind;

	const char**	nstck;
	size_t	nsize;
	size_t	msize;
};

env* create_env(size_t size = CELL_COUNT)
{
	env* e = (env*)malloc(sizeof(env));
	e->arr = (char*)malloc(size);
	memset(e->arr, 0, size);
	e->ind = 0;
	e->nstck = (const char**)malloc(sizeof(*e->nstck)*DEFAULT_NESTING);
	e->nsize = 0;
	e->msize = DEFAULT_NESTING;
	return e;
}

void destroy_env(env* e)
{
	free(e->arr);
	free(e->nstck);
	free(e);
}

void realloc_nstck(env* e)
{
	char** tmp = (char**)malloc(e->msize);
	memcpy(tmp, e->nstck, e->nsize);
	free(e->nstck);
	e->msize += DEFAULT_NESTING;
	e->nstck = (const char**)malloc(e->msize);
	memcpy(e->nstck, tmp, e->nsize);
}

void push_nstck(env* e, const char* ptr)
{
	if (e->nsize == e->msize)
	{
		realloc_nstck(e);
	}
	e->nstck[e->nsize] = ptr;
	e->nsize++;
}

void pop_nstck(env* e)
{
	e->nsize--;
}

const char* get_nstck(env* e)
{
	return e->nstck[e->nsize - 1];
}

void process_script(env* e, const char* script)
{
	const char* cur = script;
	while (*cur++)
	{
		switch (*cur)
		{
		case '+':
			e->arr[e->ind]++;
			break;
		case '-':
			e->arr[e->ind]--;
			break;
		case '>':
			e->ind++;
			break;
		case '<':
			e->ind--;
			break;
		case '.':
			putchar(e->arr[e->ind] + 1);
			break;
		case ',':
			e->arr[e->ind] = getchar();
			break;
		//TODO: fix f*cking loops
		case '[':
		{
			if (e->arr[e->ind])
			{
				push_nstck(e, cur);
				break;
			}
			int bal = -1;
			while (*cur++ && !bal)
				if (*cur == '[')
					bal--;
				else if (*cur == ']')
					bal++;
			break;
		}
		case ']':
		{
			if (e->arr[e->ind])
			{
				cur = get_nstck(e);
				break;
			}
			pop_nstck(e);
			break;
		}
		}
	}
}

void run_interactive(env* e)
{
	char line[80];
	do
	{
		printf("brfck)>");
		fflush(stdout);
		scanf("%79s", line);
		if (!strcmp(line, "quit"))
			break;
		process_script(e, line);
	} while (true);
}

void run_module(env* e, const char* filename)
{
	FILE* fptr = fopen(filename, "r");
	fseek(fptr, 0L, SEEK_END);
	size_t sz = ftell(fptr);
	rewind(fptr);

	char* script = (char*)malloc(sz + 1);
	size_t ind = 0;
	while ((script[ind++] = fgetc(fptr)) != EOF);
	script[ind] = '\0';
	process_script(e, script);
}

int main(int argc, char* argv[])
{
	env* environment = create_env();
	switch (argc)
	{
	case 1:
		run_interactive(environment);
		break;
	case 2:
		run_module(environment, argv[1]);
		break;
	default:
		printf("Error - invalid number of args \n Valid usage: no args for intaractive mode or filename for module run");
		return -3;
	}
	destroy_env(environment);
	return 0;
}

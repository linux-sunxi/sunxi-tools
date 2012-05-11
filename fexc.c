/*
 * Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fexc.h"

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 */
static struct script *script_new(void)
{
	return NULL;
}
static void script_delete(struct script *UNUSED(script))
{
}

static int script_parse(int UNUSED(mode), const char *UNUSED(filename),
			struct script *UNUSED(script))
{
	return 0;
}
static int script_generate(int UNUSED(mode), const char *UNUSED(filename),
			   struct script *UNUSED(script))
{
	return 0;
}

/*
 */
static inline void app_usage(const char *arg0, int mode)
{
	errf("Usage: %s [-vq]%s[<input> [<output>]]\n", arg0,
	     mode ? " " : " [-I <infmt>] [-O <outfmt>] ");

	if (mode == 0)
		fputs("\ninfmt:  fex, bin  (default:fex)"
		      "\noutfmt: fex, bin  (default:bin)\n",
		      stderr);
}

static inline int app_choose_mode(char *arg0)
{
	const char *name = basename(arg0);
	if (strcmp(name, "fex2bin") == 0)
		return 1;
	else if (strcmp(name, "bin2fex") == 0)
		return 2;
	else
		return 0;
}

/*
 */
int main(int argc, char *argv[])
{
	static const char *formats[] = { "fex", "bin", NULL };
	int infmt=0, outfmt=1;
	const char *filename[] = { "stdin", "stdout" };
	struct script *script;

	int app_mode = app_choose_mode(argv[0]);

	const char *opt_string = "I:O:vq?"+ ((app_mode == 0)? 0: 4);
	int opt, ret = 1;
	int verbose = 0;

	if (app_mode == 2) /* bin2fex */
		infmt = 1, outfmt = 0;

	while ((opt = getopt(argc, argv, opt_string)) != -1) {
		switch (opt) {
		case 'I':
			infmt=0;
			for (const char **f = formats; *f; f++, infmt++) {
				if (strcmp(*f, optarg) == 0)
					break;
			}
			if (!formats[infmt]) {
				errf("%s: invalid format -- \"%s\"\n",
				     argv[0], optarg);
				goto show_usage;
			}
			break;
		case 'O':
			outfmt=0;
			for (const char **f = formats; *f; f++, outfmt++) {
				if (strcmp(*f, optarg) == 0)
					break;
			}
			if (!formats[outfmt]) {
				errf("%s: invalid format -- \"%s\"\n",
				     argv[0], optarg);
				goto show_usage;
			}
			break;
		case 'v':
			verbose++;
			break;
		case 'q':
			verbose--;
			break;
		default:
show_usage:
			app_usage(argv[0], app_mode);
			goto done;
		}
	}

	switch (argc - optind) {
	case 2:
		filename[1] = argv[optind+1]; /* out */
	case 1:
		if (strcmp(argv[optind], "-") != 0)
			filename[0] = argv[optind]; /* in */
	case 0:
		break;
	default:
		goto show_usage;
	}

	if (verbose>0)
		errf("%s: %s:%s -> %s:%s\n", argv[0],
		     formats[infmt], filename[0],
		     formats[outfmt], filename[1]);

	if ((script = script_new()) == NULL) {
		perror("malloc");
		goto done;
	} else if (script_parse(infmt, filename[0], script) &&
		   script_generate(outfmt, filename[1], script)) {
		ret = 0;
	}
	script_delete(script);
done:
	return ret;
}

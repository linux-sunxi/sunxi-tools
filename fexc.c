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

#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define pr_info(...)	errf("fexc: " __VA_ARGS__)
#define pr_err(...)	pr_info("E: " __VA_ARGS__)

enum script_format {
	FEX_SCRIPT_FORMAT,
	BIN_SCRIPT_FORMAT,
};

/*
 */
static inline int script_parse(enum script_format format,
			       const char *filename,
			       struct script *script)
{
	int ret = 0;
	switch (format) {
	case FEX_SCRIPT_FORMAT: {
		FILE *in = stdin;
		if (filename && (in = fopen(filename, "r")) == NULL) {
			errf("%s: %s\n", filename, strerror(errno));
			break;
		}
		ret = script_parse_fex(in, filename ? filename : "<stdin>",
				       script);
		fclose(in);
		}; break;
	case BIN_SCRIPT_FORMAT: {
		}; break;
	}
	return ret;
}
static inline int script_generate(enum script_format format,
				  const char *filename,
				  struct script *script)
{
	int ret = 0;
	switch (format) {
	case FEX_SCRIPT_FORMAT: {
		}; break;
	case BIN_SCRIPT_FORMAT: {
		int out = 1; /* stdout */
		size_t sections, entries, bin_size;
		void *bin;

		if (!filename)
			filename = "<stdout>";
		else if ((out = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
			errf("%s: %s\n", filename, strerror(errno));
			break;
		}

		bin_size = script_bin_size(script, &sections, &entries);
		bin = calloc(1, bin_size);
		if (!bin)
			perror("malloc");
		else if (script_generate_bin(bin, bin_size, script, sections, entries)) {
			while(bin_size) {
				ssize_t wc = write(out, bin, bin_size);

				if (wc>0) {
					bin += wc;
					bin_size -= wc;
				} else if (wc < 0 && errno != EINTR) {
					pr_err("%s: write: %s\n", filename,
					       strerror(errno));
					break;
				}
			}
			if (bin_size == 0)
				ret = 0;
		}
		}; break;
	}
	return ret;
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
	enum script_format infmt=FEX_SCRIPT_FORMAT;
	enum script_format outfmt=BIN_SCRIPT_FORMAT;
	const char *filename[] = { NULL /*stdin*/, NULL /*stdout*/};
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

#include <copyright.h>
#include <config.h>

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#include <common.h>
#include "fexc.h"
#include "script.h"
#include "script_bin.h"
#include "script_fex.h"
#include "script_uboot.h"

#define pr_info(...)  errf("fexc: " __VA_ARGS__)
#define pr_err(...)   errf("E: fexc: " __VA_ARGS__)

char *read_all(int fd, const char *filename, size_t *size) {
	size_t buf_size = 4096, count = 0;
	char *p, *buf = malloc(buf_size);

	if (!buf) {
		pr_err("%s: %s\n", "malloc", strerror(errno));
		return NULL ;
	}

	p = buf;
	while (1) {
		ssize_t rc = read(fd, p, buf_size - count);
		if (rc == 0) {
			break;
		} else if (rc > 0) {
			count += rc;
			p += rc;
			if (count == buf_size) {
				char *new;
				buf_size *= 2;
				new = realloc(buf, buf_size);
				if (!new) {
					pr_err("%s: %s\n", "realloc", strerror(errno));
					free(buf);
					return NULL ;
				} else if (new != buf) {
					buf = new;
					p = buf + count;
				}
			}
		} else if (errno != EAGAIN || errno != EINTR) {
			pr_err("%s: %s: %s\n", filename, "read", strerror(errno));
			free(buf);
			return NULL ;
		}
	}

	*size = count;
	return buf;
}

/*
 */
int script_parse(enum script_format format, const char *filename,
		struct script *script) {
	int ret = 0;
	switch (format) {
	case FEX_SCRIPT_FORMAT: {
		FILE *in = stdin;
		if (!filename) {
			filename = "<stdin>";
		} else if ((in = fopen(filename, "r")) == NULL ) {
			pr_err("%s: %s\n", filename, strerror(errno));
			break;
		}
		ret = script_parse_fex(in, filename, script);
		fclose(in);
	}
		;
		break;
	case BIN_SCRIPT_FORMAT: {
		int in = 0; /* stdin */
		struct stat sb;
		void *bin = NULL;
		size_t bin_size;
		int allocated = 1;

		if (!filename) {
			filename = "<stdin>";
		} else if ((in = open(filename, O_RDONLY)) < 0) {
			pr_err("%s: %s\n", filename, strerror(errno));
			break;
		}

		if (fstat(in, &sb) == -1) {
			pr_err("%s: %s: %s\n", filename, "fstat", strerror(errno));
			close(in);
			break;
		} else if (S_ISREG(sb.st_mode)) {
			/* regular file, mmap it */
			bin = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, in, 0);
			if (bin == MAP_FAILED ) {
				pr_err("%s: %s: %s\n", filename, "mmap", strerror(errno));
				close(in);
				break;
			}
			bin_size = sb.st_size;
			allocated = 0;
		} else {
			/* something else... just read it all! */
			bin = read_all(in, filename, &bin_size);
			if (bin == NULL ) {
				close(in);
				break;
			}
			allocated = 1;
		}

		ret = script_decompile_bin(bin, bin_size, filename, script);
		if (allocated) {
			free(bin);
		} else if (munmap(bin, bin_size) == -1) {
			pr_err("%s: %s: %s\n", filename, "munmap", strerror(errno));
		}
		close(in);
	}
		;
		break;
	case UBOOT_HEADER_FORMAT: /* not valid input */
		;
	}
	return ret;
}

int script_generate(enum script_format format, const char *filename,
		struct script *script) {
	int ret = 0;
	static int (*text_gen[3])(FILE *, const char *, struct script *) = {
		[FEX_SCRIPT_FORMAT] = script_generate_fex,
		[UBOOT_HEADER_FORMAT] = script_generate_uboot,
	};

	if (text_gen[format]) {
		FILE *out = stdout;

		if (!filename) {
			filename = "<stdout>";
		} else if ((out = fopen(filename, "w")) == NULL ) {
			pr_err("%s: %s\n", filename, strerror(errno));
			return ret;
		}

		ret = text_gen[format](out, filename, script);
		fclose(out);
	} else {
		int out = 1; /* stdout */
		size_t sections, entries, bin_size;
		void *bin;

		if (!filename) {
			filename = "<stdout>";
		} else if ((out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666))
				< 0) {
			pr_err("%s: %s\n", filename, strerror(errno));
			return ret;
		}

		bin_size = script_bin_size(script, &sections, &entries);
		bin = calloc(1, bin_size);
		if (!bin) {
			pr_err("%s: %s\n", "malloc", strerror(errno));
		} else if (script_generate_bin(bin, bin_size, script, sections,
				entries)) {
			char *p = bin;
			while (bin_size) {
				ssize_t wc = write(out, p, bin_size);

				if (wc > 0) {
					p += wc;
					bin_size -= wc;
				} else if (wc < 0 && errno != EINTR) {
					pr_err("%s: %s: %s\n", filename, "write", strerror(errno));
					break;
				}
			}
			ret = (bin_size == 0);
		}
		free(bin);
		close(out);
	}
	return ret;
}

/*
 */
void app_usage(const char *arg0, int mode) {
	errf(" FEX (de)compiler:\n\n");
	errf(
			" --fexc [--help] [--verbose] [--quiet] [--input=<format>] [--output=<format>] [<input> [<output>]]\n\n");
	errf("  -?, --help               show help\n");
	errf("  -I, --input=<format>     input format (fex, bin - default: fex)\n");
	errf(
			"  -O, --output<format>     output format (fex, bin, uboot - default: bin)\n");
	errf("  -v, --verbose            verbose mode\n");
	errf("  -q, --quiet              quiet mode\n");
	errf("  <input>                  file to convert (default: stdin)\n");
	errf("  <output>                 converted file (default: stdout)\n");

}

int app_choose_mode(char *arg0) {
	const char *name = basename(arg0);
	if (strcmp(name, "fex2bin") == 0) {
		return 1;
	} else if (strcmp(name, "bin2fex") == 0) {
		return 2;
	} else {
		return 0;
	}
}

/*
 */
int fexc(int argc, char **argv) {
	static const char *formats[] = { "fex", "bin", "uboot", NULL };
	enum script_format infmt = FEX_SCRIPT_FORMAT;
	enum script_format outfmt = BIN_SCRIPT_FORMAT;
	const char *filename[] = { NULL /*stdin*/, NULL /*stdout*/};
	struct script *script;

	int app_mode = app_choose_mode(argv[0]);

	const char *opt_string = "I:O:vq?" + ((app_mode == 0) ? 0 : 4);
	int opt, ret = 1;
	int verbose = 0;

	static struct option long_options[] = {
			{ "verbose", no_argument, NULL, 'v' }, { "quiet", no_argument, NULL,
					'q' }, { "input", required_argument, NULL, 'I' }, {
					"output", required_argument, NULL, 'O' }, { "help",
					no_argument, NULL, '?' }, { 0, 0, NULL, 0 } };

	opterr = 0;

	if (app_mode == 2) { /* bin2fex */
		infmt = BIN_SCRIPT_FORMAT;
		outfmt = FEX_SCRIPT_FORMAT;
	}

	while ((opt = getopt_long(argc, argv, "I:O:vq?", long_options, NULL )) != -1) {
		switch (opt) {
		case 'I':
			infmt = 0;
			{
				const char **f;
				for (f = formats; *f; f++, infmt++) {
					if (strcmp(*f, optarg) == 0) {
						break;
					}
				}
			}
			switch (infmt) {
			case FEX_SCRIPT_FORMAT:
			case BIN_SCRIPT_FORMAT:
				break;
			default:
				errf("%s: invalid format -- \"%s\"\n", argv[0], optarg);
				app_usage(argv[0], app_mode);
				return ret;
			}
			break;
		case 'O':
			outfmt = 0;
			{
				const char **f;
				for (f = formats; *f; f++, outfmt++) {
					if (strcmp(*f, optarg) == 0) {
						break;
					}
				}
			}
			if (!formats[outfmt]) {
				errf("%s: invalid format -- \"%s\"\n", argv[0], optarg);
				app_usage(argv[0], app_mode);
				return ret;
			}
			break;
		case 'v':
			verbose++;
			break;
		case 'q':
			verbose--;
			break;
		default:
			app_usage(argv[0], app_mode);
			return ret;
		}
	}

	switch (argc - optind) {
	case 2:
		filename[1] = argv[optind + 1]; /* out */
		/* no break */
	case 1:
		if (strcmp(argv[optind], "-") != 0) {
			filename[0] = argv[optind]; /* in */
		}
		/* no break */
	case 0:
		break;
	default:
		app_usage(argv[0], app_mode);
		return ret;
	}

	if (verbose > 0) {
		errf("%s: from %s:%s to %s:%s\n", argv[0], formats[infmt],
				filename[0] ? filename[0] : "<stdin>", formats[outfmt],
				filename[1] ? filename[1] : "<stdout>");
	}

	if ((script = script_new()) == NULL ) {
		perror("malloc");
		return ret;
	} else if (script_parse(infmt, filename[0], script)
			&& script_generate(outfmt, filename[1], script)) {
		ret = 0;
	}
	script_delete(script);
	return ret;
}

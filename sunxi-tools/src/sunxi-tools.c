#include <copyright.h>
#include <config.h>
#include <common.h>
#include <sysexits.h>

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define HELP(x)	          optind = 0; x( 2, (char *[]){ argv[0], "--help", (char *) NULL } )

void show_version(void) {
	printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
	printf("%s %s\n", i18n("Copyright (C)"), PACKAGE_COPYRIGHT);
	printf("%s %s\n", i18n("License"), PACKAGE_LICENCE);
	printf("%s\n", i18n("This is free software: you are free to change and redistribute it."));
	printf("%s\n", i18n("There is NO WARRANTY, to the extent permitted by law."));
}

void show_help(void) {
	printf("%s <%s>\n", i18n("Report bugs to:"), PACKAGE_BUGREPORT);
	printf("%s <%s>\n", i18n("pkg home page:"), PACKAGE_HOMEPAGE);
	printf("%s <%s>\n", i18n("General help using SUNXI compatible software:"), PACKAGE_HELPPAGE);
}

int main(int argc, char **argv) {
	int c = 0 , result = EXIT_SUCCESS, done = 0;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	printf ("Arguments : %d\n", argc);

	if (argc < 2) {
		show_version();
		printf("\n");
		HELP(fexc);
		printf("\n");
		show_help();
		exit(EX_USAGE);
	}

	static struct option long_options[] = {
			{"fexc",        no_argument, NULL, 'f' },
			{"bootinfo",    no_argument, NULL, 'b' },
			{"fel",         no_argument, NULL, 'l' },
			{"pio",         no_argument, NULL, 'p' },
			{"nandpart",    no_argument, NULL, 'n' },
			{"phoenixinfo", no_argument, NULL, 'x' },
			{"version",     no_argument, NULL, 'V' },
			{"help",        no_argument, NULL, '?' },
			{0,             0,           NULL,  0 }
	};

	while ( ( c = getopt_long(argc, argv, "fblpnxV?", long_options, NULL) ) != -1 ) {
		switch (c) {
		case 'f':
			result = fexc(argc, argv);
			done = 1;
			break;
		case 'b':
			printf(i18n("BOOT information\n"));
			break;
		case 'l':
			printf(i18n("FEL manager\n"));
			break;
		case 'p':
			printf(i18n("PIO manager\n"));
			break;
		case 'n':
			printf(i18n("NAND partition manager\n"));
			break;
		case 'x':
			printf(i18n("PHOENIX manager\n"));
			break;
		case 'V':
			show_version();
			break;
		case '?':
		default:
			show_version();
			printf("\n");
			HELP(fexc);
			printf("\n");
			show_help();
			done = 1;
			result=EX_USAGE;
			break;
		}
		if (done) {
			break;
		}
	}
	exit(result);
}

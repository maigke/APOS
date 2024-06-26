/******************************************************************************
 *   file: main.c                                                             *
 *                                                                            *
 *	 AVR Project Open Source (APOS)                                                                      *
 *                                                                            *
 *   This program is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation, either version 3 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>     *
 *                                                                            *
 *   Written by Alfredo Orozco <alfredoopa@gmail.com>                         *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "include/templates.h"
#include "include/info.h"
#include "include/args.h"

char txt[4096]; //Se amplia el valor de las dimensiones del archivo
/*
sprintf quien hace uso de este buff debe ser ampliado, ya que,
bajo las condiciones actuales, no alcanza con las dimensiones
que han sido especificadas.
*/

void create_makefile(options_t *options) ;
void create_mainfile(options_t *options);
void create_gitrepo(options_t *options);
void create_readme(options_t *options) ;
void create_project_folder(char *project_name);
void create_project(options_t *options) ;

int main(int argc, char const *argv[])
{
	int parse_result;
	options_t options;

	// If not pass arguments to program show the help
	if(argc < 2) {
		show_version();
		show_usage();
		return 0;
	}

	parse_result = parse_args(argc, argv, &options);

	if(options.version)
	{
		show_version();
		return 0;
	}
	if(options.help)
	{
		show_help();
		return 0;
	}

	if(!parse_result){
		printf("Use"BOLD_YELLOW" --help"RESET" option to show help\n\n");
		return 0;
	}

	create_project(&options);
	
	printf(BOLD_GREEN  "\n  apos: Project \"%s\" created!\n", options.project_name);
	printf(BOLD_MAGENTA"\n  Project Information:\n\n");
	printf(BOLD_CYAN"   Project Name:     "BOLD_YELLOW"\"%s\"\n", options.project_name);
	printf(BOLD_CYAN"   Git Repository:   "BOLD_YELLOW"%s\n", options.create_git ? "Yes":"No");
	printf(BOLD_CYAN"   Blink Template:   "BOLD_YELLOW"%s\n", options.blink_template ? "Yes":"No");
	printf(BOLD_CYAN"   Microcontroller:  "BOLD_YELLOW"%s\n", options.device);
	printf(BOLD_CYAN"   CPU Frequency:    "BOLD_YELLOW"%s\n", options.f_cpu);
	printf(BOLD_CYAN"   Low Fuse:         "BOLD_YELLOW"%s\n", options.low_f);
	printf(BOLD_CYAN"   High Fuse:        "BOLD_YELLOW"%s\n", options.high_f);
	printf(BOLD_CYAN"   Extended Fuse:    "BOLD_YELLOW"%s\n", options.extended_f);
	printf(BOLD_CYAN"   Programmer:       "BOLD_YELLOW"%s\n", options.programmer);
	printf(BOLD_CYAN"   ABOS Port:        "BOLD_YELLOW"%s\n", options.abos_port);
	printf(BOLD_CYAN"   ABOS Baudrate:    "BOLD_YELLOW"%s\n", options.abos_baudrate);
	printf("\n");

	return 0;
}


/**
 * Creates the Makefile from template and save it in a file.
*/
void create_makefile(options_t *options) 
{
	FILE *f;
	// Creates the "Makefile" whit the content.
	strcpy(txt, options->project_name);
	strcat(txt, "/Makefile");
	f = fopen(txt, "w");
	
	
	fwrite(makefile_apos_header,sizeof(char), strlen(makefile_apos_header), f);
	fprintf(f, "\nPROJECT_NAME  = %s\nDEVICE        = %s\nCLOCK         = %s\nFUSES         = -U hfuse:w:0x%s:m -U lfuse:w:0x%s:m -U efuse:w:0x%s:m\nAVRDUDE_PROG  = -c %s -P usb\nABOS_PORT     = %s\nABOS_BAUDRATE = %s",
	options->project_name, options->device, options->f_cpu, options->high_f, options->low_f, options->extended_f, options->programmer, options->abos_port, options->abos_baudrate);
	fwrite(makefile_apos_body, sizeof(char), strlen(makefile_apos_body), f);
	
	fclose(f);
}

/**
 * Creates and save the main.c file from template.
*/
void create_mainfile(options_t *options)
{
	FILE *f;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	strcpy(txt, options->project_name);
	strcat(txt, "/main.c");
	f = fopen(txt, "w");
	fprintf(f,"/******************************************************\n * main.c\n *\n * Program Name:  %s\n *         Date:  %04d-%02d-%02d", options->project_name, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday );
	
	if(options->blink_template)
		fwrite(blink_program, sizeof(char), strlen(blink_program), f);
	else
		fwrite(empty_template_body, sizeof(char), strlen(empty_template_body), f);

	fclose(f);
}

/**
 * Initialize the project folder with a git repository.
*/
void create_gitrepo(options_t *options)
{
	FILE *f;
	sprintf(txt, "cd %s && git init && git add * && git commit -m \"Initial commit for APOS AVR project ~%s~\"",options->project_name, options->project_name);
	system(txt);
	// Creates .gitignore
	strcpy(txt, options->project_name);
	strcat(txt, "/.gitignore");
	f = fopen(txt, "w");
	fwrite(gitignore, sizeof(char), strlen(gitignore), f);
	fclose(f);
}

/**
 * Creates and save the README.md file.
*/
void create_readme(options_t *options) 
{
		FILE *f;
		// Creates README.md
		strcpy(txt, options->project_name);
		strcat(txt, "/README.md");
		f = fopen(txt, "w");
		fprintf(f, "\n# %s \n\n### Project created with APOS (AVR Project Open Source)\n\nTo edit the project settings, open the Makefile and edit the first lines of the file.\n\n", options->project_name);
		fclose(f);
}

/**
 * Creates the project folder basen on the project name.
 * Replace all spaces in the project name for underscores to
 * prevent errors in the avr-gcc file paths.
*/
void create_project_folder(char* project_name)
{
	FILE *f;
	// Replace the spaces in the project name by "_", to avoid compilation problems
	for (int i = 0; i < strlen(project_name); ++i)
	{
		if(project_name[i]==' ')
			project_name[i]= '_';
	}

	if((f=fopen(project_name, "r")))
	{
		printf(BOLD_RED "\napos" RESET ": [" YELLOW "error" RESET "] the project " BOLD_YELLOW "\"%s\"" RESET " already exist!\n\n", project_name);
		fclose(f);
		exit(1);
	}
	// Creates the project folder with the name passed in the program arguments
	mkdir(project_name, 0700);
	// Creates the "lib" folder
	strcpy(txt, project_name);
	strcat(txt, "/lib");
	mkdir(txt, 0700);
}

void create_project(options_t *options) 
{
	create_project_folder(options->project_name);
	if(options->create_git)
		create_gitrepo(options);
	create_makefile(options);
	create_mainfile(options);
	create_readme(options);

}


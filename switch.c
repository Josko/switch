/*
* Switch, switches the pointer and focus to the next X screen using xcb
* Copyright (C) 2011 Joško Nikolić
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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <xcb/xcb.h>

#define PROGRAM_NAME "Switch"
#define PROGRAM_VERSION "0.1"

static struct option const long_options[] =
{
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{"screen", required_argument, NULL, 's'},
	{NULL, 0, NULL, 0}
};

static void usage(char *argv[])
{
	printf("Usage: %s\n"
		"\t-v, --version	Show version and exit\n"
		"\t-h, --help	Show this help message and exit\n",
		argv[0]);
}

int main(int argc, char *argv[])
{
	/*
	 * variables
	 */

	char c;
	int i, screenNum;

	xcb_connection_t 							*connection;
	xcb_screen_t 									*next_screen;
	xcb_screen_t									*current_screen;
	xcb_screen_iterator_t 				iterator;
	xcb_setup_t 									*setup;
	xcb_query_pointer_reply_t			*pointer;

	/*
	 * handle the arguments
	 */

	while (-1 != (c = getopt_long(argc, argv, "hv", long_options, NULL)))
	{
		switch (c)
		{
			case 'h':
				usage(argv);
				return 0;

			case 'v':
				printf("GPL %s: %s\n"
							 "Copyright (C) 2011 Joško Nikolić\n", PROGRAM_NAME, PROGRAM_VERSION);
				return 0;

			default:
				usage(argv);
		}
	}

	/*
	 * open the connection and find the current screen
	 */

	connection = xcb_connect(NULL, &screenNum);

	if (xcb_connection_has_error(connection))
	{
		puts("ERROR: couldn't open display");
		exit(EXIT_FAILURE);
	}

	setup = xcb_get_setup(connection);
	iterator = xcb_setup_roots_iterator(setup);

	for (i = 0; i < screenNum; ++i)
		xcb_screen_next(&iterator);


	current_screen = iterator.data;

	/*
	 * get the next screen to switch to
	 */

	xcb_screen_next(&iterator);

	/*
	 * if the next screen doesn't exist move the iterator to start
	 */

	if (0 == (iterator.data)->width_in_pixels && 0 == (iterator.data)->height_in_pixels)
	{
		iterator = xcb_setup_roots_iterator(setup);

		/*
		 * the next screen is the current one
		 */

		if ((iterator.data)->width_in_pixels == current_screen->width_in_pixels && (iterator.data)->height_in_pixels == current_screen->height_in_pixels)
		{
			puts("ERROR: only one screen");
			return EXIT_SUCCESS;
		}
	}

	next_screen = iterator.data;

	/*
	 * get pointer information
	 */

	pointer = xcb_query_pointer_reply(connection,
																		xcb_query_pointer_unchecked(connection, current_screen->root),
																		NULL);

	if (NULL != pointer)
	{
	 /*
	 	* warp the pointer
	 	*/

		xcb_warp_pointer(connection, XCB_NONE, next_screen->root, 0, 0, 0, 0, pointer->win_x, pointer->win_y);
		xcb_set_input_focus(connection, XCB_INPUT_FOCUS_PARENT, next_screen->root, XCB_CURRENT_TIME);
		xcb_flush(connection);

		free(pointer);
	}
	else
		puts("ERROR: couldn't query pointer");
	
	xcb_disconnect(connection);

	return EXIT_SUCCESS;
}

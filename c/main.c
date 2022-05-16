/* Main 
parser experiment */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "levenshtein.h"
#include "linked_list.h"
#include "world.h"
#include "world_const.h"

enum game_states {game_run, game_exit};

/* globals (boooh) */
int game_state = game_run;
BOOL has_moved = TRUE;
#define STR_BUFFER_SIZE 256

#define OBJECT_IN_ROOM(I) (objectGetRoom(I) == worldGetCurrentRoom())
#define OBJECT_IN_INVENTORY(I) (objectGetRoom(I) == TAG_PLAYER)

/* private functions */
unsigned int _query_word_count(char *query) {
	unsigned int i, l, word_count = 1;
	l = (unsigned int)strlen(query);
	for (i = 0; i < l; i++)
		if (query[i] == ' ') word_count++;

	return word_count;
}

char *_cleanup_str(char *src_str) {
	short j;
	char *token, *next_token, *str_copy, *clean_str;
	str_copy = (char *)malloc(sizeof(char) * STR_BUFFER_SIZE);
	strcpy(str_copy, src_str);
	clean_str = (char *)malloc(sizeof(char) * STR_BUFFER_SIZE);
	memset(clean_str, 0, sizeof(char) * STR_BUFFER_SIZE);

	token = strtok(str_copy, " ");
	while (token) {
		next_token = strtok(NULL, " ");
		j = 0;
		while (j < vocabularyGetIgnoredWordsCount() && strcmp(token, ignored_words[j]))
			j++;
		if (j >= vocabularyGetIgnoredWordsCount()) {
			clean_str = strcat(clean_str, token);
			if (next_token)
				strcat(clean_str, " ");
		}
		token = next_token;
	}
	strcpy(src_str, clean_str);
	free(str_copy);
	free(clean_str);
	return src_str;
}

void _action_go_direction(short dir) {
	short room_index;
	room_index = roomGetLinkDirection(worldGetCurrentRoom(), dir);
	if (room_index > -1) {
		printf("Let's go !\n");
		worldSetCurrentRoom(room_index);
		has_moved = TRUE;
	}
	else {
		printf("You can't go there.\n");
		has_moved = FALSE;
	}
}

void action_exit_game(void) {
	game_state = game_exit;
}

void action_look(void) {
	short i, j = 0;

	/* Get the description of the room */
	printf("%s\n", roomGetDescription(worldGetCurrentRoom()));
	printf("You see: ");

	/* Iterate on all world objects and find those in the current room */
	for (i = 0; i < worldGetMaxObject(); i++) {
//		printf("%s, %d, %s, %s, %d\n", objectGetNameByIndex(i), objectGetVisibleByIndex(i), objectGetRoomByIndex(i), worldGetCurrentRoom(), strcmp(objectGetRoomByIndex(i), worldGetCurrentRoom()));
		if (OBJECT_IN_ROOM(i))
			if (objectGetVisible(i)) {
				if (j++ > 0) printf(", ");
					printf("%s", objectGetName(i));
			}
	}

	if (j == 0) printf("nothing.");
	printf("\n");

	has_moved = FALSE;
}

void action_take_object(char *object_name) {
	short object_index;
	has_moved = FALSE;

	object_index = objectGetIndexByName(object_name);

	if (object_index > -1)
		if (OBJECT_IN_ROOM(object_index)) /* if the object is around */
			if (objectGetVisible(object_index)) /* if this object is visible */
				if (objectGetTakeable(object_index)) { /* if this object can be taken */
					printf("You take the %s.\n", object_name);
					objectSetRoom(object_index, TAG_PLAYER);
					return;
				}
				else {
					printf("You cannot take the %s.\n", object_name);
					return;
				}

	printf("There is no %s here.\n", object_name);
}

void action_look_object(char *object_name) {
	short object_index;
	char *desc;

	has_moved = FALSE;
	object_index = objectGetIndexByName(object_name);

	if (object_index > -1)
	{
		if (OBJECT_IN_ROOM(object_index) || OBJECT_IN_INVENTORY(object_index))/* if the object is around */
		{
			if (objectGetVisible(object_index))
			{ /* if this object is visible */
				desc = objectGetDescription(object_index);
				if (desc[0])
				{ /* if this object has a description */
					printf("%s\n", desc);
					return;
				}
				else
				{
					printf("It's a %s.\n", objectGetName(object_index));
					return;
				}
			}
		}
	}
	
	printf("There is no %s here.\n", object_name);
}

void action_inventory(void) {
	int i, j = 0;
	printf("You carry :\n");
	for (i = 0; i < worldGetMaxObject(); i++) {
		if (OBJECT_IN_INVENTORY(i)) {
			if (j++ > 0) printf(", ");
			printf("%s", objectGetName(i));
		}
	}
	if (i == 0)
		printf(" ...nothing!\n");
	else
		printf("\n");

	has_moved = FALSE;
}

void action_go_north(void) {
	_action_go_direction((short)dir_north);
}

void action_go_east(void) {
	_action_go_direction((short)dir_east);
}

void action_go_south(void) {
	_action_go_direction((short)dir_south);
}

void action_go_west(void) {
	_action_go_direction((short)dir_west);
}

void action_where_am_i(void) {
	printf("You are in a %s.\n", roomGetName(worldGetCurrentRoom()));
	has_moved = FALSE;
}

void action_take_without_object(void) {
	printf("Ok, but what ?\n");
	has_moved = FALSE;
}

/* verb-only actions */
const static struct {
	const char *name;
	void(*func)(void);
} verb_function_map[] = {
	{ "exit", action_exit_game },
	{ "where", action_where_am_i },
	{ "look", action_look },
	{ "take", action_take_without_object },
	{ "inventory", action_inventory },
	{ "north", action_go_north },
	{ "east", action_go_east },
	{ "south", action_go_south },
	{ "west", action_go_west }
};

int interpret_verb_query(const char *name)
{
	int i;

	for (i = 0; i < (sizeof(verb_function_map) / sizeof(verb_function_map[0])); i++) {
		if (fuzzy_strcmp(verb_function_map[i].name, name) >= 80 && verb_function_map[i].func) {

			verb_function_map[i].func();
			return 0;
		}
	}

	return -1;
}

/* verb+subject actions */
const static struct {
	const char *name;
	void(*func)(char *);
} verb_subject_function_map[] = {
	{ "take", action_take_object },
	{ "look", action_look_object }
};

int interpret_verb_subject_query(const char *verb, const char *subject)
{
	int i;

	for (i = 0; i < (sizeof(verb_subject_function_map) / sizeof(verb_subject_function_map[0])); i++) {
		if (strcmp(verb_subject_function_map[i].name, verb) == 0 && verb_subject_function_map[i].func) {

			verb_subject_function_map[i].func((char *)subject);
			return 0;
		}
	}

	return -1;
}

int main(int argc, char **argv) {
	char *query, *verb, *subject;

	printf("*** PARSER EXPERIMENT ***\n");

#ifdef _WIN32
	printf("WIN32 COMPATIBLE BUILD\n");
#else
	printf("AMIGA SAS/C COMPATIBLE BUILD\n");
	#define _AMIGA
#endif

	query = (char *)malloc(sizeof(char) * STR_BUFFER_SIZE);
	verb = (char *)malloc(sizeof(char) * STR_BUFFER_SIZE);
	subject = (char *)malloc(sizeof(char) * STR_BUFFER_SIZE);

	/* World Test */
	printf("World settings :\n");
	printf(" * %d rooms\n", worldGetMaxRoom());
	printf(" * %d objects\n", worldGetMaxObject());

	while(game_state != game_exit) {
		if (has_moved) action_where_am_i();

		printf("> ");
		fgets(query, sizeof(char) * STR_BUFFER_SIZE, stdin);

		query = strtok(query, "\n");
		query = _cleanup_str(query);
		printf(" > %s\n", query);

		switch (_query_word_count(query)) {
			case 1:
				if (interpret_verb_query(query) == -1)
					printf("Sorry, I didn't get that.\n");
				break;
			case 2:
				verb = strtok(query, " ");
				subject = strtok(NULL, " ");
				if (interpret_verb_subject_query(verb, subject) == -1)
					printf("Sorry, I didn't get that verb/subject.\n");
				break;
			default:
				printf("Sorry, I can't read sentence with more than 2 words.\n");
				break;
		}
	}

	free(query);
	free(verb);
	free(subject);

	printf("You've just quit the adventure!\n");
#ifdef _WIN32
	printf("Press [ENTER]\n");

	fgets(query, sizeof(char) * STR_BUFFER_SIZE, stdin);
#endif
	return 1;
}
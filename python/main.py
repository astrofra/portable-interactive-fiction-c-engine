# -*- coding: utf-8 -*-

# text adventure parser & navigator
#
# Notes : each room or object is indexed with a unique ID,
# whose range goes from 0 to n
# This unique ID is based on an initial alphanum sort of each
# room name or object name.
# As a consequence, changing the language might change the order
# and thus the ID, hence making a save game invalid when switching
# from a language to another.
# To avoid this, the translation process occurs after the
# indexed are built.

import json
import os
import hashlib

world_file = os.path.realpath(os.path.join(os.getcwd(), "world.json"))
out_c_file = "../c/world.c"
out_h_file = "../c/world_const.h"

enum_link_direction = ["north", "east", "south", "west", "up", "down", "other"]

FILENAME_OBFUSCATION_ENABLED = False
MAX_DESCRIPTION_CHAR_LEN = 256
MAX_FILENAME_CHAR_LEN = 64
MAX_INTERACTION_PER_OBJECT = 4
FILENAME_SALT = "CrueltyhasahumanheartAndJealousyahumanfaceTerrorthehumanformdivineAndSecresythehumandress\
ThehumandressisforgedironThehumanformafieryforgeThehumanfaceafurnacesealedThehumanheartitshungrygorge"

room_names = []
object_names = []
interaction_count = 0
interaction_table = []
verbs = []
callbacks = []
dialogue_count = 0
object_count = 0


def find_element_in_list(element, list_element):
	try:
		index_element = list_element.index(element)
		return index_element
	except ValueError:
		return None


def bool_to_str(flag):
	if flag:
		return 'TRUE'
	else:
		return 'FALSE'


def str_or_none(val):
	if val is None:
		return 'None'
	else:
		return str(val)


def tr(original_string):
	translated_string = original_string
	return translated_string


def obfuscate_filename(filename_str):
	s = hashlib.sha1()
	s.update((filename_str.split(".")[0] + FILENAME_SALT).encode())

	return filename_str[:3] + '_' + s.hexdigest()[::3] + "." + filename_str.split(".")[-1]


if os.path.exists(world_file):
	with open(world_file) as json_data:
		out = open(out_c_file, "w")
		out_h = open(out_h_file, "w")

		world = json.loads(json_data.read())

		# enumerate rooms, object & verbs listed in a sorted way
		for room_name in sorted(world["rooms"].keys()):
			room_names.append(room_name)
			# Count how many objects
			if "objects" in world["rooms"][room_name]:
				for object_name in world["rooms"][room_name]["objects"]:
					object_count += 1
					if "interaction" in world["rooms"][room_name]["objects"][object_name]:
						for interaction in world["rooms"][room_name]["objects"][object_name]["interaction"]:
							if "action" in interaction:
								interaction_count += 1
								if "verb" in interaction["action"]:
									verb = interaction["action"]["verb"]
									if not(verb in verbs):
										verbs.append(verb)
							if "feedback" in interaction:
								if "callback" in interaction["feedback"]:
									callback = interaction["feedback"]["callback"]
									if not(callback in callbacks):
										callbacks.append(callback)
					if "dialogue" in world["rooms"][room_name]["objects"][object_name]:
						for dialogue in world["rooms"][room_name]["objects"][object_name]["dialogue"]:
							dialogue_count += 1

					# if find_element_in_list(object_name, "object_names"):
					# 	print("/!\ ERROR : Object '" + object_name + "' is already in the objects list.")
					# 	exit(-1)
					# object_names.append(object_name)

		# sort by alphanum
		room_names.sort()
		# object_names.sort()

		# World constants
		out_h.write('/* World constants */\n')
		out_h.write('#define MAX_ROOM ' + str(len(world["rooms"])) + '\n')
		out_h.write('#define MAX_OBJECT ' + str(object_count) + '\n')
		out_h.write('#define MAX_INTERACTION ' + str(interaction_count) + '\n')
		out_h.write('#define MAX_DIALOGUE ' + str(dialogue_count) + '\n')

		# C initialization
		out.write('#include <stdio.h>\n')
		out.write('#include <string.h>\n')
		out.write('#include <stdlib.h>\n')
		out.write('#include "world.h"\n\n')
		out.write('#include "world_const.h"\n\n')

		# Vocabulary #
		# Define a set of excluded words that will be ignored by the parser
		if "vocabulary" in world and "ignored" in world["vocabulary"] and len(world["vocabulary"]["ignored"]) > 0:
			out_h.write('#define IGNORED_WORDS_COUNT ' + str(len(world["vocabulary"]["ignored"])) + '\n')
			out_h.write('\n')
			out_h.write('char *ignored_words[IGNORED_WORDS_COUNT];\n')

			out.write('char *ignored_words[IGNORED_WORDS_COUNT] = {\n')
			out.write('\t')
			i = 0
			for word in world["vocabulary"]["ignored"]:
				out.write('"' + word + '"')
				if i < len(world["vocabulary"]["ignored"]) - 1:
					out.write(', ')
				i += 1
			out.write('};\n')
			out.write('\n')

		# actions (verbs)
		if len(verbs) > 0:
			out.write('/* verbs (entry point of an action) found */\n')
			out.write('/* ')
			for verb in verbs:
				out.write(verb + ', ')
			out.write('*/\n\n')

		if len(callback) > 0:
			out.write('/* callbacks (how the action affects the game) found */\n')
			out.write('/* ')
			for callback in callbacks:
				out.write(callback + ', ')
			out.write('*/\n\n')

		# World data (as arrays of structs)

		# collect all rooms
		out.write('/***********/\n')
		out.write('/*  Rooms  */\n')
		out.write('/***********/\n')

		out.write('\n')

		# Game initial state (where the player is spawned...)
		if 'room' in world['init']:
			out.write('short current_room = ' + str(room_names.index(world['init']['room'])) + ';\n')
		else:
			raise ValueError('No current_room defined.')

		out.write('\n')

		i = 0
		out.write('room rooms[MAX_ROOM] = {\n')
		for room_name in room_names:
			print(room_name)
			out.write('\t{ /*    ' + str(room_name).upper() + '    */\n')
			# room name
			if "short" in world["rooms"][room_name]:
				out.write('\t\t"' + world["rooms"][room_name]["short"] + '",\n')

			# room description
			description = ''
			if "description" in world["rooms"][room_name]:
				description = world["rooms"][room_name]["description"][:MAX_DESCRIPTION_CHAR_LEN]
			out.write('\t\t"' + description + '",\n')

			# bitmap
			bitmap_filename = ''
			if "bitmap" in world["rooms"][room_name]:
				bitmap_filename = world["rooms"][room_name]["bitmap"][:MAX_FILENAME_CHAR_LEN]
				if FILENAME_OBFUSCATION_ENABLED:
					bitmap_filename = obfuscate_filename(bitmap_filename)
			out.write('\t\t"' + bitmap_filename + '",\n')

			# links to the other rooms
			links = [None] * len(enum_link_direction)
			if "links" in world["rooms"][room_name]:
				j = 0
				for link_dir in enum_link_direction:
					if link_dir in world["rooms"][room_name]["links"]:
						links[j] = world["rooms"][room_name]["links"][link_dir]["room"]
						# link_index = room_names.index(link_to_room_name)
					j += 1

			out.write('\t\t{')
			str_comment = '/* '
			for j in range(len(enum_link_direction)):
				link_index = -1
				if links[j] is not None:
					link_index = room_names.index(links[j])
					str_comment += enum_link_direction[j] + ':' + links[j].upper() + ','
				out.write(str(link_index))
				if j < len(enum_link_direction) - 1:
					out.write(',')
					# else:
					# 	out.write('NULL;\n')
			str_comment += ' */'
			out.write('}')
			out.write('\t' + str_comment)
			out.write('\n')

			out.write('\t},\n')
			out.write('\n')
			i += 1
		out.write('\t/* End of rooms list */\n')
		out.write('};\n')
		out.write('\n')

		# collect all objects
		out.write('/***********/\n')
		out.write('/* Objects */\n')
		out.write('/***********/\n')

		out.write('object objects[MAX_OBJECT] = {\n')

		object_index = 0
		for room_name in room_names:
			if "objects" in world["rooms"][room_name]:
				for object_name in world["rooms"][room_name]["objects"]:
					object_names.append(object_name)

					current_object = world["rooms"][room_name]["objects"][object_name]
					out.write('\t{ /* ' + object_name.upper() + ' */\n')
					# object name
					out.write('\t\t"' + object_name + '",\n')

					# object description
					description = ''
					if "description" in current_object:
						description = current_object["description"][:MAX_DESCRIPTION_CHAR_LEN]
					out.write('\t\t"' + description + '",\n')

					# room in which the object is found initially
					room_index = room_names.index(room_name)
					out.write('\t\t' + str(room_index) + ', /* ' + room_name + ' */\n')

					# interactions
					object_interaction_table = [-1] * MAX_INTERACTION_PER_OBJECT
					if "interaction" in world["rooms"][room_name]["objects"][object_name]:
						j = 0
						for interaction in world["rooms"][room_name]["objects"][object_name]["interaction"]:
							interaction_table.append({"object_name": object_name, "object_index": object_index, "interaction": interaction})
							object_interaction_table[j] = len(interaction_table) - 1
							j += 1

					out.write('\t\t{')
					str_comment = '/* '
					j = 0
					for interaction_index in object_interaction_table:
						# str_comment += enum_link_direction[j] + ':' + links[j].upper() + ','
						out.write(str(interaction_index))
						if j < len(enum_link_direction) - 1:
							out.write(',')
						j += 1
					# else:
					# 	out.write('NULL;\n')
					str_comment += ' */'
					out.write('},')
					out.write('\t' + str_comment)
					out.write('\n')

					# takeable/visible status
					takeable = True
					if "takeable" in current_object:
						takeable = current_object["takeable"]
					out.write('\t\t' + bool_to_str(takeable) + ', /* Takeable */\n')

					visible = True
					if "visible" in current_object:
						visible = current_object["visible"]
					out.write('\t\t' + bool_to_str(visible) + ', /* Visible */\n')

					out.write('\t},\n\n')

				object_index += 1

		out.write('\t/* End of objects list */\n')
		out.write('};\n')

		out.write('\n')

		# collect all interactions
		out.write('/****************/\n')
		out.write('/* Interactions */\n')
		out.write('/****************/\n')

		out.write('interaction interactions[MAX_INTERACTION] = {\n')

		for interaction in interaction_table:
			out.write('\t{ /* subject : ' + interaction["object_name"] + '*/ \n')
			if "action" in interaction["interaction"]:
				if "verb" in interaction["interaction"]["action"]:
					out.write('\t\t' + str(find_element_in_list(interaction["interaction"]["action"]["verb"], verbs)) + ',')
					out.write(' /* ' + interaction["interaction"]["action"]["verb"] + ' */\n')
				else:
					out.write('\t\t-1,\n')

			if "object_index" in interaction:
				out.write('\t\t' + str(interaction["object_index"]) + ',')
				out.write(' /* ' + interaction["object_name"] + ' */\n')
			else:
				out.write('\t\t-1,\n')

			if "feedback" in interaction["interaction"]:
				if "callback" in interaction["interaction"]["feedback"]:
					out.write('\t\t' + str(find_element_in_list(interaction["interaction"]["feedback"]["callback"], callbacks)) + ',')
					out.write(' /* ' + interaction["interaction"]["feedback"]["callback"] + ' */\n')
				else:
					out.write('\t\t-1,\n')

				if "target" in interaction["interaction"]["feedback"]:
					out.write('\t\t' + str(find_element_in_list(interaction["interaction"]["feedback"]["target"], object_names)) + ',')
					out.write(' /* ' + interaction["interaction"]["feedback"]["target"] + ' */\n')
				else:
					out.write('\t\t-1,\n')

				if "response" in interaction["interaction"]["feedback"]:
					out.write('\t\t"' + interaction["interaction"]["feedback"]["response"] + '",\n')
				else:
					out.write('\t\t"\\0",\n')

			out.write('\t},\n')

		out.write('\t/* End of interactions list */\n')
		out.write('};\n')

		out.write('\n')

		# functions

		out.write('/* Various accessors */\n')
		if "vocabulary" in world and "ignored" in world["vocabulary"] and len(world["vocabulary"]["ignored"]) > 0:
			out.write('short vocabularyGetIgnoredWordsCount(void) { return ' + str(len(world["vocabulary"]["ignored"])) + '; }\n')
			out.write('\n')

		out.write('short worldGetMaxRoom(void) { return MAX_ROOM; }\n')
		out.write('short worldGetMaxObject(void) { return MAX_OBJECT; }\n')
		out.write('short worldGetCurrentRoom(void) { return current_room; }\n')
		out.write('void worldSetCurrentRoom(short room_index) { current_room = room_index; }\n')

		out.write('short roomGetIndexByName(char *room_name) { /* returns the index of a room if found, -1 otherwise */\n\
\tshort i;\n\
\tfor(i = 0; i < MAX_ROOM; i++)\n\
\t\tif(strncmp(room_name, rooms[i].name, MAX_NAME_CHAR_LEN) == 0) return(i);\n\
\treturn(-1);\n\
}\n')

		out.write('short objectGetIndexByName(char *object_name) { /* returns the index of an object if found, -1 otherwise */\n\
\tshort i;\n\
\tfor(i = 0; i < MAX_OBJECT ; i++)\n\
\t\tif(strncmp(object_name, objects[i].name, MAX_NAME_CHAR_LEN) == 0) return(i);\n\
\treturn(-1);\n\
}\n')

		out.write('char *objectGetName(short object_index) { return objects[object_index].name; }\n')
		out.write('short objectGetRoom(short object_index) { return objects[object_index].room; }\n')
		out.write('void objectSetRoom(short object_index, short room_index) { objects[object_index].room = room_index; }\n')
		out.write('BOOL objectGetVisible(short object_index) { return objects[object_index].visible; }\n')
		out.write('BOOL objectGetTakeable(short object_index) { return objects[object_index].takeable; }\n')
		out.write('char *objectGetDescription(short object_index) { return objects[object_index].description; }\n')

		out.write('char *roomGetName(short room_index) { return rooms[room_index].name; }\n')
		out.write('char *roomGetDescription(short room_index) { return rooms[room_index].description; }\n')
		out.write('short roomGetLinkDirection(short room_index, short dir) { return rooms[room_index].links[dir]; }\n')
		out.write('\n')
#
		out_h.close()
		out.close()
		json_data.close()
# else:
# 	print("File : " + world_file + " not found!")
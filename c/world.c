#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "world.h"

#include "world_const.h"

char *ignored_words[IGNORED_WORDS_COUNT] = {
	"the", "a", "at", "to", "for", "with"};

/* verbs (entry point of an action) found */
/* look, give, */

/* callbacks (how the action affects the game) found */
/* show, acquire, */

/***********/
/*  Rooms  */
/***********/

short current_room = 0;

room rooms[MAX_ROOM] = {
	{ /*    CNO025    */
		"harbor",
		"Your boat is moored at the edge of the dock. To the north, there is an entrance to the city.",
		"vue_25.pak",
		{4,1,-1,-1,-1,-1,-1}	/* north:CNO029,east:CNO026, */
	},

	{ /*    CNO026    */
		"mosque",
		"You move forward a little along the dock. The lighthouse seems deserted; a magnificent and very old mosque has been erected here.",
		"vue_26.pak",
		{-1,2,-1,0,-1,-1,-1}	/* east:CNO027,west:CNO025, */
	},

	{ /*    CNO027    */
		"fishermen",
		"A light ripple against the shore accompanies the few fishermen in their boats.",
		"vue_27.pak",
		{-1,3,-1,1,-1,-1,-1}	/* east:CNO028,west:CNO026, */
	},

	{ /*    CNO028    */
		"rack",
		"At the other end of the dock, there is a drying rack for fish and octopus.",
		"vue_28.pak",
		{-1,-1,-1,2,-1,-1,-1}	/* west:CNO027, */
	},

	{ /*    CNO029    */
		"north entrance",
		"You are at the north entrance of the village. Going south takes you back to the dock.",
		"vue_29.pak",
		{5,11,0,-1,-1,-1,-1}	/* north:CNO030,east:CNO036,south:CNO025, */
	},

	{ /*    CNO030    */
		"street",
		"A few little houses here and there, one of which seems to be a shop.",
		"vue_30.pak",
		{7,-1,4,6,-1,-1,-1}	/* north:CNO032,south:CNO029,west:CNO031, */
	},

	{ /*    CNO031    */
		"wine shop",
		"All sorts of amphoras are stacked here, and the wine merchant looks at you with interest.",
		"vue_31.pak",
		{-1,5,-1,-1,-1,-1,-1}	/* east:CNO030, */
	},

	{ /*    CNO032    */
		"village",
		"You journey through the village, a seemingly playful dog watches you and wags his tail.",
		"vue_32.pak",
		{-1,8,5,-1,-1,-1,-1}	/* east:CNO033,south:CNO030, */
	},

	{ /*    CNO033    */
		"square",
		"You move forward along the square. Time seems frozen in these old stones!",
		"vue_33.pak",
		{9,-1,11,7,-1,-1,-1}	/* north:CNO034,south:CNO036,west:CNO032, */
	},

	{ /*    CNO034    */
		"inn entrance",
		"You climb the few steps that separate you from the arbour.",
		"vue_34.pak",
		{16,-1,8,-1,-1,-1,-1}	/* north:CNO041,south:CNO033, */
	},

	{ /*    CNO035    */
		"inn",
		"You're in the inn, the surrounding is relaxed. All you can hear is heavy snoring.",
		"vue_35.pak",
		{-1,9,-1,-1,-1,-1,-1}	/* east:CNO034, */
	},

	{ /*    CNO036    */
		"village",
		"You walk in the village. It seems to be asleep.",
		"vue_36.pak",
		{8,12,-1,4,-1,-1,-1}	/* north:CNO033,east:CNO037,west:CNO029, */
	},

	{ /*    CNO037    */
		"village",
		"You're at the southern edge of the village. This intricately carved door seems well guarded. Be wary...",
		"vue_37.pak",
		{-1,-1,-1,11,-1,-1,-1}	/* west:CNO036, */
	},

	{ /*    CNO038    */
		"room",
		"This room has been turned upside down... Those responsible were looking for something...",
		"vue_38.pak",
		{14,-1,12,-1,-1,-1,-1}	/* north:CNO039,south:CNO037, */
	},

	{ /*    CNO039    */
		"room",
		"They haven't spared anything here either. What a rampage!",
		"vue_39.pak",
		{-1,-1,13,-1,-1,-1,-1}	/* south:CNO038, */
	},

	{ /*    CNO040    */
		"secret room",
		"The mechanism gave you access to this room. A pedestal is in its centre.",
		"vue_40.pak",
		{-1,-1,14,-1,-1,-1,-1}	/* south:CNO039, */
	},

	{ /*    CNO041    */
		"end of the village",
		"It's impossible to go further. A street peddler is standing here.",
		"vue_41.pak",
		{-1,-1,9,-1,-1,-1,-1}	/* south:CNO034, */
	},

	{ /*    CNO042    */
		"with Lilla",
		"Lilla seems to be so grateful for the good news you bring that she let herself go a bit.",
		"vue_42.pak",
		{-1,-1,-1,-1,-1,-1,-1}	/*  */
	},

	/* End of rooms list */
};

/***********/
/* Objects */
/***********/
object objects[MAX_OBJECT] = {
	{ /* BOAT */
		"boat",
		"A magnificient ship, really!",
		0, /* cno025 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* CAT */
		"cat",
		"He implores you with his eyes! What a showman!",
		1, /* cno026 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* WELL-FED CAT */
		"well-fed cat",
		"The cat is well-fed without a doubt! Hmm...",
		1, /* cno026 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		FALSE, /* Visible */
	},

	{ /* MOSQUE */
		"mosque",
		"It's closed!",
		1, /* cno026 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* FISHERMAN */
		"fisherman",
		"",
		2, /* cno027 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* DRYING RACK */
		"drying rack",
		"",
		3, /* cno028 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* POT */
		"pot",
		"A vase planted with a few flowers.",
		4, /* cno029 */
		{0,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* FLOWER */
		"flower",
		"These are crocuses.",
		4, /* cno029 */
		{-1,-1,-1,-1,},	/*  */
		TRUE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* SHOP */
		"shop",
		"This little house is a wine merchant's stall.",
		5, /* cno030 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* AMPHORA */
		"amphora",
		"Magnificent antique amphoras, made in a way only the Minoan civilisation does.",
		6, /* cno031 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* FILLED OINOCHOE */
		"filled oinochoe",
		"",
		6, /* cno031 */
		{-1,-1,-1,-1,},	/*  */
		TRUE, /* Takeable */
		FALSE, /* Visible */
	},

	{ /* SHOPKEEPER */
		"shopkeeper",
		"...",
		6, /* cno031 */
		{1,2,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* DOG */
		"dog",
		"He's obviously waiting to run after something!",
		7, /* cno032 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* TREE */
		"tree",
		"A beautiful olive tree.",
		8, /* cno033 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* LILLA */
		"Lilla",
		"Lilla seems to be waiting for you in front of the inn.",
		9, /* cno034 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* INNKEEPER */
		"innkeeper",
		"The innkeeper seems to consume more than he serves, he's slumped behind the counter.",
		10, /* cno035 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* DOOR */
		"door",
		"The door is guarded.",
		12, /* cno037 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* GUARD */
		"guard",
		"",
		12, /* cno037 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* OINOCHOE */
		"oinochoe",
		"What a beautiful oinochoe!",
		13, /* cno038 */
		{-1,-1,-1,-1,},	/*  */
		TRUE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* MESS */
		"mess",
		"It seems that they haven't spared anything, except maybe various pots and pitchers.",
		13, /* cno038 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* CHEST */
		"chest",
		"There's nothing interesting there.",
		13, /* cno038 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* BOARD */
		"board",
		"The board topples over, revealing a series of strange symbols. This may well be what the perpetrators were looking for.",
		14, /* cno039 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* SYMBOLS */
		"symbols",
		"Strange symbols.",
		14, /* cno039 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* TENTACLE */
		"tentacle",
		"A fossilized tentacle rests on it. Expect to see a huge stone emerge if you take it!",
		15, /* cno040 */
		{-1,-1,-1,-1,},	/*  */
		TRUE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* PEDESTAL */
		"pedestal",
		"Nothing but this pedestal, which takes centre stage.",
		15, /* cno040 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* PEDDLER */
		"peddler",
		"A street peddler is standing standing in front of you.",
		16, /* cno041 */
		{3,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* DOORS */
		"doors",
		"Both doors appear to be closed.",
		16, /* cno041 */
		{-1,-1,-1,-1,},	/*  */
		FALSE, /* Takeable */
		TRUE, /* Visible */
	},

	{ /* SAFFRON */
		"saffron",
		"",
		16, /* cno041 */
		{-1,-1,-1,-1,},	/*  */
		TRUE, /* Takeable */
		TRUE, /* Visible */
	},

	/* End of objects list */
};

/****************/
/* Interactions */
/****************/
interaction interactions[MAX_INTERACTION] = {
	{ /* subject : pot*/ 
		0, /* look */
		4, /* pot */
		0, /* show */
		7, /* flower */
		"A vase planted with a few flowers.",
	},
	{ /* subject : shopkeeper*/ 
		1, /* give */
		6, /* shopkeeper */
		-1,
		-1,
		"Ah! Thank you, this ingredient is rare for me, I owe you big time...",
	},
	{ /* subject : shopkeeper*/ 
		1, /* give */
		6, /* shopkeeper */
		1, /* acquire */
		10, /* filled oinochoe */
		"That's such a beautiful oinochoe, I'll fill it up with nectar for you. You'll tell me what happens to it! Take care though, eh?",
	},
	{ /* subject : peddler*/ 
		1, /* give */
		15, /* peddler */
		1, /* acquire */
		27, /* saffron */
		"Thank you! Here, take some saffron in exchange.",
	},
	/* End of interactions list */
};

/* Various accessors */
short vocabularyGetIgnoredWordsCount(void) { return 6; }

short worldGetMaxRoom(void) { return MAX_ROOM; }
short worldGetMaxObject(void) { return MAX_OBJECT; }
short worldGetCurrentRoom(void) { return current_room; }
void worldSetCurrentRoom(short room_index) { current_room = room_index; }
short roomGetIndexByName(char *room_name) { /* returns the index of a room if found, -1 otherwise */
	short i;
	for(i = 0; i < MAX_ROOM; i++)
		if(strncmp(room_name, rooms[i].name, MAX_NAME_CHAR_LEN) == 0) return(i);
	return(-1);
}
short objectGetIndexByName(char *object_name) { /* returns the index of an object if found, -1 otherwise */
	short i;
	for(i = 0; i < MAX_OBJECT ; i++)
		if(strncmp(object_name, objects[i].name, MAX_NAME_CHAR_LEN) == 0) return(i);
	return(-1);
}
char *objectGetName(short object_index) { return objects[object_index].name; }
short objectGetRoom(short object_index) { return objects[object_index].room; }
void objectSetRoom(short object_index, short room_index) { objects[object_index].room = room_index; }
BOOL objectGetVisible(short object_index) { return objects[object_index].visible; }
BOOL objectGetTakeable(short object_index) { return objects[object_index].takeable; }
char *objectGetDescription(short object_index) { return objects[object_index].description; }
char *roomGetName(short room_index) { return rooms[room_index].name; }
char *roomGetDescription(short room_index) { return rooms[room_index].description; }
short roomGetLinkDirection(short room_index, short dir) { return rooms[room_index].links[dir]; }


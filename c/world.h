#define MAX_LINK_PER_ROOM 7
#define MAX_INTERACTION_PER_OBJECT 4
#define MAX_DIALOGUE_PER_OBJECT 4
#define MAX_NAME_CHAR_LEN 32
#define MAX_FILENAME_CHAR_LEN 64
#define MAX_DESCRIPTION_CHAR_LEN 256
#define TAG_PLAYER -1

#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// char *ignored_words[];

/* Link directions, CW then UP and DOWN then some other random exit */
enum link_direction { dir_north, dir_east, dir_south, dir_west, dir_up, dir_down, dir_other};

typedef struct {
	char question[MAX_DESCRIPTION_CHAR_LEN];
	char answer[MAX_DESCRIPTION_CHAR_LEN];
} dialogue;

typedef struct {
	short verb; /* "look", "give", "move", "lift", "pull" */
	short subject; /* target id of the action (object, room, exit...) */
	short callback; /* "acquire", "unlock", "show" */
	short target; /* target id of the callback (object, room, exit...) */
	char response[MAX_DESCRIPTION_CHAR_LEN];
} interaction;

typedef struct {
	char name[MAX_NAME_CHAR_LEN];
	char description[MAX_DESCRIPTION_CHAR_LEN];
	short room; /* index of the room where the object is located */
	short interactions[MAX_INTERACTION_PER_OBJECT]; /* array of indexes */
	// short dialogues[MAX_DIALOGUE_PER_OBJECT];
	BOOL takeable; /* ex : door = false; book = true */
	BOOL visible;
} object;

// typedef struct {
// 	char *name; /* north, south, ... */
// 	char *description; /* ex: a large gate blocks the way */
// 	char *room; /* room name */
// 	bool open; /* default : true */
// 	bool visible; /* default : true */
// } link;

typedef struct {
	char name[MAX_NAME_CHAR_LEN];
	char description[MAX_DESCRIPTION_CHAR_LEN]; /* textual description of the room */
	char bitmap[MAX_FILENAME_CHAR_LEN];
	short links[MAX_LINK_PER_ROOM];
} room;

/* For automatic sentence composition with a correct grammar */ 
#define GENDER_NEUTRAL 0
#define GENDER_FEMININE 1
#define GENDER_MASCULINE 2

// gender_table[2][2] = {
// 	{ "guard", GENDER_MASCULINE },
// 	{ "key", GENDER_NEUTRAL }
// }

short vocabularyGetIgnoredWordsCount(void);

short worldGetMaxRoom(void);
short worldGetMaxObject(void);
short worldGetCurrentRoom(void);
void worldSetCurrentRoom(short room_index);
short roomGetIndexByName(char *room_name);
short objectGetIndexByName(char *object_name);
char *objectGetName(short object_index);
short objectGetRoom(short object_index);
void objectSetRoom(short object_index, short room_index);
BOOL objectGetVisible(short object_index);
BOOL objectGetTakeable(short object_index);
char *objectGetDescription(short object_index);
char *roomGetName(short room_index);
char *roomGetDescription(short room_index);
short roomGetLinkDirection(short room_index, short dir);
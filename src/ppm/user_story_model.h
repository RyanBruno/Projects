#include <stddef.h>
#include <string.h>

// TODO time & person
struct user_story_stage {
    unsigned long id;
    const char* stage;
};

struct user_story_life_cycle {
    unsigned long id;
    unsigned long user_story_id;
    unsigned long user_story_stage_id;
};

struct user_story_field {
    unsigned long id;
    const char* field; // message or desc
};

struct user_story_edit {
    unsigned long id;
    unsigned long user_story_id;
    unsigned long user_story_field_id;
    const char* value;
};

struct user_story { // agg root
    unsigned long id;
};

#define user_story_move_mem(c, o) memcpy(o, c, sizeof(struct user_story))
#define user_story_edit_move_mem(c, o) memcpy(o, c, sizeof(struct user_story_edit))
#define user_story_field_move_mem(c, o) memcpy(o, c, sizeof(struct user_story_field))
#define user_story_life_cycle_move_mem(c, o) memcpy(o, c, sizeof(struct user_story_life_cycle))
#define user_story_stage_move_mem(c, o) memcpy(o, c, sizeof(struct user_story_stage))


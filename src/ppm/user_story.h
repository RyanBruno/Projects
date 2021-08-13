#include <stddef.h>

// TODO time & person
struct user_story_stage {
    unsigned long id;
    const char* stage;
};
struct user_story_life_cycle {
    unsigned long id;
    unsigned long user_story_stage_id;

    struct user_story_stage* user_story_stage;
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

    struct user_story_field* user_story_field;
};
struct user_story { // agg root
    unsigned long id;

    /* TODO arrays */
    size_t user_story_edit_len;
    struct user_story_edit** user_story_edit;
    size_t user_story_life_cycle_len;
    struct user_story_life_cycle** user_story_life_cycle;
};

/* TODO string */
const char*
str_move(const char* str);

void
user_story_repo_get_by_id(unsigned long id,
        struct user_story* user_story_r);

void
user_story_repo_list(struct user_story** user_story_r,
        int* num_r);

void
user_story_update_message(struct user_story* user_story,
        const char* message /* move */);

void
user_story_update_description(struct user_story* user_story,
        const char* description /* move */);

void
user_story_update_stage(struct user_story* user_story,
        const char* stage /* const-ref */);

const char*
user_story_str(struct user_story* user_story /* const-ref */);

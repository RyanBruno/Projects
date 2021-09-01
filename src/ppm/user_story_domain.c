
/* TODO string */
const char*
str_move(const char* str);

void
user_story_update_message(struct user_story* user_story,
        const char* message /* move */);

void
user_story_update_description(struct user_story* user_story,
        const char* description /* move */);

void
user_story_update_stage(struct user_story* user_story,
        const char* stage /* const-ref */);

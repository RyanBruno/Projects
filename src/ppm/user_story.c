#if 0
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

const char*
str_move(const char* str)
{
    const char* other;

    other = str;
    str = NULL;

    return other;
}

void
user_story_repo_get_by_id(unsigned long id,
        struct user_story* user_story_r)
{
    (void) id;
    (void) user_story_r;
}

void
user_story_repo_list(struct user_story** user_story_r,
        int* num_r)
{
    assert(*num_r > 2);
    *num_r = 3;

    user_story_r[0] = &_0;
    user_story_r[1] = &_1;
    user_story_r[2] = &_2;
}

/* TODO functional */
/* pass-by-move user story then return by value */
static void
user_story_update(struct user_story* user_story,
        const char* field /* const-ref */,
        const char* value /* move */)
{
    struct user_story_edit* user_story_edit;
    (void) field;

    user_story_edit = malloc(sizeof(struct user_story_edit));
    assert(user_story_edit != NULL);
    user_story_edit->id = 0;
    user_story_edit->user_story_id = user_story->id;
    user_story_edit->user_story_field_id = 0; // TODO Find the field
    user_story_edit->value = str_move(value);

    /* TODO arrays */
    user_story->user_story_edit[user_story->user_story_edit_len++] = user_story_edit;
}

void
user_story_update_message(struct user_story* user_story,
        const char* message /* move */)
{
    user_story_update(user_story, "message", str_move(message));
}

void
user_story_update_description(struct user_story* user_story,
        const char* description /* move */)
{
    user_story_update(user_story, "description", str_move(description));
}

void
user_story_update_stage(struct user_story* user_story,
        const char* stage /* const-ref */)
{
    struct user_story_life_cycle* user_story_life_cycle;
    (void) stage;

    user_story_life_cycle = malloc(sizeof(struct user_story_life_cycle));
    assert(user_story_life_cycle != NULL);
    user_story_life_cycle->id = 0;
    user_story_life_cycle->user_story_stage = NULL; // Find the stage

    /* TODO arrays */
    user_story->user_story_life_cycle[user_story->user_story_life_cycle_len++] = user_story_life_cycle;
}

const char*
user_story_str(struct user_story* user_story /* const-ref */)
{
    printf("%lu\n", user_story->id);
    return NULL;
}

#endif

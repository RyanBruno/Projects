#include "user_story.h"

void
user_story_repo_get_by_id(unsigned long id,
        struct user_story* user_story_r);

void
user_story_repo_list(struct user_story** user_story_r,
        int* num_r);

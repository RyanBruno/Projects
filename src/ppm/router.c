#include "user_story.h"

void
router_insert(const char* uri)
{
    int num = 10;
    struct user_story* us[10];

    user_story_repo_list(us, &num);
    user_story_str(us[0]);
}

int main()
{
    router_insert("");
    return 0;
}

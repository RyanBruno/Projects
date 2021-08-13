struct user_story_field _field_desc = {
    .id = 0,
    .field = "description",
};
struct user_story_field _field_msg = {
    .id = 1,
    .field = "message",
};
struct user_story_edit _0_msg = {
    .id = 0,
    .user_story_id = 0,
    .user_story_field_id = 0,
    .value = "Test description",
};
struct user_story_edit _0_desc = {
    .id = 1,
    .user_story_id = 0,
    .user_story_field_id = 1,
    .value = "Test message",
};
struct user_story_edit _1_msg = {
    .id = 2,
    .user_story_id = 1,
    .user_story_field_id = 0,
    .value = "Test description1",
};
struct user_story_edit _1_desc = {
    .id = 3,
    .user_story_id = 1,
    .user_story_field_id = 1,
    .value = "Test message1",
};
struct user_story_edit* _0_edit[] = {&_0_msg, &_0_desc};
struct user_story_edit* _1_edit[] = {&_1_msg, &_1_desc};
struct user_story _0 = {
    .id = 0,
    .user_story_edit_len = 2,
    .user_story_edit = _0_edit,
    .user_story_life_cycle_len = 1,
    .user_story_life_cycle = NULL,
};
struct user_story _1 = {
    .id = 1,
    .user_story_edit_len = 2,
    .user_story_edit = _1_edit,
};
struct user_story _2 = {
    .id = 2,
};

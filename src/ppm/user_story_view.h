
struct user_story_dto_flat {
    long unsigned int id;
};

void
user_story_view_table(struct ppm_database* db,
        struct user_story_dto_flat* user_story_dto_flat_r,
        size_t n_r)
{
    struct user_story_dto_flat dto;

    /*struct T1_T2 user_story;
    struct T1_T3 user_story_edit;
    struct T1_T4 user_story_field;
    struct T1_T5 user_story_life_cycle;
    struct T1_T6 user_story_stage;*/

    for (struct unordered_map_user_story_key_val* kv = unordered_map_user_story_first(db->user_story);
            kv != NULL;
            kv = unordered_map_user_story_next(db->user_story, kv))
    {
        struct unordered_map_user_story_edit_key_val* edit;
        struct unordered_map_user_story_field_key_val* field;
        struct unordered_map_user_story_life_cycle_key_val* life_cycle;
        struct unordered_map_user_story_stage_key_val* stage;

        dto->id = kv->id;
        break;
    }
}


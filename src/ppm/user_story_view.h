
struct user_story_dto_flat {
    long unsigned int id;
    const char* message;
    const char* description;
    const char* stage;
};

void
user_story_view_table(struct ppm_database* db,
        struct user_story_dto_flat* user_story_dto_flat_r,
        size_t n_r)
{
    struct user_story_dto_flat dto;

    memset(&dto, '\0', sizeof(struct user_story_dto_flat));

    for (struct unordered_map_user_story_key_val* kv = unordered_map_user_story_first(&db->user_story);
            kv != NULL;
            kv = unordered_map_user_story_next(&db->user_story, kv))
    {
        struct unordered_map_user_story_edit_key_val* edit;
        struct unordered_map_user_story_field_key_val* field;
        struct unordered_map_user_story_life_cycle_key_val* life_cycle;
        struct unordered_map_user_story_stage_key_val* stage;

        dto.id = kv->val.id;

        const char* f = "description";
        field = find_user_story_field_field(
                &db->user_story_field,
                &f);
        printf("hi %s\n", field->val.field);

        /* Fields */
        // filter by field as well as user_story_id
        // TODO get latest (not first)
        edit = find_user_story_edit_user_story_id(
                &db->user_story_edit,
                &dto.id);

        if (edit != NULL)
            field = find_user_story_field_id(
                    &db->user_story_field,
                    &edit->val.user_story_field_id);


        if (edit != NULL && field != NULL) {
            if (field->val.field == "message") dto.message = strdup(edit->val.value);
            if (field->val.field == "description") dto.description = strdup(edit->val.value);
        }

        /* Other Field */
        edit = find_next_user_story_edit_user_story_id(
                &db->user_story_edit,
                &dto.id,
                edit);

        if (edit != NULL)
            field = find_user_story_field_id(
                    &db->user_story_field,
                    &edit->val.user_story_field_id);


        if (edit != NULL && field != NULL) {
            if (field->val.field == "message") dto.message = strdup(edit->val.value);
            if (field->val.field == "description") dto.description = strdup(edit->val.value);
        }

        /* Stage */
        // TODO get latest (not first)
        life_cycle = find_user_story_life_cycle_user_story_id(
                &db->user_story_life_cycle,
                &dto.id);

        if (life_cycle != NULL)
            stage = find_user_story_stage_id(
                    &db->user_story_stage,
                    &life_cycle->val.user_story_stage_id);

        if (life_cycle != NULL && stage != NULL)
            dto.stage = strdup(stage->val.stage);

        printf("{\"id\":%lu,\"message\":\"%s\",\"description\":\"%s\",\"stage\":\"%s\"}\n",
                dto.id,
                dto.message,
                dto.description,
                dto.stage);
        exit(-1);
    }
}


#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)

#define T1_T2 CCAT2(T1_PREFIX, T2)
#define T1_T2_PREFIX CCAT2(T1_PREFIX, T2_PREFIX)
#define T2_CCAT(x) CCAT2(T1_T2_PREFIX, x)

#define T1_T3 CCAT2(T1_PREFIX, T3)
#define T1_T3_PREFIX CCAT2(T1_PREFIX, T3_PREFIX)
#define T3_CCAT(x) CCAT2(T1_T3_PREFIX, x)

#define T1_T4 CCAT2(T1_PREFIX, T4)
#define T1_T4_PREFIX CCAT2(T1_PREFIX, T4_PREFIX)
#define T4_CCAT(x) CCAT2(T1_T4_PREFIX, x)

#define T1_T5 CCAT2(T1_PREFIX, T5)
#define T1_T5_PREFIX CCAT2(T1_PREFIX, T5_PREFIX)
#define T5_CCAT(x) CCAT2(T1_T5_PREFIX, x)

#define T1_T6 CCAT2(T1_PREFIX, T6)
#define T1_T6_PREFIX CCAT2(T1_PREFIX, T6_PREFIX)
#define T6_CCAT(x) CCAT2(T1_T6_PREFIX, x)

struct ppm_database {
    struct T1_T2 user_story;
    struct T1_T3 user_story_edit;
    struct T1_T4 user_story_field;
    struct T1_T5 user_story_life_cycle;
    struct T1_T6 user_story_stage;
};

void ppm_database_construct(struct ppm_database* db)
{
    T2_CCAT(construct)(&db->user_story);
    T3_CCAT(construct)(&db->user_story_edit);
    T4_CCAT(construct)(&db->user_story_field);
    T5_CCAT(construct)(&db->user_story_life_cycle);
    T6_CCAT(construct)(&db->user_story_stage);
}

#undef CCAT
#undef CCAT2

#undef T1_T6
#undef T1_T6_PREFIX
#undef T6_CCAT
#undef T6_PREFIX
#undef T6

#undef T1_T5
#undef T1_T5_PREFIX
#undef T5_CCAT
#undef T5_PREFIX
#undef T5

#undef T1_T4
#undef T1_T4_PREFIX
#undef T4_CCAT
#undef T4_PREFIX
#undef T4

#undef T1_T3
#undef T1_T3_PREFIX
#undef T3_CCAT
#undef T3_PREFIX
#undef T3

#undef T1_T2
#undef T1_T2_PREFIX
#undef T2_CCAT
#undef T2_PREFIX
#undef T2

#undef T1_PREFIX

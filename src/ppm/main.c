#include <sys/socket.h>
#include <arpa/inet.h>
#define ptr_move(x) x

/* buf */
#include "buf.h"

/* ulong */
#include "ulong.h"

/* str */
#include "str.h"

/* user_story */
#include "user_story_model.h"

/* unordered_map */
#define T1 long unsigned int
#define T1_PREFIX ulong_
#define T2 struct user_story
#define T2_TYPE user_story
#define T2_PREFIX user_story_
#include "unordered_map.h"

#define T1 long unsigned int
#define T1_PREFIX ulong_
#define T2 struct user_story_edit
#define T2_TYPE user_story_edit
#define T2_PREFIX user_story_edit_
#include "unordered_map.h"

#define T1 long unsigned int
#define T1_PREFIX ulong_
#define T2 struct user_story_field
#define T2_TYPE user_story_field
#define T2_PREFIX user_story_field_
#include "unordered_map.h"

#define T1 long unsigned int
#define T1_PREFIX ulong_
#define T2 struct user_story_life_cycle
#define T2_TYPE user_story_life_cycle
#define T2_PREFIX user_story_life_cycle_
#include "unordered_map.h"

#define T1 long unsigned int
#define T1_PREFIX ulong_
#define T2 struct user_story_stage
#define T2_TYPE user_story_stage
#define T2_PREFIX user_story_stage_
#include "unordered_map.h"

/* find */
#define T1 struct unordered_map
#define T1_PREFIX unordered_map_
#define T2 user_story_edit
#define T2_TYPE struct user_story_edit
#define T2_PREFIX user_story_edit_
#define T3 user_story_id
#define T4 unsigned long
#define T4_PREFIX ulong_
#include "find.h"

/* find */
#define T1 struct unordered_map
#define T1_PREFIX unordered_map_
#define T2 user_story_field
#define T2_TYPE struct user_story_field
#define T2_PREFIX user_story_field_
#define T3 id
#define T4 unsigned long
#define T4_PREFIX ulong_
#include "find.h"

/* find */
#define T1 struct unordered_map
#define T1_PREFIX unordered_map_
#define T2 user_story_field
#define T2_TYPE struct user_story_field
#define T2_PREFIX user_story_field_
#define T3 field
#define T4 const char* 
#define T4_PREFIX str_
#include "find.h"

/* find */
#define T1 struct unordered_map
#define T1_PREFIX unordered_map_
#define T2 user_story_life_cycle
#define T2_TYPE struct user_story_life_cycle
#define T2_PREFIX user_story_life_cycle_
#define T3 user_story_id
#define T4 unsigned long
#define T4_PREFIX ulong_
#include "find.h"

/* find */
#define T1 struct unordered_map
#define T1_PREFIX unordered_map_
#define T2 user_story_stage
#define T2_TYPE struct user_story_stage
#define T2_PREFIX user_story_stage_
#define T3 id
#define T4 unsigned long
#define T4_PREFIX ulong_
#include "find.h"

/* ppm_database */
#define T1_PREFIX unordered_map_
#define T2 user_story
#define T2_PREFIX user_story_
#define T3 user_story_edit
#define T3_PREFIX user_story_edit_
#define T4 user_story_field
#define T4_PREFIX user_story_field_
#define T5 user_story_life_cycle
#define T5_PREFIX user_story_life_cycle_
#define T6 user_story_stage
#define T6_PREFIX user_story_stage_
#include "ppm_database.h"

/* router */
#define T1 buf
#define T1_PREFIX buf_
#include "router.h"

/* http */
#define T1 buf
#define T1_PREFIX buf_
#define T2 struct router
#define T2_PREFIX router_
#include "http.h"

/* work */
#define T1 struct net
#define T1_PREFIX net_
#include "work.h"

/* epoll_event */
#define T1 struct work
#define T1_PREFIX work_
#include "epoll_event.h"

/* linux_net */
#define T1 struct work
#define T1_PREFIX work_
#include "linux_net.h"

/* net */
#define T1 linux_net
#define T1_PREFIX linux_net_
#define T2 struct epoll_event
#define T2_PREFIX epoll_event_
#define T3 struct work
#define T3_PREFIX work_
#define T4 buf
#define T4_PREFIX buf_
#define T5 http
#define T5_PREFIX http_
#include "net.h"

/* user_story_view */
#include "user_story_view.h"

int main(int argc, char* argv[])
{
    struct ppm_database db;

    ppm_database_construct(&db);

    struct user_story us = {
        .id = 1,
    };

    struct user_story_edit us_edit_msg = {
        .id = 2,
        .user_story_id = 1,
        .user_story_field_id = 4,
        .value = "Hello",
    };
    struct user_story_edit us_edit_desc = {
        .id = 3,
        .user_story_id = 1,
        .user_story_field_id = 5,
        .value = "World",
    };
    struct user_story_field us_field_msg = {
        .id = 4,
        .field = "message",
    };
    struct user_story_field us_field_desc = {
        .id = 5,
        .field = "description",
    };
    struct user_story_life_cycle us_life_cycle = {
        .id = 6,
        .user_story_id = 1,
        .user_story_stage_id = 7,
    };
    struct user_story_stage us_stage = {
        .id = 7,
        .stage = "In Progress",
    };

    unordered_map_user_story_insert(&db.user_story,
            &us.id,
            &us);
    unordered_map_user_story_edit_insert(&db.user_story_edit,
            &us_edit_msg.id,
            &us_edit_msg);
    unordered_map_user_story_edit_insert(&db.user_story_edit,
            &us_edit_desc.id,
            &us_edit_desc);
    unordered_map_user_story_field_insert(&db.user_story_field,
            &us_field_msg.id,
            &us_field_msg);
    unordered_map_user_story_field_insert(&db.user_story_field,
            &us_field_desc.id,
            &us_field_desc);
    unordered_map_user_story_life_cycle_insert(&db.user_story_life_cycle,
            &us_life_cycle.id,
            &us_life_cycle);
    unordered_map_user_story_stage_insert(&db.user_story_stage,
            &us_stage.id,
            &us_stage);

    user_story_view_table(&db, NULL, 0);
    /*assert(argc > 1);
    net_listen_inet((int) strtol(argv[1], NULL, 10));*/
    return 0;
}

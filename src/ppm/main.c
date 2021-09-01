#include <sys/socket.h>
#include <arpa/inet.h>
#define ptr_move(x) x

/* buf */
#include "buf.h"

/* ulong */
#include "ulong.h"

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

int main(int argc, char* argv[])
{
    assert(argc > 1);
    net_listen_inet((int) strtol(argv[1], NULL, 10));
}

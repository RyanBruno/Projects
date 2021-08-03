import Vue from "vue/dist/vue.esm.js";
import Router from "vue-router";

import TaskListTableRoute from "./tasklist/routes/TableRoute.vue";

Vue.use(Router);

Vue.config.productionTip = false;

/* eslint-disable-next-line no-new */
new Vue({
    el: "#app",
    data: {

        /* page: [
        ], */
    },
    router: new Router({
        routes: [
            {
                path: "/",
                component: TaskListTableRoute,
                props: route => ({ URI: route.fullPath }),
            },
        ],
    }),
});

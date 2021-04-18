import Vue from "vue/dist/vue.esm.js";
import Router from "vue-router";

import PageRoute from "./page/routes/PageRoute.vue";
import HomeRoute from "./page/routes/HomeRoute.vue";

Vue.use(Router);

export default new Router({
    routes: [
        {
            path: "*/page/:page",
            component: PageRoute,
            props: route => ({ URI: route.fullPath }),
        },
        { path: "*", component: HomeRoute },
    ],
});

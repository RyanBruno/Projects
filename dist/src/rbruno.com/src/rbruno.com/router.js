import Vue from "vue/dist/vue.esm.js";
import Router from "vue-router";

import PagePageRoute from "./page/routes/PageRoute.vue";
import PageHomeRoute from "./page/routes/HomeRoute.vue";

import MediaMediaRoute from "./media/routes/MediaRoute.vue";
import DebtDebtRoute from "./debt/routes/DebtRoute.vue";

import NexusNexusRoute from "./nexus/routes/NexusRoute.vue";

import BillsBillsRoute from "./bills/routes/BillsRoute.vue";

Vue.use(Router);

export default new Router({
    routes: [
        {
            path: "*/page/:page",
            component: PagePageRoute,
            props: route => ({ URI: route.fullPath }),
        },
        { path: "/", component: PageHomeRoute },
        {
            path: "/media/:media",
            component: MediaMediaRoute,
            props: route => ({ URI: route.fullPath }),
        },
        { path: "/debt/", component: DebtDebtRoute },
        { path: "/nexus/", component: NexusNexusRoute },
        { path: "/bills/", component: BillsBillsRoute },
    ],
});

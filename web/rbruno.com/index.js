import Vue from "vue/dist/vue.esm.js";
import Router from "vue-router";

import vueJsStaticPost from "../data/rbruno.com/page/page/vuejs-static.js";
import aboutPost from "../data/rbruno.com/page//page/about.js";
import smokingMeatPost from "../data/rbruno.com/page/page/smoking-meat.js";
import helloWorldPost from "../data/rbruno.com/page/page/hello-world.js";

import debt from "../data/rbruno.com/debt/debt.json";

import nexus from "../data/rbruno.com/nexus/nexus.json";

import helloWorldMedia from "../data/rbruno.com/media/media/hello-world.js";
import twoMedia from "../data/rbruno.com/media/media/two.js";
import threeMedia from "../data/rbruno.com/media/media/three.js";

import bills from "../data/rbruno.com/bills/data.js";

import PagePageRoute from "./page/routes/PageRoute.vue";
import PageHomeRoute from "./page/routes/HomeRoute.vue";

import MediaMediaRoute from "./media/routes/MediaRoute.vue";
import DebtDebtRoute from "./debt/routes/DebtRoute.vue";

import NexusNexusRoute from "./nexus/routes/NexusRoute.vue";

import BillsBillsRoute from "./bills/routes/BillsRoute.vue";

import GnuCashAppRoute from "./gnucash/routes/AppRoute.vue";
import GnuCashDashboardRoute from "./gnucash/routes/DashboardRoute.vue";

Vue.use(Router);

Vue.config.productionTip = false;

/* eslint-disable-next-line no-new */
new Vue({
    el: "#app",
    data: {
        debt,
        nexus,
        media: [
            helloWorldMedia,
            twoMedia,
            threeMedia,
        ],
        page: [
            vueJsStaticPost,
            aboutPost,
            smokingMeatPost,
            helloWorldPost,
        ],
        bills,
    },
    router: new Router({
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
            { path: "/gnucash/", component: GnuCashAppRoute },
            { path: "/gnucash-dashboard/", component: GnuCashDashboardRoute },
        ],
    }),
});

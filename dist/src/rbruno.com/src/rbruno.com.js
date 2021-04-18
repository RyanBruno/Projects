import Vue from "vue/dist/vue.esm.js";
import Router from "./rbruno.com/router.js";

import data from "../data/rbruno.com.js";

import TheHeader from "./rbruno.com/page/components/TheHeader.vue";

Vue.config.productionTip = false;

/* eslint-disable-next-line no-new */
new Vue({
    el: "#app",
    components: {
        theHeader: TheHeader,
    },
    router: Router,
    data,
});

import vueJsStaticPost from "./rbruno.com/page/page/vuejs-static.js";
import aboutPost from "./rbruno.com/page//page/about.js";
import smokingMeatPost from "./rbruno.com/page/page/smoking-meat.js";
import helloWorldPost from "./rbruno.com/page/page/hello-world.js";

import debt from "./rbruno.com/debt/debt.json";

import nexus from "./rbruno.com/nexus/nexus.json";

import helloWorldMedia from "./rbruno.com/media/media/hello-world.js";
import twoMedia from "./rbruno.com/media/media/two.js";
import threeMedia from "./rbruno.com/media/media/three.js";

import bills from "./rbruno.com/bills/data.js";

export default {
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
};

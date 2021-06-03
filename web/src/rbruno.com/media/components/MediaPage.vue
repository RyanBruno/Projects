<template>
    <div>
        <media-header>
        </media-header>
        <div id="container">
            <video id="the-video" :poster="media.thumbnail" controls>
                <source :src="media.media" type="video/mp4">
                Your browser does not support the video tag.
            </video> 
            <h1>{{ media.title }}</h1>
            <p>{{ media.description }}</p>

            <h4>Related</h4>
            <div id="box">
                <div v-for="r in related.slice(0, 3)"
                    :key="r.URI"
                    class="related">
                    <router-link :to="r.URI" >
                        <img :src="r.thumbnail" class="thumbnail"/>
                        {{ r.title }}
                    </router-link>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import Models from '../domain/Models.js';
import MediaHeader from "./MediaHeader.vue";

export default {
    props: [ "media", "related" ],
    computed: {
        //media: function() { return Models.media.call(this); },
    },
    updated: function () {
        this.$nextTick(function () {
            document.getElementById("the-video").src = this.media.media;
        });
    },
    components: {
        mediaHeader: MediaHeader,
    },
}
</script>

<style scoped>
#container {
    width: 98%;
    max-width: 700px;
    margin: 50px auto;
}
#box {
    display: flex;
}
.related a {
    color: black;
}
.related {
    width: calc(100% / 3);
    margin: 5px;
}
img.thumbnail {
    width: 100%;
}
video {
    width: 100%;
    height: 394px;
    max-height: calc(100vw / 1.8);
    display: block;
    background-color: black;
}
a {
    text-decoration: none;
}
</style>

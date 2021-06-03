<template>
    <media-page :media="media" :related="related" />
</template>

<script>
import Models from '../domain/Models.js';
import MediaPage from "../components/MediaPage.vue";

export default {
    props: [ "URI" ],
    computed: {
        media: function() { return Models.media.call(this); },
        related: function() { return Models.related.call(this, this.media); },
    },
    components: {
        mediaPage: MediaPage,
    },
}
</script>

<style>
</style>

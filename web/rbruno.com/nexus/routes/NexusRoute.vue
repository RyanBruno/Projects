<template>
    <nexus-page :links="links"/>
</template>

<script>
import Models from '../domain/Models.js';
import NexusPage from "../components/NexusPage.vue";

export default {
    props: [ "URI" ],
    computed: {
        links: function() { return Models.links.call(this); },
    },
    components: {
        nexusPage: NexusPage,
    },
}
</script>

<style>
</style>

<template>
    <page-content :page="page"/>
</template>

<script>
import Models from '../domain/Models.js';
import PageContent from "../components/PageContent.vue";

export default {
    props: [ "URI" ],
    computed: {
        page: function() { return Models.page.call(this); },
    },
    components: {
        pageContent: PageContent,
    },
}
</script>

<style>
</style>

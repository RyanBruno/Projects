<template>
    <div style="margin: 10px 0;">
        <the-header></the-header>
        <page-content :page="page"/>
    </div>
</template>

<script>
import Models from '../domain/Models.js';
import TheHeader from "../components/TheHeader.vue";
import PageContent from "../components/PageContent.vue";

export default {
    props: [ "URI" ],
    computed: {
        page: function() { return Models.page.call(this); },
    },
    components: {
        theHeader: TheHeader,
        pageContent: PageContent,
    },
}
</script>

<style>
</style>

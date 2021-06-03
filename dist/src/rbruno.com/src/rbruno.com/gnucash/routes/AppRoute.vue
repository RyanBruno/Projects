<template>
    <app-page/>
</template>

<script>
//import Models from '../domain/Models.js';
import AppPage from "../components/AppPage.vue";

export default {
    props: [ "URI" ],
    /*computed: {
        data: function() { return Models.data.call(this); },
    },*/
    components: {
        appPage: AppPage,
    },
}
</script>

<style>
</style>

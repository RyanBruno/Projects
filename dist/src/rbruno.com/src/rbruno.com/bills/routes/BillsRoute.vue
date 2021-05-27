<template>
    <bills-page :data="data"/>
</template>

<script>
import Models from '../domain/Models.js';
import BillsPage from "../components/BillsPage.vue";

export default {
    props: [ "URI" ],
    computed: {
        data: function() { return Models.data.call(this); },
    },
    components: {
        billsPage: BillsPage,
    },
}
</script>

<style>
</style>

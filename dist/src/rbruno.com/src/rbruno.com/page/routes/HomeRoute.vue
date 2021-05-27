<template>
    <div style="margin: 10px 0;">
        <the-header></the-header>
        <multi-slate-table :slates="slates"/>
    </div>
</template>

<script>
import Models from '../domain/Models.js';
import TheHeader from "../components/TheHeader.vue";
import MultiSlateTable from '../components/MultiSlateTable.vue';

export default {
    computed: {
        slates: function() { return Models.slates.call(this) },
    },
    components: {
        theHeader: TheHeader,
        multiSlateTable: MultiSlateTable,
    },
}
</script>

<style>
</style>

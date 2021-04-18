<template>
    <multi-slate-table :slates="slates"/>
</template>

<script>
import Models from '../domain/Models.js';
import MultiSlateTable from '../components/MultiSlateTable.vue';

export default {
    computed: {
        slates: function() { return Models.slates.call(this) },
    },
    components: {
        multiSlateTable: MultiSlateTable,
    },
}
</script>

<style>
</style>

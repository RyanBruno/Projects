<template>
    <div>
        <multi-slate-table-item v-for="slate in slates"
                      :key = "slate.URI"
                      v-bind:slate="slate"/>

        <div style="position: fixed; right: 5px; top: 0;
                     width: 2px; height: 100vh;
            background-color: white;"></div>
    </div>
</template>

<script>
import MultiSlateTableItem from './MultiSlateTableItem.vue';

export default {
    props: {
        slates: Array,
    },
    components: {
        multiSlateTableItem: MultiSlateTableItem,
    },
}
</script>

<style>
</style>

<template>
    <table>
        <simple-table-item vfor="item in items"
                           :header="header"
                           :item="item" />
    </table>
</template>

<script>
import SimpleTableItem from './SimpleTableItem.vue';

export default {
    props: {
        header: Array,
        item: String,
    },
    components: {
        simpleTableItem: SimpleTableItem,
    },
}
</script>

<style>
</style>

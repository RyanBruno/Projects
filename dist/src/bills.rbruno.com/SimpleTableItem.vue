<template>
    <tr>
        <simple-table-item-item v-for="head in header"
                                :header="head"
                                :item="item[head]"/>
    </tr>
</template>

<script>
import SimpleTableItemItem from './SimpleTableITemItem.vue';

export default {
    props: {
        header: Array,
        item: Object,
    },
    components: {
        simpleTableItemItem: SimpleTableItemItem,
    },
}
</script>

<style>
</style>

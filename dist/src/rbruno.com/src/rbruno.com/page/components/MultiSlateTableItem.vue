<template>
    <div id="news-page" style="padding-top: 5px;">
        <p style="text-align: center; margin: 5px 0;
            width: 100%;">{{ slate.title }}</p>
        <div style="display: flex;
                 margin: 0px 5px;
                 flex-wrap: wrap;
                 flex-direction: row;">
            <div style="background-color: lightgrey; width: 100%;
                height: 2px;">
            </div>

            <multi-slate-table-item-item v-for="page in slate.pages"
                        :key = "page.URI"
                        v-bind:page="page"/>
        </div>
    </div>
</template>

<script>
import MultiSlateTableItemItem from './MultiSlateTableItemItem.vue'
export default {
    props: {
        slate: Object,
    },
    components: {
        multiSlateTableItemItem: MultiSlateTableItemItem,
    },
}
</script>

<style>
</style>

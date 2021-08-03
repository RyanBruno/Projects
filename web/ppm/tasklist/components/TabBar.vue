<template>
    <div id="bar">
        <div v-for="tab in tabs"
             v-on:click="click(tab)">
             <div id="selected" class="tab" v-if="tab === selected">
                {{tab}}
             </div>
             <div class="tab" v-if="!(tab === selected)">
                {{tab}}
             </div>
        </div>
    </div>
</template>

<script>
export default {
    props: {
        tabs: Array,
    },
    data: function() {
        return {
            selected: "All",
        };
    },
    watch: {
    },
    methods: {
        click: function(tab) {
            this.selected = tab;
            this.$emit("click", tab);
        },
    },
}
</script>

<style scoped>
div#bar {
    display: flex;
    flex-direction: row-reverse;
}
div#selected {
    border: var(--primary) 1px solid;
    border-bottom: none;
}
div.tab:hover {
    border: var(--light) 1px solid;
    border-bottom: none;
}
div.tab {
    border: white 1px solid;
    border-bottom: none;
    padding: 4px 10px 2px 10px;
}
</style>

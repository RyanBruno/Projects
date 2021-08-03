<template>
    <input type="button" :value="value" v-on:click="clicked">
</template>

<script>
export default {
    props: {
        value: String,
    },
    methods: {
        clicked: function() {
            this.$emit("click");
        },
    },
}
</script>

<style scoped>
</style>

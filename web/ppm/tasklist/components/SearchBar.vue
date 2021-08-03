<template>
    <input type="text" v-model="searchTerm" placeholder="Search">
</template>

<script>
export default {
    props: {
        initialTerm: String,
    },
    data: function() {
        return {
            searchTerm: this.initialTerm,
        };
    },
    watch: {
        searchTerm: function(newTerm, oldTerm) {
            this.$emit("searchTerm", newTerm);
        },
    },
}
</script>

<style scoped>
</style>

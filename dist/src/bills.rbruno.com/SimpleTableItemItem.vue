<template>
    <td :v-html="item">
    </td>
</template>

<script>
export default {
    props: {
        item: String,
        header: Array,
    },
}
</script>

<style>
</style>

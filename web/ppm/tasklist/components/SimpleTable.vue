<template>
    <table>
        <tr>
            <th v-for="header in headers">
             {{ header }}
            </th>
        </tr>

        <tr v-for="item in items">
            <td v-for="col in cols">
                 <div v-html="item[col]" v-on:click="click(item)">
                 </div>
            </td>
        </tr>
    </table>
</template>

<script>
export default {
    props: {
        headers: Array,
        cols: Array,
        items: Array,
        links: Array,
    },
    methods: {
        click: function(item) {
            this.$emit('click', item);
        },
    },
}
</script>

<style scoped>
table {
    font-family: arial, sans-serif;
    border-collapse: collapse;
    width: 100%;
}
td, th {
    border: 1px solid var(--primary);
    text-align: left;
    padding: 8px;
}
tr:nth-child(even) {
    background-color: var(--light);
}
</style>

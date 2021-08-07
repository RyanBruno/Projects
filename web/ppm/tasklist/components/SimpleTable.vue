<template>
    <table>
        <tr>
            <th v-for="header in headers">
             {{ header }}
            </th>
        </tr>

        <tr v-for="item in items">
            <td v-for="col in cols">
                <input type="text" :value="item[col]"
                        v-on:keydown.enter="edit"
                        v-if="editable.item == item && editable.col == col">
                <div v-on:click="click(item, col)"
                        v-if="!(editable.item == item && editable.col == col)">
                        <p v-html="item[col]"></p>
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
        editable: Object,
    },
    methods: {
        edit: function(val) {
            this.$emit('edit', val.target.value);
        },
        click: function(item, col) {
            this.$emit('click', item, col);
        },
    },
}
</script>

<style scoped>
table {
    font-family: arial, sans-serif;
    border-collapse: collapse;
    width: 100%;
    border-top: 1px solid var(--primary);
}
td, th {
    text-align: left;
    padding: 8px;
}
tr:nth-child(even) {
    background-color: var(--light);
}
td p, th p {
    margin: 0;
}
td:nth-child(1), th:nth-child(1) {
    width: 250px;
    max-width: 20vw;
}
td:nth-child(1) p {
    overflow: hidden;
    display: -webkit-box;
    -webkit-box-orient: vertical;
    -webkit-line-clamp: 2;
}
td:nth-child(2) p {
    overflow: hidden;
    display: -webkit-box;
    -webkit-box-orient: vertical;
    -webkit-line-clamp: 1;
}
td:nth-child(3), th:nth-child(3) {
    width: 100px;
    max-width: 10vw;
}
input {
    width: 100px;
    max-width: 10vw;
}
</style>

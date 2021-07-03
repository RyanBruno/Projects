<template>
    <table>
        <tr>
            <th v-for="header in headers">
             {{ header }}
            </th>
        </tr>

        <tr v-for="item in items">
            <td v-for="header in headers">
             <div v-html="item[header]">
             </div>
            </td>
        </tr>
    </table>
</template>

<script>
export default {
    props: {
        headers: Array,
        items: Array,
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
    border: 1px solid #dddddd;
    text-align: left;
    padding: 8px;
}
tr:nth-child(even) {
    background-color: #dddddd;
}
</style>

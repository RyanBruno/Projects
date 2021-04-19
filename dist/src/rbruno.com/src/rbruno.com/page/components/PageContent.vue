<template>
    <div style="margin: 0 auto;
                width: 98vw;
                max-width: 500px;">
        <h1 style="margin: 10px 2px;
                   font-size: 22px;">{{ page.title }}</h1>
        <p style="margin: 2px 2px;
                  font-size: 14px;"
                  v-html="page.content/*.replace(/$^$/gm,'<br/><br/>')*/">
        </p>
    </div>
</template>

<script>
export default {
    props: {
        page: Object,
    },
}
</script>

<style>
pre {
    padding: 0 25px;
    background-color: #ebf1f5;
    border: 1px lightblue solid;
}
code {
    background-color: #ebf1f5;
}
table {
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

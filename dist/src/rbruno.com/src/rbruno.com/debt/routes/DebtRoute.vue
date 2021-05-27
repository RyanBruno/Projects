<template>
    <debt-page :debt="debt"/>
</template>

<script>
import Models from '../domain/Models.js';
import DebtPage from "../components/DebtPage.vue";

export default {
    props: [ "URI" ],
    computed: {
        debt: function() { return Models.debt.call(this); },
    },
    components: {
        debtPage: DebtPage,
    },
}
</script>

<style>
</style>

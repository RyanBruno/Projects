<template>
    <dashboard-page/>
</template>

<script>
import DashboardPage from "../components/DashboardPage.vue";

export default {
    props: [ "URI" ],
    /*computed: {
        data: function() { return Models.data.call(this); },
    },*/
    components: {
        dashboardPage: DashboardPage,
    },
}
</script>

<style>
</style>

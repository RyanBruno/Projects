<template>
    <task-list-page />
</template>

<script>
import TaskListPage from '../pages/TaskListPage.vue';
import "@fontsource/noto-sans";

export default {
    props: [ "URI" ],
    components: {
        taskListPage: TaskListPage,
    },
}
</script>

<style>
html {
    font-family: "Noto Sans";
}
:root {
    --light:  #D4F1F4;
    --secondary:  #75E6DA;
    --primary: #189AB4;
    --primary-selected: #05445E;
}
</style>

<template>
    <div>
        <div id="link" v-for="link in links">
            <router-link :to="link.to">
                <h1>{{ link.title }}</h1>
            </router-link>
        </div>
    </div>
</template>

<script>

export default {
    props: {
        links: Array,
    },
}
</script>

<style scoped>
a {
    color: white;
    text-decoration: none;
}
#link {
    width: 600px;
    max-width: 90%;
    margin: 0 auto;
    text-align: center;
    background-color: #06aa06;
    border-radius: 10px;
}
</style>

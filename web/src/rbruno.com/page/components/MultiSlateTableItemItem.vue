<template>
    <div class="news-page-item" style="
        margin-top: -2px;
        padding: 5px 8px;
        box-sizing: border-box;
        color: black;
        width: calc(100% / 3);
        flex: 0 0 auto;
        border-style: solid;
        border-color: lightgrey;
        border-width: 2px 2px 0 0;"
    >
        <router-link :to="page.URI || '#'">
            <div>
                <h1 style="margin: 2px 2px;
                           font-size: 16px;">{{ page.title }}</h1>
                <p style="margin: 2px 2px;
                          font-size: 10px;"
                          v-html="page.content
                                              .replace(/<[^>]*>/g,'')
                                              .substr(0, 400)
                                              .concat('  Click to read more.')">
                </p>
            </div>
        </router-link>
    </div>
</template>

<script>
export default {
    props: {
        page: Object,
    },
}
</script>

<style scoped>
a {
    text-decoration: inherit;
    color: inherit;
}
</style>

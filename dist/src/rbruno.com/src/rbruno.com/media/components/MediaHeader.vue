<template>
    <div id="header">
        <div id="logo">
            <a href="/">
                <h3>Ryan Bruno</h3>
            </a>
        </div>

        <div id="hamburger">
            <i class="fa fa-bars"></i>
        </div>
    </div>
</template>

<script>
export default {
}
</script>

<style scoped>
#header {
    background-color: #959595;
    display: flex;
    padding: 10px 5px;
}
#logo {
    flex-grow: 1;
}
#logo a {
    text-decoration: none;
}
#logo h3 {
    color: white;
    margin: 0 0 0 10px;
}
#hamburger {
    color: white;
    width: 60px;
    font-size: 28px;
    margin: 0 auto;
}
</style>

<template>
    <div id="container">
        <h1>Welcome User!</h1>
        <router-link to="/gnucash/" id="link">
            <button>Enter App</button>
        </router-link>
    </div>
</template>

<script>
export default {
}
</script>

<style scoped>
h1 {
    text-align: center;
}
#container {
    width: 100%;
}
#link {
    display: block;
    text-decoration: none;
}
button {
    background-color: #4CAF50;
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: block;
    font-size: 16px;
    margin: 0 auto;
    cursor: pointer;
}
</style>

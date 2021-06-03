<template>
    <div id="container">
    </div>
</template>

<script>
import RFB from "@novnc/novnc";

export default {
    mounted: function () {
        this.$nextTick(function () {
            let rfb = new RFB(
                        document.getElementById("container"),
                        "ws://10.0.0.218:6080/websockify",
                        {
                            credentials: {
                                password: "password"
                            }
                        });
            rfb.resizeSession = true;
        })
    }
}
</script>

<style scoped>
body {
    width: 100%;
    height: 100vh;
}
.container {
    width: 100%;
    height: 100%;
}
</style>

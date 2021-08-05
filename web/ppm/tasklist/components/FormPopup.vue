<template>
    <div>
        <div id="background" v-on:click="close"></div>
        <div id="box">
            <p id="title" v-if="data.uuid">Edit Task</p>
            <p id="title" v-if="!data.uuid">Add Task</p>
            <click-to-edit-field name="title"
                   placeholder="title"
                   label="Title"
                   v-model="data.title" />
            <click-to-edit-field name="description"
                   placeholder="description"
                   label="Description"
                   v-model="data.description" />
            <click-to-edit-field name="stage"
                   placeholder="stage"
                   label="Stage"
                   v-model="data.stage" />
            <input type="button" value="Submit" v-on:click="clicked">
        </div>
    </div>
</template>

<script>
import ClickToEditField from "../components/ClickToEditField.vue";

export default {
    props: {
        initialData: Object,
    },
    components: {
        clickToEditField: ClickToEditField,
    },
    data: function() {
        return {
            data: Object.assign({}, this.initialData),
        };
    },
    methods: {
        close: function() {
            this.$emit("close");
        },
        clicked: function() {
            console.log(this.data);
            this.$emit("submit", this.data);
        },
    },
}
</script>

<style scoped>
div#background {
    position: fixed;
    top: 0;
    left: 0;
    height: 100vh;
    width: 100vw;
    background-color: rgba(100,100,100,0.5);
}
div#box {
    position: fixed;
    top: 10%;
    left: 10vw;
    background-color: white;
    width: 80vw;
    padding: 20px 20px 20px 20px;
}
input[type="text"] {
    width: 100%;
}
p#title {
    margin: 3px auto;
    text-align: center;
    font-size: 1.5em;
}
</style>

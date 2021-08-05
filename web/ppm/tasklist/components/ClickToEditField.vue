<template>
    <div>
        <label :for="name">{{label}}</label>
        <p v-on:click="click" v-if="!edit">{{value}}</p>
        <input type="text"
               v-model="privValue"
               :placeholder="placeholder"
               :name="name"
               v-if="edit">
    </div>
</template>

<script>
export default {
    props: {
        value: String,
        placeholder: String,
        name: String,
        label: String,
    },
    data: function() {
        return {
            privValue: this.value,
            edit: this.value == "" || this.value == undefined,
        };
    },
    watch: {
        privValue: function(value) {
            this.$emit("input", value);
        },
    },
    methods: {
        click: function() {
            this.$emit("click");
            this.edit = true;
        },
    },
}
</script>

<style scoped>
div {
    margin: 15px 0;
}
label {
    font-size: 0.75em;
    font-weight: bold;
}
input[type="text"] {
    width: 100%;
}
p:hover {
    border: #8f8f9d solid 1px;
}
p {
    border: white solid 1px;
    margin: 0;
}
</style>

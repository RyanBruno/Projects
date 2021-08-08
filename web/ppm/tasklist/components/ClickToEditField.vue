<template>
    <div id="field">
        <label :for="name">{{label}}</label>
        <p v-on:click="click" v-if="!edit">{{value}}</p>
        <simple-input
               :value="value"
               v-on:input="input"
               :placeholder="placeholder"
               :name="name"
               :max="100"
               v-if="edit && !textArea" />
        <textarea type="text"
               :value="value"
               v-on:input="input"
               :placeholder="placeholder"
               :name="name"
               v-if="edit && textArea" />
    </div>
</template>

<script>
import SimpleInput from "./SimpleInput.vue"

export default {
    props: {
        value: String,
        placeholder: String,
        name: String,
        label: String,
        textArea: Boolean,
    },
    data: function() {
        return {
            edit: this.value == "" || this.value == undefined,
        };
    },
    methods: {
        input: function(value) {
            this.$emit("input", value);
        },
        click: function() {
            this.$emit("click");
            this.edit = true;
        },
    },
    components: {
        simpleInput: SimpleInput,
    },
}
</script>

<style scoped>
div#field {
    margin: 15px 0;
}
label {
    font-size: 0.75em;
    font-weight: bold;
}
input[type="text"] {
    width: 100%;
}
textarea {
    width: 100%;
    height: 150px;
}
p:hover {
    border: #8f8f9d solid 1px;
}
p {
    border: white solid 1px;
    margin: 0;
}
</style>

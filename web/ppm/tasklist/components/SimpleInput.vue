<template>
    <div id="input">
        <input
            type="text"
            v-model="privValue"
            :placeholder="placeholder"
            :name="name"
        />
        <div id="counter">
            {{n}}/{{max}}
        </div>
    </div>
</template>

<script>
export default {
    props: {
        value: String,
        placeholder: String,
        name: String,
        max: Number,
    },
    data: function() {
        return {
            privValue: this.value || "",
            n: (this.value || "").length,
        };
    },
    watch: {
        privValue: function(value) {
            // Validate
            this.n = value.length;
            this.$emit("input", value);
        },
    },
}
</script>

<style scoped>
div#input {
    width: 100%;
    display: flex;
    border: solid grey 1px;
}
input[type="text"] {
    width: 100%;
    border: none; 
    padding: 0;
    height: 1.5em;
}
#counter {
    background-color: #21ff21;
    margin: 0;
    font-size: 0.9em;
    padding: 0 4px;
    height: 1.5em;
}
</style>

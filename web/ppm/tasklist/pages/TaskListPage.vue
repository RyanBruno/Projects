<template>
    <div id="page">
        <search-bar v-on:searchTerm="searchTerm"></search-bar>
        <simple-button value="Add Task" v-on:click="showAddTask"></simple-button>
        <tab-bar :tabs="tabs"
                 v-on:click="tabSelected" />
        <simple-table :headers="headers"
                      v-on:click="editTask"
                      v-on:edit="editStage"
                      :editable="editable"
                      :cols="cols"
                      :items="visableItems" />
        <form-popup :initialData="formData"
                        v-if="isPopup"
                        v-on:close="hidePopup"
                        v-on:submit="addTask"/>
    </div>
</template>

<script>
import SimpleTable from "../components/SimpleTable.vue";
import SearchBar from "../components/SearchBar.vue";
import SimpleButton from "../components/SimpleButton.vue";
import TabBar from "../components/TabBar.vue";
import TestData from "../testdata.js";
import FormPopup from "../components/FormPopup.vue";

import { v4 as uuidv4 } from "uuid";

export default {

    // props: [ "URI" ],
    data: function() {
        return {
            tabs: [ "Done", "Scoped", "Backlog", "Idea", "All", ],
            headers: [ "Title", "Description", "Stage", ],
            cols: [ "title", "description", "stage", ],
            items: TestData.tasks,
            visableItems: TestData.tasks,
            isPopup: false,
            formData: {},
            editable: {},
        };
    },
    components: {
        searchBar: SearchBar,
        simpleButton: SimpleButton,
        tabBar: TabBar,
        simpleTable: SimpleTable,
        formPopup: FormPopup,
    },
    watch: {
        items: function() {
            this.visableItems = this.items;
        },
    },
    methods: {
        tabSelected: function(tab) {
            let temp = [];

            if (tab === "All") {
                this.visableItems = this.items;
                return;
            }

            for (let i = 0; i < this.items.length; i++) {
                if (this.items[i].stage === tab)
                    temp.push(this.items[i]);
            }

            this.visableItems = temp;
        },
        editStage: function(val) {
            this.editable.item.stage = val;
            this.editable = {};
        },
        editTask: function(item, col) {
            if (col == "stage") {
                this.editable = {
                    item: item,
                    col: col,
                };
                return;
            }
            this.formData = item;
            this.isPopup = true;
        },
        showAddTask: function() {
            this.formData = {};
            this.isPopup = true;
        },
        hidePopup: function() {
            this.isPopup = false;
        },
        addTask: function(task) {
            if (!task.uuid)
                task.uuid = uuidv4();

            this.items = this.items.filter(x => x.uuid != task.uuid);
            this.items.push(task);
            this.isPopup = false;
        },
        searchTerm: function(searchTerm) {
            let temp = [];

            for (let i = 0; i < this.items.length; i++) {
                if (this.items[i].title.includes(searchTerm) ||
                    this.items[i].description.includes(searchTerm))
                    temp.push(this.items[i]);
            }

            this.visableItems = temp;
        },
    },
}
</script>

<style>
div#page {
    margin: 15px calc(15px + calc(100% - 100vw)) 0 15px;
}
</style>

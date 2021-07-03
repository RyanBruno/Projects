<template>
    <div id="container">
        <div id="container-bar">
            <bar-chart :data="plotData"
                       :options="{responsive: true, maintainAspectRatio: false}"/>
        </div>

        <simple-table :items="links"
                      :headers="data.headers"/>
    </div>
</template>

<script>
import SimpleTable from './SimpleTable.vue';
import BarChart from './BarChart.vue';

export default {
    props: {
        data: Object,
    },
    components: {
        simpleTable: SimpleTable,
        barChart: BarChart,
    },
    computed: {
        records: function() {
            return this.data.records.map((x) => Object.assign({}, x));
        },
        monthAggregation: function() {
            let x = this.records;
            let y = []

            for (let i in x) {
                let d = new Date(x[i].date);
                let z = y.findIndex((f) => f.x == d.getUTCMonth());

                if (z < 0) {
                    y.push({ x: d.getUTCMonth(), y: 0 });
                    z = y.length - 1;
                }

                y[z].y += x[i].data.Amount;
            }

            return y;
        },
        plotData: function() {
            let x = this.monthAggregation;

            return { datasets: [{ data: x.map((m) => m.y),
                                  label: 'Bills',
                                  fill: false,
                                backgroundColor: [
                                'rgba(255, 99, 132, 0.2)',
                                'rgba(255, 159, 64, 0.2)',
                                'rgba(255, 205, 86, 0.2)',
                                'rgba(75, 192, 192, 0.2)',
                                'rgba(54, 162, 235, 0.2)',
                                'rgba(153, 102, 255, 0.2)',
                                'rgba(201, 203, 207, 0.2)' ],
                                borderColor: [
                                'rgb(255, 99, 132)',
                                'rgb(255, 159, 64)',
                                'rgb(255, 205, 86)',
                                'rgb(75, 192, 192)',
                                'rgb(54, 162, 235)',
                                'rgb(153, 102, 255)',
                                'rgb(201, 203, 207)' ],
                                borderWidth: 1}],
                                labels: [ "Jan", "Feb", "Mar", "Apr" ]};
        },
        items: function() {
            return this.data.records.map((x) => Object.assign({}, x.data));
        },
        cents: function() {
            let x = this.items;

            for (let i in x) {
                let d = new Date(x[i].Due);

                x[i].Due = d.toUTCString()
                            .split(" ")
                            .filter((x, i) => i > 0 && i < 4);

                let tmp = x[i].Due[1];
                x[i].Due[1] = x[i].Due[0];
                x[i].Due[0] = tmp;
                x[i].Due = x[i].Due.join(" ");

                x[i].Amount = "$" + (Math.round(x[i].Amount) / 100).toFixed(2);
                x[i].Paid = (x[i].Paid ? "Yes" : "No");
            }

            return x;
        },
        links: function() {
            let x = this.cents;

            for (let i in x) {
                x[i].Bill = "<a href=\"" + x[i].Bill + "\">" + x[i].Bill + "</a>";
            }

            return x;
        },
    },
}
</script>

<style scoped>
#container {
    width: 98%;
    margin: 5px auto;
    max-width: 900px;
}
#container-bar {
    width: 100%;
    height: 400px;
}
</style>

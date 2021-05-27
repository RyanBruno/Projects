<template>
    <div>
        <div id="current-debt">
            <h4>Current Debt</h4>
            <h1 style="color: red;">{{ formatDebt }}</h1>
        </div>

        <div id="debt-stats">
            <div>
                <h4>Current Assets</h4>
                <h1 style="color: green;">{{ formatAssets }}</h1>
            </div>
            <div>
                <h4>Interest Per Month</h4>
                <h1 style="color: grey;">$0.00</h1>
            </div>
            <div>
                <h4>Payoff Date</h4>
                <h1 style="color: green;">{{ debt.payoffDate }}</h1>
            </div>
        </div>
    </div>
</template>

<script>

export default {
    props: {
        debt: Object,
    },
    computed: {
        formatAssets: function() {
            var formatter = new Intl.NumberFormat('en-US', {
                style: 'currency',
                currency: 'USD',
            });

            return formatter.format(this.debt.assets);
        },
        formatDebt: function() {
            var formatter = new Intl.NumberFormat('en-US', {
                style: 'currency',
                currency: 'USD',
            });

            return formatter.format(this.debt.debt);
        },
    },
    /*components: {
        mediaHeader: MediaHeader,
    },*/
}
</script>

<style>
#current-debt, #current-debt * {
    width: 100%;
    text-align: center;
}

#debt-stats {
    width: 96%;
    max-width: 700px;
    display: flex;
    margin: 0 auto;
    text-align: center;
}

#debt-stats * {
    width: calc(100% / 3);
}

#debt-stats * * {
    width: 100%;
}
</style>

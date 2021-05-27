export default {
    media() {
        const m = this.$root.media
            .find(x => x.URI === this.URI);

        if (!m) {
            return { description: "Not Found", content: "" };
        }

        return m;
    },
    related(media) {
        const index = {};
        const scores = [];

        this.$root.media.forEach(x => {
            if (x.URI === this.URI) {
                return;
            }

            x.tags.forEach(y => {
                if (index[y]) {
                    index[y].push(x);
                } else {
                    index[y] = [ x ];
                }
            });
        });


        this.$root.media.forEach(x => {
            x.indexScore = 0;
        });

        media.tags.forEach(x => {
            if (!index[x]) {
                return;
            }

            index[x].forEach(y => {
                if (!y.indexScore) {
                    y.indexScore = 0;
                }

                y.indexScore += 1;
            });
        });

        this.$root.media.forEach(x => {
            if (x.indexScore) {
                scores.push(x);
            }
        });

        scores.sort((a, b) => b.indexScore - a.indexScore);

        return scores;
    },
};

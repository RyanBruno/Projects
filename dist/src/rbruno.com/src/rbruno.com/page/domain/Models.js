export default {
    slates() {
        const p = [];

        this.$root.page.forEach(x =>
            x.slate.forEach(y => {
                const c = p.find(i => i.title === y);

                if (!c) {
                    p.push({ title: y, pages: [ x ] });
                    return;
                }

                c.pages.push(x);
            }));

        return p;
    },
    page() {
        const p = this.$root.page
            .find(x => x.URI === this.URI);

        if (!p) {
            return { title: "Not Found", content: "" };
        }

        return p;
    },
};

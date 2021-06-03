/* Date-based Record
 *  - UUID (uuidgen -t)
 *
 *  - Date (date -Idate) <- x
 *
 *  - value (json)           <- y
 *   - Amount (cents) = long <-
 *   - Company = string
 *   - Due Date =
 *   - PDF URI
 *
 *  - Maps
 *   - Combind categories(strings)
 *   - Transform (numeric, case_when...)
 *
 *  - Aggregations
 *   - Bucket Per month/year(x)...
 *   - Bucket Per Category(string)...
 *
 *  - Duplication Detection
 *  - Missing Entry Detection
 *
 */
export default {
    headers: [
        "Company",
        "Category",
        "Amount",
        "Paid",
        "Due",
        "Bill",
    ],
    records: [
        {
            uuid: "77872fed-96ba-40a4-b07d-c520d78f1cd4",
            date: "2021-01-10",
            data: {
                Company: "Xfinity",
                Category: "Cable",
                Amount: 16074,
                Paid: true,
                Due: "2021-02-03",
                Bill: "01_Jan_Cable.pdf",
            },
        },
        {
            uuid: "8257357e-d183-4aa1-8138-0f8a522c8d35",
            date: "2021-01-15",
            data: {
                Company: "UGI",
                Category: "Gas",
                Amount: 18265,
                Paid: true,
                Due: "2021-02-05",
                Bill: "01_Jan_Gas.pdf",
            },
        },
        {
            uuid: "9fdd7cfe-6955-4ac8-83e7-d39219dc1d39",
            date: "2021-01-19",
            data: {
                Company: "Penelec",
                Category: "Power",
                Amount: 12562,
                Paid: true,
                Due: "2021-02-08",
                Bill: "01_Jan_Power.pdf",
            },
        },
        {
            uuid: "45417abe-449a-4032-83b5-aa685b5bd3db",
            date: "2021-01-15",
            data: {
                Company: "Sewer",
                Category: "Sewer",
                Amount: 11100,
                Paid: true,
                Due: "2021-01-15",
                Bill: "01_Jan_Sewer.pdf",
            },
        },
        {
            uuid: "c6362139-187b-41f2-ad3d-1b0fe8142b2c",
            date: "2021-01-01",
            data: {
                Company: "Trash",
                Category: "Trash",
                Amount: 4647,
                Paid: true,
                Due: "2021-01-01",
                Bill: "01_Jan_Trash.pdf",
            },
        },
        {
            uuid: "53fda890-54be-4875-b767-c1f26ddc3b76",
            date: "2021-01-01",
            data: {
                Company: "Water",
                Category: "Water",
                Amount: 9458,
                Paid: true,
                Due: "2021-01-01",
                Bill: "01_Jan_Water.pdf",
            },
        },
        {
            uuid: "435f2d93-b503-4718-aa0e-17961a620143",
            date: "2021-01-15",
            data: {
                Company: "UGI",
                Category: "Gas",
                Amount: 18265,
                Paid: true,
                Due: "2021-02-05",
                Bill: "02_Feb_Gas.pdf",
            },
        },
        {
            uuid: "d070d5df-203d-4825-bc16-b46626339253",
            date: "2021-02-16",
            data: {
                Company: "Penelec",
                Category: "Power",
                Amount: 10350,
                Paid: true,
                Due: "2021-03-08",
                Bill: "02_Feb_Electric.pdf",
            },
        },
        {
            uuid: "67a65430-cb37-4299-b247-62a7691de758",
            date: "2021-02-10",
            data: {
                Company: "Xfinity",
                Category: "Cable",
                Amount: 16074,
                Paid: true,
                Due: "2021-03-03",
                Bill: "02_Feb_Cable.pdf",
            },
        },
        {
            uuid: "4300847b-b8e7-42e4-a030-dc4e177d7de3",
            date: "2021-03-18",
            data: {
                Company: "Penelec",
                Category: "Power",
                Amount: 16487,
                Paid: true,
                Due: "2021-04-07",
                Bill: "01_Mar_Power.pdf",
            },
        },
        {
            uuid: "fe648208-0ced-4b19-a30e-3766d42690f7",
            date: "2021-03-17",
            data: {
                Company: "UGI",
                Category: "Gas",
                Amount: 14919,
                Paid: true,
                Due: "2021-04-07",
                Bill: "03_Mar_Gas.pdf",
            },
        },
        {
            uuid: "78f57687-ca20-4120-ada0-9be4da812317",
            date: "2021-03-10",
            data: {
                Company: "Xfinity",
                Category: "Cable",
                Amount: 17132,
                Paid: true,
                Due: "2021-04-01",
                Bill: "03_Mar_Cable.pdf",
            },
        },
        {
            uuid: "41719988-9c04-499b-985a-89a6ca489314",
            date: "2021-04-10",
            data: {
                Company: "Xfinity",
                Category: "Cable",
                Amount: 17600,
                Paid: true,
                Due: "2021-05-03",
                Bill: "04_Apr_Cable.pdf",
            },
        },
        {
            uuid: "08d654f5-b94c-4736-ab2c-9ead63f198ac",
            date: "2021-04-16",
            data: {
                Company: "UGI",
                Category: "Gas",
                Amount: 5920,
                Paid: true,
                Due: "2021-05-07",
                Bill: "04_Apr_Gas.pdf",
            },
        },
        {
            uuid: "d47218ae-bac2-4b03-83dc-e7251c2181ad",
            date: "2021-04-01",
            data: {
                Company: "Water",
                Category: "Water",
                Amount: 7202,
                Paid: true,
                Due: "2021-04-01",
                Bill: "04_Apr_Trash.pdf",
            },
        },
        {
            uuid: "6f8a2422-f5e0-4b51-bbef-35d00af83834",
            date: "2021-04-01",
            data: {
                Company: "Trash",
                Category: "Trash",
                Amount: 4647,
                Paid: true,
                Due: "2021-04-01",
                Bill: "04_Apr_Trash.pdf",
            },
        },

        /* Power, Sewer */
    ],
};

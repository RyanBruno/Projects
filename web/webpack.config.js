// webpack.config.js
const { VueLoaderPlugin } = require("vue-loader")
const path = require("path")
const ESLintPlugin = require("eslint-webpack-plugin")
const HtmlWebpackPlugin = require('html-webpack-plugin');

module.exports = {
    mode: "development",
    module: {
        rules: [
            {
                test: /\.cson$/,
                loader: "cson-loader",
            },
            {
                test: /\.md$/,
                use: [
                    {
                        loader: "html-loader",
                    },
                    {
                        loader: "markdown-loader",
                    },
                ]
            },
            {
              test: /\.css$/,
              use: [
                "vue-style-loader",
                "css-loader",
              ]
            },
            {
                test: /\.vue$/,
                loader: "vue-loader",
            },
        ]
    },
    plugins: [
        new VueLoaderPlugin(),
        new ESLintPlugin(),
        new HtmlWebpackPlugin({
            filename: "ppm.html",
            template: "./ppm/index.html",
            chunks: [ "ppm" ],
        }),
        new HtmlWebpackPlugin({
            filename: "rbruno.com.html",
            template: "./rbruno.com/index.html",
            chunks: [ "rbruno.com" ],
        }),
        new HtmlWebpackPlugin({
            filename: "web3.com.html",
            template: "./web3/index.html",
            chunks: [ "web3" ],
        }),
    ],
    entry: {
        rbruno_com: "./rbruno.com/index.js",
        ppm: "./ppm/index.js",
        web3: "./web3/index.js",
    },
    output: {
        filename: "[name].[fullhash].js",
        path: path.resolve(__dirname, "dist"),
        publicPath: "",
    },
}


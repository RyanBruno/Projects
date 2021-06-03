module.exports = {
    "env": {
        "browser": true,
        "es2021": true
    },
    "parserOptions": {
        "ecmaVersion": 12,
        "sourceType": "module"
    },
    "extends": [
        "eslint",
        "plugin:vue/essential"
    ],
    "plugins": [
        "vue"
    ],
    "rules": {
        "indent": [
            "error",
            4,
        ],
        "linebreak-style": [
            "error",
            "unix",
        ],
        "quotes": [
            "error",
            "double",
        ],
        "semi": [
            "error",
            "always",
        ],
        "comma-dangle": [
            "error",
            "always-multiline",
        ],
        "array-bracket-spacing": [
            "error",
            "always",
        ],
        "node/no-unsupported-features/es-syntax": [
            "error",
            { ignores: [ "modules" ] },
        ],
    }
};

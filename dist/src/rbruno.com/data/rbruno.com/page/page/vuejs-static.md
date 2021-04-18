If you are already familiar with building websites with plain old HTML,
CSS, and JavaScript then you know the headache of building these sites
from scratch.
You know the pain of copying and pasting tag all over your document,
tracking down which class a tag belongs to, and why the style you
thought you applied to said tag is not being applied.
[Vue.js](https://vuejs.org/)
is one of many web frontend frameworks that seeks to solve these
issues.
Vue.js is very similar to
[Angular](https://angularjs.org/) and
[React](https://reactjs.org/)
which is great because the concepts of one map almost directly to the
others.

##### Components

The most basic feature of Vue.js is the ability to break large,
repetitive, and unmaintainable websites into small pieces called
components.
Components assist with code duplication issues and also provide you, the
developer, an independent "thing" you can work with as opposed to an
entire file.
We typically start our webpages with an `<html>`, `<head>`, and `<body>`
tag.
Inside the body comes a `<div>` which houses our navigation bar.
Right here we run into code duplication issues as for each pages we need
copy this navigation bar.
But it goes even further as the content of our page is typically made up
of very similar chunks of code repeater with slightly different
information in them.
Components package all this up into one "thing".
They have a template which keeps only one copy of the HTML and are given
input data for the information that is different across each component
instances.
So replace those huge hunks of copies and pasted tags with three "Post"
components and provide the content as its input.
This is starting to sound more like object-oriented programming then web
development.
Code changes are easier now since all the HTML for multiple post are
contained in a single file.

Remove code duplication.
Improve the dynamics of your site.
And learn the latest in frontend paradigm.
Lets learn Vue.js.

##### Quick Start

Here is what you will need

- [ ] A text editor.
- [ ] A web browser.
- [ ] Some HTML, CSS, and JS knowledge.

In regards to a text editing on Windows, you can use Sublime Text, VS
Code, or good old NotePad.
On Linux, you know what to use (vim).

To begin, create a new file \`index.html\` and write.

```
<html>
    <head>
        <title>Example Page</title>
    </head>
 
    <body>
        <div id="app">
            <example-component
                v-bind:message="message">
            </example-component>
        </div>
    </body>
    <script src="./index.js"></script>
</html>
```

Then create a new file \`index.js\` and write:

```
Vue.component("example-component", {
    props: ['message'],
    template: `
        {{ message }}
    `
});
 
const app = new Vue({
    el: "#app",
    data: {
        message: "Hello World",
    },
});
```

Then open your web browser, type `file:///`, and navigate to your index.html file.

##### How does this work?

Lets define some terms.

The div that looks like `<div id="app">` in `index.html` is your
_application_.
We tell Vue.js that this is our _application_ in `index.js` by creating
a new Vue `instance`.
This is Vue's starting point.
Everything inside our _application_ will be processed by Vue.js and any
_components_ will be rendered by Vue.js.

Currently we have one _component_ called "example-component".
*Before* we create the Vue _instance_ we need to declare all the
_components_ we are going to use.
We declared _components_ in `index.js` with `Vue.component(<name>, options)`
and use the components in `index.html` with `<name ...></name>`.

To recap, `new Vue(...` starts the process by telling Vue.js which
element to start with.
Vue.js finds this element by id then begins a rendering process for all
of that elements children elements.
If that element is a registered component then replace it with that
component's template (see next paragraph) otherwise the element is
left alone.
Repeat.

##### Template Rendering

A little more logic goes into rendering a components then simple find
and replace.
If all _instances_ of a _component_ were that same then they would not
be that useful.
In our example, our component's template looked like this `{{ message }}`.
When rendering, Vue.js will interpret anything inside `{{` brackets `}}`
as a variable.
The variable we used 'message' is given as an input to the
`example-component`.
This is to define clear boundaries between a component and the rest of
our site.
In `index.html` we set 'message' on this line
`v-bind:message="message">`.

##### Properties

Why are _component_ inputs called properties?
Lets break down a regular HTML element.
HTML elements always start with a "<".
What follows is an HTML tag name (div, p, body...).
In Vue.js, we replace the HTML tag name with a component name.
Vue.js will then replace this entire tag with the components template.
This is why if you name your component the same name as an HTML
tag bad things will happen.
After the HTML tag name comes properties.
They take the form key=value.
Normal, HTML element properties tell the browser some information about
the element but Vue.js passes this information as inputs to the components.
On top of that Vue.JS makes properties even more powerful by evaluating
the value of each property as JavaScript.
Anything inside of that value quotes is JavaScript so we can supply
JavaScript variable, expressions, call functions...


##### Property Binding

Now you may have noticed that the property in the example starts with
"v-bind:".
This is another powerful feature of Vue.js called _property binding_.
Property binding means that if there are any changes to the value of the
property it will automatically be updated as input to the component and
re-render the template.

Now you can build your website piece by piece.
Data like page content, blog posts, pictures, animations, user data,
microblog content, shop items are set in your Vue.js _instance_'s data
attribute.

##### For Loops

What we have so far are some _components_, some data, and a way to
bind the data to a _component_ by adding it's tag to either the HTML file
or a template of another _component_.
However, what if we have an array of data, for example posts, and want to render
a _component_ for each item in that array?
This is where for loops come into play in Vue.js.

As an Example (in index.html)

```
    <example-component v-for="message in message"
                  v-bind:message="message"></example-component>
```

To render multiple components replace the `data` part of your Vue
instance with:

```
    data: {
        messages: [
            "Hello World",
            "Goodbye",
        ]
    },
```


Tips:
 - Checkout Vue's documentation for more advanced features
   such as conditional rendering and event.
 - Don't get to worried about how good your site looks are
   first. You are going to change it a million times anyway if you keep it running for long enough.

Check back for more updates in the future.`

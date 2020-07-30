## Bootstrapping Blog...

------

###### \[ [Ryan Bruno](https://rbruno.com/about) \] \[ [more](https://blog.rbruno.com) - [perma](https://blog.rbruno.com/p/first-post) \] \[ 06-25-2020 \]

Welcome to [blog.rbruno.com](https://blog.rbruno.com).
When the blog is fully operational you can expect posts about once a week.
I, myself, am a software developer, Linux user, and open source fanboy.
This blog serves as a place to share experiences, news, articles, tips, and tricks.

### Blog setup

Naturally the first bit I can share is how I post to this blog.
It all begins with an idea in my head...
Once I talk myself in to believing that some bored soul will be interested in what I have to say I create a markdown document.
Yes markdown.
Markdown is fine for what I need currently.
That may change.
Example:

```
## An interesting title

------

### Section

Test [Link](example.com).
```

There are some special styling to help get my point across such as the \<code\> tag (shown above).
The styling is applied from a base HTML header file.
So when I run:

```
# pandoc -f markdown -t html <post_id>.md
```

It transforms the markdown document to a html chunk that then needs to be built with a header and footer file (see below).
The base html and CSS is extremely simple; you can take a look by using "inspecting element" in your browser.
The below script is all that is need to build the post page:

```
#!/bin/sh

pandoc -f markdown -t html md/$1.md | cat header.html - footer.html > p/$1.html 
```

### Hosting

For now, the static files are served by a S3 compatible Linode bucket.
Since one of the purposes of this blog is self-hosting a new setup and post to accompany may be on their way.

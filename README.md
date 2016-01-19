# furries
furries is a simple twitter bot inspired by the following image:

![the furries are back](http://www.wtae.com/image/view/-/20831998/highRes/1/-/maxh/630/maxw/1200/-/tyl7ng/-/img-The-Furries-Are-Back-Who-Are-They.jpg)

It tweets something that the furries are every three hours. Currently, it uses the [WordNet database](http://wordnet.princeton.edu/wordnet/) to pick adjectives and adverbs describing the furries.

The bot uses [YAMLcpp](https://github.com/jbeder/yaml-cpp) to read a configuration file, MySQL C API to read the WordNet database (seeded with [this MySQL version of WordNet 2.0](androidtech.com/html/wordnet-mysql-20.php)), and [twitcurl](https://github.com/swatkat/twitcurl) to send the status updates to Twitter.

The canonical furries handle is [@thefurriesare](https://twitter.com/thefurriesare).
